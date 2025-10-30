# M5Gotchi Safe Pet - Flash Script
# Detecta automaticamente a porta COM e faz o upload

Write-Host "🐱 M5Gotchi Safe Pet - Flash Automático" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Detectar porta COM
Write-Host "🔍 Detectando portas COM..." -ForegroundColor Yellow
$ports = [System.IO.Ports.SerialPort]::getportnames()

if ($ports.Count -eq 0) {
    Write-Host "❌ Nenhuma porta COM detectada!" -ForegroundColor Red
    Write-Host ""
    Write-Host "📝 Conecte o M5Cardputer via USB-C e tente novamente." -ForegroundColor Yellow
    Write-Host ""
    pause
    exit 1
}

Write-Host "✅ Portas encontradas: $($ports -join ', ')" -ForegroundColor Green
Write-Host ""

# Se tiver múltiplas portas, perguntar
if ($ports.Count -gt 1) {
    Write-Host "🔢 Múltiplas portas detectadas. Escolha uma:" -ForegroundColor Yellow
    for ($i = 0; $i -lt $ports.Count; $i++) {
        Write-Host "  [$i] $($ports[$i])"
    }
    Write-Host ""
    $choice = Read-Host "Digite o número da porta"
    $selectedPort = $ports[$choice]
} else {
    $selectedPort = $ports[0]
}

Write-Host ""
Write-Host "📡 Usando porta: $selectedPort" -ForegroundColor Cyan
Write-Host ""

# Verificar se PlatformIO está instalado
Write-Host "🔧 Verificando PlatformIO..." -ForegroundColor Yellow
$pioInstalled = Get-Command pio -ErrorAction SilentlyContinue

if (-not $pioInstalled) {
    Write-Host "❌ PlatformIO não encontrado!" -ForegroundColor Red
    Write-Host ""
    Write-Host "📦 Opções:" -ForegroundColor Yellow
    Write-Host "  1. Instale PlatformIO: https://platformio.org/install/cli"
    Write-Host "  2. OU use M5Burner: https://m5burner.m5stack.com/"
    Write-Host "  3. OU use esptool: pip install esptool"
    Write-Host ""
    
    # Tentar esptool
    $esptoolInstalled = Get-Command esptool.py -ErrorAction SilentlyContinue
    
    if ($esptoolInstalled) {
        Write-Host "✅ esptool.py encontrado! Usando método alternativo..." -ForegroundColor Green
        Write-Host ""
        Write-Host "🔥 Flasheando firmware com esptool..." -ForegroundColor Cyan
        
        esptool.py --chip esp32s3 --port $selectedPort --baud 921600 `
          --before default_reset --after hard_reset write_flash -z `
          --flash_mode dio --flash_freq 80m --flash_size detect `
          0x0000 bin/bootloader.bin `
          0x8000 bin/partitions.bin `
          0x10000 bin/M5Gotchi_Safe_Pet.bin
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host ""
            Write-Host "✅ Flash concluído com sucesso!" -ForegroundColor Green
            Write-Host "🐱 Seu M5Gotchi Safe Pet está pronto!" -ForegroundColor Cyan
        } else {
            Write-Host ""
            Write-Host "❌ Erro durante o flash!" -ForegroundColor Red
        }
    } else {
        Write-Host "❌ Nenhuma ferramenta de flash encontrada!" -ForegroundColor Red
        Write-Host ""
        Write-Host "📥 Instale uma das opções acima e tente novamente." -ForegroundColor Yellow
    }
    
    Write-Host ""
    pause
    exit 1
}

# Fazer upload com PlatformIO
Write-Host "✅ PlatformIO encontrado!" -ForegroundColor Green
Write-Host ""
Write-Host "🔥 Flasheando firmware..." -ForegroundColor Cyan
Write-Host ""

pio run -e m5stack-cardputer -t upload --upload-port $selectedPort

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "✅ Flash concluído com sucesso!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "🐱 Seu M5Gotchi Safe Pet está pronto!" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "🎮 Controles:" -ForegroundColor Yellow
    Write-Host "  G = Menu"
    Write-Host "  ; = Cima"
    Write-Host "  . = Baixo"
    Write-Host "  Enter = Selecionar"
    Write-Host "  Space = Fazer carinho"
    Write-Host "  O = Alimentar"
    Write-Host ""
    Write-Host "🛡️ Este firmware é SEGURO (não salva senhas WiFi)" -ForegroundColor Green
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "❌ Erro durante o flash!" -ForegroundColor Red
    Write-Host ""
    Write-Host "💡 Dicas:" -ForegroundColor Yellow
    Write-Host "  - Verifique se o cabo USB está conectado"
    Write-Host "  - Tente segurar o botão G0 enquanto conecta"
    Write-Host "  - Tente outra porta USB"
    Write-Host ""
}

Write-Host ""
pause
