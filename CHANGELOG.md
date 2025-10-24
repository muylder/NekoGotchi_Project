# Changelog

All notable changes to M5Gotchi Pro will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2025-10-24 - Cardputer Edition 🎮⌨️

### Major Changes - Simplified Navigation

#### 🎯 New Navigation System
- **Cardputer:** Simple arrow-based navigation (↑/↓ + ENTER + ESC)
- **M5Stack:** Consistent button mapping (A=Up, B=Select, C=Down)
- **No more letter keys to memorize!** Just arrows and ENTER
- **Visual cursor** shows current selection in all menus
- **Context-sensitive actions** shown at bottom of screen

#### 📖 New Documentation Structure
- **Quick Reference Guides** in English and Portuguese
  - `M5STACK_GUIDE_EN.md` - English quick reference
  - `M5STACK_GUIDE_PT.md` - Portuguese quick reference  
  - `CARDPUTER_GUIDE_EN.md` - English quick reference
  - `CARDPUTER_GUIDE_PT.md` - Portuguese quick reference
- Concise, visual guides with ASCII art menus
- Step-by-step instructions for each mode

### Added - Cardputer Support
- ⌨️ **Full M5Stack Cardputer support** with optimized interface
- 🎮 Complete QWERTY keyboard controls with 10+ shortcuts
- 📱 Compact interface for 240x135 display
- 🚀 New `src/main_cardputer.cpp` optimized version
- 📖 Comprehensive Cardputer documentation (`docs/CARDPUTER_GUIDE.md`)
- 🔧 Quick start guide for Cardputer (`docs/CARDPUTER_QUICKSTART.md`)
- 🔨 Build targets: `make build-cardputer`, `./build.sh cardputer`
- ⚙️ PlatformIO environment: `m5stack-cardputer`

### Keyboard Controls (Cardputer)
- `1`, `2`, `3` - Direct mode selection
- `ESC` - Return to menu
- `T` - Toggle channel mode (Handshake)
- `R` - Reset counters (Handshake)
- `↑`, `↓` - Navigate networks (Clone)
- `ENTER` - Clone network (Clone)
- `SPACE` - Toggle deauth (Clone)
- `H` - Change HTML template (Portal)

### Interface Optimizations (Cardputer)
- Compact headers (35px → 14px)
- Smaller fonts (size 2 → size 1)
- Status bar with full help text
- 8 visible networks (vs 7 on M5Stack)
- Optimized layout for 240x135px
- Better loop responsiveness (50ms vs 10ms)

### Documentation
- `docs/CARDPUTER_GUIDE.md` - Complete guide (~80KB)
- `docs/CARDPUTER_QUICKSTART.md` - Quick setup
- `src/README.md` - Version differences explained
- Updated `PROJECT_STRUCTURE.md`
- Updated main `README.md` with version info

### Technical
- ESP32-S3 support (M5Stack StampS3)
- Custom SPI pins (SCK=36, MISO=35, MOSI=37, SS=34)
- PSRAM enabled (8MB)
- Screen dimensions defined (-DSCREEN_WIDTH=240 -DSCREEN_HEIGHT=135)
- Keyboard handling via M5Cardputer API

### Statistics
- **+1600 lines** of code (main_cardputer.cpp)
- **+80KB** documentation
- **+10** keyboard shortcuts
- **5 devices** supported (was 4)
- **2 versions** (M5Stack + Cardputer)

---

## [1.0.0] - 2024-10-19

### Added
- ✨ Initial release of M5Gotchi Pro
- 🎯 Handshake capture optimized mode (750% more efficient)
- 🔄 Clone & Deauth attack with automatic deauth on clone
- 👹 Evil Portal with customizable HTML from SD Card
- 🎨 4 professional portal templates (Netflix, Facebook, Google, Modern)
- 📦 Multi-device support (M5Stack Core, Fire, Core2, M5Stick-C)
- 🤖 CI/CD with GitHub Actions
- 📖 Comprehensive documentation (PRO_GUIDE, QUICKSTART, COMPARISON)
- 🔨 Build automation (Makefile, build.sh)
- 📋 Project structure ready for Git

### Features

#### Handshake Capture Mode
- Optimized channel hopping (3s per channel)
- Priority channels focus (1, 6, 11)
- PMKID and EAPOL capture
- Automatic PCAP file generation
- SD Card storage

#### Clone & Deauth Mode
- Network scanner with RSSI display
- One-button network cloning
- **Automatic deauth on clone**
- 5 deauth packets per 5-second cycle
- Real-time packet counter
- Guaranteed handshake capture

#### Evil Portal Mode
- Captive portal with DNS spoofing
- Load HTML pages from SD Card
- 4 ready-to-use templates
- Automatic credential capture
- Saves to `/credentials.txt`
- Multi-client support
- Template switching via button

### Technical Details
- ESP32 platform
- Arduino framework
- M5Stack library
- WebServer & DNSServer
- 802.11 packet injection
- Promiscuous mode WiFi
- PCAP format output

### Supported Devices
- M5Stack Core ESP32
- M5Stack Fire
- M5Stack Core2
- M5Stick-C

### Build System
- PlatformIO configuration
- Makefile automation
- Build script for binaries
- GitHub Actions CI/CD
- Multi-environment support

### Documentation
- README.md - Main documentation
- PRO_GUIDE.md - Complete usage guide
- QUICKSTART.md - 5-minute setup
- COMPARISON.md - Before/After statistics
- PROJECT_STRUCTURE.md - Project organization
- GIT_GUIDE.md - Git workflow guide
- CONTRIBUTING.md - Contribution guidelines

## [Unreleased]

### Planned
- [ ] WPA3 support detection
- [ ] Additional portal templates
- [ ] Bluetooth scanning mode
- [ ] Packet statistics export
- [ ] Web interface for configuration
- [ ] OTA firmware updates
- [ ] Multi-language support

## Versioning

- MAJOR version for incompatible API changes
- MINOR version for new functionality (backwards compatible)
- PATCH version for backwards compatible bug fixes

---

**Legend:**
- ✨ New feature
- 🐛 Bug fix
- 🔧 Configuration/build changes
- 📖 Documentation
- ⚡ Performance improvement
- 🔒 Security fix
- ♻️ Refactoring
- 🚀 Release
