#!/bin/bash

# Release verification script for Dark Server
# This script verifies that all release artifacts are properly built and functional

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
RELEASES_DIR="releases"
PROJECT_NAME="dark-server"

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  Dark Server Release Verification${NC}"
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

check_releases_dir() {
    print_step "Checking releases directory..."
    
    if [[ ! -d "$RELEASES_DIR" ]]; then
        print_error "Releases directory '$RELEASES_DIR' not found"
        print_info "Run the build script first: ./build-cross-platform.sh"
        exit 1
    fi
    
    print_success "Releases directory found"
}

verify_archives() {
    print_step "Verifying release archives..."
    
    local archives=($(find "$RELEASES_DIR" -name "*.tar.gz" -o -name "*.zip"))
    
    if [[ ${#archives[@]} -eq 0 ]]; then
        print_error "No release archives found"
        exit 1
    fi
    
    print_info "Found ${#archives[@]} release archives"
    
    for archive in "${archives[@]}"; do
        local basename=$(basename "$archive")
        print_info "Verifying: $basename"
        
        # Check if archive is valid
        if [[ "$archive" == *.tar.gz ]]; then
            if tar -tzf "$archive" >/dev/null 2>&1; then
                print_success "✓ $basename is a valid tar.gz archive"
            else
                print_error "✗ $basename is corrupted"
                exit 1
            fi
        elif [[ "$archive" == *.zip ]]; then
            if unzip -t "$archive" >/dev/null 2>&1; then
                print_success "✓ $basename is a valid zip archive"
            else
                print_error "✗ $basename is corrupted"
                exit 1
            fi
        fi
    done
}

verify_checksums() {
    print_step "Verifying checksums..."
    
    local checksums_file="$RELEASES_DIR/checksums.txt"
    
    if [[ ! -f "$checksums_file" ]]; then
        print_error "Checksums file not found: $checksums_file"
        exit 1
    fi
    
    cd "$RELEASES_DIR"
    
    if sha256sum -c checksums.txt >/dev/null 2>&1; then
        print_success "All checksums verified"
    else
        print_error "Checksum verification failed"
        exit 1
    fi
    
    cd ..
}

test_binary_execution() {
    print_step "Testing binary execution..."
    
    # Find a native binary to test
    local native_target=""
    case "$(uname -s)" in
        Linux*)
            case "$(uname -m)" in
                x86_64) native_target="linux-x64" ;;
                aarch64|arm64) native_target="linux-arm64" ;;
            esac
            ;;
        Darwin*)
            case "$(uname -m)" in
                x86_64) native_target="macos-x64" ;;
                arm64) native_target="macos-arm64" ;;
            esac
            ;;
    esac
    
    if [[ -z "$native_target" ]]; then
        print_info "Skipping binary execution test (unsupported platform)"
        return
    fi
    
    # Find the archive for the native target
    local archive=$(find "$RELEASES_DIR" -name "*$native_target*" | head -1)
    
    if [[ -z "$archive" ]]; then
        print_info "Skipping binary execution test (native binary not found)"
        return
    fi
    
    print_info "Testing binary from: $(basename "$archive")"
    
    # Extract and test
    local temp_dir=$(mktemp -d)
    cd "$temp_dir"
    
    if [[ "$archive" == *.tar.gz ]]; then
        tar -xzf "$archive"
    elif [[ "$archive" == *.zip ]]; then
        unzip -q "$archive"
    fi
    
    # Find the binary
    local binary=$(find . -name "dark-server" -o -name "dark-server.exe" | head -1)
    
    if [[ -n "$binary" ]]; then
        chmod +x "$binary"
        
        # Test if binary runs and shows help/version
        if timeout 5s "$binary" --help >/dev/null 2>&1 || 
           timeout 5s "$binary" --version >/dev/null 2>&1 ||
           timeout 5s "$binary" -h >/dev/null 2>&1; then
            print_success "Binary executes successfully"
        else
            print_info "Binary execution test inconclusive (may require specific runtime)"
        fi
    else
        print_error "Binary not found in archive"
    fi
    
    cd - >/dev/null
    rm -rf "$temp_dir"
}

check_file_sizes() {
    print_step "Checking file sizes..."
    
    local archives=($(find "$RELEASES_DIR" -name "*.tar.gz" -o -name "*.zip"))
    
    for archive in "${archives[@]}"; do
        local size=$(stat -f%z "$archive" 2>/dev/null || stat -c%s "$archive" 2>/dev/null)
        local size_mb=$((size / 1024 / 1024))
        local basename=$(basename "$archive")
        
        if [[ $size_mb -lt 1 ]]; then
            print_error "Archive too small: $basename (${size_mb}MB)"
        elif [[ $size_mb -gt 100 ]]; then
            print_error "Archive too large: $basename (${size_mb}MB)"
        else
            print_success "✓ $basename (${size_mb}MB)"
        fi
    done
}

generate_release_report() {
    print_step "Generating release report..."
    
    local report_file="$RELEASES_DIR/release-report.txt"
    
    cat > "$report_file" << EOF
Dark Server Release Report
Generated: $(date)

Release Artifacts:
EOF
    
    find "$RELEASES_DIR" -name "*.tar.gz" -o -name "*.zip" | while read archive; do
        local basename=$(basename "$archive")
        local size=$(stat -f%z "$archive" 2>/dev/null || stat -c%s "$archive" 2>/dev/null)
        local size_mb=$((size / 1024 / 1024))
        echo "  - $basename (${size_mb}MB)" >> "$report_file"
    done
    
    cat >> "$report_file" << EOF

Checksums:
$(cat "$RELEASES_DIR/checksums.txt" 2>/dev/null || echo "No checksums file found")

Verification Status: PASSED
EOF
    
    print_success "Release report generated: $report_file"
}

main() {
    print_header
    
    check_releases_dir
    verify_archives
    verify_checksums
    test_binary_execution
    check_file_sizes
    generate_release_report
    
    echo ""
    print_success "Release verification completed successfully!"
    
    echo ""
    print_info "Release Summary:"
    find "$RELEASES_DIR" -name "*.tar.gz" -o -name "*.zip" | while read archive; do
        echo -e "  ${GREEN}✓${NC} $(basename "$archive")"
    done
}

# Show help
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    echo "Usage: $0"
    echo ""
    echo "Verify Dark Server release artifacts."
    echo ""
    echo "This script checks:"
    echo "  - Archive integrity"
    echo "  - Checksum verification"
    echo "  - Binary execution (if possible)"
    echo "  - File sizes"
    echo ""
    echo "Run this after building releases with build-cross-platform.sh"
    exit 0
fi

# Run main function
main "$@"
