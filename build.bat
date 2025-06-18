@echo off
setlocal enabledelayedexpansion

REM Dark Server C++ Windows 构建脚本
REM 使用方法: build.bat [clean|install-deps|vcpkg|build|run|help]

set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build"
set "THIRD_PARTY_DIR=%PROJECT_DIR%third_party"

REM 颜色定义 (Windows 10+)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

:log_info
echo %BLUE%[INFO]%NC% %~1
goto :eof

:log_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:log_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:log_error
echo %RED%[ERROR]%NC% %~1
goto :eof

:check_command
where %1 >nul 2>&1
if errorlevel 1 (
    call :log_error "%1 未找到，请先安装"
    exit /b 1
)
goto :eof

:clean_build
call :log_info "清理构建目录..."
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    call :log_success "构建目录已清理"
) else (
    call :log_info "构建目录不存在，无需清理"
)
goto :eof

:install_deps_manual
call :log_info "手动安装依赖到 third_party 目录..."

if not exist "%THIRD_PARTY_DIR%" mkdir "%THIRD_PARTY_DIR%"
cd /d "%THIRD_PARTY_DIR%"

REM httplib
if not exist "httplib" (
    call :log_info "下载 httplib..."
    git clone --depth 1 https://github.com/yhirose/cpp-httplib.git httplib
    if errorlevel 1 (
        call :log_error "httplib 下载失败"
        exit /b 1
    )
    call :log_success "httplib 下载完成"
) else (
    call :log_info "httplib 已存在，跳过下载"
)

REM websocketpp
if not exist "websocketpp" (
    call :log_info "下载 websocketpp..."
    git clone --depth 1 https://github.com/zaphoyd/websocketpp.git websocketpp
    if errorlevel 1 (
        call :log_error "websocketpp 下载失败"
        exit /b 1
    )
    call :log_success "websocketpp 下载完成"
) else (
    call :log_info "websocketpp 已存在，跳过下载"
)

REM nlohmann/json
if not exist "nlohmann" (
    call :log_info "下载 nlohmann/json..."
    git clone --depth 1 https://github.com/nlohmann/json.git nlohmann
    if errorlevel 1 (
        call :log_error "nlohmann/json 下载失败"
        exit /b 1
    )
    call :log_success "nlohmann/json 下载完成"
) else (
    call :log_info "nlohmann/json 已存在，跳过下载"
)

cd /d "%PROJECT_DIR%"
call :log_success "所有依赖安装完成"
goto :eof

:install_deps_vcpkg
call :log_info "使用 vcpkg 安装依赖..."

call :check_command vcpkg
if errorlevel 1 (
    call :log_error "vcpkg 未找到，请先安装 vcpkg"
    call :log_info "安装方法："
    call :log_info "  git clone https://github.com/Microsoft/vcpkg.git"
    call :log_info "  cd vcpkg && .\bootstrap-vcpkg.bat"
    call :log_info "  将 vcpkg 目录添加到 PATH 环境变量"
    exit /b 1
)

call :log_info "安装 C++ 依赖包..."
vcpkg install httplib:x64-windows websocketpp:x64-windows nlohmann-json:x64-windows boost-system:x64-windows
if errorlevel 1 (
    call :log_error "vcpkg 依赖安装失败"
    exit /b 1
)

call :log_success "vcpkg 依赖安装完成"
goto :eof

:check_system_deps
call :log_info "检查系统依赖..."

REM 检查 CMake
call :check_command cmake
if errorlevel 1 exit /b 1

REM 检查 Visual Studio 或其他编译器
where cl >nul 2>&1
if not errorlevel 1 (
    call :log_info "找到 MSVC 编译器"
) else (
    where g++ >nul 2>&1
    if not errorlevel 1 (
        call :log_info "找到 GCC 编译器"
    ) else (
        where clang++ >nul 2>&1
        if not errorlevel 1 (
            call :log_info "找到 Clang 编译器"
        ) else (
            call :log_error "未找到 C++ 编译器"
            call :log_info "请安装 Visual Studio 或 MinGW-w64"
            exit /b 1
        )
    )
)

