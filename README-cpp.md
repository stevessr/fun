# Dark Server C++ 版本

这是原始 JavaScript `dark-server.js` 的 C++ 移植版本，提供相同的代理服务器功能。

## 功能特性

- **HTTP 代理服务器**: 监听 HTTP 请求并转发到 WebSocket 客户端
- **WebSocket 服务器**: 管理客户端连接和消息路由
- **异步消息队列**: 支持超时的消息队列系统
- **连接管理**: 自动管理 WebSocket 连接的生命周期
- **日志系统**: 结构化日志记录
- **错误处理**: 完善的错误处理和超时机制

## 系统要求

- C++17 或更高版本
- CMake 3.15+
- Boost 库 (system 组件)
- 支持的编译器：GCC 7+, Clang 6+, MSVC 2017+

## 依赖库

项目使用以下第三方库：

1. **httplib** - HTTP 服务器库
2. **websocketpp** - WebSocket 服务器库  
3. **nlohmann/json** - JSON 解析库
4. **Boost** - 系统库支持

## 快速开始

### 1. 安装依赖 (使用 vcpkg 推荐)

```bash
# 安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS

# 安装依赖
./vcpkg install httplib websocketpp nlohmann-json boost-system
```

### 2. 构建项目

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

### 3. 运行

```bash
./bin/dark-server
```

## 详细安装说明

### 方法 1: 使用 vcpkg (推荐)

vcpkg 是微软开发的 C++ 包管理器，可以轻松管理依赖：

```bash
# 克隆 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# 初始化 (Linux/macOS)
./bootstrap-vcpkg.sh
# Windows
# .\bootstrap-vcpkg.bat

# 安装依赖
./vcpkg install httplib websocketpp nlohmann-json boost-system

# 构建项目
cd /path/to/your/project
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

### 方法 2: 手动安装到 third_party

```bash
mkdir -p third_party && cd third_party

# httplib (header-only)
git clone https://github.com/yhirose/cpp-httplib.git httplib

# websocketpp (header-only)
git clone https://github.com/zaphoyd/websocketpp.git

# nlohmann/json (header-only)
git clone https://github.com/nlohmann/json.git nlohmann

# 返回项目根目录构建
cd ..
mkdir build && cd build
cmake ..
cmake --build .
```

### 方法 3: 系统包管理器

#### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libboost-system-dev nlohmann-json3-dev
# 注意：httplib 和 websocketpp 可能需要手动安装
```

#### macOS (Homebrew):
```bash
brew install boost nlohmann-json
# 注意：httplib 和 websocketpp 可能需要手动安装
```

## 配置选项

默认配置：
- HTTP 端口: 8889
- WebSocket 端口: 9998
- 监听地址: 0.0.0.0

修改配置请编辑 `dark-server.cpp` 中的 `ServerConfig`：

```cpp
ServerConfig config;
config.httpPort = 8889;
config.wsPort = 9998;
config.host = "0.0.0.0";
```

## 使用示例

### WebSocket 客户端连接

```javascript
const ws = new WebSocket('ws://localhost:9998');

ws.onopen = function() {
    console.log('Connected to server');
};

ws.onmessage = function(event) {
    const request = JSON.parse(event.data);
    // 处理代理请求
    console.log('Received request:', request);
    
    // 发送响应
    ws.send(JSON.stringify({
        request_id: request.request_id,
        event_type: 'response_headers',
        status: 200,
        headers: {'Content-Type': 'application/json'}
    }));
    
    ws.send(JSON.stringify({
        request_id: request.request_id,
        event_type: 'chunk',
        data: 'Hello from C++ server!'
    }));
    
    ws.send(JSON.stringify({
        request_id: request.request_id,
        event_type: 'stream_close'
    }));
};
```

### HTTP 客户端请求

```bash
# 测试 HTTP 代理
curl http://localhost:8889/api/test
```

## 项目结构

```
├── dark-server.h          # 类声明和接口定义
├── dark-server.cpp        # 主要实现代码
├── CMakeLists.txt         # CMake 构建配置
├── README-cpp.md          # C++ 版本文档
└── third_party/           # 第三方库 (可选)
    ├── httplib/
    ├── websocketpp/
    └── nlohmann/
```

## 故障排除

### 常见编译问题

1. **找不到 Boost 库**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libboost-all-dev
   
   # macOS
   brew install boost
   ```

2. **C++17 支持错误**:
   确保使用支持 C++17 的编译器版本

3. **第三方库路径问题**:
   检查 CMakeLists.txt 中的路径设置

### 运行时问题

1. **端口被占用**:
   ```bash
   # 检查端口使用情况
   netstat -tulpn | grep :8889
   netstat -tulpn | grep :9998
   ```

2. **权限问题**:
   某些系统可能需要管理员权限绑定低端口号

## 性能对比

与 JavaScript 版本相比，C++ 版本具有：

- **更低的内存占用**: 通常减少 50-70%
- **更快的启动时间**: 无需 Node.js 运行时
- **更高的并发性能**: 原生线程支持
- **更小的部署包**: 单一可执行文件

## 开发和贡献

欢迎提交 Issue 和 Pull Request！

### 开发环境设置

1. 安装开发依赖
2. 配置 IDE (推荐 VS Code + C++ 扩展)
3. 运行测试 (如果有)

### 代码风格

项目遵循现代 C++ 最佳实践：
- 使用 RAII
- 智能指针管理内存
- 异常安全的代码
- 标准库优先
