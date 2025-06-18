# Dark Server Release Build Script
# PowerShell script for building optimized release binaries

param(
    [string]$Target = "",
    [switch]$Clean = $false,
    [switch]$Test = $false,
    [switch]$Package = $false
)

Write-Host "🚀 Dark Server Release Build Script" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

# Clean previous builds if requested
if ($Clean) {
    Write-Host "🧹 Cleaning previous builds..." -ForegroundColor Yellow
    cargo clean
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ Clean failed!" -ForegroundColor Red
        exit 1
    }
    Write-Host "✅ Clean completed" -ForegroundColor Green
}

# Run tests if requested
if ($Test) {
    Write-Host "🧪 Running tests..." -ForegroundColor Yellow
    cargo test --release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ Tests failed!" -ForegroundColor Red
        exit 1
    }
    Write-Host "✅ All tests passed" -ForegroundColor Green
}

# Build the release binary
Write-Host "🔨 Building release binary..." -ForegroundColor Yellow

if ($Target) {
    Write-Host "📦 Building for target: $Target" -ForegroundColor Blue
    cargo build --release --target $Target
} else {
    cargo build --release
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Build completed successfully" -ForegroundColor Green

# Get binary information
$BinaryPath = if ($Target) {
    "target/$Target/release/dark-server.exe"
} else {
    "target/release/dark-server.exe"
}

if (Test-Path $BinaryPath) {
    $FileInfo = Get-Item $BinaryPath
    $SizeMB = [math]::Round($FileInfo.Length / 1MB, 2)
    Write-Host "📊 Binary size: $SizeMB MB" -ForegroundColor Blue
    Write-Host "📍 Binary location: $BinaryPath" -ForegroundColor Blue
} else {
    Write-Host "⚠️  Binary not found at expected location" -ForegroundColor Yellow
}

# Package release if requested
if ($Package) {
    Write-Host "📦 Creating release package..." -ForegroundColor Yellow
    
    $Version = (cargo metadata --format-version 1 --no-deps | ConvertFrom-Json).packages[0].version
    $PackageName = "dark-server-v$Version-windows-x64"
    $PackageDir = "releases/$PackageName"
    
    # Create package directory
    New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null
    
    # Copy binary
    Copy-Item $BinaryPath "$PackageDir/dark-server.exe"
    
    # Copy documentation
    Copy-Item "README.md" "$PackageDir/"
    Copy-Item "LICENSE" "$PackageDir/"
    Copy-Item "CHANGELOG.md" "$PackageDir/"
    Copy-Item "RUST_CONVERSION_SUMMARY.md" "$PackageDir/"
    
    # Create run script
    @"
@echo off
echo Starting Dark Server v$Version...
echo HTTP Server: http://localhost:8889
echo WebSocket Server: ws://localhost:9998
echo.
echo Press Ctrl+C to stop the server
echo.
dark-server.exe
pause
"@ | Out-File -FilePath "$PackageDir/start-server.bat" -Encoding ASCII
    
    # Create ZIP archive
    $ZipPath = "releases/$PackageName.zip"
    Compress-Archive -Path "$PackageDir/*" -DestinationPath $ZipPath -Force
    
    Write-Host "✅ Release package created: $ZipPath" -ForegroundColor Green
    
    # Show package contents
    Write-Host "📋 Package contents:" -ForegroundColor Blue
    Get-ChildItem $PackageDir | ForEach-Object {
        Write-Host "   - $($_.Name)" -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "🎉 Release build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Test the binary: $BinaryPath" -ForegroundColor White
Write-Host "  2. Verify HTTP server: http://localhost:8889" -ForegroundColor White  
Write-Host "  3. Verify WebSocket: ws://localhost:9998" -ForegroundColor White
Write-Host "  4. Check logs for proper startup" -ForegroundColor White

if ($Package) {
    Write-Host "  5. Distribute the package: releases/$PackageName.zip" -ForegroundColor White
}

Write-Host ""
Write-Host "Usage examples:" -ForegroundColor Cyan
Write-Host "  .\build-release.ps1 -Clean -Test -Package" -ForegroundColor White
Write-Host "  .\build-release.ps1 -Target x86_64-pc-windows-msvc" -ForegroundColor White
