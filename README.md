# 🚀 M5Gotchi Pro - Ultimate WiFi Pentest Suite

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![M5Stack](https://img.shields.io/badge/device-M5Stack-red.svg)](https://m5stack.com/)

**M5Gotchi Pro** é uma suite completa de pentest WiFi para M5Stack com recursos avançados de captura de handshakes, clone de redes com deauth automático e evil portal customizável.

## 🎮 Versões Disponíveis

### M5Stack Core/Fire/Core2 - 3-Button Interface
- **Controls:** A (Up/Toggle), B (Confirm/Menu), C (Down/Back)
- **Display:** 320x240 pixels
- **Documentation:** [English](docs/M5STACK_GUIDE_EN.md) | [Português](docs/M5STACK_GUIDE_PT.md)

### M5Stack Cardputer ⌨️ - QWERTY Keyboard  
- **Controls:** Arrow keys (↑/↓), ENTER (Confirm), ESC (Back)
- **Display:** 240x135 pixels (optimized)
- **Documentation:** [English](docs/CARDPUTER_GUIDE_EN.md) | [Português](docs/CARDPUTER_GUIDE_PT.md)

## 🎯 Simple Navigation System

**No complex key combinations!** All modes use simple, consistent controls:

**Cardputer:**
- ↑/↓ arrows = Navigate
- ENTER = Select/Execute  
- ESC = Back to menu

**M5Stack:**
- Button A/C = Navigate
- Button B = Select/Execute/Back

## 🚀 Quick Start

```bash
# M5Stack Core/Fire/Core2
pio run -e m5stack-core-esp32 --target upload

# M5Stack Cardputer (teclado QWERTY)
pio run -e m5stack-cardputer --target upload

# Ou compilar todos
./build.sh all
```

## 📦 Estrutura do Projeto

```
M5Gotchi_Pro_Project/
├── src/
│   ├── main.cpp              # Código M5Stack Core
│   └── main_cardputer.cpp    # Código Cardputer (otimizado)
├── data/portal_templates/    # Templates HTML
├── bin/                      # Binários compilados
├── docs/
│   ├── PRO_GUIDE.md          # Guia M5Stack
│   └── CARDPUTER_GUIDE.md    # Guia Cardputer
├── platformio.ini            # Config PlatformIO
├── Makefile                  # Build automation
└── build.sh                  # Script compilação
```

## ⚡ Recursos

- 🎯 **Handshake Capture** otimizado (750% mais eficiente)
- 🔄 **Clone & Deauth** automático ao clonar rede
- 👹 **Evil Portal** com HTML customizável do SD Card
- 📦 4 templates prontos (Netflix, Facebook, Google)

## 📖 Documentação

### Quick Reference Guides (Recommended)
- 📘 [M5Stack Guide (English)](docs/M5STACK_GUIDE_EN.md)
- 📘 [M5Stack Guide (Português)](docs/M5STACK_GUIDE_PT.md)
- ⌨️ [Cardputer Guide (English)](docs/CARDPUTER_GUIDE_EN.md) 
- ⌨️ [Cardputer Guide (Português)](docs/CARDPUTER_GUIDE_PT.md)

### Detailed Documentation
- [Complete PRO Guide](docs/PRO_GUIDE.md)
- [Cardputer Full Guide](docs/CARDPUTER_GUIDE.md)
- [Quick Start](docs/QUICKSTART.md)
- [Comparisons](docs/COMPARISON.md)

## ⚠️ Aviso Legal

**USO EXCLUSIVAMENTE EDUCACIONAL**
Apenas para testes em suas próprias redes ou com autorização.
Uso não autorizado é CRIME.

---

**Hack responsibly! 🛡️**
