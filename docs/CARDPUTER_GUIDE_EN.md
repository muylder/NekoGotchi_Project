# 🎮 M5Gotchi Pro - Cardputer Edition
## Quick Reference Guide (English)

### 📱 Overview
Optimized WiFi pentesting suite for M5Stack Cardputer with QWERTY keyboard and 240x135 display.

---

## ⌨️ Simple Navigation System

**All modes use the same controls:**
- **↑ (Up Arrow)**: Navigate up / Previous option
- **↓ (Down Arrow)**: Navigate down / Next option  
- **ENTER**: Confirm selection / Execute action
- **ESC (DEL)**: Return to main menu

**No need to memorize letter keys!** Just use arrows and ENTER.

---

## 🏠 Main Menu

```
╔═══════════════════════════════════════╗
║  M5GOTCHI PRO - CARDPUTER             ║
╠═══════════════════════════════════════╣
║                                       ║
║  > Handshake Capture    <-- cursor    ║
║    Clone + Deauth                     ║
║    Evil Portal                        ║
║                                       ║
║  SD Card: OK                          ║
║                                       ║
╠═══════════════════════════════════════╣
║ UP/DN: Navigate | ENTER: Select       ║
╚═══════════════════════════════════════╝
```

| Key | Action |
|-----|--------|
| ↑/↓ | Navigate options |
| ENTER | Enter selected mode |

---

## 📡 Mode 1: Handshake Capture

### Display
```
╔═══════════════════════════════════════╗
║  HANDSHAKE CAPTURE                    ║
╠═══════════════════════════════════════╣
║  Channel: 06 | PRIORITY               ║
║  Packets: 45832                       ║
║  EAPOL: 12                            ║
║  PMKID: 3                             ║
║  File: 1.pcap (2456KB)                ║
║                                       ║
║  > Toggle Mode         <-- cursor     ║
║    Reset Counters                     ║
║    Back to Menu                       ║
╠═══════════════════════════════════════╣
║ UP/DN: Select | ENTER: Execute        ║
╚═══════════════════════════════════════╝
```

### Actions Menu
| Option | Action |
|--------|--------|
| Toggle Mode | Switch between PRIORITY (1,6,11) and ALL (1-13) channels |
| Reset Counters | Clear packet/EAPOL/PMKID counts |
| Back to Menu | Stop capture and return to main menu |

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
║                                       ║
║  > Clone Network       <-- cursor     ║
║    Toggle Deauth                      ║
║    Back to Menu                       ║
║                                       ║
║  DEAUTH ACTIVE: 125 pkts              ║
╠═══════════════════════════════════════╣
║ UP/DN: Select | ENTER: Execute        ║
╚═══════════════════════════════════════╝
```

### Two-Level Navigation
1. **Network List**: Use ↑/↓ to select target network
2. **Actions Menu**: Continue pressing ↑/↓ to reach actions

### Actions Menu
| Option | Action |
|--------|--------|
| Clone Network | Create fake AP with selected network's SSID |
| Toggle Deauth | Start/stop deauth attack (forces clients to disconnect) |
| Back to Menu | Stop attack and return to main menu |

### Attack Flow
1. Navigate to target network → ENTER "Clone Network"
2. AP is cloned, deauth starts automatically
3. Navigate to "Toggle Deauth" → ENTER to stop attack
4. Navigate to "Back to Menu" → ENTER to exit

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
║  Template:                            ║
║  /portal.html                         ║
║                                       ║
║  > Change Template     <-- cursor     ║
║    Back to Menu                       ║
╠═══════════════════════════════════════╣
║ UP/DN: Select | ENTER: Execute        ║
╚═══════════════════════════════════════╝
```

### Actions Menu
| Option | Action |
|--------|--------|
| Change Template | Cycle through: portal.html → netflix → facebook → google → portal |
| Back to Menu | Stop portal and return to main menu |

### Available Templates
1. **portal.html** - Modern gradient design
2. **portal_netflix.html** - Netflix-style interface
3. **portal_facebook.html** - Facebook-style interface
4. **portal_google.html** - Google Material Design

### What it does
- Creates fake WiFi "Free WiFi"
- Captures credentials in `/credentials.txt`
- DNS spoofing redirects all sites to portal

---

## 🚀 Quick Start

### 1. Compile & Upload
```bash
# Option 1: PlatformIO
pio run -e m5stack-cardputer --target upload

# Option 2: Makefile
make upload-cardputer

# Option 3: Build script
./build.sh cardputer
```

### 2. Prepare SD Card
- Format as **FAT32**
- Copy HTML templates from `data/portal_templates/`
- Insert into Cardputer

### 3. First Use
1. Power on → Main menu appears
2. Use ↑/↓ arrows to navigate
3. Press ENTER to select mode
4. Follow on-screen actions menu
5. Press ESC anytime to return to main menu

---

## 💡 Tips & Tricks

### Handshake Capture
- **PRIORITY mode** focuses on channels 1, 6, 11 (80% of networks)
- **ALL mode** scans all 13 channels (slower but complete)
- PMKID = full handshake without deauth attack!
- Let it run 10-15 minutes for best results

### Clone & Deauth
- Select network with strongest signal (highest dB)
- Clone creates fake AP, deauth forces disconnection
- Clients reconnect to fake AP
- Use ESC to stop everything quickly

### Evil Portal
- Works best in crowded areas
- Change template to match target audience
- Credentials saved even if device loses power
- Check `/credentials.txt` on SD Card

---

## 🐛 Troubleshooting

### SD Card not detected
- Format as FAT32 (not exFAT)
- Max 32GB recommended
- Clean contacts with isopropyl alcohol

### Keyboard not responding
- Delay is set to 50ms (responsive)
- Try restarting Cardputer
- Update M5Unified library

### No handshakes captured
- Use PRIORITY mode first (faster)
- Wait at least 10 minutes
- Handshakes only occur when device connects

### Portal doesn't redirect
- iOS: Popup appears automatically
- Android: "Login required" notification
- Desktop: Manually visit `192.168.4.1`

---

## ⚖️ Legal & Ethical Use

**EDUCATIONAL PURPOSES ONLY**

✅ **Allowed:**
- Testing your own networks
- Authorized security audits
- Controlled lab environments
- Academic research with permission

❌ **Prohibited:**
- Attacking third-party networks
- Capturing others' data
- Any malicious use
- Privacy violations

**Unauthorized use is a CRIME**

---

## 📊 Performance

- **Handshake Capture**: 5-15 handshakes in 30 min
- **Deauth Attack**: ~100 packets/min
- **Evil Portal**: 3-10 simultaneous clients
- **Battery Life**: ~8h (1400mAh)

---

## 🎯 Control Summary

```
EVERYWHERE:
  ↑ ↓      = Navigate options
  ENTER    = Confirm / Execute
  ESC      = Back to main menu

That's it! Simple and consistent.
```

---

**Version:** 2.0.0 Cardputer Edition  
**Last Updated:** 2025-10-24  

For detailed information, see full documentation in `docs/` folder.
