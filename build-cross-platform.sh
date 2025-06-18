#!/bin/bash

# Cross-platform build script for Dark Server
# This script builds the project for multiple platforms and architectures

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="dark-server"
VERSION=${1:-"dev"}
OUTPUT_DIR="releases"

# Supported targets
TARGETS=(
    "x86_64-unknown-linux-gnu"
    "aarch64-unknown-linux-gnu"
    "x86_64-apple-darwin"
    "aarch64-apple-darwin"
    "x86_64-pc-windows-msvc"
    "aarch64-pc-windows-msvc"
)

# Platform-specific settings
declare -A PLATFORM_NAMES=(
    ["x86_64-unknown-linux-gnu"]="linux-x64"
    ["aarch64-unknown-linux-gnu"]="linux-arm64"
    ["x86_64-apple-darwin"]="macos-x64"
    ["aarch64-apple-darwin"]="macos-arm64"
    ["x86_64-pc-windows-msvc"]="windows-x64"
    ["aarch64-pc-windows-msvc"]="windows-arm64"
)

declare -A BINARY_NAMES=(
    ["x86_64-unknown-linux-gnu"]="dark-server"
    ["aarch64-unknown-linux-gnu"]="dark-server"
    ["x86_64-apple-darwin"]="dark-server"
    ["aarch64-apple-darwin"]="dark-server"
    ["x86_64-pc-windows-msvc"]="dark-server.exe"
    ["aarch64-pc-windows-msvc"]="dark-server.exe"
)

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Dark Server Cross-Platform Build${NC}"
    echo -e "${BLUE}================================${NC}"
    echo ""
}

print_step() {
    echo -e "${YELLOW}[STEP]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

check_dependencies() {
    print_step "Checking dependencies..."
    
    # Check if Rust is installed
    if ! command -v cargo &> /dev/null; then
        print_error "Cargo not found. Please install Rust: https://rustup.rs/"
        exit 1
    fi
    
    # Check if cross is installed for cross-compilation
    if ! command -v cross &> /dev/null; then
        print_info "Installing cross for cross-compilation..."
        cargo install cross --git https://github.com/cross-rs/cross
    fi
    
    print_success "Dependencies checked"
}

install_targets() {
    print_step "Installing Rust targets..."
    
    for target in "${TARGETS[@]}"; do
        print_info "Installing target: $target"
        rustup target add "$target" || true
    done
    
    print_success "Targets installed"
}

build_target() {
    local target=$1
    local platform_name=${PLATFORM_NAMES[$target]}
    local binary_name=${BINARY_NAMES[$target]}
    
    print_step "Building for $target ($platform_name)..."
    
    # Use cross for cross-compilation, cargo for native builds
    local build_cmd="cargo"
    if [[ "$target" != "$(rustc -vV | sed -n 's|host: ||p')" ]]; then
        build_cmd="cross"
    fi
    
    # Build the project
    if $build_cmd build --release --target "$target"; then
        print_success "Built successfully for $target"
        
        # Create output directory
        local output_path="$OUTPUT_DIR/${PROJECT_NAME}-v${VERSION}-${platform_name}"
        mkdir -p "$output_path"
        
        # Copy binary
        cp "target/$target/release/$binary_name" "$output_path/"
        
        # Copy additional files
        if [[ -f "README.md" ]]; then
            cp "README.md" "$output_path/"
        fi
        if [[ -f "LICENSE" ]]; then
            cp "LICENSE" "$output_path/"
        fi
        if [[ -f "CHANGELOG.md" ]]; then
            cp "CHANGELOG.md" "$output_path/"
        fi
        
        # Create archive
        cd "$OUTPUT_DIR"
        if [[ "$target" == *"windows"* ]]; then
            zip -r "${PROJECT_NAME}-v${VERSION}-${platform_name}.zip" "${PROJECT_NAME}-v${VERSION}-${platform_name}/"
        else
            tar -czf "${PROJECT_NAME}-v${VERSION}-${platform_name}.tar.gz" "${PROJECT_NAME}-v${VERSION}-${platform_name}/"
        fi
        cd ..
        
        print_success "Archive created for $platform_name"
    else
        print_error "Failed to build for $target"
        return 1
    fi
}

build_all() {
    print_step "Building for all targets..."
    
    # Clean previous builds
    rm -rf "$OUTPUT_DIR"
    mkdir -p "$OUTPUT_DIR"
    
    local failed_builds=()
    
    for target in "${TARGETS[@]}"; do
        if ! build_target "$target"; then
            failed_builds+=("$target")
        fi
        echo ""
    done
    
    # Report results
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Build Summary${NC}"
    echo -e "${BLUE}================================${NC}"
    
    if [[ ${#failed_builds[@]} -eq 0 ]]; then
        print_success "All builds completed successfully!"
    else
        print_error "Some builds failed:"
        for target in "${failed_builds[@]}"; do
            echo -e "  ${RED}✗${NC} $target"
        done
    fi
    
    echo ""
    print_info "Build artifacts are in the '$OUTPUT_DIR' directory"
    ls -la "$OUTPUT_DIR"
}

generate_checksums() {
    print_step "Generating checksums..."
    
    cd "$OUTPUT_DIR"
    sha256sum *.tar.gz *.zip 2>/dev/null > checksums.txt || true
    cd ..
    
    print_success "Checksums generated"
}

main() {
    print_header
    
    check_dependencies
    install_targets
    build_all
    generate_checksums
    
    echo ""
    print_success "Cross-platform build completed!"
    echo -e "${BLUE}Version:${NC} $VERSION"
    echo -e "${BLUE}Output:${NC} $OUTPUT_DIR"
}

# Show help
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: $0 [VERSION]"
    echo ""
    echo "Build Dark Server for multiple platforms and architectures."
    echo ""
    echo "Arguments:"
    echo "  VERSION    Version string (default: 'dev')"
    echo ""
    echo "Examples:"
    echo "  $0                # Build with version 'dev'"
    echo "  $0 1.0.0          # Build with version '1.0.0'"
    echo ""
    echo "Supported platforms:"
    for target in "${TARGETS[@]}"; do
        echo "  - $target (${PLATFORM_NAMES[$target]})"
    done
    exit 0
fi

# Run main function
main "$@"
