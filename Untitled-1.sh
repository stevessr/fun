#!/bin/bash

# Dark Server Development Environment Setup Script
set -e

echo "🚀 Setting up Dark Server development environment..."

# Update system packages
echo "📦 Updating system packages..."
sudo apt-get update -y

# Install essential build tools
echo "🔧 Installing essential build tools..."
sudo apt-get install -y \
    curl \
    wget \
    git \
    build-essential \
    pkg-config \
    libssl-dev \
    ca-certificates \
    gnupg \
    lsb-release

# Install Rust toolchain
echo "🦀 Installing Rust toolchain..."
if ! command -v rustc &> /dev/null; then
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --default-toolchain stable
    echo 'export PATH="$HOME/.cargo/bin:$PATH"' >> $HOME/.profile
    source $HOME/.cargo/env
else
    echo "Rust already installed, updating..."
    rustup update stable
fi

# Ensure Rust is in PATH for current session
export PATH="$HOME/.cargo/bin:$PATH"

# Install required Rust components
echo "🔧 Installing Rust components..."
rustup component add rustfmt clippy

# Install Node.js and npm (for JavaScript dependencies)
echo "📦 Installing Node.js..."
if ! command -v node &> /dev/null; then
    curl -fsSL https://deb.nodesource.com/setup_lts.x | sudo -E bash -
    sudo apt-get install -y nodejs
fi

# Install pnpm (package manager used in the project)
echo "📦 Installing pnpm..."
if ! command -v pnpm &> /dev/null; then
    curl -fsSL https://get.pnpm.io/install.sh | sh -
    echo 'export PATH="$HOME/.local/share/pnpm:$PATH"' >> $HOME/.profile
    export PATH="$HOME/.local/share/pnpm:$PATH"
fi

# Install Python and pip (for Python scripts in the project)
echo "🐍 Installing Python development tools..."
sudo apt-get install -y python3 python3-pip python3-venv

# Install Docker (for containerization)
echo "🐳 Installing Docker..."
if ! command -v docker &> /dev/null; then
    sudo apt-get install -y docker.io
    sudo systemctl start docker
    sudo systemctl enable docker
    sudo usermod -aG docker $USER
fi

# Install Docker Compose
echo "🐳 Installing Docker Compose..."
if ! command -v docker-compose &> /dev/null; then
    sudo apt-get install -y docker-compose
fi

# Navigate to project directory and install dependencies
cd /mnt/persist/workspace

# Install Rust dependencies (this will also validate Cargo.toml)
echo "📦 Installing Rust dependencies..."
cargo fetch

# Install Node.js dependencies
echo "📦 Installing Node.js dependencies..."
if [ -f "package.json" ]; then
    pnpm install
fi

# Build the project to ensure everything is working
echo "🔨 Building the project..."
cargo build

# Verify installation
echo "✅ Verifying installation..."
echo "Rust version: $(rustc --version)"
echo "Cargo version: $(cargo --version)"
echo "Node.js version: $(node --version)"
echo "pnpm version: $(pnpm --version)"
echo "Python version: $(python3 --version)"
echo "Docker version: $(docker --version)"

echo "🎉 Development environment setup complete!"
echo "💡 You may need to log out and back in for Docker group permissions to take effect."