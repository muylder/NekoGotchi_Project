# 📦 M5Gotchi - Binários para Flash

Esta pasta contém os binários compilados do M5Gotchi prontos para flash.

## 🐱 M5Gotchi Safe Pet (RECOMENDADO)

**Versão segura do pet virtual SEM recursos de pentesting**

- ✅ NÃO salva senhas WiFi
- ✅ NÃO registra credenciais
- ✅ Seguro para uso doméstico
- 🐱 Pet animado kawaii
- 📡 Scanner WiFi (somente passivo)

**Arquivos:**
- `M5Gotchi_Safe_Pet.bin` (811 KB) - Firmware principal
- `bootloader.bin` (13.7 KB) - Bootloader ESP32-S3
- `partitions.bin` (3 KB) - Tabela de partições
- `M5Gotchi_Safe_Pet.json` - Metadados para M5Launcher

---

## 🚀 Instalação Rápida

### Opção 1: Script Automático (Mais Fácil!)

```powershell
# Execute no PowerShell
.\flash_safe_pet.ps1
```

O script vai:
1. Auto-detectar a porta COM
2. Verificar ferramentas de flash (PlatformIO ou esptool)
3. Flashear o firmware
4. Mostrar os controles

### Opção 2: Flash Manual com PlatformIO

```bash
# Detectar porta
[System.IO.Ports.SerialPort]::getportnames()

# Flash (substitua COM3 pela sua porta)
pio run -e m5stack-cardputer -t upload --upload-port COM3
```

### Opção 3: M5Burner (Interface Gráfica)

1. Baixe o M5Burner: https://m5burner.m5stack.com/
2. Conecte o M5Cardputer via USB-C
3. Adicione firmware customizado:
   - `0x0000` → `bootloader.bin`
   - `0x8000` → `partitions.bin`
   - `0x10000` → `M5Gotchi_Safe_Pet.bin`
4. Clique em "Burn"!

### Opção 4: esptool (Linha de Comando)

```bash
esptool.py --chip esp32s3 --port COM3 --baud 921600 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 80m --flash_size detect \
  0x0000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 M5Gotchi_Safe_Pet.bin
```

---

## 📚 Documentação

- **INSTALLATION_GUIDE.md** - Guia detalhado de instalação
- **M5Gotchi_Safe_Pet.json** - Configuração para M5Launcher
- **flash_safe_pet.ps1** - Script automático de flash

---

## 📊 Especificações Técnicas

| Especificação | Valor |
|---------------|-------|
| MCU | ESP32-S3 (240MHz) |
| RAM | 320 KB (14.3% usado) |
| Flash | 3 MB (26.4% usado) |
| Tamanho Firmware | ~830 KB |
| Framework | Arduino-ESP32 2.0.11 |
| Bibliotecas | M5Unified 0.2.10, M5Cardputer 1.1.1 |

---

## 🔧 Compilar do Código-Fonte

Para regenerar os binários:

```bash
# Limpeza
pio run -e m5stack-cardputer -t clean

# Compilar
pio run -e m5stack-cardputer

# Binários estarão em:
# .pio/build/m5stack-cardputer/firmware.bin
# .pio/build/m5stack-cardputer/bootloader.bin
# .pio/build/m5stack-cardputer/partitions.bin
```

---

## 🛡️ Aviso de Segurança

O firmware **Safe Pet** incluído aqui:
- ✅ NÃO salva senhas WiFi
- ✅ NÃO registra credenciais
- ✅ NÃO inclui ferramentas de pentesting
- ✅ Seguro para uso pessoal/doméstico

Para recursos avançados (captura de handshakes, etc.), compile o `main_cardputer.cpp` - mas use **APENAS em redes autorizadas!**

---

## 🎮 Controles

| Tecla | Ação |
|-------|------|
| **G** | Menu |
| **;** | Cima ▲ |
| **.** | Baixo ▼ |
| **Enter** | Selecionar ✅ |
| **Space** | Fazer carinho 🐾 |
| **O** | Alimentar 🍕 |

---

## 🐱 Features do Pet

- **Sistema de Status:**
  - 🍕 Fome (diminui a cada 30s)
  - 💗 Felicidade (diminui a cada 45s)
  - ⚡ Energia (diminui a cada 60s)

- **Progressão:**
  - 🏆 XP e Level
  - 🎨 Sprite do gato 3x maior
  - 📡 Scanner WiFi passivo

- **Tema Kawaii:**
  - 💗 Rosa: `0xFD20`
  - 🩵 Ciano: `0x07FF`
  - 🌙 Fundo: `0x0010`

---

## 🛠️ Troubleshooting

### Dispositivo não detectado
- Instale o driver CH340/CP2102
- Verifique se o cabo USB suporta dados
- Teste outras portas USB

### Erro durante o flash
- Segure o botão G0 ao conectar
- Tente uma taxa de baud menor: `--baud 115200`
- Use outra porta USB

### Porta COM não aparece
```powershell
# Reinstale drivers:
# https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
```

---

## 📞 Suporte

- GitHub: https://github.com/muylder/NekoGotchi_Project
- Issues: https://github.com/muylder/NekoGotchi_Project/issues
- Documentação: [../docs/](../docs/)

---

**Aproveite seu pet virtual! 🐾✨**

*Compiled binaries ready to flash - Safe version without pentesting features*
