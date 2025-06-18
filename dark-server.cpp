#include "dark-server.h"
#include <httplib.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

using json = nlohmann::json;
using namespace std::chrono;

namespace DarkServer {

// LoggingService 实现
LoggingService::LoggingService(const std::string& serviceName) : serviceName_(serviceName) {}

std::string LoggingService::formatMessage(const std::string& level, const std::string& message) {
    auto now = system_clock::now();
    auto time_t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << "[" << level << "] " 
       << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count() << "Z"
       << " [" << serviceName_ << "] - " << message;
    return ss.str();
}

void LoggingService::info(const std::string& message) {
    std::cout << formatMessage("INFO", message) << std::endl;
}

void LoggingService::error(const std::string& message) {
    std::cerr << formatMessage("ERROR", message) << std::endl;
}

void LoggingService::warn(const std::string& message) {
    std::cout << formatMessage("WARN", message) << std::endl;
}

void LoggingService::debug(const std::string& message) {
    std::cout << formatMessage("DEBUG", message) << std::endl;
}

// MessageQueue 实现
MessageQueue::MessageQueue(std::chrono::milliseconds timeoutMs) : defaultTimeout_(timeoutMs) {}

MessageQueue::~MessageQueue() {
    close();
}

void MessageQueue::enqueue(const Message& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (closed_) return;
    
    if (!waitingPromises_.empty()) {
        auto promise = std::move(waitingPromises_.front());
        waitingPromises_.pop();
        promise.set_value(message);
    } else {
        messages_.push(message);
    }
}

std::future<Message> MessageQueue::dequeue(std::chrono::milliseconds timeoutMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (closed_) {
        std::promise<Message> promise;
        promise.set_exception(std::make_exception_ptr(std::runtime_error("Queue is closed")));
        return promise.get_future();
    }
    
    if (!messages_.empty()) {
        auto message = messages_.front();
        messages_.pop();
        std::promise<Message> promise;
        promise.set_value(message);
        return promise.get_future();
    }
    
    std::promise<Message> promise;
    auto future = promise.get_future();
    waitingPromises_.push(std::move(promise));
    
    // 设置超时处理
    auto timeout = (timeoutMs.count() == 0) ? defaultTimeout_ : timeoutMs;
    std::thread([this, timeout, &future]() {
        if (future.wait_for(timeout) == std::future_status::timeout) {
            std::lock_guard<std::mutex> lock(mutex_);
            // 超时处理逻辑
        }
    }).detach();
    
    return future;
}

void MessageQueue::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    
    while (!waitingPromises_.empty()) {
        auto promise = std::move(waitingPromises_.front());
        waitingPromises_.pop();
        promise.set_exception(std::make_exception_ptr(std::runtime_error("Queue closed")));
    }
    
    std::queue<Message> empty;
    messages_.swap(empty);
}

bool MessageQueue::isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

// ConnectionRegistry 实现
ConnectionRegistry::ConnectionRegistry(std::shared_ptr<LoggingService> logger) : logger_(logger) {}

ConnectionRegistry::~ConnectionRegistry() {
    for (auto& [requestId, queue] : messageQueues_) {
        queue->close();
    }
}

void ConnectionRegistry::addConnection(websocketpp::connection_hdl::type hdl, const ClientInfo& clientInfo) {
    {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        connections_.insert(hdl);
    }
    
    logger_->info("新客户端连接: " + clientInfo.address);
    
    for (auto& callback : connectionAddedCallbacks_) {
        callback(hdl);
    }
}

void ConnectionRegistry::removeConnection(websocketpp::connection_hdl::type hdl) {
    {
        std::lock_guard<std::mutex> lock(connectionsMutex_);
        connections_.erase(hdl);
    }
    
    logger_->info("客户端连接断开");
    
    // 关闭所有相关的消息队列
    for (auto& [requestId, queue] : messageQueues_) {
        queue->close();
    }
    messageQueues_.clear();
    
    for (auto& callback : connectionRemovedCallbacks_) {
        callback(hdl);
    }
}

