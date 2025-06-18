# Dark Server Build Guide

This document explains how to build Dark Server for multiple platforms and architectures.

## Quick Start

### Automated Cross-Platform Build

For Unix-like systems (Linux, macOS):
```bash
./build-cross-platform.sh
```

For Windows (PowerShell):
```powershell
.\build-cross-platform.ps1
```

### Verify Release
```bash
./verify-release.sh
```

## Supported Platforms

Dark Server supports the following platforms and architectures:

| Platform | Architecture | Target Triple | Binary Name |
|----------|-------------|---------------|-------------|
| Linux | x64 | `x86_64-unknown-linux-gnu` | `dark-server` |
| Linux | ARM64 | `aarch64-unknown-linux-gnu` | `dark-server` |
| macOS | Intel x64 | `x86_64-apple-darwin` | `dark-server` |
| macOS | Apple Silicon | `aarch64-apple-darwin` | `dark-server` |
| Windows | x64 | `x86_64-pc-windows-msvc` | `dark-server.exe` |
| Windows | ARM64 | `aarch64-pc-windows-msvc` | `dark-server.exe` |

## Manual Build Instructions

### Prerequisites

1. **Rust Toolchain**: Install from [rustup.rs](https://rustup.rs/)
2. **Cross-compilation tool**: Install with `cargo install cross`
3. **Platform-specific tools**:
   - Linux ARM64: `sudo apt-get install gcc-aarch64-linux-gnu`
   - Windows: Visual Studio Build Tools or Visual Studio

### Install Targets

```bash
# Install all supported targets
rustup target add x86_64-unknown-linux-gnu
rustup target add aarch64-unknown-linux-gnu
rustup target add x86_64-apple-darwin
rustup target add aarch64-apple-darwin
rustup target add x86_64-pc-windows-msvc
rustup target add aarch64-pc-windows-msvc
```

### Build for Specific Target

```bash
# Native build (current platform)
cargo build --release

# Cross-compilation (example for Linux ARM64)
cross build --release --target aarch64-unknown-linux-gnu

# Windows from Linux/macOS
cross build --release --target x86_64-pc-windows-msvc
```

### Configure Cross-Compilation (Linux ARM64)

Create or edit `~/.cargo/config.toml`:
```toml
[target.aarch64-unknown-linux-gnu]
linker = "aarch64-linux-gnu-gcc"
```

## GitHub Actions

The project includes comprehensive GitHub Actions workflows:

### CI/CD Pipeline (`.github/workflows/ci.yml`)
- Runs tests on multiple Rust versions
- Performs code formatting and linting checks
- Security audit with `cargo audit`
- Cross-platform builds
- Docker image building

### Release Workflow (`.github/workflows/release.yml`)
- Triggered on release creation
- Builds for all supported platforms
- Creates release archives
- Generates checksums
- Uploads assets to GitHub Releases

### Triggering a Release

1. **Create a Git tag**:
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```

2. **Create a GitHub Release**:
   - Go to GitHub repository
   - Click "Releases" → "Create a new release"
   - Select the tag you created
   - Fill in release notes
   - Publish the release

3. **Automatic Build**: GitHub Actions will automatically:
   - Build for all platforms
   - Create archives
   - Upload to the release

## Build Scripts

### `build-cross-platform.sh` (Unix)

Features:
- Automatic dependency checking
- Cross-compilation setup
- Archive creation (tar.gz for Unix, zip for Windows)
- Checksum generation
- Colored output and progress reporting

Usage:
```bash
./build-cross-platform.sh [VERSION]

# Examples
./build-cross-platform.sh          # Version: "dev"
./build-cross-platform.sh 1.0.0    # Version: "1.0.0"
```

### `build-cross-platform.ps1` (Windows)

PowerShell equivalent with the same features.

Usage:
```powershell
.\build-cross-platform.ps1 [-Version <VERSION>]

# Examples
.\build-cross-platform.ps1                # Version: "dev"
.\build-cross-platform.ps1 -Version 1.0.0 # Version: "1.0.0"
```

### `verify-release.sh`

Verification script that checks:
- Archive integrity
- Checksum verification
- Binary execution (when possible)
- File sizes
- Generates release report

## Output Structure

After building, the `releases/` directory will contain:

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

Each archive contains:
- The binary executable
- README.md (if present)
- LICENSE (if present)
- CHANGELOG.md (if present)

## Troubleshooting

### Common Issues

1. **Cross-compilation fails**:
   - Ensure `cross` is installed: `cargo install cross`
   - Check Docker is running (required by `cross`)

2. **Linux ARM64 build fails**:
   - Install cross-compiler: `sudo apt-get install gcc-aarch64-linux-gnu`
   - Configure cargo config as shown above

3. **Windows build fails on Linux/macOS**:
   - Use `cross` instead of `cargo`
   - Ensure Docker is available

4. **Permission denied on scripts**:
   ```bash
   chmod +x build-cross-platform.sh
   chmod +x verify-release.sh
   ```

### Debug Build

For debugging, use debug builds:
```bash
cargo build --target <TARGET>  # Without --release
```

### Clean Build

To clean previous builds:
```bash
cargo clean
rm -rf releases/
```

## Performance Optimization

The release builds are optimized with:
- `--release` flag (optimizations enabled)
- Binary stripping (reduces size)
- LTO (Link Time Optimization) can be enabled in `Cargo.toml`

## Security

- All builds are reproducible
- Checksums are generated for verification
- Security audit runs in CI/CD
- No secrets are embedded in binaries

## Contributing

When adding new platforms:
1. Add target to the build scripts
2. Update the platform tables in this document
3. Test the build on the target platform
4. Update GitHub Actions if needed
