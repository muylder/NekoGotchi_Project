# Install Doxygen on Windows
# Run as Administrator: Set-ExecutionPolicy Bypass -Scope Process -Force; .\install_doxygen.ps1

Write-Host "🔧 M5Gotchi Pro - Doxygen Installation Script" -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as Administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Host "⚠️  This script needs Administrator privileges!" -ForegroundColor Yellow
    Write-Host "   Please right-click PowerShell and 'Run as Administrator'" -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "✓ Running with Administrator privileges" -ForegroundColor Green
Write-Host ""

# Check if Chocolatey is installed
Write-Host "📦 Checking for Chocolatey package manager..." -ForegroundColor Yellow

if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "   Chocolatey not found. Installing..." -ForegroundColor Yellow
    
    # Install Chocolatey
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    
    try {
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
        Write-Host "✓ Chocolatey installed successfully!" -ForegroundColor Green
    } catch {
        Write-Host "❌ Failed to install Chocolatey" -ForegroundColor Red
        Write-Host "   Please install manually from: https://chocolatey.org/install" -ForegroundColor Yellow
        Read-Host "Press Enter to exit"
        exit 1
    }
} else {
    Write-Host "✓ Chocolatey already installed" -ForegroundColor Green
}

Write-Host ""

# Install Doxygen
Write-Host "📚 Installing Doxygen..." -ForegroundColor Yellow

try {
    choco install doxygen.install -y
    Write-Host "✓ Doxygen installed successfully!" -ForegroundColor Green
} catch {
    Write-Host "❌ Failed to install Doxygen" -ForegroundColor Red
    Write-Host "   Try installing manually from: https://www.doxygen.nl/download.html" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""

# Optional: Install Graphviz for diagrams
Write-Host "📊 Installing Graphviz (for diagrams)..." -ForegroundColor Yellow

try {
    choco install graphviz -y
    Write-Host "✓ Graphviz installed successfully!" -ForegroundColor Green
} catch {
    Write-Host "⚠️  Graphviz installation failed (optional)" -ForegroundColor Yellow
}

Write-Host ""

# Refresh environment variables
Write-Host "🔄 Refreshing environment variables..." -ForegroundColor Yellow
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

Write-Host ""
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host "✅ Installation Complete!" -ForegroundColor Green
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host ""

# Verify installation
Write-Host "📋 Verification:" -ForegroundColor Cyan
Write-Host ""

if (Get-Command doxygen -ErrorAction SilentlyContinue) {
    $version = doxygen --version
    Write-Host "✓ Doxygen version: $version" -ForegroundColor Green
} else {
    Write-Host "⚠️  Doxygen not found in PATH. You may need to:" -ForegroundColor Yellow
    Write-Host "   1. Close and reopen PowerShell" -ForegroundColor Yellow
    Write-Host "   2. Or restart your computer" -ForegroundColor Yellow
}

if (Get-Command dot -ErrorAction SilentlyContinue) {
    Write-Host "✓ Graphviz installed (diagrams enabled)" -ForegroundColor Green
} else {
    Write-Host "⚠️  Graphviz not in PATH (diagrams disabled)" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "📖 Next Steps:" -ForegroundColor Cyan
Write-Host "   1. Close this PowerShell window" -ForegroundColor White
Write-Host "   2. Open a NEW PowerShell window" -ForegroundColor White
Write-Host "   3. Run: cd 'C:\Users\andre\Gotchi_mod\M5Gotchi_Pro_Project'" -ForegroundColor White
Write-Host "   4. Run: .\generate_docs.ps1" -ForegroundColor White
Write-Host ""

Read-Host "Press Enter to exit"
