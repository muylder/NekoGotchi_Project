# Generate Doxygen Documentation
# Usage: .\generate_docs.ps1

Write-Host "📚 M5Gotchi Pro - Documentation Generator" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

# Check if Doxygen is installed
if (-not (Get-Command doxygen -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Doxygen not found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install Doxygen first:" -ForegroundColor Yellow
    Write-Host "  Option 1: Run .\install_doxygen.ps1 as Administrator" -ForegroundColor White
    Write-Host "  Option 2: Download from https://www.doxygen.nl/download.html" -ForegroundColor White
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

# Get Doxygen version
$doxygenVersion = doxygen --version
Write-Host "✓ Doxygen version: $doxygenVersion" -ForegroundColor Green
Write-Host ""

# Check if Doxyfile exists
if (-not (Test-Path "Doxyfile")) {
    Write-Host "❌ Doxyfile not found in current directory!" -ForegroundColor Red
    Write-Host "   Make sure you're in the project root." -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "✓ Found Doxyfile" -ForegroundColor Green
Write-Host ""

# Clean previous output
if (Test-Path "docs\doxygen") {
    Write-Host "🧹 Cleaning previous output..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "docs\doxygen" -ErrorAction SilentlyContinue
    Write-Host "✓ Cleaned" -ForegroundColor Green
    Write-Host ""
}

# Generate documentation
Write-Host "⚙️  Generating documentation..." -ForegroundColor Yellow
Write-Host ""
Write-Host "This may take 30-60 seconds..." -ForegroundColor Gray
Write-Host ""

$startTime = Get-Date

try {
    # Run Doxygen and capture output
    doxygen Doxyfile 2>&1 | Tee-Object -Variable output
    
    $endTime = Get-Date
    $duration = ($endTime - $startTime).TotalSeconds
    
    Write-Host ""
    Write-Host "=========================================" -ForegroundColor Cyan
    Write-Host "✅ Documentation Generated Successfully!" -ForegroundColor Green
    Write-Host "=========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "⏱️  Generation time: $([math]::Round($duration, 2)) seconds" -ForegroundColor White
    Write-Host ""
    
    # Count files
    $htmlFiles = (Get-ChildItem -Path "docs\doxygen\html" -Filter "*.html" -Recurse).Count
    Write-Host "📄 HTML files generated: $htmlFiles" -ForegroundColor White
    
    # Get output size
    $outputSize = (Get-ChildItem -Path "docs\doxygen" -Recurse | Measure-Object -Property Length -Sum).Sum
    $outputSizeMB = [math]::Round($outputSize / 1MB, 2)
    Write-Host "💾 Output size: $outputSizeMB MB" -ForegroundColor White
    Write-Host ""
    
    # Parse warnings
    $warnings = ($output | Select-String -Pattern "warning:").Count
    if ($warnings -gt 0) {
        Write-Host "⚠️  $warnings warnings found (see output above)" -ForegroundColor Yellow
    } else {
        Write-Host "✓ No warnings" -ForegroundColor Green
    }
    
    Write-Host ""
    Write-Host "📂 Output location:" -ForegroundColor Cyan
    Write-Host "   $(Resolve-Path 'docs\doxygen\html')" -ForegroundColor White
    Write-Host ""
    
    # Ask to open
    Write-Host "Would you like to open the documentation in your browser? (Y/N): " -ForegroundColor Cyan -NoNewline
    $response = Read-Host
    
    if ($response -eq "Y" -or $response -eq "y") {
        Write-Host ""
        Write-Host "🌐 Opening documentation..." -ForegroundColor Yellow
        Start-Process "docs\doxygen\html\index.html"
    }
    
} catch {
    Write-Host ""
    Write-Host "❌ Documentation generation failed!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Error: $_" -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "📖 Quick Links:" -ForegroundColor Cyan
Write-Host "   Main Page:    docs\doxygen\html\index.html" -ForegroundColor White
Write-Host "   Classes:      docs\doxygen\html\annotated.html" -ForegroundColor White
Write-Host "   Files:        docs\doxygen\html\files.html" -ForegroundColor White
Write-Host "   Architecture: docs\ARCHITECTURE.md" -ForegroundColor White
Write-Host "   API Ref:      docs\API_REFERENCE.md" -ForegroundColor White
Write-Host ""
Write-Host "✨ Done!" -ForegroundColor Green
Write-Host ""
