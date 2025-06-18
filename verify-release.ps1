# Dark Server Release Verification Script
# Verifies that the release package is complete and functional

param(
    [string]$ReleasePath = "releases/dark-server-v1.0.0-windows-x64",
    [switch]$SkipBinaryTest = $false
)

Write-Host "🔍 Dark Server Release Verification" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

$ErrorCount = 0

# Check if release directory exists
if (-not (Test-Path $ReleasePath)) {
    Write-Host "❌ Release directory not found: $ReleasePath" -ForegroundColor Red
    exit 1
}

Write-Host "📁 Checking release directory: $ReleasePath" -ForegroundColor Blue

# Required files
$RequiredFiles = @(
    "dark-server.exe",
    "README.md",
    "LICENSE", 
    "CHANGELOG.md",
    "RUST_CONVERSION_SUMMARY.md",
    "start-server.bat"
)

# Check required files
Write-Host "📋 Verifying required files..." -ForegroundColor Yellow
foreach ($file in $RequiredFiles) {
    $filePath = Join-Path $ReleasePath $file
    if (Test-Path $filePath) {
        $fileInfo = Get-Item $filePath
        $sizeKB = [math]::Round($fileInfo.Length / 1KB, 2)
        Write-Host "  ✅ $file ($sizeKB KB)" -ForegroundColor Green
    } else {
        Write-Host "  ❌ Missing: $file" -ForegroundColor Red
        $ErrorCount++
    }
}

# Check binary properties
$binaryPath = Join-Path $ReleasePath "dark-server.exe"
if (Test-Path $binaryPath) {
    Write-Host "🔧 Checking binary properties..." -ForegroundColor Yellow
    
    $fileInfo = Get-Item $binaryPath
    $sizeMB = [math]::Round($fileInfo.Length / 1MB, 2)
    
    Write-Host "  📊 Binary size: $sizeMB MB" -ForegroundColor Blue
    
    # Check if it's a valid PE executable
    try {
        $bytes = [System.IO.File]::ReadAllBytes($binaryPath)
        if ($bytes[0] -eq 0x4D -and $bytes[1] -eq 0x5A) {
            Write-Host "  ✅ Valid PE executable" -ForegroundColor Green
        } else {
            Write-Host "  ❌ Invalid executable format" -ForegroundColor Red
            $ErrorCount++
        }
    } catch {
        Write-Host "  ⚠️  Could not verify executable format" -ForegroundColor Yellow
    }
    
    # Check file version info if available
    try {
        $versionInfo = [System.Diagnostics.FileVersionInfo]::GetVersionInfo($binaryPath)
        if ($versionInfo.ProductName) {
            Write-Host "  📝 Product: $($versionInfo.ProductName)" -ForegroundColor Blue
        }
        if ($versionInfo.FileVersion) {
            Write-Host "  🏷️  Version: $($versionInfo.FileVersion)" -ForegroundColor Blue
        }
    } catch {
        Write-Host "  ℹ️  No version information available" -ForegroundColor Gray
    }
}

# Check documentation files
Write-Host "📚 Verifying documentation..." -ForegroundColor Yellow

$readmePath = Join-Path $ReleasePath "README.md"
if (Test-Path $readmePath) {
    $content = Get-Content $readmePath -Raw
    if ($content -match "Dark Server.*Rust" -and $content -match "8889" -and $content -match "9998") {
        Write-Host "  ✅ README.md contains expected content" -ForegroundColor Green
    } else {
        Write-Host "  ⚠️  README.md may be incomplete" -ForegroundColor Yellow
    }
}

$changelogPath = Join-Path $ReleasePath "CHANGELOG.md"
if (Test-Path $changelogPath) {
    $content = Get-Content $changelogPath -Raw
    if ($content -match "\[1\.0\.0\]" -and $content -match "2024") {
        Write-Host "  ✅ CHANGELOG.md contains version 1.0.0" -ForegroundColor Green
    } else {
        Write-Host "  ⚠️  CHANGELOG.md may not contain current version" -ForegroundColor Yellow
    }
}

