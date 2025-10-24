# 🎮 M5Gotchi Pro - M5Stack Edition
## Quick Reference Guide (English)

### 📱 Overview
WiFi pentesting suite for M5Stack Core/Fire/Core2 with 3-button interface and 320x240 display.

---

## 🎮 Button Controls

**Consistent across all modes:**
- **Button A (Left)**: Navigate up / Previous / Toggle
- **Button B (Center)**: Confirm / Menu / Toggle action
- **Button C (Right)**: Navigate down / Next / Back

---

## 🏠 Main Menu

```
╔═══════════════════════════════════════╗
║       M5GOTCHI PRO                    ║
╠═══════════════════════════════════════╣
║                                       ║
║    1. Handshake Capture               ║
║                                       ║
║    2. Clone + Deauth                  ║
║                                       ║
║    3. Evil Portal                     ║
║                                       ║
║    SD: ACTIVE                         ║
║                                       ║
╠═══════════════════════════════════════╣
║ [A]1    [B]2    [C]3                  ║
╚═══════════════════════════════════════╝
```

| Button | Action |
|--------|--------|
| A | Enter Handshake Capture mode |
| B | Enter Clone & Deauth mode |
| C | Enter Evil Portal mode |

---

## 📡 Mode 1: Handshake Capture

### Display
```
╔═══════════════════════════════════════╗
║  HANDSHAKE CAPTURE                    ║
╠═══════════════════════════════════════╣
║  Channel: 06                          ║
║  Mode: PRIORITY                       ║
║  Packets: 45832                       ║
║  EAPOL: 12                            ║
║  PMKID: 3                             ║
╠═══════════════════════════════════════╣
║ [A]Mode [B]Menu [C]Reset              ║
╚═══════════════════════════════════════╝
```

| Button | Action |
|--------|--------|
| A | Toggle between PRIORITY (1,6,11) and ALL (1-13) channels |
| B | Return to main menu |
| C | Reset packet/EAPOL/PMKID counters |

### What it does
- Captures WPA/WPA2 handshakes passively
- Saves to .pcap files on SD Card
- PMKID capture (handshake without deauth!)

---

## 🎯 Mode 2: Clone & Deauth

### Display
```
╔═══════════════════════════════════════╗
║  CLONE & DEAUTH                       ║
╠═══════════════════════════════════════╣
║▓▓Home WiFi                        -45dB  <-- selected
║  My Network 5G                    -52dB
║  Starbucks WiFi                   -67dB
║  NETGEAR_2G                       -71dB
║  TP-Link_5678                     -73dB
║  iPhone                           -78dB
║  Guest Network                    -81dB
║                                       ║
║  DEAUTH: 125 packets                  ║
╠═══════════════════════════════════════╣
║ [A]Clone [B]Deauth [C]Menu            ║
╚═══════════════════════════════════════╝
```

| Button | Action |
|--------|--------|
| A | Clone selected network |
| A (Hold) | Navigate up in network list |
| B | Toggle deauth attack on/off |
| C | Return to main menu |
| C (Hold) | Navigate down in network list |

### Attack Flow
1. Use A/C (hold) to select target network
2. Press A (quick) to clone
3. Deauth starts automatically
4. Press B to stop deauth
5. Press C to exit

---

## 🕷️ Mode 3: Evil Portal

### Display
```
╔═══════════════════════════════════════╗
║  EVIL PORTAL                          ║
╠═══════════════════════════════════════╣
║  SSID: Free WiFi                      ║
║  IP: 192.168.4.1                      ║
║  Clients: 3                           ║
║  Visits: 15                           ║
║  Captured: 2                          ║
║  HTML: /portal.html                   ║
╠═══════════════════════════════════════╣
║ [A]Reload [B]Menu [C]Change           ║
╚═══════════════════════════════════════╝
```

| Button | Action |
|--------|--------|
| A | Reload HTML template |
| B | Return to main menu |
| C | Cycle through templates |

### Available Templates
Pressing C cycles through:
1. portal.html (modern gradient)
2. portal2.html (if exists)
3. Back to portal.html

---

## 🚀 Quick Start

### 1. Compile & Upload
```bash
# Option 1: PlatformIO
pio run -e m5stack-core-esp32 --target upload

# Option 2: Makefile
make upload-core

# Option 3: Build script
./build.sh core
```

### 2. Prepare SD Card
- Format as **FAT32**
- Copy HTML templates from `data/portal_templates/`
- Insert into M5Stack

### 3. First Use
1. Power on → Main menu appears
2. Press A/B/C to select mode
3. Follow button labels at bottom of screen
4. Press B to return to menu (usually center button)

---

## 💡 Tips & Tricks

### Handshake Capture
- PRIORITY mode (channels 1,6,11) is faster
- ALL mode scans all 13 channels completely
- PMKID = handshake without deauth!
- Run for 10-15 minutes for best results

### Clone & Deauth
- Select network with strongest signal (highest dB)
- Hold A/C to scroll through networks
- Quick press A to clone
- Press B to toggle deauth on/off

### Evil Portal
- Works best in crowded areas
- Press C to change template style
- Credentials saved to `/credentials.txt`
- Check SD Card for captured data

---

## 🐛 Troubleshooting

### SD Card not detected
- Format as FAT32 (not exFAT)
- Max 32GB recommended
- Check SD card slot contacts

### Buttons not responding
- Restart M5Stack
- Check battery level
- Update M5Unified library

### No handshakes captured
- Use PRIORITY mode first
- Wait at least 10 minutes
- Networks must have active clients

---

## ⚖️ Legal & Ethical Use

**EDUCATIONAL PURPOSES ONLY**

✅ **Allowed:**
- Testing your own networks
- Authorized security audits  
- Lab environments
- Academic research

❌ **Prohibited:**
- Attacking third-party networks
- Capturing others' data
- Malicious use
- Privacy violations

**Unauthorized use is a CRIME**

---

## 🎯 Button Summary

```
MODE SELECTION (Main Menu):
  [A] = Mode 1 (Handshake)
  [B] = Mode 2 (Clone)
  [C] = Mode 3 (Portal)

IN ANY MODE:
  [B] = Return to menu (center button)
  
Follow the button labels at bottom of screen!
```

---

**Version:** 2.0.0  
**Last Updated:** 2025-10-24

For detailed information, see full documentation in `docs/` folder.
