# � M5Gotchi Pro - Educational Pentesting Tamagotchi

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue.svg)](https://www.espressif.com/en/products/socs/esp32-s3)
[![M5Stack](https://img.shields.io/badge/device-M5Stack_Cardputer-red.svg)](https://m5stack.com/)
[![Version](https://img.shields.io/badge/version-2.0-green.svg)](https://github.com/muylder/NekoGotchi_Project)
[![Documentation](https://img.shields.io/badge/docs-Doxygen-orange.svg)](docs/DOCUMENTATION_GUIDE.md)

**M5Gotchi Pro** is a comprehensive educational pentesting suite for M5Stack Cardputer, combining security research tools with gamification and a kawaii virtual pet companion. Learn WiFi/BLE/RFID security through interactive achievements, mini-games, and hands-on experimentation in a safe environment.

## ✨ What's New in v2.0

### 🎯 Major Updates (Oct 2025)

- **✅ Memory Optimization System** - Saved 34KB RAM with PROGMEM, Object Pooling, Circular Buffers
- **✅ Achievement System v2.0** - Event-driven O(1) architecture, separated UI/logic (CC < 10)
- **✅ Interactive Tutorial System** - 9-step wizard, 20+ contextual helps, multi-language (EN/PT-BR)
- **✅ RFID/NFC Multi-Tool** - PN532/RC522/RDM6300 support, dictionary attacks, cloning
- **✅ Chameleon Ultra Integration** - Serial/BLE interface, 8 HF + 8 LF slots, emulation
- **✅ Comprehensive Documentation** - Full Doxygen API docs, 5 Mermaid diagrams, architecture guide

### 📊 Performance Improvements

| Metric | Before | After | Improvement |
|--------|--------|-------|-------------|
| RAM Usage | 210KB (64%) | 176KB (54%) | -34KB (-16%) |
| Heap Fragmentation | ~45% | ~25% | -44% |
| Achievement CC | 120 | <10 | -92% |
| Event Lookup | O(n) | O(1) | ∞ faster |
| String Storage | RAM | Flash | 14KB saved |

**M5Gotchi Pro** é uma suite educacional completa de pentesting para M5Stack Cardputer, combinando ferramentas de pesquisa de segurança com gamificação e um companheiro virtual pet kawaii.

## 🎮 Supported Devices

### M5Stack Cardputer ⌨️ (Primary Platform)
- **MCU:** ESP32-S3 (Dual-core 240MHz, 8MB PSRAM, 16MB Flash)
- **Display:** 240x135 LCD
- **Input:** Full QWERTY keyboard + Arrow keys
- **Expansion:** Grove ports, microSD slot
- **Documentation:** [Cardputer Guide](docs/CARDPUTER_GUIDE.md) | [Quick Start](docs/CARDPUTER_QUICKSTART.md)

### M5Stack Core/Fire/Core2 (Legacy Support)
- **MCU:** ESP32 Classic
- **Display:** 320x240 LCD
- **Input:** 3-button interface (A/B/C)
- **Documentation:** [M5Stack Guide EN](docs/M5STACK_GUIDE_EN.md) | [PT](docs/M5STACK_GUIDE_PT.md)

## 🎯 Features

### 🔐 Security Tools

**WiFi Arsenal:**
- 📡 Advanced WiFi Scanner with channel analyzer
- 💥 Deauth Attack (broadcast & targeted)
- 🤝 Handshake Capture with EAPOL filtering
- 🎣 Evil Portal (4 templates: Facebook, Google, Netflix, Custom)
- 📊 Real-time packet statistics
- 💾 PCAP export to SD card

**Bluetooth Tools:**
- 📱 BLE Spam Attacks (Sour Apple, Samsung, Google, Microsoft)
- 🔍 BLE Scanner with device discovery
- 📊 Advertisement packet analysis

**RFID/NFC Multi-Tool:**
- 🔍 Scanner (HF 13.56MHz + LF 125kHz)
- 📖 Reader (Mifare Classic/Ultralight, NTAG, EM410x)
- ✏️ Writer with dump support
- 🔄 Cloner (3-step process)
- 🔬 Analyzer (card detection, features)
- ⚔️ Attacks (Dictionary, Darkside, Nested)
- 🎴 Chameleon Ultra integration (Serial/BLE)

**Advanced Features:**
- 🚗 GPS Wardriving with KML export
- 📻 RF433/SubGHz transceiver
- 📡 IR universal remote
- 🔐 Chameleon Ultra interface (8 HF + 8 LF slots)

### � Gamification

**Achievement System v2.0:**
- 🏆 50+ achievements across 7 categories
- 📊 Event-driven tracking (O(1) lookup)
- 💾 Persistent progress saving
- 🎯 Rarity system (Common → Legendary)
- 📈 Statistics dashboard
- 🔔 Real-time unlock notifications

**Educational Mini-Games:**
- 🍣 Sushi SQL Injection Game (learn SQL injection)
- 🐾 Paw Scanner (pattern matching)
- 🎮 Arcade Games Collection
- 🎯 Score tracking & leaderboards

**Kawaii Features:**
- 🐱 Virtual Pet Companion (6 evolution stages)
- 💬 Nekogram Chat (P2P encrypted messaging)
- 🎵 Neko Sound System (7 personalities)
- 🌸 Kawaii Social Network (profiles, leaderboards)
- ✨ Visual effects & animations

### 📚 Learning & Help

**Interactive Tutorial System:**
- 📖 9-step first-run wizard
- 💡 Contextual help (press ? key)
- 🎯 Auto-hints on first visit
- 🌍 Multi-language (EN/PT-BR)
- 💾 Progress tracking

**Documentation:**
- 📐 Architecture diagrams (5 Mermaid charts)
- 📚 Complete API reference (40+ examples)
- 🔧 Memory optimization guide
- 🎓 Doxygen HTML documentation
- 🚀 Quick start guides

## 🚀 Quick Start

### Prerequisites

- **Hardware:** M5Stack Cardputer
- **Software:** PlatformIO (VSCode extension) or Arduino IDE
- **Optional:** microSD card (for logs/captures), RFID reader module

### Installation

**Method 1: PlatformIO (Recommended)**

```bash
# Clone repository
git clone https://github.com/muylder/NekoGotchi_Project.git
cd M5Gotchi_Pro_Project

# Install dependencies
pio pkg install

# Build and upload for Cardputer
pio run -e m5stack-cardputer --target upload

# Monitor serial output
pio device monitor -b 115200
```

**Method 2: Arduino IDE**

1. Install M5Unified library via Library Manager
2. Install NimBLE-Arduino library
3. Open `src/main_cardputer.cpp`
4. Select board: "ESP32S3 Dev Module"
5. Configure: 8MB Flash, 8MB PSRAM
6. Upload

### First Run

1. **Tutorial Wizard** will start automatically
2. Follow 9 interactive steps
3. Learn controls and navigation
4. Complete to unlock all features

### Controls

**M5Stack Cardputer:**
- `↑/↓` - Navigate menus
- `ENTER` - Select/Confirm
- `ESC` - Back/Cancel
- `?` - Show contextual help
- `TAB` - Switch modes/views
- `R` - Refresh/Rescan

**Universal Actions:**
- All modules use consistent controls
- Help available anytime (press `?`)
- Tutorial can be replayed from settings

## 📂 Project Structure

```
M5Gotchi_Pro_Project/
├── src/                                    # Source code
│   ├── main_cardputer.cpp                  # Cardputer entry point
│   ├── main.cpp                            # M5Stack Classic entry point
│   ├── m5gotchi_wifi_analyzer.h            # WiFi scanner & analyzer
│   ├── m5gotchi_rfid_nfc.h                 # RFID/NFC multi-tool
│   ├── m5gotchi_chameleon_ultra.h          # Chameleon Ultra interface
│   ├── m5gotchi_achievement_manager.h      # Achievement logic (v2.0)
│   ├── m5gotchi_tutorial_system.h          # Interactive tutorials
│   ├── m5gotchi_memory_utils.h             # Memory optimization tools
│   ├── m5gotchi_progmem_strings.h          # PROGMEM string library
│   ├── m5gotchi_neko_sounds.h              # Sound system
│   ├── m5gotchi_neko_virtual_pet.h         # Virtual pet companion
│   ├── m5gotchi_sushi_sql_game.h           # SQL injection game
│   ├── bluetooth_attacks.h                 # BLE spam attacks
│   └── ...                                 # Other modules
├── docs/                                   # Documentation
│   ├── ARCHITECTURE.md                     # System architecture + diagrams
│   ├── API_REFERENCE.md                    # Complete API reference
│   ├── MEMORY_OPTIMIZATION.md              # Optimization guide
│   ├── DOCUMENTATION_GUIDE.md              # Generate HTML docs
│   ├── CARDPUTER_GUIDE.md                  # Cardputer user guide
│   ├── CARDPUTER_QUICKSTART.md             # Quick start guide
│   └── doxygen/                            # Generated HTML docs (after build)
├── examples/                               # Example sketches
│   ├── M5Gotchi_Basic.ino                  # Basic usage
│   └── memory_optimization_demo.ino        # Memory tools demo
├── data/                                   # Data files
│   └── portal_templates/                   # Evil portal HTML templates
├── platformio.ini                          # PlatformIO configuration
├── Doxyfile                                # Doxygen configuration
├── generate_docs.ps1                       # Generate documentation (Windows)
├── install_doxygen.ps1                     # Install Doxygen (Windows)
└── README.md                               # This file
```

## 🔧 Configuration

### Memory Optimization

The project includes comprehensive memory optimization:

```cpp
// Use PROGMEM for strings (saves RAM)
PRINT_P(CommonStrings::MSG_SUCCESS);

// Use Object Pools for frequent allocations
ObjectPool<NetworkData> pool(20);
auto* net = pool.acquire();
// ... use ...
pool.release(net);

// Use Circular Buffers instead of vectors
CircularBuffer<int, 50> buffer;
buffer.push(value);

// Check memory status
MemoryMonitor::printStats();
if (MemoryMonitor::isLowMemory()) {
    cleanup();
}
```

See [MEMORY_OPTIMIZATION.md](docs/MEMORY_OPTIMIZATION.md) for details.

### Customization

**Change Tutorial Language:**
```cpp
tutorial.setLanguage(LANG_EN);     // English
tutorial.setLanguage(LANG_PT_BR);  // Portuguese
```

**Adjust Sound Settings:**
```cpp
sounds.setVolume(75);              // 0-100%
sounds.setPersonality(MIKU_SOUND); // 7 personalities
sounds.setEnabled(true);           // Enable/disable
```

**Configure Achievement System:**
```cpp
achievementMgr.init();
achievementMgr.onEvent(EVENT_WIFI_SCAN, 1);  // Report events
```

## 📚 Documentation

### Quick Links

- **📐 [Architecture](docs/ARCHITECTURE.md)** - System design with Mermaid diagrams
- **📚 [API Reference](docs/API_REFERENCE.md)** - Complete API with examples
- **💾 [Memory Guide](docs/MEMORY_OPTIMIZATION.md)** - Optimization techniques
- **📖 [Doxygen Guide](docs/DOCUMENTATION_GUIDE.md)** - Generate HTML docs
- **🚀 [Quick Start](docs/CARDPUTER_QUICKSTART.md)** - Get started in 5 minutes

### Generate HTML Documentation

**Windows:**
```powershell
# Install Doxygen (run as Administrator)
.\install_doxygen.ps1

# Generate docs
.\generate_docs.ps1

# Opens in browser automatically
```

**Linux/macOS:**
```bash
# Install Doxygen
sudo apt-get install doxygen graphviz  # Ubuntu/Debian
brew install doxygen graphviz          # macOS

# Generate docs
doxygen Doxyfile

# Open in browser
open docs/doxygen/html/index.html
```

## 🎓 Learning Resources

### Educational Games

**🍣 Sushi SQL Injection Game:**
- Learn SQL injection techniques safely
- 7 sushi types (difficulty levels)
- 7 bypass methods (encoding techniques)
- 5 payload levels (from basic to advanced)
- Real SQL syntax with explanations

**🐾 Paw Scanner:**
- Pattern matching & memory game
- Improve reflexes and recognition

### Tutorial System

- **First-run wizard:** 9 interactive steps
- **Contextual help:** Press `?` anytime
- **Auto-hints:** Shown on first visit to each screen
- **Multi-language:** English and Portuguese support

### Achievements

Learn by doing! Unlock achievements for:
- WiFi scanning & analysis
- Handshake captures
- RFID card operations
- Game completions
- Social interactions

## ⚠️ Legal Disclaimer

**IMPORTANT:** This tool is for **EDUCATIONAL PURPOSES ONLY**.

- ✅ Use on **YOUR OWN** networks and devices
- ✅ Use in **authorized security assessments**
- ✅ Use for **learning** network security concepts
- ❌ **DO NOT** use on networks without explicit permission
- ❌ **DO NOT** use for malicious purposes
- ❌ **DO NOT** violate local laws and regulations

**Unauthorized access to computer networks is illegal in most jurisdictions.**

The developers assume **NO LIABILITY** for misuse of this software. By using M5Gotchi Pro, you agree to use it responsibly and legally.

## 🤝 Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
# Clone with submodules
git clone --recursive https://github.com/muylder/NekoGotchi_Project.git

# Create feature branch
git checkout -b feature/my-new-feature

# Make changes and test
pio test

# Commit with conventional commits
git commit -m "feat: add new feature"

# Push and create PR
git push origin feature/my-new-feature
```

### Code Style

- Use Doxygen comments for public APIs
- Follow existing naming conventions
- Use PROGMEM for constant strings
- Optimize for memory usage
- Test on real hardware

## 📊 Performance Benchmarks

| Operation | Time | Memory |
|-----------|------|--------|
| WiFi Scan (50 networks) | 3-5s | 12KB |
| Handshake Capture | Real-time | 8KB |
| Achievement Check | <0.5ms | 800B |
| RFID Card Read | 100-300ms | 2KB |
| Tutorial Step | <50ms | 4KB |

**Total RAM Usage:** ~176KB / 327KB (54%)  
**Heap Fragmentation:** ~25% (optimized from 45%)

## 🐛 Troubleshooting

### Common Issues

**Issue:** "Out of memory" errors
- **Solution:** Reduce MAX_NETWORKS or MAX_SAVED_CARDS in headers

**Issue:** WiFi not working
- **Solution:** Check antenna connection, try different channels

**Issue:** RFID not detecting cards
- **Solution:** Verify I2C connections (SDA/SCL), check module power

**Issue:** Doxygen not found
- **Solution:** Run `.\install_doxygen.ps1` as Administrator

**Issue:** Compilation errors
- **Solution:** Update M5Unified library, check PlatformIO dependencies

See [Troubleshooting Guide](docs/TROUBLESHOOTING.md) for more solutions.

## 📝 Changelog

### v2.0 (October 2025)
- ✨ Memory optimization system (34KB saved)
- ✨ Achievement System v2.0 (event-driven, O(1))
- ✨ Interactive tutorial system (9 steps, 20+ helps)
- ✨ RFID/NFC multi-tool with attacks
- ✨ Chameleon Ultra integration
- ✨ Complete Doxygen documentation
- ✨ PROGMEM string library
- � Fixed heap fragmentation issues
- 🐛 Reduced cyclomatic complexity

### v1.5 (October 2025)
- Added BLE spam attacks
- Added GPS wardriving
- Improved UI responsiveness

### v1.0 (Initial Release)
- WiFi scanner & analyzer
- Deauth attacks
- Handshake capture
- Evil portal
- Basic achievement system

See [CHANGELOG.md](CHANGELOG.md) for complete history.

## 📜 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **M5Stack** for amazing hardware
- **ESP32 Community** for libraries and support
- **Security Research Community** for educational resources
- **Contributors** for improvements and bug fixes

## 📞 Contact

- **GitHub:** [@muylder](https://github.com/muylder)
- **Repository:** [NekoGotchi_Project](https://github.com/muylder/NekoGotchi_Project)
- **Issues:** [GitHub Issues](https://github.com/muylder/NekoGotchi_Project/issues)

---

**Made with 💖 and 🐱 by the M5Gotchi Pro Team**

*Educational security research tool - Use responsibly!*

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