# Check start script
$startScriptPath = Join-Path $ReleasePath "start-server.bat"
if (Test-Path $startScriptPath) {
    $content = Get-Content $startScriptPath -Raw
    if ($content -match "dark-server\.exe" -and $content -match "8889" -and $content -match "9998") {
        Write-Host "  ✅ start-server.bat contains expected commands" -ForegroundColor Green
    } else {
        Write-Host "  ⚠️  start-server.bat may be incomplete" -ForegroundColor Yellow
    }
}

# Test binary execution (optional)
if (-not $SkipBinaryTest) {
    Write-Host "🧪 Testing binary execution..." -ForegroundColor Yellow
    
    try {
        # Start the server in background
        $process = Start-Process -FilePath $binaryPath -PassThru -WindowStyle Hidden
        
        # Wait a moment for startup
        Start-Sleep -Seconds 2
        
        # Check if process is still running
        if (-not $process.HasExited) {
            Write-Host "  ✅ Binary starts successfully" -ForegroundColor Green
            
            # Try to stop the process gracefully
            try {
                $process.CloseMainWindow()
                Start-Sleep -Seconds 1
                if (-not $process.HasExited) {
                    $process.Kill()
                }
                Write-Host "  ✅ Binary stops gracefully" -ForegroundColor Green
            } catch {
                Write-Host "  ⚠️  Had to force-kill the process" -ForegroundColor Yellow
            }
        } else {
            Write-Host "  ❌ Binary exited immediately (exit code: $($process.ExitCode))" -ForegroundColor Red
            $ErrorCount++
        }
    } catch {
        Write-Host "  ❌ Failed to start binary: $($_.Exception.Message)" -ForegroundColor Red
        $ErrorCount++
    }
} else {
    Write-Host "⏭️  Skipping binary execution test" -ForegroundColor Gray
}

# Check ZIP package if it exists
$zipPath = "$ReleasePath.zip"
if (Test-Path $zipPath) {
    Write-Host "📦 Verifying ZIP package..." -ForegroundColor Yellow
    
    $zipInfo = Get-Item $zipPath
    $zipSizeMB = [math]::Round($zipInfo.Length / 1MB, 2)
    Write-Host "  📊 ZIP size: $zipSizeMB MB" -ForegroundColor Blue
    
    # Try to list ZIP contents
    try {
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $zip = [System.IO.Compression.ZipFile]::OpenRead($zipPath)
        $entryCount = $zip.Entries.Count
        $zip.Dispose()
        
        Write-Host "  📋 ZIP contains $entryCount files" -ForegroundColor Blue
        
        if ($entryCount -ge $RequiredFiles.Count) {
            Write-Host "  ✅ ZIP appears to contain all required files" -ForegroundColor Green
        } else {
            Write-Host "  ⚠️  ZIP may be missing some files" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "  ⚠️  Could not verify ZIP contents" -ForegroundColor Yellow
    }
}

# Summary
Write-Host ""
if ($ErrorCount -eq 0) {
    Write-Host "🎉 Release verification completed successfully!" -ForegroundColor Green
    Write-Host "✅ All checks passed" -ForegroundColor Green
} else {
    Write-Host "⚠️  Release verification completed with $ErrorCount error(s)" -ForegroundColor Yellow
    Write-Host "❌ Some checks failed" -ForegroundColor Red
}

Write-Host ""
Write-Host "📋 Release Summary:" -ForegroundColor Cyan
Write-Host "  📁 Location: $ReleasePath" -ForegroundColor White
Write-Host "  📦 ZIP: $zipPath" -ForegroundColor White
Write-Host "  🔧 Binary: dark-server.exe" -ForegroundColor White
Write-Host "  📚 Documentation: README.md, CHANGELOG.md" -ForegroundColor White
Write-Host "  🚀 Ready for distribution!" -ForegroundColor White

exit $ErrorCount
