# 🐱 M5Gotchi PRO - Launcher Installation Package

## 📦 Conteúdo do Pacote

```
launcher_package/
├── M5Gotchi_PRO.bin      # Firmware principal (1.31MB)
├── bootloader.bin         # Bootloader ESP32-S3
├── partitions.bin         # Tabela de partições (huge_app)
├── M5Gotchi_PRO.json      # Metadados do firmware
└── README.md              # Este arquivo
```

---

## 🚀 INSTALAÇÃO VIA LAUNCHER (RECOMENDADO)

### Método 1: Browser do Launcher

1. **Abra o Cardputer Launcher**
   - Pressione o botão RESET
   - O Launcher deve aparecer na tela

2. **Acesse o Browser**
   - Navegue até: `Browser` → `Add Custom`

3. **Selecione o arquivo**
   - Escolha `M5Gotchi_PRO.bin`

4. **Instale**
   - Aguarde a instalação (±30 segundos)
   - Reinicie o Cardputer

### Método 2: Upload via SD Card

1. **Prepare o SD Card**
   - Formate como FAT32
   - Crie pasta: `/launcher/apps/`

2. **Copie o arquivo**
   - Copie `M5Gotchi_PRO.bin` para `/launcher/apps/`

3. **Instale via Launcher**
   - Abra Launcher → Browser
   - Selecione M5Gotchi PRO
   - Instale

---

## 🔧 INSTALAÇÃO MANUAL (se Launcher não funcionar)

### Usando esptool (Windows/Linux/Mac)

```bash
# 1. Apague a flash (IMPORTANTE!)
python -m esptool --port COM3 erase_flash

# 2. Grave o bootloader
python -m esptool --port COM3 --chip esp32s3 \
  --before default_reset --after hard_reset \
  write_flash --flash_mode dio --flash_freq 80m \
  0x0 bootloader.bin

# 3. Grave a tabela de partições
python -m esptool --port COM3 --chip esp32s3 \
  write_flash 0x8000 partitions.bin

# 4. Grave o firmware
python -m esptool --port COM3 --chip esp32s3 \
  write_flash 0x10000 M5Gotchi_PRO.bin

# 5. Pressione RESET no Cardputer
```

**⚠️ IMPORTANTE:** Substitua `COM3` pela porta correta do seu dispositivo!

### Usando PlatformIO (para desenvolvedores)

```bash
cd M5Gotchi_Pro_Project
pio run -e m5stack-cardputer --target upload
```

---

## ✨ RECURSOS

### 🔐 Segurança WiFi
- 🔍 Scanner de redes (canais, força, criptografia)
- 💥 Deauth Attack (desconexão de dispositivos)
- 🤝 Evil Twin Portal (AP falso com captive portal)
- 📡 Beacon Spam (inundação de APs falsos)
- 🎣 Handshake Capture (captura WPA/WPA2)
- 📊 Channel Analyzer (análise em tempo real)
- 📶 WiFi Analyzer (métricas detalhadas)

### 🔵 Hardware Hacking
- Bluetooth Scanner
- RF 433MHz Transceiver
- RFID/NFC Reader/Writer
- LoRa Messenger
- GPS Wardriving
- IR Universal Remote

### 🎮 Gamificação
- 🏆 Sistema de Conquistas (7 categorias)
- 📚 Tutorial Interativo (8 etapas)
- 🎮 Mini Games (Paw Scanner, Sushi SQL)
- 🔗 Detector de Pwnagotchi

### 🎨 Interface
- 🛡️ Controles Universais
- 🎨 Sistema de Temas
- ✨ Efeitos Visuais
- 📊 Dashboard Web
- 💾 Suporte SD Card

---

## 🎮 CONTROLES

| Ação | Controle |
|------|----------|
| **Navegar** | Teclado ↑↓←→ ou Trackball |
| **Selecionar** | ENTER ou Click do Trackball |
| **Voltar** | ESC ou Botão G0 |
| **Power** | Segure botão PWR |

---

## 📊 ESPECIFICAÇÕES TÉCNICAS

```
Dispositivo:    M5Stack Cardputer
Chip:           ESP32-S3 (240MHz)
RAM:            320KB (uso: 19.6% - 64KB)
Flash:          8MB (uso: 41.7% - 1.31MB)
Partições:      huge_app.csv (3.15MB app)
Compilador:     PlatformIO + Arduino 2.0.11
Libraries:      M5Unified 0.2.0, NimBLE, IRremote, etc.
```

---

## 🐛 TROUBLESHOOTING

### Travado na tela de loading
1. Tente resetar via Launcher
2. Reflashe o bootloader manualmente
3. Apague completamente a flash

### WiFi não funciona
- Verifique conexão da antena
- Reinicie o dispositivo

### Erro de SD Card
- Formate como FAT32
- Verifique conexão dos pinos

### Bluetooth falha
- Desative WiFi antes de scan BLE
- ESP32-S3 não suporta WiFi+BLE simultâneos

---

## 📚 DOCUMENTAÇÃO COMPLETA

Para mais informações, consulte:
- `README.md` - Guia principal
- `docs/` - Documentação detalhada
- GitHub: https://github.com/muylder/NekoGotchi_Project

---

## 🙏 CRÉDITOS

**M5Gotchi PRO** - NekoHacker Team  
Baseado em: Pwnagotchi, M5Cardputer, ESP32 Security Tools

---

## ⚠️ AVISO LEGAL

Este firmware é para **FINS EDUCACIONAIS APENAS**.  
Use apenas em redes que você possui ou tem permissão.  
O uso indevido é **ILEGAL** e de responsabilidade do usuário.

---

**🐱 Enjoy your NekoGotchi! Nya~ 😸**