void ConnectionRegistry::handleIncomingMessage(const std::string& messageData) {
    try {
        auto parsedMessage = json::parse(messageData);
        
        if (!parsedMessage.contains("request_id")) {
            logger_->warn("收到无效消息：缺少request_id");
            return;
        }
        
        std::string requestId = parsedMessage["request_id"];
        auto it = messageQueues_.find(requestId);
        
        if (it != messageQueues_.end()) {
            Message msg;
            msg.requestId = requestId;
            if (parsedMessage.contains("event_type")) {
                msg.eventType = parsedMessage["event_type"];
            }
            if (parsedMessage.contains("data")) {
                msg.data = parsedMessage["data"];
            }
            if (parsedMessage.contains("status")) {
                msg.status = parsedMessage["status"];
            }
            
            routeMessage(msg, it->second);
        } else {
            logger_->warn("收到未知请求ID的消息: " + requestId);
        }
    } catch (const std::exception& e) {
        logger_->error("解析WebSocket消息失败: " + std::string(e.what()));
    }
}

void ConnectionRegistry::routeMessage(const Message& message, std::shared_ptr<MessageQueue> queue) {
    const std::string& eventType = message.eventType;
    
    if (eventType == "response_headers" || eventType == "chunk" || eventType == "error") {
        queue->enqueue(message);
    } else if (eventType == "stream_close") {
        Message endMsg;
        endMsg.type = "STREAM_END";
        queue->enqueue(endMsg);
    } else {
        logger_->warn("未知的事件类型: " + eventType);
    }
}

bool ConnectionRegistry::hasActiveConnections() const {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    return !connections_.empty();
}

websocketpp::connection_hdl::type ConnectionRegistry::getFirstConnection() const {
    std::lock_guard<std::mutex> lock(connectionsMutex_);
    if (!connections_.empty()) {
        return *connections_.begin();
    }
    return websocketpp::connection_hdl::type();
}

std::shared_ptr<MessageQueue> ConnectionRegistry::createMessageQueue(const std::string& requestId) {
    auto queue = std::make_shared<MessageQueue>();
    messageQueues_[requestId] = queue;
    return queue;
}

void ConnectionRegistry::removeMessageQueue(const std::string& requestId) {
    auto it = messageQueues_.find(requestId);
    if (it != messageQueues_.end()) {
        it->second->close();
        messageQueues_.erase(it);
    }
}

void ConnectionRegistry::onConnectionAdded(ConnectionCallback callback) {
    connectionAddedCallbacks_.push_back(callback);
}

void ConnectionRegistry::onConnectionRemoved(ConnectionCallback callback) {
    connectionRemovedCallbacks_.push_back(callback);
}

// RequestHandler 实现
RequestHandler::RequestHandler(std::shared_ptr<ConnectionRegistry> connectionRegistry,
                               std::shared_ptr<LoggingService> logger)
    : connectionRegistry_(connectionRegistry), logger_(logger) {}

void RequestHandler::processRequest(const httplib::Request& req, httplib::Response& res) {
    logger_->info("处理请求: " + req.method + " " + req.path);

    if (!connectionRegistry_->hasActiveConnections()) {
        sendErrorResponse(res, 503, "没有可用的浏览器连接");
        return;
    }

    std::string requestId = generateRequestId();
    Message proxyRequest = buildProxyRequest(req, requestId);

    auto messageQueue = connectionRegistry_->createMessageQueue(requestId);

    try {
        forwardRequest(proxyRequest);
        handleResponse(messageQueue, res);
    } catch (const std::exception& error) {
        handleRequestError(error, res);
    }

    connectionRegistry_->removeMessageQueue(requestId);
}

std::string RequestHandler::generateRequestId() {
    auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 35);

    std::string randomStr;
    for (int i = 0; i < 9; ++i) {
        char c = (dis(gen) < 10) ? ('0' + dis(gen)) : ('a' + dis(gen) - 10);
        randomStr += c;
    }

    return std::to_string(now) + "_" + randomStr;
}

