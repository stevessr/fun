#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <thread>
#include <atomic>

// Forward declarations
namespace httplib { class Server; class Request; class Response; }
namespace websocketpp { 
    namespace config { namespace asio { struct server; } }
    template<typename config> class server;
    namespace connection_hdl { typedef std::weak_ptr<void> type; }
}

namespace DarkServer {

// 日志记录器类
class LoggingService {
public:
    explicit LoggingService(const std::string& serviceName = "ProxyServer");
    
    void info(const std::string& message);
    void error(const std::string& message);
    void warn(const std::string& message);
    void debug(const std::string& message);

private:
    std::string serviceName_;
    std::string formatMessage(const std::string& level, const std::string& message);
};

// 消息结构
struct Message {
    std::string type;
    std::string data;
    std::map<std::string, std::string> headers;
    int status = 200;
    std::string eventType;
    std::string requestId;
};

// 消息队列类
class MessageQueue {
public:
    explicit MessageQueue(std::chrono::milliseconds timeoutMs = std::chrono::milliseconds(600000));
    ~MessageQueue();
    
    void enqueue(const Message& message);
    std::future<Message> dequeue(std::chrono::milliseconds timeoutMs = std::chrono::milliseconds(0));
    void close();
    bool isClosed() const;

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<Message> messages_;
    std::queue<std::promise<Message>> waitingPromises_;
    std::chrono::milliseconds defaultTimeout_;
    std::atomic<bool> closed_{false};
};

// WebSocket连接信息
struct ClientInfo {
    std::string address;
    std::chrono::system_clock::time_point connectTime;
};

// 事件回调类型
using ConnectionCallback = std::function<void(websocketpp::connection_hdl::type)>;
using MessageCallback = std::function<void(const std::string&)>;

// WebSocket连接管理器
class ConnectionRegistry {
public:
    explicit ConnectionRegistry(std::shared_ptr<LoggingService> logger);
    ~ConnectionRegistry();
    
    void addConnection(websocketpp::connection_hdl::type hdl, const ClientInfo& clientInfo);
    void removeConnection(websocketpp::connection_hdl::type hdl);
    void handleIncomingMessage(const std::string& messageData);
    
    bool hasActiveConnections() const;
    websocketpp::connection_hdl::type getFirstConnection() const;
    
    std::shared_ptr<MessageQueue> createMessageQueue(const std::string& requestId);
    void removeMessageQueue(const std::string& requestId);
    
    // 事件回调设置
    void onConnectionAdded(ConnectionCallback callback);
    void onConnectionRemoved(ConnectionCallback callback);

private:
    std::shared_ptr<LoggingService> logger_;
    mutable std::mutex connectionsMutex_;
    std::set<websocketpp::connection_hdl::type, std::owner_less<websocketpp::connection_hdl::type>> connections_;
    std::map<std::string, std::shared_ptr<MessageQueue>> messageQueues_;
    
    std::vector<ConnectionCallback> connectionAddedCallbacks_;
    std::vector<ConnectionCallback> connectionRemovedCallbacks_;
    
    void routeMessage(const Message& message, std::shared_ptr<MessageQueue> queue);
};

// 请求处理器
class RequestHandler {
public:
    RequestHandler(std::shared_ptr<ConnectionRegistry> connectionRegistry, 
                   std::shared_ptr<LoggingService> logger);
    
    void processRequest(const httplib::Request& req, httplib::Response& res);

private:
    std::shared_ptr<ConnectionRegistry> connectionRegistry_;
    std::shared_ptr<LoggingService> logger_;
    
    std::string generateRequestId();
    Message buildProxyRequest(const httplib::Request& req, const std::string& requestId);
    void forwardRequest(const Message& proxyRequest);
    void handleResponse(std::shared_ptr<MessageQueue> messageQueue, httplib::Response& res);
    void setResponseHeaders(httplib::Response& res, const Message& headerMessage);
    void streamResponseData(std::shared_ptr<MessageQueue> messageQueue, httplib::Response& res);
    void handleRequestError(const std::exception& error, httplib::Response& res);
    void sendErrorResponse(httplib::Response& res, int status, const std::string& message);
};

// 服务器配置
struct ServerConfig {
    int httpPort = 8889;
    int wsPort = 9998;
    std::string host = "0.0.0.0";
};

// 主服务器类
class ProxyServerSystem {
public:
    explicit ProxyServerSystem(const ServerConfig& config = ServerConfig{});
    ~ProxyServerSystem();
    
    void start();
    void stop();
    
    // 事件回调
    void onStarted(std::function<void()> callback);
    void onError(std::function<void(const std::string&)> callback);

private:
    ServerConfig config_;
    std::shared_ptr<LoggingService> logger_;
    std::shared_ptr<ConnectionRegistry> connectionRegistry_;
    std::shared_ptr<RequestHandler> requestHandler_;
    
    std::unique_ptr<httplib::Server> httpServer_;
    std::unique_ptr<websocketpp::server<websocketpp::config::asio::server>> wsServer_;
    
    std::thread httpThread_;
    std::thread wsThread_;
    std::atomic<bool> running_{false};
    
    std::vector<std::function<void()>> startedCallbacks_;
    std::vector<std::function<void(const std::string&)>> errorCallbacks_;
    
    void startHttpServer();
    void startWebSocketServer();
    void setupHttpRoutes();
    void setupWebSocketHandlers();
};

// 初始化函数
void initializeServer();

} // namespace DarkServer
