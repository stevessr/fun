use anyhow::Result;
use bytes::Bytes;
use chrono::Utc;
use dashmap::DashMap;
use futures_util::{SinkExt, StreamExt};

use serde::{Deserialize, Serialize};
use std::{
    collections::HashMap,
    net::SocketAddr,
    sync::{
        atomic::{AtomicBool, Ordering},
        Arc,
    },
    time::Duration,
};
use tokio::{
    sync::{mpsc, oneshot, RwLock},
    time::timeout,
};
use tokio_tungstenite::{
    accept_async,
    tungstenite::Message as WsMessage,
};
use warp::Filter;

// 日志记录器模块 (Logging Service Module)
#[derive(Clone)]
pub struct LoggingService {
    service_name: String,
}

impl LoggingService {
    pub fn new(service_name: &str) -> Self {
        Self {
            service_name: service_name.to_string(),
        }
    }

    fn format_message(&self, level: &str, message: &str) -> String {
        let timestamp = Utc::now().to_rfc3339();
        format!("[{}] {} [{}] - {}", level, timestamp, self.service_name, message)
    }

    pub fn info(&self, message: &str) {
        println!("{}", self.format_message("INFO", message));
    }

    pub fn error(&self, message: &str) {
        eprintln!("{}", self.format_message("ERROR", message));
    }

    pub fn warn(&self, message: &str) {
        eprintln!("{}", self.format_message("WARN", message));
    }

    pub fn debug(&self, message: &str) {
        println!("{}", self.format_message("DEBUG", message));
    }
}

// 消息队列实现 (Message Queue Implementation)
pub struct MessageQueue<T> {
    messages: Arc<RwLock<Vec<T>>>,
    waiting_resolvers: Arc<RwLock<Vec<oneshot::Sender<T>>>>,
    default_timeout: Duration,
    closed: Arc<AtomicBool>,
}

impl<T: Clone + Send + 'static> MessageQueue<T> {
    pub fn new(timeout_ms: u64) -> Self {
        Self {
            messages: Arc::new(RwLock::new(Vec::new())),
            waiting_resolvers: Arc::new(RwLock::new(Vec::new())),
            default_timeout: Duration::from_millis(timeout_ms),
            closed: Arc::new(AtomicBool::new(false)),
        }
    }

    pub async fn enqueue(&self, message: T) {
        if self.closed.load(Ordering::Relaxed) {
            return;
        }

        let mut resolvers = self.waiting_resolvers.write().await;
        if let Some(resolver) = resolvers.pop() {
            let _ = resolver.send(message);
        } else {
            drop(resolvers);
            let mut messages = self.messages.write().await;
            messages.push(message);
        }
    }

    pub async fn dequeue(&self) -> Result<T> {
        self.dequeue_with_timeout(self.default_timeout).await
    }

    pub async fn dequeue_with_timeout(&self, timeout_duration: Duration) -> Result<T> {
        if self.closed.load(Ordering::Relaxed) {
            return Err(anyhow::anyhow!("Queue is closed"));
        }

        let mut messages = self.messages.write().await;
        if let Some(message) = messages.pop() {
            return Ok(message);
        }
        drop(messages);

        let (tx, rx) = oneshot::channel();
        {
            let mut resolvers = self.waiting_resolvers.write().await;
            resolvers.push(tx);
        }

        match timeout(timeout_duration, rx).await {
            Ok(Ok(message)) => Ok(message),
            Ok(Err(_)) => Err(anyhow::anyhow!("Queue closed")),
            Err(_) => Err(anyhow::anyhow!("Queue timeout")),
        }
    }

    pub async fn close(&self) {
        self.closed.store(true, Ordering::Relaxed);
        let mut resolvers = self.waiting_resolvers.write().await;
        resolvers.clear();
        let mut messages = self.messages.write().await;
        messages.clear();
    }
}

