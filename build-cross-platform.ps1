# Cross-platform build script for Dark Server (PowerShell)
# This script builds the project for multiple platforms and architectures

param(
    [string]$Version = "dev",
    [switch]$Help
)

# Configuration
$ProjectName = "dark-server"
$OutputDir = "releases"

# Supported targets
$Targets = @(
    "x86_64-unknown-linux-gnu",
    "aarch64-unknown-linux-gnu", 
    "x86_64-apple-darwin",
    "aarch64-apple-darwin",
    "x86_64-pc-windows-msvc",
    "aarch64-pc-windows-msvc"
)

# Platform-specific settings
$PlatformNames = @{
    "x86_64-unknown-linux-gnu" = "linux-x64"
    "aarch64-unknown-linux-gnu" = "linux-arm64"
    "x86_64-apple-darwin" = "macos-x64"
    "aarch64-apple-darwin" = "macos-arm64"
    "x86_64-pc-windows-msvc" = "windows-x64"
    "aarch64-pc-windows-msvc" = "windows-arm64"
}

$BinaryNames = @{
    "x86_64-unknown-linux-gnu" = "dark-server"
    "aarch64-unknown-linux-gnu" = "dark-server"
    "x86_64-apple-darwin" = "dark-server"
    "aarch64-apple-darwin" = "dark-server"
    "x86_64-pc-windows-msvc" = "dark-server.exe"
    "aarch64-pc-windows-msvc" = "dark-server.exe"
}

function Write-Header {
    Write-Host "================================" -ForegroundColor Blue
    Write-Host "  Dark Server Cross-Platform Build" -ForegroundColor Blue
    Write-Host "================================" -ForegroundColor Blue
    Write-Host ""
}

function Write-Step {
    param([string]$Message)
    Write-Host "[STEP] $Message" -ForegroundColor Yellow
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor Green
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Blue
}

function Test-Dependencies {
    Write-Step "Checking dependencies..."
    
    # Check if Rust is installed
    if (-not (Get-Command cargo -ErrorAction SilentlyContinue)) {
        Write-Error "Cargo not found. Please install Rust: https://rustup.rs/"
        exit 1
    }
    
    # Check if cross is installed for cross-compilation
    if (-not (Get-Command cross -ErrorAction SilentlyContinue)) {
        Write-Info "Installing cross for cross-compilation..."
        cargo install cross --git https://github.com/cross-rs/cross
    }
    
    Write-Success "Dependencies checked"
}

function Install-Targets {
    Write-Step "Installing Rust targets..."
    
    foreach ($target in $Targets) {
        Write-Info "Installing target: $target"
        rustup target add $target 2>$null
    }
    
    Write-Success "Targets installed"
}

function Build-Target {
    param([string]$Target)
    
    $PlatformName = $PlatformNames[$Target]
    $BinaryName = $BinaryNames[$Target]
    
    Write-Step "Building for $Target ($PlatformName)..."
    
    # Use cross for cross-compilation, cargo for native builds
    $BuildCmd = "cargo"
    $HostTarget = (rustc -vV | Select-String "host:" | ForEach-Object { $_.ToString().Split(": ")[1] })
    if ($Target -ne $HostTarget) {
        $BuildCmd = "cross"
    }
    
    # Build the project
    $BuildResult = & $BuildCmd build --release --target $Target
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Built successfully for $Target"
        
        # Create output directory
        $OutputPath = Join-Path $OutputDir "$ProjectName-v$Version-$PlatformName"
        New-Item -ItemType Directory -Path $OutputPath -Force | Out-Null
        
        # Copy binary
        $SourceBinary = Join-Path "target" $Target "release" $BinaryName
        Copy-Item $SourceBinary $OutputPath
        
        # Copy additional files
        if (Test-Path "README.md") {
            Copy-Item "README.md" $OutputPath
        }
        if (Test-Path "LICENSE") {
            Copy-Item "LICENSE" $OutputPath
        }
        if (Test-Path "CHANGELOG.md") {
            Copy-Item "CHANGELOG.md" $OutputPath
        }
        
        # Create archive
        Push-Location $OutputDir
        if ($Target -like "*windows*") {
            Compress-Archive -Path "$ProjectName-v$Version-$PlatformName" -DestinationPath "$ProjectName-v$Version-$PlatformName.zip" -Force
        } else {
            # Use tar if available, otherwise use Compress-Archive
            if (Get-Command tar -ErrorAction SilentlyContinue) {
                tar -czf "$ProjectName-v$Version-$PlatformName.tar.gz" "$ProjectName-v$Version-$PlatformName"
            } else {
                Compress-Archive -Path "$ProjectName-v$Version-$PlatformName" -DestinationPath "$ProjectName-v$Version-$PlatformName.zip" -Force
            }
        }
        Pop-Location
        
        Write-Success "Archive created for $PlatformName"
        return $true
    } else {
        Write-Error "Failed to build for $Target"
        return $false
    }
}

function Build-All {
    Write-Step "Building for all targets..."
    
    # Clean previous builds
    if (Test-Path $OutputDir) {
        Remove-Item $OutputDir -Recurse -Force
    }
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    
    $FailedBuilds = @()
    
    foreach ($target in $Targets) {
        if (-not (Build-Target $target)) {
            $FailedBuilds += $target
        }
        Write-Host ""
    }
    
    # Report results
    Write-Host "================================" -ForegroundColor Blue
    Write-Host "  Build Summary" -ForegroundColor Blue
    Write-Host "================================" -ForegroundColor Blue
    
    if ($FailedBuilds.Count -eq 0) {
        Write-Success "All builds completed successfully!"
    } else {
        Write-Error "Some builds failed:"
        foreach ($target in $FailedBuilds) {
            Write-Host "  ✗ $target" -ForegroundColor Red
        }
    }
    
    Write-Host ""
    Write-Info "Build artifacts are in the '$OutputDir' directory"
    Get-ChildItem $OutputDir
}

function New-Checksums {
    Write-Step "Generating checksums..."
    
    Push-Location $OutputDir
    $Files = Get-ChildItem -Filter "*.tar.gz", "*.zip"
    $Checksums = @()
    
    foreach ($file in $Files) {
        $Hash = Get-FileHash $file.Name -Algorithm SHA256
        $Checksums += "$($Hash.Hash.ToLower())  $($file.Name)"
    }
    
    $Checksums | Out-File -FilePath "checksums.txt" -Encoding UTF8
    Pop-Location
    
    Write-Success "Checksums generated"
}

function Show-Help {
    Write-Host "Usage: .\build-cross-platform.ps1 [-Version <VERSION>] [-Help]"
    Write-Host ""
    Write-Host "Build Dark Server for multiple platforms and architectures."
    Write-Host ""
    Write-Host "Parameters:"
    Write-Host "  -Version    Version string (default: 'dev')"
    Write-Host "  -Help       Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build-cross-platform.ps1                # Build with version 'dev'"
    Write-Host "  .\build-cross-platform.ps1 -Version 1.0.0 # Build with version '1.0.0'"
    Write-Host ""
    Write-Host "Supported platforms:"
    foreach ($target in $Targets) {
        $platformName = $PlatformNames[$target]
        Write-Host "  - $target ($platformName)"
    }
}

function Main {
    if ($Help) {
        Show-Help
        return
    }
    
    Write-Header
    
    Test-Dependencies
    Install-Targets
    Build-All
    New-Checksums
    
    Write-Host ""
    Write-Success "Cross-platform build completed!"
    Write-Host "Version: $Version" -ForegroundColor Blue
    Write-Host "Output: $OutputDir" -ForegroundColor Blue
}

# Run main function
Main
