# GitHub Actions 和跨平台构建总结

本文档总结了为 Dark Server Rust 项目添加的 GitHub Actions 工作流和跨平台构建功能。

## 🚀 已添加的功能

### 1. GitHub Actions 工作流

#### `.github/workflows/ci.yml` - CI/CD 流水线
- **测试阶段**：
  - 多个 Rust 版本测试 (stable, beta, nightly)
  - 代码格式检查 (`cargo fmt`)
  - 静态分析 (`cargo clippy`)
  - 单元测试和集成测试
  - 安全审计 (`cargo audit`)

- **构建阶段**：
  - 跨平台构建支持 6 个目标平台
  - 自动二进制文件优化 (strip)
  - 构建产物上传

- **Docker 阶段**：
  - 多架构 Docker 镜像构建 (amd64, arm64)
  - 自动推送到 Docker Hub (发布时)

#### `.github/workflows/release.yml` - 发布工作流
- **自动化发布**：
  - 标签创建时自动触发
  - 支持手动触发 (workflow_dispatch)
  - 生成完整的发布说明

- **跨平台构建**：
  - Linux: x64, ARM64
  - macOS: Intel x64, Apple Silicon ARM64
  - Windows: x64, ARM64

- **发布产物**：
  - 自动创建压缩包 (tar.gz for Unix, zip for Windows)
  - 生成 SHA256 校验和
  - 上传到 GitHub Releases

### 2. 本地构建脚本

#### `build-cross-platform.sh` (Unix/Linux/macOS)
- **功能**：
  - 自动依赖检查
  - 交叉编译环境设置
  - 多平台并行构建
  - 自动打包和压缩
  - 校验和生成

- **使用方法**：
  ```bash
  ./build-cross-platform.sh [VERSION]
  ```

#### `build-cross-platform.ps1` (Windows PowerShell)
- **功能**：与 Unix 脚本相同的功能
- **使用方法**：
  ```powershell
  .\build-cross-platform.ps1 [-Version <VERSION>]
  ```

### 3. 验证和测试脚本

#### `verify-release.sh` - 发布验证
- **功能**：
  - 压缩包完整性检查
  - 校验和验证
  - 二进制文件执行测试
  - 文件大小检查
  - 生成发布报告

#### `test-binary.sh` - 二进制测试
- **功能**：
  - 二进制文件基本信息检查
  - 启动和关闭测试
  - HTTP/WebSocket 端点测试
  - 性能基准测试
  - 依赖关系检查

### 4. 文档和模板

#### `BUILD.md` - 构建指南
- 详细的构建说明
- 跨平台编译指导
- 故障排除指南
- 性能优化建议

#### `.github/RELEASE_TEMPLATE.md` - 发布模板
- 发布检查清单
- 发布说明模板
- 版本号规范
- 发布流程指导

## 🎯 支持的平台

| 平台 | 架构 | 目标三元组 | 二进制名称 | 压缩格式 |
|------|------|------------|------------|----------|
| Linux | x64 | `x86_64-unknown-linux-gnu` | `dark-server` | tar.gz |
| Linux | ARM64 | `aarch64-unknown-linux-gnu` | `dark-server` | tar.gz |
| macOS | Intel x64 | `x86_64-apple-darwin` | `dark-server` | tar.gz |
| macOS | Apple Silicon | `aarch64-apple-darwin` | `dark-server` | tar.gz |
| Windows | x64 | `x86_64-pc-windows-msvc` | `dark-server.exe` | zip |
| Windows | ARM64 | `aarch64-pc-windows-msvc` | `dark-server.exe` | zip |

## 🔄 工作流程

### 自动发布流程
1. **创建标签**：`git tag v1.0.0 && git push origin v1.0.0`
2. **GitHub Actions 触发**：自动开始构建所有平台
3. **构建完成**：生成 6 个平台的二进制文件
4. **创建发布**：自动创建 GitHub Release
5. **上传产物**：所有压缩包和校验和文件自动上传

### 本地构建流程
1. **运行构建脚本**：`./build-cross-platform.sh 1.0.0`
2. **验证构建**：`./verify-release.sh`
3. **测试二进制**：`./test-binary.sh`
4. **手动发布**：上传到 GitHub Releases

## 📁 输出结构

构建完成后，`releases/` 目录包含：

```
releases/
├── dark-server-v1.0.0-linux-x64/
├── dark-server-v1.0.0-linux-x64.tar.gz
├── dark-server-v1.0.0-linux-arm64/
├── dark-server-v1.0.0-linux-arm64.tar.gz
├── dark-server-v1.0.0-macos-x64/
├── dark-server-v1.0.0-macos-x64.tar.gz
├── dark-server-v1.0.0-macos-arm64/
├── dark-server-v1.0.0-macos-arm64.tar.gz
├── dark-server-v1.0.0-windows-x64/
├── dark-server-v1.0.0-windows-x64.zip
├── dark-server-v1.0.0-windows-arm64/
├── dark-server-v1.0.0-windows-arm64.zip
├── checksums.txt
└── release-report.txt
```

## 🛠️ 技术特性

### 交叉编译支持
- 使用 `cross` 工具进行交叉编译
- 自动配置交叉编译环境
- 支持 ARM64 Linux 交叉编译

### 优化特性
- 发布版本优化 (`--release`)
- 二进制文件压缩 (`strip`)
- 缓存优化 (GitHub Actions)
- 并行构建支持

### 安全特性
- 自动安全审计
- SHA256 校验和生成
- 可重现构建
- 无密钥嵌入

## 🚀 使用方法

### 开发者
```bash
# 本地开发
cargo run

# 本地构建所有平台
./build-cross-platform.sh

# 验证构建
./verify-release.sh
```

### 发布管理员
```bash
# 创建新版本
git tag v1.0.0
git push origin v1.0.0

# GitHub Actions 会自动处理其余工作
```

### 用户
```bash
# 下载预构建二进制
curl -L https://github.com/stevessr/dark-server/releases/download/v1.0.0/dark-server-v1.0.0-linux-x64.tar.gz | tar -xz

# 运行
./dark-server
```

## 🎉 优势

1. **完全自动化**：从代码提交到发布完全自动化
2. **多平台支持**：支持所有主流平台和架构
3. **质量保证**：全面的测试和验证流程
4. **用户友好**：提供预构建二进制，无需编译
5. **开发者友好**：本地构建脚本，便于开发和调试
6. **安全可靠**：校验和验证，确保文件完整性

## 📝 维护说明

- **定期更新**：保持 GitHub Actions 版本最新
- **测试验证**：每次发布前运行完整测试
- **文档同步**：保持文档与实际功能同步
- **监控构建**：关注构建失败和性能问题

这套完整的 CI/CD 系统确保了 Dark Server 项目能够高效、可靠地构建和发布到所有支持的平台。
