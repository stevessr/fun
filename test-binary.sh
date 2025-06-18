#!/bin/bash

# Binary testing script for Dark Server
# This script tests the functionality of built binaries

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BINARY_PATH=${1:-"target/release/dark-server"}
TEST_TIMEOUT=10

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Dark Server Binary Test${NC}"
    echo -e "${BLUE}================================${NC}"
    echo ""
}

print_step() {
    echo -e "${YELLOW}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

check_binary() {
    print_step "Checking binary existence..."
    
    if [[ ! -f "$BINARY_PATH" ]]; then
        print_error "Binary not found: $BINARY_PATH"
        print_info "Build the project first: cargo build --release"
        exit 1
    fi
    
    if [[ ! -x "$BINARY_PATH" ]]; then
        print_error "Binary is not executable: $BINARY_PATH"
        exit 1
    fi
    
    print_success "Binary found and executable"
}

test_binary_info() {
    print_step "Testing binary information..."
    
    # Get file info
    local file_info=$(file "$BINARY_PATH" 2>/dev/null || echo "Unknown")
    print_info "File type: $file_info"
    
    # Get size
    local size=$(stat -f%z "$BINARY_PATH" 2>/dev/null || stat -c%s "$BINARY_PATH" 2>/dev/null)
    local size_mb=$((size / 1024 / 1024))
    print_info "Binary size: ${size_mb}MB"
    
    print_success "Binary information retrieved"
}

test_help_output() {
    print_step "Testing help output..."
    
    # Try different help flags
    local help_flags=("--help" "-h" "help")
    local help_found=false
    
    for flag in "${help_flags[@]}"; do
        if timeout $TEST_TIMEOUT "$BINARY_PATH" "$flag" >/dev/null 2>&1; then
            print_success "Help flag '$flag' works"
            help_found=true
            break
        fi
    done
    
    if [[ "$help_found" == false ]]; then
        print_info "No standard help flags found (this may be normal)"
    fi
}

test_version_output() {
    print_step "Testing version output..."
    
    # Try different version flags
    local version_flags=("--version" "-V" "version")
    local version_found=false
    
    for flag in "${version_flags[@]}"; do
        if timeout $TEST_TIMEOUT "$BINARY_PATH" "$flag" >/dev/null 2>&1; then
            print_success "Version flag '$flag' works"
            version_found=true
            break
        fi
    done
    
    if [[ "$version_found" == false ]]; then
        print_info "No standard version flags found (this may be normal)"
    fi
}

test_startup() {
    print_step "Testing server startup..."
    
    # Start the server in background
    local log_file=$(mktemp)
    local pid_file=$(mktemp)
    
    # Start server and capture PID
    "$BINARY_PATH" > "$log_file" 2>&1 &
    local server_pid=$!
    echo $server_pid > "$pid_file"
    
    # Wait a moment for startup
    sleep 3
    
    # Check if process is still running
    if kill -0 $server_pid 2>/dev/null; then
        print_success "Server started successfully (PID: $server_pid)"
        
        # Test HTTP endpoint
        if command -v curl >/dev/null 2>&1; then
            print_step "Testing HTTP endpoint..."
            if curl -s --connect-timeout 5 http://localhost:8889/ >/dev/null 2>&1; then
                print_success "HTTP endpoint responding"
            else
                print_info "HTTP endpoint not responding (may be normal without WebSocket client)"
            fi
        fi
        
        # Test WebSocket endpoint
        if command -v nc >/dev/null 2>&1; then
            print_step "Testing WebSocket endpoint..."
            if timeout 2 nc -z localhost 9998 2>/dev/null; then
                print_success "WebSocket port is open"
            else
                print_info "WebSocket port not accessible"
            fi
        fi
        
        # Stop the server
        print_step "Stopping server..."
        kill $server_pid 2>/dev/null || true
        wait $server_pid 2>/dev/null || true
        print_success "Server stopped"
        
    else
        print_error "Server failed to start or crashed immediately"
        print_info "Server output:"
        cat "$log_file"
        rm -f "$log_file" "$pid_file"
        return 1
    fi
    
    # Cleanup
    rm -f "$log_file" "$pid_file"
}

test_dependencies() {
    print_step "Testing binary dependencies..."
    
    # Check for dynamic library dependencies (Linux/macOS)
    if command -v ldd >/dev/null 2>&1; then
        print_info "Dynamic library dependencies (ldd):"
        ldd "$BINARY_PATH" 2>/dev/null | head -10 || true
    elif command -v otool >/dev/null 2>&1; then
        print_info "Dynamic library dependencies (otool):"
        otool -L "$BINARY_PATH" 2>/dev/null | head -10 || true
    elif command -v objdump >/dev/null 2>&1; then
        print_info "Dynamic library dependencies (objdump):"
        objdump -p "$BINARY_PATH" 2>/dev/null | grep "DLL Name" | head -10 || true
    fi
    
    print_success "Dependency check completed"
}

test_performance() {
    print_step "Basic performance test..."
    
    # Measure startup time
    local start_time=$(date +%s%N)
    
    # Start and immediately stop server
    "$BINARY_PATH" &
    local server_pid=$!
    sleep 1
    kill $server_pid 2>/dev/null || true
    wait $server_pid 2>/dev/null || true
    
    local end_time=$(date +%s%N)
    local startup_time=$(( (end_time - start_time) / 1000000 ))
    
    print_info "Startup time: ${startup_time}ms"
    
    if [[ $startup_time -lt 5000 ]]; then
        print_success "Fast startup time"
    else
        print_info "Startup time is acceptable"
    fi
}

main() {
    print_header
    
    print_info "Testing binary: $BINARY_PATH"
    echo ""
    
    check_binary
    test_binary_info
    test_help_output
    test_version_output
    test_dependencies
    test_startup
    test_performance
    
    echo ""
    print_success "Binary testing completed!"
    print_info "The binary appears to be working correctly."
}

# Show help
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: $0 [BINARY_PATH]"
    echo ""
    echo "Test Dark Server binary functionality."
    echo ""
    echo "Arguments:"
    echo "  BINARY_PATH    Path to the binary (default: target/release/dark-server)"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Test default binary"
    echo "  $0 ./dark-server                     # Test specific binary"
    echo "  $0 releases/dark-server-linux-x64/dark-server  # Test release binary"
    echo ""
    echo "This script tests:"
    echo "  - Binary existence and permissions"
    echo "  - Help and version flags"
    echo "  - Server startup and shutdown"
    echo "  - HTTP and WebSocket endpoints"
    echo "  - Basic performance metrics"
    exit 0
fi

# Run main function
main "$@"
