# Dark Server Release Template

Use this template when creating new releases.

## Release Checklist

- [ ] Update version in `Cargo.toml`
- [ ] Update `CHANGELOG.md` with new features and fixes
- [ ] Test all functionality locally
- [ ] Run cross-platform build: `./build-cross-platform.sh`
- [ ] Verify builds: `./verify-release.sh`
- [ ] Create and push git tag: `git tag vX.X.X && git push origin vX.X.X`
- [ ] Create GitHub release with this template

## Release Notes Template

```markdown
## Dark Server vX.X.X

High-performance HTTP-to-WebSocket proxy server written in Rust.

### 🎉 What's New

- New feature 1
- New feature 2
- Performance improvement

### 🐛 Bug Fixes

- Fixed issue with connection handling
- Resolved memory leak in message queue
- Improved error handling

### 🔧 Changes

- Updated dependency versions
- Improved logging format
- Enhanced documentation

### 📦 Downloads

#### Linux
- **x64**: `dark-server-vX.X.X-linux-x64.tar.gz`
- **ARM64**: `dark-server-vX.X.X-linux-arm64.tar.gz`

#### macOS
- **Intel (x64)**: `dark-server-vX.X.X-macos-x64.tar.gz`
- **Apple Silicon (ARM64)**: `dark-server-vX.X.X-macos-arm64.tar.gz`

#### Windows
- **x64**: `dark-server-vX.X.X-windows-x64.zip`
- **ARM64**: `dark-server-vX.X.X-windows-arm64.zip`

### 🚀 Quick Start

1. Download the appropriate binary for your platform
2. Extract the archive: `tar -xzf dark-server-vX.X.X-*.tar.gz` (Unix) or unzip (Windows)
3. Run the server: `./dark-server` (Unix) or `dark-server.exe` (Windows)

### 📋 Default Configuration

- HTTP Server: `http://localhost:8889`
- WebSocket Server: `ws://localhost:9998`

### 🔍 Verification

Verify your download with checksums:
```bash
sha256sum -c checksums.txt
```

### 📚 Documentation

- [README.md](https://github.com/stevessr/dark-server/blob/main/README.md) - Usage instructions
- [BUILD.md](https://github.com/stevessr/dark-server/blob/main/BUILD.md) - Build instructions
- [CHANGELOG.md](https://github.com/stevessr/dark-server/blob/main/CHANGELOG.md) - Detailed changes

### 🐛 Known Issues

- None at this time

### 🤝 Contributing

See [CONTRIBUTING.md](https://github.com/stevessr/dark-server/blob/main/CONTRIBUTING.md) for guidelines.

---

**Full Changelog**: https://github.com/stevessr/dark-server/compare/vX.X.X-1...vX.X.X
```

## Version Numbering

Follow [Semantic Versioning](https://semver.org/):

- **MAJOR** version for incompatible API changes
- **MINOR** version for backwards-compatible functionality additions  
- **PATCH** version for backwards-compatible bug fixes

Examples:
- `v1.0.0` - Initial stable release
- `v1.1.0` - New features, backwards compatible
- `v1.1.1` - Bug fixes only
- `v2.0.0` - Breaking changes

## Pre-release Versions

For pre-releases, use:
- `v1.0.0-alpha.1` - Alpha version
- `v1.0.0-beta.1` - Beta version
- `v1.0.0-rc.1` - Release candidate

## Automated Release Process

1. **Create Tag**: `git tag v1.0.0 && git push origin v1.0.0`
2. **GitHub Actions**: Automatically builds all platforms
3. **Create Release**: Use GitHub web interface with the template above
4. **Verify**: Check that all assets are uploaded correctly

## Manual Release Process

If automated release fails:

1. **Build Locally**:
   ```bash
   ./build-cross-platform.sh 1.0.0
   ./verify-release.sh
   ```

2. **Create Release**: Use GitHub web interface

3. **Upload Assets**: Manually upload files from `releases/` directory

## Post-Release Tasks

- [ ] Announce on relevant channels
- [ ] Update documentation if needed
- [ ] Monitor for issues
- [ ] Plan next release