Message RequestHandler::buildProxyRequest(const httplib::Request& req, const std::string& requestId) {
    Message proxyRequest;
    proxyRequest.requestId = requestId;
    proxyRequest.type = "proxy_request";

    // 构建请求数据 (简化版本)
    json requestData;
    requestData["path"] = req.path;
    requestData["method"] = req.method;
    requestData["request_id"] = requestId;
    requestData["body"] = req.body;

    // 添加headers
    json headers;
    for (const auto& [name, value] : req.headers) {
        headers[name] = value;
    }
    requestData["headers"] = headers;

    proxyRequest.data = requestData.dump();
    return proxyRequest;
}

void RequestHandler::forwardRequest(const Message& proxyRequest) {
    // 这里需要通过WebSocket发送请求
    // 实际实现需要访问WebSocket服务器实例
    // 暂时留空，需要在ProxyServerSystem中实现
}

void RequestHandler::handleResponse(std::shared_ptr<MessageQueue> messageQueue, httplib::Response& res) {
    try {
        // 等待响应头
        auto headerFuture = messageQueue->dequeue();
        auto headerMessage = headerFuture.get();

        if (headerMessage.eventType == "error") {
            sendErrorResponse(res, headerMessage.status, headerMessage.data);
            return;
        }

        // 设置响应头
        setResponseHeaders(res, headerMessage);

        // 处理流式数据
        streamResponseData(messageQueue, res);
    } catch (const std::exception& e) {
        throw;
    }
}

void RequestHandler::setResponseHeaders(httplib::Response& res, const Message& headerMessage) {
    res.status = headerMessage.status;

    for (const auto& [name, value] : headerMessage.headers) {
        res.set_header(name, value);
    }
}

void RequestHandler::streamResponseData(std::shared_ptr<MessageQueue> messageQueue, httplib::Response& res) {
    std::string responseBody;

    while (true) {
        try {
            auto dataFuture = messageQueue->dequeue();
            auto dataMessage = dataFuture.get();

            if (dataMessage.type == "STREAM_END") {
                break;
            }

            if (!dataMessage.data.empty()) {
                responseBody += dataMessage.data;
            }
        } catch (const std::exception& e) {
            std::string errorMsg = e.what();
            if (errorMsg.find("timeout") != std::string::npos) {
                // 处理超时
                break;
            } else {
                throw;
            }
        }
    }

    res.body = responseBody;
}

void RequestHandler::handleRequestError(const std::exception& error, httplib::Response& res) {
    std::string errorMsg = error.what();
    if (errorMsg.find("timeout") != std::string::npos) {
        sendErrorResponse(res, 504, "请求超时");
    } else {
        logger_->error("请求处理错误: " + errorMsg);
        sendErrorResponse(res, 500, "代理错误: " + errorMsg);
    }
}

void RequestHandler::sendErrorResponse(httplib::Response& res, int status, const std::string& message) {
    res.status = status;
    res.body = message;
    res.set_header("Content-Type", "text/plain; charset=utf-8");
}

// ProxyServerSystem 实现
ProxyServerSystem::ProxyServerSystem(const ServerConfig& config)
    : config_(config), logger_(std::make_shared<LoggingService>("ProxyServer")) {

    connectionRegistry_ = std::make_shared<ConnectionRegistry>(logger_);
    requestHandler_ = std::make_shared<RequestHandler>(connectionRegistry_, logger_);
}

ProxyServerSystem::~ProxyServerSystem() {
    stop();
}

void ProxyServerSystem::start() {
    try {
        running_ = true;

        startHttpServer();
        startWebSocketServer();

        logger_->info("代理服务器系统启动完成");

        for (auto& callback : startedCallbacks_) {
            callback();
        }
    } catch (const std::exception& error) {
        std::string errorMsg = "启动失败: " + std::string(error.what());
        logger_->error(errorMsg);

        for (auto& callback : errorCallbacks_) {
            callback(errorMsg);
        }
        throw;
    }
}