call :log_success "系统依赖检查通过"
goto :eof

:build_project
call :log_info "开始构建项目..."

REM 创建构建目录
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM 检查是否使用 vcpkg
set "CMAKE_ARGS="
vcpkg integrate install >nul 2>&1
if not errorlevel 1 (
    for /f "tokens=*" %%i in ('vcpkg integrate install 2^>nul ^| findstr "CMake"') do (
        for /f "tokens=2 delims= " %%j in ("%%i") do (
            if exist "%%j" (
                set "CMAKE_ARGS=-DCMAKE_TOOLCHAIN_FILE=%%j"
                call :log_info "使用 vcpkg 工具链: %%j"
            )
        )
    )
)

REM 检测 Visual Studio 版本
where cl >nul 2>&1
if not errorlevel 1 (
    call :log_info "配置 CMake (Visual Studio)..."
    cmake .. -G "Visual Studio 16 2019" -A x64 %CMAKE_ARGS%
) else (
    call :log_info "配置 CMake (MinGW)..."
    cmake .. -G "MinGW Makefiles" %CMAKE_ARGS%
)

if errorlevel 1 (
    call :log_error "CMake 配置失败"
    exit /b 1
)

REM 构建
call :log_info "编译项目..."
cmake --build . --config Release
if errorlevel 1 (
    call :log_error "项目编译失败"
    exit /b 1
)

cd /d "%PROJECT_DIR%"
call :log_success "项目构建完成"
goto :eof

:run_project
call :log_info "运行 dark-server..."

if exist "%BUILD_DIR%\bin\Release\dark-server.exe" (
    "%BUILD_DIR%\bin\Release\dark-server.exe"
) else if exist "%BUILD_DIR%\Release\dark-server.exe" (
    "%BUILD_DIR%\Release\dark-server.exe"
) else if exist "%BUILD_DIR%\dark-server.exe" (
    "%BUILD_DIR%\dark-server.exe"
) else (
    call :log_error "找不到可执行文件，请先构建项目"
    exit /b 1
)
goto :eof

:show_help
echo Dark Server C++ Windows 构建脚本
echo.
echo 使用方法: %~nx0 [选项]
echo.
echo 选项:
echo   clean           清理构建目录
echo   install-deps    手动安装依赖到 third_party 目录
echo   vcpkg          使用 vcpkg 安装依赖
echo   build          构建项目 (默认)
echo   run            构建并运行项目
echo   help           显示此帮助信息
echo.
echo 示例:
echo   %~nx0                    # 构建项目
echo   %~nx0 clean build        # 清理后构建
echo   %~nx0 install-deps build # 安装依赖后构建
echo   %~nx0 vcpkg build        # 使用 vcpkg 安装依赖后构建
goto :eof

:main
cd /d "%PROJECT_DIR%"

if "%~1"=="" (
    REM 默认行为：检查依赖并构建
    call :check_system_deps
    if not errorlevel 1 call :build_project
    goto :eof
)

:parse_args
if "%~1"=="" goto :eof

if /i "%~1"=="clean" (
    call :clean_build
) else if /i "%~1"=="install-deps" (
    call :install_deps_manual
) else if /i "%~1"=="vcpkg" (
    call :install_deps_vcpkg
) else if /i "%~1"=="build" (
    call :check_system_deps
    if not errorlevel 1 call :build_project
) else if /i "%~1"=="run" (
    call :check_system_deps
    if not errorlevel 1 (
        call :build_project
        if not errorlevel 1 call :run_project
    )
) else if /i "%~1"=="help" (
    call :show_help
    exit /b 0
) else if /i "%~1"=="--help" (
    call :show_help
    exit /b 0
) else if /i "%~1"=="-h" (
    call :show_help
    exit /b 0
) else (
    call :log_error "未知选项: %~1"
    call :show_help
    exit /b 1
)

shift
goto :parse_args

REM 运行主函数
call :main %*