// 消息类型定义 (Message Type Definitions)
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ProxyRequest {
    pub path: String,
    pub method: String,
    pub headers: HashMap<String, String>,
    pub query_params: HashMap<String, String>,
    pub body: String,
    pub request_id: String,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
#[serde(tag = "event_type", rename_all = "snake_case")]
pub enum ClientMessage {
    ResponseHeaders {
        request_id: String,
        status: u16,
        headers: HashMap<String, String>,
    },
    Chunk {
        request_id: String,
        data: String,
    },
    Error {
        request_id: String,
        status: Option<u16>,
        message: String,
    },
    StreamClose {
        request_id: String,
    },
}

#[derive(Debug, Clone)]
pub struct StreamEndMessage {
    pub message_type: String,
}

impl StreamEndMessage {
    pub fn new() -> Self {
        Self {
            message_type: "STREAM_END".to_string(),
        }
    }
}

// WebSocket连接管理器 (WebSocket Connection Registry)
pub struct ConnectionRegistry {
    logger: LoggingService,
    connections: Arc<RwLock<Vec<mpsc::UnboundedSender<String>>>>,
    message_queues: Arc<DashMap<String, Arc<MessageQueue<ClientMessage>>>>,
}

impl ConnectionRegistry {
    pub fn new(logger: LoggingService) -> Self {
        Self {
            logger,
            connections: Arc::new(RwLock::new(Vec::new())),
            message_queues: Arc::new(DashMap::new()),
        }
    }

    pub async fn add_connection(&self, sender: mpsc::UnboundedSender<String>, client_info: &str) {
        let mut connections = self.connections.write().await;
        connections.push(sender);
        self.logger.info(&format!("新客户端连接: {}", client_info));
    }

    pub async fn remove_connection(&self, _sender: &mpsc::UnboundedSender<String>) {
        self.logger.info("客户端连接断开");

        // 关闭所有相关的消息队列
        for entry in self.message_queues.iter() {
            entry.value().close().await;
        }
        self.message_queues.clear();
    }

    pub async fn handle_incoming_message(&self, message_data: &str) {
        match serde_json::from_str::<ClientMessage>(message_data) {
            Ok(parsed_message) => {
                let request_id = self.get_request_id(&parsed_message);

                if request_id.is_empty() {
                    self.logger.warn("收到无效消息：缺少request_id");
                    return;
                }

                if let Some(queue) = self.message_queues.get(&request_id) {
                    self.route_message(parsed_message, queue.value()).await;
                } else {
                    self.logger.warn(&format!("收到未知请求ID的消息: {}", request_id));
                }
            }
            Err(_) => {
                self.logger.error("解析WebSocket消息失败");
            }
        }
    }

    fn get_request_id(&self, message: &ClientMessage) -> String {
        match message {
            ClientMessage::ResponseHeaders { request_id, .. } => request_id.clone(),
            ClientMessage::Chunk { request_id, .. } => request_id.clone(),
            ClientMessage::Error { request_id, .. } => request_id.clone(),
            ClientMessage::StreamClose { request_id, .. } => request_id.clone(),
        }
    }

    async fn route_message(&self, message: ClientMessage, queue: &MessageQueue<ClientMessage>) {
        match &message {
            ClientMessage::ResponseHeaders { .. }
            | ClientMessage::Chunk { .. }
            | ClientMessage::Error { .. } => {
                queue.enqueue(message).await;
            }
            ClientMessage::StreamClose { .. } => {
                queue.enqueue(ClientMessage::StreamClose {
                    request_id: self.get_request_id(&message)
                }).await;
            }
        }
    }

    pub async fn has_active_connections(&self) -> bool {
        let connections = self.connections.read().await;
        !connections.is_empty()
    }

    pub async fn get_first_connection(&self) -> Option<mpsc::UnboundedSender<String>> {
        let connections = self.connections.read().await;
        connections.first().cloned()
    }

    pub fn create_message_queue(&self, request_id: String) -> Arc<MessageQueue<ClientMessage>> {
        let queue = Arc::new(MessageQueue::new(600000)); // 10 minutes timeout
        self.message_queues.insert(request_id, queue.clone());
        queue
    }

    pub async fn remove_message_queue(&self, request_id: &str) {
        if let Some((_, queue)) = self.message_queues.remove(request_id) {
            queue.close().await;
        }
    }
}

// 请求处理器 (Request Handler)
pub struct RequestHandler {
    connection_registry: Arc<ConnectionRegistry>,
    logger: LoggingService,
}

impl RequestHandler {
    pub fn new(connection_registry: Arc<ConnectionRegistry>, logger: LoggingService) -> Self {
        Self {
            connection_registry,
            logger,
        }
    }

    pub async fn process_request(&self, req: warp::http::Request<Bytes>) -> Result<warp::http::Response<String>> {
        let method = req.method().to_string();
        let path = req.uri().path().to_string();

        self.logger.info(&format!("处理请求: {} {}", method, path));

        if !self.connection_registry.has_active_connections().await {
            return self.send_error_response(503, "没有可用的浏览器连接");
        }

        let request_id = self.generate_request_id();
        let proxy_request = self.build_proxy_request(req, &request_id)?;

        let message_queue = self.connection_registry.create_message_queue(request_id.clone());

        match self.forward_request(proxy_request).await {
            Ok(_) => {
                match self.handle_response(message_queue, &request_id).await {
                    Ok(response) => {
                        self.connection_registry.remove_message_queue(&request_id).await;
                        Ok(response)
                    }
                    Err(e) => {
                        self.connection_registry.remove_message_queue(&request_id).await;
                        self.handle_request_error(e)
                    }
                }
            }
            Err(e) => {
                self.connection_registry.remove_message_queue(&request_id).await;
                self.handle_request_error(e)
            }
        }
    }

    fn generate_request_id(&self) -> String {
        let timestamp = chrono::Utc::now().timestamp_millis();
        let random_part: String = (0..9)
            .map(|_| {
                let random_byte = (timestamp as u8).wrapping_add(rand::random::<u8>()) % 26;
                char::from(b'a' + random_byte)
            })
            .collect();
        format!("{}_{}", timestamp, random_part)
    }

    fn build_proxy_request(&self, req: warp::http::Request<Bytes>, request_id: &str) -> Result<ProxyRequest> {
        let method = req.method().to_string();
        let path = req.uri().path().to_string();

        let headers: HashMap<String, String> = req
            .headers()
            .iter()
            .map(|(k, v)| (k.to_string(), v.to_str().unwrap_or("").to_string()))
            .collect();

        let query_params: HashMap<String, String> = req
            .uri()
            .query()
            .unwrap_or("")
            .split('&')
            .filter_map(|pair| {
                let mut parts = pair.split('=');
                match (parts.next(), parts.next()) {
                    (Some(key), Some(value)) => Some((key.to_string(), value.to_string())),
                    _ => None,
                }
            })
            .collect();

        let body = String::from_utf8(req.body().to_vec()).unwrap_or_default();

        Ok(ProxyRequest {
            path,
            method,
            headers,
            query_params,
            body,
            request_id: request_id.to_string(),
        })
    }

    async fn forward_request(&self, proxy_request: ProxyRequest) -> Result<()> {
        if let Some(connection) = self.connection_registry.get_first_connection().await {
            let message = serde_json::to_string(&proxy_request)?;
            connection.send(message).map_err(|_| anyhow::anyhow!("Failed to send message"))?;
            Ok(())
        } else {
            Err(anyhow::anyhow!("No active connections"))
        }
    }

    async fn handle_response(
        &self,
        message_queue: Arc<MessageQueue<ClientMessage>>,
        _request_id: &str,
    ) -> Result<warp::http::Response<String>> {
        // 等待响应头
        let header_message = message_queue.dequeue().await?;

        match header_message {
            ClientMessage::Error { status, message, .. } => {
                return self.send_error_response(status.unwrap_or(500), &message);
            }
            ClientMessage::ResponseHeaders { status, headers, .. } => {
                // 设置响应头并处理流式数据
                let mut response_body = String::new();

                // 处理流式数据
                loop {
                    match message_queue.dequeue_with_timeout(Duration::from_secs(10)).await {
                        Ok(ClientMessage::Chunk { data, .. }) => {
                            response_body.push_str(&data);
                        }
                        Ok(ClientMessage::StreamClose { .. }) => {
                            break;
                        }
                        Ok(ClientMessage::Error { message, .. }) => {
                            self.logger.warn(&format!("Stream interrupted by error: {}", message));
                            break;
                        }
                        Err(e) => {
                            if e.to_string().contains("timeout") {
                                // Handle timeout for event streams
                                response_body.push_str(": keepalive\n\n");
                                continue;
                            } else {
                                break;
                            }
                        }
                        _ => {
                            self.logger.warn("Received unexpected message during stream");
                        }
                    }
                }

                let mut response = warp::http::Response::builder().status(status);

                for (key, value) in headers {
                    response = response.header(&key, &value);
                }

                Ok(response.body(response_body)?)
            }
            _ => {
                self.send_error_response(500, "Invalid response format")
            }
        }
    }

    fn handle_request_error(&self, error: anyhow::Error) -> Result<warp::http::Response<String>> {
        if error.to_string().contains("timeout") {
            self.send_error_response(504, "请求超时")
        } else {
            self.logger.error(&format!("请求处理错误: {}", error));
            self.send_error_response(500, &format!("代理错误: {}", error))
        }
    }

    fn send_error_response(&self, status: u16, message: &str) -> Result<warp::http::Response<String>> {
        Ok(warp::http::Response::builder()
            .status(status)
            .body(message.to_string())?)
    }
}

// 主服务器类 (Main Proxy Server System)
pub struct ProxyServerSystem {
    config: ServerConfig,
    logger: LoggingService,
    connection_registry: Arc<ConnectionRegistry>,
    request_handler: Arc<RequestHandler>,
}

#[derive(Clone)]
pub struct ServerConfig {
    pub http_port: u16,
    pub ws_port: u16,
    pub host: String,
}

impl Default for ServerConfig {
    fn default() -> Self {
        Self {
            http_port: 8889,
            ws_port: 9998,
            host: "0.0.0.0".to_string(),
        }
    }
}

impl ProxyServerSystem {
    pub fn new(config: Option<ServerConfig>) -> Self {
        let config = config.unwrap_or_default();
        let logger = LoggingService::new("ProxyServer");
        let connection_registry = Arc::new(ConnectionRegistry::new(logger.clone()));
        let request_handler = Arc::new(RequestHandler::new(connection_registry.clone(), logger.clone()));

        Self {
            config,
            logger,
            connection_registry,
            request_handler,
        }
    }

    pub async fn start(&self) -> Result<()> {
        // Start HTTP server
        let http_task = self.start_http_server();

        // Start WebSocket server
        let ws_task = self.start_websocket_server();

        // Wait for both servers
        let (http_result, ws_result) = tokio::join!(http_task, ws_task);

        http_result?;
        ws_result?;

        self.logger.info("代理服务器系统启动完成");
        Ok(())
    }

    async fn start_http_server(&self) -> Result<()> {
        let request_handler = self.request_handler.clone();

        let routes = warp::any()
            .and(warp::method())
            .and(warp::path::full())
            .and(warp::header::headers_cloned())
            .and(warp::body::bytes())
            .and_then(move |method: warp::http::Method, path: warp::path::FullPath, headers: warp::http::HeaderMap, body: Bytes| {
                let handler = request_handler.clone();
                async move {
                    let mut req_builder = warp::http::Request::builder()
                        .method(method)
                        .uri(path.as_str());

                    for (key, value) in headers.iter() {
                        req_builder = req_builder.header(key, value);
                    }

                    let req = req_builder.body(body).map_err(|_| warp::reject::reject())?;

                    match handler.process_request(req).await {
                        Ok(response) => {
                            let (parts, body) = response.into_parts();
                            let mut builder = warp::http::Response::builder().status(parts.status);

                            for (key, value) in parts.headers.iter() {
                                builder = builder.header(key, value);
                            }

                            Ok::<_, warp::Rejection>(builder.body(body).unwrap())
                        }
                        Err(_) => Err(warp::reject::reject()),
                    }
                }
            });

        let addr: SocketAddr = format!("{}:{}", self.config.host, self.config.http_port).parse()?;
        self.logger.info(&format!("HTTP服务器启动: http://{}", addr));

        warp::serve(routes).run(addr).await;
        Ok(())
    }

    async fn start_websocket_server(&self) -> Result<()> {
        let addr: SocketAddr = format!("{}:{}", self.config.host, self.config.ws_port).parse()?;
        let listener = tokio::net::TcpListener::bind(&addr).await?;

        self.logger.info(&format!("WebSocket服务器启动: ws://{}", addr));

        while let Ok((stream, addr)) = listener.accept().await {
            let registry = self.connection_registry.clone();
            let logger = self.logger.clone();

            tokio::spawn(async move {
                if let Err(e) = Self::handle_websocket_connection(stream, addr, registry, logger).await {
                    eprintln!("WebSocket connection error: {}", e);
                }
            });
        }

        Ok(())
    }

    async fn handle_websocket_connection(
        stream: tokio::net::TcpStream,
        addr: SocketAddr,
        registry: Arc<ConnectionRegistry>,
        _logger: LoggingService,
    ) -> Result<()> {
        let ws_stream = accept_async(stream).await?;
        let (mut ws_sender, mut ws_receiver) = ws_stream.split();

        let (tx, mut rx) = mpsc::unbounded_channel::<String>();

        // Add connection to registry
        registry.add_connection(tx.clone(), &addr.to_string()).await;

        // Spawn task to handle outgoing messages
        tokio::spawn(async move {
            while let Some(message) = rx.recv().await {
                if let Err(_) = ws_sender.send(WsMessage::Text(message)).await {
                    break;
                }
            }
        });

        // Handle incoming messages
        while let Some(msg) = ws_receiver.next().await {
            match msg? {
                WsMessage::Text(text) => {
                    registry.handle_incoming_message(&text).await;
                }
                WsMessage::Close(_) => {
                    break;
                }
                _ => {}
            }
        }

        registry.remove_connection(&tx).await;
        Ok(())
    }
}

// 启动函数 (Initialization Function)
pub async fn initialize_server() -> Result<()> {
    let server_system = ProxyServerSystem::new(None);
    server_system.start().await
}

// 主函数 (Main Function)
#[tokio::main]
async fn main() -> Result<()> {
    // Initialize logging
    tracing_subscriber::fmt::init();

    // Start the server
    if let Err(e) = initialize_server().await {
        eprintln!("服务器启动失败: {}", e);
        std::process::exit(1);
    }

    Ok(())
}