void ProxyServerSystem::stop() {
    if (!running_) return;

    running_ = false;

    if (httpServer_) {
        httpServer_->stop();
    }

    if (wsServer_) {
        wsServer_->stop();
    }

    if (httpThread_.joinable()) {
        httpThread_.join();
    }

    if (wsThread_.joinable()) {
        wsThread_.join();
    }

    logger_->info("代理服务器系统已停止");
}

void ProxyServerSystem::startHttpServer() {
    httpServer_ = std::make_unique<httplib::Server>();
    setupHttpRoutes();

    httpThread_ = std::thread([this]() {
        std::string address = config_.host + ":" + std::to_string(config_.httpPort);
        logger_->info("HTTP服务器启动: http://" + address);

        if (!httpServer_->listen(config_.host, config_.httpPort)) {
            throw std::runtime_error("HTTP服务器启动失败");
        }
    });

    // 等待服务器启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ProxyServerSystem::startWebSocketServer() {
    using WSServer = websocketpp::server<websocketpp::config::asio>;

    wsServer_ = std::make_unique<WSServer>();
    setupWebSocketHandlers();

    wsThread_ = std::thread([this]() {
        try {
            wsServer_->set_access_channels(websocketpp::log::alevel::all);
            wsServer_->clear_access_channels(websocketpp::log::alevel::frame_payload);
            wsServer_->init_asio();

            wsServer_->listen(config_.wsPort);
            wsServer_->start_accept();

            std::string address = config_.host + ":" + std::to_string(config_.wsPort);
            logger_->info("WebSocket服务器启动: ws://" + address);

            wsServer_->run();
        } catch (const std::exception& e) {
            logger_->error("WebSocket服务器错误: " + std::string(e.what()));
        }
    });
}

void ProxyServerSystem::setupHttpRoutes() {
    // 处理所有HTTP请求
    httpServer_->set_mount_point("/", ".");

    // 通用请求处理器
    auto handler = [this](const httplib::Request& req, httplib::Response& res) {
        requestHandler_->processRequest(req, res);
    };

    httpServer_->Get(".*", handler);
    httpServer_->Post(".*", handler);
    httpServer_->Put(".*", handler);
    httpServer_->Delete(".*", handler);
    httpServer_->Patch(".*", handler);
}

void ProxyServerSystem::setupWebSocketHandlers() {
    using WSServer = websocketpp::server<websocketpp::config::asio>;

    // 连接建立处理
    wsServer_->set_open_handler([this](websocketpp::connection_hdl hdl) {
        ClientInfo clientInfo;
        clientInfo.address = "unknown"; // 简化版本
        clientInfo.connectTime = system_clock::now();

        connectionRegistry_->addConnection(hdl, clientInfo);
    });

    // 连接关闭处理
    wsServer_->set_close_handler([this](websocketpp::connection_hdl hdl) {
        connectionRegistry_->removeConnection(hdl);
    });

    // 消息处理
    wsServer_->set_message_handler([this](websocketpp::connection_hdl hdl, WSServer::message_ptr msg) {
        connectionRegistry_->handleIncomingMessage(msg->get_payload());
    });
}

void ProxyServerSystem::onStarted(std::function<void()> callback) {
    startedCallbacks_.push_back(callback);
}

void ProxyServerSystem::onError(std::function<void(const std::string&)> callback) {
    errorCallbacks_.push_back(callback);
}

// 初始化函数
void initializeServer() {
    try {
        ProxyServerSystem serverSystem;

        serverSystem.onStarted([]() {
            std::cout << "服务器启动成功!" << std::endl;
        });

        serverSystem.onError([](const std::string& error) {
            std::cerr << "服务器错误: " << error << std::endl;
        });

        serverSystem.start();

        // 保持程序运行
        std::cout << "按 Enter 键停止服务器..." << std::endl;
        std::cin.get();

        serverSystem.stop();
    } catch (const std::exception& error) {
        std::cerr << "服务器启动失败: " << error.what() << std::endl;
        std::exit(1);
    }
}

} // namespace DarkServer

// 主函数
int main() {
    DarkServer::initializeServer();
    return 0;
}
