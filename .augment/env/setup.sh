#!/bin/bash

# Dark Server Setup Script for Ubuntu Linux
# This script installs Rust, configures the environment, and prepares for testing

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_step() {
    echo -e "${YELLOW}[SETUP]${NC} $1"
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

print_step "Starting Dark Server environment setup..."

# Update system packages
print_step "Updating system packages..."
sudo apt-get update -y

# Install essential build tools and dependencies
print_step "Installing build dependencies..."
sudo apt-get install -y \
    curl \
    build-essential \
    pkg-config \
    libssl-dev \
    ca-certificates \
    git \
    wget

# Install Rust using rustup
print_step "Installing Rust toolchain..."
if ! command -v rustc &> /dev/null; then
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable
    
    # Add Rust to PATH for current session
    export PATH="$HOME/.cargo/bin:$PATH"
    
    # Add Rust to PATH permanently in .profile
    echo 'export PATH="$HOME/.cargo/bin:$PATH"' >> $HOME/.profile
    
    print_success "Rust installed successfully"
else
    print_info "Rust is already installed"
    export PATH="$HOME/.cargo/bin:$PATH"
fi

# Verify Rust installation
print_step "Verifying Rust installation..."
rustc --version
cargo --version

# Update Rust to latest stable
print_step "Updating Rust to latest stable version..."
rustup update stable
rustup default stable

# Install additional Rust components that might be needed
print_step "Installing additional Rust components..."
rustup component add rustfmt clippy

# Navigate to project directory
cd /mnt/persist/workspace

# Verify Cargo.toml exists
if [[ ! -f "Cargo.toml" ]]; then
    print_error "Cargo.toml not found in current directory"
    exit 1
fi

print_success "Found Cargo.toml - this is a valid Rust project"

# Clean any previous builds
print_step "Cleaning previous builds..."
cargo clean

# Update dependencies and build project
print_step "Fetching and updating dependencies..."
cargo fetch

print_step "Building project in debug mode..."
cargo build

print_step "Building project in release mode..."
cargo build --release

print_success "Environment setup completed successfully!"
print_info "Rust version: $(rustc --version)"
print_info "Cargo version: $(cargo --version)"
print_info "Project ready for testing"