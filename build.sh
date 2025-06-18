#!/bin/bash

# Dark Server C++ 构建脚本
# 使用方法: ./build.sh [clean|install-deps|vcpkg]

set -e  # 遇到错误时退出

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
THIRD_PARTY_DIR="$PROJECT_DIR/third_party"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查命令是否存在
check_command() {
    if ! command -v "$1" &> /dev/null; then
        log_error "$1 未找到，请先安装"
        return 1
    fi
}

# 清理构建目录
clean_build() {
    log_info "清理构建目录..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        log_success "构建目录已清理"
    else
        log_info "构建目录不存在，无需清理"
    fi
}

# 安装依赖到 third_party 目录
install_deps_manual() {
    log_info "手动安装依赖到 third_party 目录..."
    
    mkdir -p "$THIRD_PARTY_DIR"
    cd "$THIRD_PARTY_DIR"
    
    # httplib
    if [ ! -d "httplib" ]; then
        log_info "下载 httplib..."
        git clone --depth 1 https://github.com/yhirose/cpp-httplib.git httplib
        log_success "httplib 下载完成"
    else
        log_info "httplib 已存在，跳过下载"
    fi
    
    # websocketpp
    if [ ! -d "websocketpp" ]; then
        log_info "下载 websocketpp..."
        git clone --depth 1 https://github.com/zaphoyd/websocketpp.git websocketpp
        log_success "websocketpp 下载完成"
    else
        log_info "websocketpp 已存在，跳过下载"
    fi
    
    # nlohmann/json
    if [ ! -d "nlohmann" ]; then
        log_info "下载 nlohmann/json..."
        git clone --depth 1 https://github.com/nlohmann/json.git nlohmann
        log_success "nlohmann/json 下载完成"
    else
        log_info "nlohmann/json 已存在，跳过下载"
    fi
    
    cd "$PROJECT_DIR"
    log_success "所有依赖安装完成"
}

# 使用 vcpkg 安装依赖
install_deps_vcpkg() {
    log_info "使用 vcpkg 安装依赖..."
    
    if ! check_command "vcpkg"; then
        log_error "vcpkg 未找到，请先安装 vcpkg"
        log_info "安装方法："
        log_info "  git clone https://github.com/Microsoft/vcpkg.git"
        log_info "  cd vcpkg && ./bootstrap-vcpkg.sh"
        log_info "  export PATH=\$PATH:\$(pwd)"
        return 1
    fi
    
    log_info "安装 C++ 依赖包..."
    vcpkg install httplib websocketpp nlohmann-json boost-system
    
    log_success "vcpkg 依赖安装完成"
}

# 检查系统依赖
check_system_deps() {
    log_info "检查系统依赖..."
    
    # 检查必要的工具
    check_command "cmake" || return 1
    check_command "make" || check_command "ninja" || {
        log_error "需要 make 或 ninja 构建工具"
        return 1
    }
    
    # 检查编译器
    if check_command "g++"; then
        GCC_VERSION=$(g++ --version | head -n1 | grep -oE '[0-9]+\.[0-9]+' | head -1)
        log_info "找到 GCC $GCC_VERSION"
    elif check_command "clang++"; then
        CLANG_VERSION=$(clang++ --version | head -n1 | grep -oE '[0-9]+\.[0-9]+' | head -1)
        log_info "找到 Clang $CLANG_VERSION"
    else
        log_error "未找到 C++ 编译器 (g++ 或 clang++)"
        return 1
    fi
    
    log_success "系统依赖检查通过"
}

# 构建项目
build_project() {
    log_info "开始构建项目..."
    
    # 创建构建目录
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 配置 CMake
    CMAKE_ARGS=""
    if command -v vcpkg &> /dev/null && [ -f "$(vcpkg list | head -1 | cut -d' ' -f1 2>/dev/null || echo '')" ]; then
        VCPKG_ROOT=$(vcpkg integrate install 2>/dev/null | grep -o '/.*vcpkg' | head -1 || echo '')
        if [ -n "$VCPKG_ROOT" ] && [ -f "$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" ]; then
            CMAKE_ARGS="-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
            log_info "使用 vcpkg 工具链: $VCPKG_ROOT"
        fi
    fi
    
    log_info "配置 CMake..."
    cmake .. $CMAKE_ARGS
    
    # 构建
    log_info "编译项目..."
    cmake --build . --config Release
    
    cd "$PROJECT_DIR"
    log_success "项目构建完成"
}

# 运行项目
run_project() {
    log_info "运行 dark-server..."
    
    if [ -f "$BUILD_DIR/bin/dark-server" ]; then
        "$BUILD_DIR/bin/dark-server"
    elif [ -f "$BUILD_DIR/dark-server" ]; then
        "$BUILD_DIR/dark-server"
    else
        log_error "找不到可执行文件，请先构建项目"
        return 1
    fi
}

# 显示帮助信息
show_help() {
    echo "Dark Server C++ 构建脚本"
    echo ""
    echo "使用方法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  clean           清理构建目录"
    echo "  install-deps    手动安装依赖到 third_party 目录"
    echo "  vcpkg          使用 vcpkg 安装依赖"
    echo "  build          构建项目 (默认)"
    echo "  run            构建并运行项目"
    echo "  help           显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                    # 构建项目"
    echo "  $0 clean build        # 清理后构建"
    echo "  $0 install-deps build # 安装依赖后构建"
    echo "  $0 vcpkg build        # 使用 vcpkg 安装依赖后构建"
}

# 主函数
main() {
    cd "$PROJECT_DIR"
    
    if [ $# -eq 0 ]; then
        # 默认行为：检查依赖并构建
        check_system_deps && build_project
        return $?
    fi
    
    while [ $# -gt 0 ]; do
        case $1 in
            clean)
                clean_build
                ;;
            install-deps)
                install_deps_manual
                ;;
            vcpkg)
                install_deps_vcpkg
                ;;
            build)
                check_system_deps && build_project
                ;;
            run)
                check_system_deps && build_project && run_project
                ;;
            help|--help|-h)
                show_help
                exit 0
                ;;
            *)
                log_error "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
        shift
    done
}

# 运行主函数
main "$@"
