# 📚 M5GOTCHI PRO - API REFERENCE

## Overview

This document provides a comprehensive API reference for all major M5Gotchi Pro modules. For detailed architecture information, see [ARCHITECTURE.md](ARCHITECTURE.md).

---

## Table of Contents

1. [Core Systems](#core-systems)
2. [Memory Management](#memory-management)
3. [Achievement System](#achievement-system)
4. [Tutorial System](#tutorial-system)
5. [WiFi Tools](#wifi-tools)
6. [Bluetooth Tools](#bluetooth-tools)
7. [RFID/NFC Tools](#rfidnfc-tools)
8. [Games](#games)
9. [Kawaii Features](#kawaii-features)

---

## Core Systems

### Universal Controls

```cpp
#include "m5gotchi_universal_controls.h"

// Action constants
ACTION_UP       // Navigate up
ACTION_DOWN     // Navigate down
ACTION_LEFT     // Navigate left
ACTION_RIGHT    // Navigate right
ACTION_SELECT   // Confirm/Select
ACTION_BACK     // Cancel/Back
ACTION_HELP     // Show help (?)
ACTION_NONE     // No input

// Usage
int action = getUniversalAction();
if (action == ACTION_SELECT) {
    // Handle selection
}
```

---

## Memory Management

### Memory Monitor

```cpp
#include "m5gotchi_memory_utils.h"

MemoryMonitor monitor;

// Get statistics
monitor.printStats();              // Print to Serial
uint32_t free = monitor.getFreeHeap();
uint32_t total = monitor.getTotalHeap();
float frag = monitor.getFragmentation();  // 0-100%

// Alerts
if (monitor.isLowMemory()) {
    // Memory below 20KB
}
```

### Object Pool

```cpp
#include "m5gotchi_memory_utils.h"

// Create pool of 10 objects
ObjectPool<NetworkData> networkPool(10);

// Acquire object
NetworkData* net = networkPool.acquire();
if (net) {
    net->ssid = "TestAP";
    // Use object...
    
    // Release when done
    networkPool.release(net);
}

// Check status
int free = networkPool.getFreeCount();
int used = networkPool.getUsedCount();
```

### Circular Buffer

```cpp
#include "m5gotchi_memory_utils.h"

// Fixed-size FIFO buffer (no heap allocation)
CircularBuffer<int, 50> buffer;

// Add items (automatically discards oldest)
buffer.push(123);
buffer.push(456);

// Read items
int value = buffer.pop();  // 123
value = buffer.peek();     // 456 (doesn't remove)

// Status
bool empty = buffer.isEmpty();
bool full = buffer.isFull();
int count = buffer.size();
buffer.clear();
```

### String Pool

```cpp
#include "m5gotchi_memory_utils.h"

StringPool pool;

// Store string (automatically deduplicates)
const char* str1 = pool.get("Hello World");
const char* str2 = pool.get("Hello World");
// str1 == str2 (same pointer, saved memory!)

// Check stats
int count = pool.getCount();
pool.clear();
```

### PROGMEM Strings

```cpp
#include "m5gotchi_progmem_strings.h"

// Define PROGMEM string
PROGMEM_STRING(MY_STRING, "This is in Flash!");

// Print PROGMEM string
PRINT_P(CommonStrings::MSG_SUCCESS);
PRINTLN_P(WiFiStrings::TITLE_SCANNER);

// Format with PROGMEM
PRINTF_P(CommonStrings::FMT_COUNT, 42);  // "Count: 42"

// Convert to String
String str = PSTR_TO_STRING(ErrorStrings::ERR_OUT_OF_MEMORY);

// Display on screen
DISPLAY_PRINT_P(AchievementStrings::TITLE_ACHIEVEMENTS);

// Common strings available:
// - CommonStrings::BTN_*, MSG_*, WARN_*, UNIT_*, NAV_*
// - WiFiStrings::TITLE_*, MSG_*, WARN_*
// - BLEStrings::*, RFIDStrings::*, GameStrings::*
// - ErrorStrings::ERR_*
// - HelpStrings::HELP_*
// - Emoji::WiFi, Trophy, Cat, Alert, etc.
```

---

## Achievement System

### Achievement Manager

```cpp
#include "m5gotchi_achievement_manager.h"

AchievementManager mgr;

// Initialize
mgr.init();

// Report events
mgr.onEvent(EVENT_WIFI_SCAN);           // Increment by 1
mgr.onEvent(EVENT_WIFI_HANDSHAKE, 5);   // Increment by 5
mgr.onEvent(EVENT_RFID_CLONE);
mgr.onEvent(EVENT_GAME_WIN);

// Check for new unlocks
if (mgr.hasNewUnlocks()) {
    auto unlocked = mgr.getNewUnlocks();  // std::vector<uint8_t>
    for (uint8_t achId : unlocked) {
        const Achievement* ach = mgr.getAchievement(achId);
        // Show notification with ach->name, ach->description, ach->emoji
    }
}

// Query achievements
uint8_t total = mgr.countUnlocked();
uint8_t byCategory = mgr.countByCategory(CATEGORY_WIFI);
float progress = mgr.getOverallProgress();  // 0.0 - 100.0

// Player stats
const PlayerStats& stats = mgr.getStats();
Serial.printf("WiFi Scans: %d\n", stats.wifiScansPerformed);
Serial.printf("Handshakes: %d\n", stats.handshakesCaptured);
Serial.printf("Total Points: %d\n", stats.totalPoints);

// Persistence
mgr.save();      // Manual save
mgr.update();    // Auto-save if dirty (call in loop)

// Reset progress
mgr.reset();
```

### Achievement Events

```cpp
// Available events (see m5gotchi_achievement_manager.h):
EVENT_WIFI_SCAN          // WiFi scan completed
EVENT_WIFI_HANDSHAKE     // Handshake captured
EVENT_WIFI_DEAUTH        // Deauth packet sent
EVENT_PORTAL_CREATE      // Evil portal created
EVENT_BLE_SCAN           // BLE scan completed
EVENT_BLE_SPAM           // BLE spam sent
EVENT_RFID_SCAN          // RFID card scanned
EVENT_RFID_CLONE         // Card cloned
EVENT_GAME_PLAY          // Game played
EVENT_GAME_WIN           // Game won
EVENT_FRIEND_ADD         // Friend added
EVENT_MESSAGE_SEND       // Message sent
EVENT_SOUND_PLAY         // Sound played
// ... and more
```

---

## Tutorial System

### Tutorial System

```cpp
#include "m5gotchi_tutorial_system.h"

TutorialSystem tutorial;

// Initialize
tutorial.init();

// Check first run
if (tutorial.isFirstRun()) {
    tutorial.startWizard();
}

// Main loop
while (tutorial.isTutorialActive()) {
    M5Cardputer.update();
    
    int action = getUniversalAction();
    tutorial.handleUniversalControls(action);
    
    tutorial.update();  // Updates animations/tooltips
    delay(50);
}

// Contextual help
tutorial.setContext(CONTEXT_WIFI_SCANNER);
if (buttonPressed('?')) {
    tutorial.showContextualHelp();
}

// Tooltips (auto-dismiss after 3s)
tutorial.showTooltip("Press SELECT to continue", 3000);

// Hints (auto-shown on first visit)
tutorial.showAutoHint(CONTEXT_RFID_READER);

// Skip/resume
tutorial.skipTutorial();
tutorial.resetProgress();

// Language
tutorial.setLanguage(LANG_EN);     // English
tutorial.setLanguage(LANG_PT_BR);  // Portuguese
```

### Tutorial Contexts

```cpp
// Available contexts:
CONTEXT_MAIN_MENU
CONTEXT_WIFI_SCANNER
CONTEXT_WIFI_DEAUTH
CONTEXT_WIFI_HANDSHAKE
CONTEXT_EVIL_PORTAL
CONTEXT_BLE_SPAM
CONTEXT_RFID_READER
CONTEXT_RFID_CLONER
CONTEXT_GPS_WARDRIVER
CONTEXT_ACHIEVEMENT_LIST
CONTEXT_SETTINGS
// ... and more (20+ contexts)
```

---

## WiFi Tools

### WiFi Analyzer

```cpp
#include "m5gotchi_wifi_analyzer.h"

WiFiAnalyzer analyzer;

// Start analyzer
analyzer.start();

// Main loop
while (analyzer.isRunning()) {
    analyzer.run();  // Auto-scans every 5s
    delay(100);
}

// Stop
analyzer.stop();

// Display modes (cycled with TAB):
// 0 = Channel Graph
// 1 = Network List
// 2 = Statistics
// 3 = Heatmap

// Controls:
// TAB/SPACE - Cycle modes
// R - Force rescan
// ESC (`) - Exit
```

---

## Bluetooth Tools

### Bluetooth Attacks

```cpp
#include "bluetooth_attacks.h"

BluetoothAttacks ble;

// Initialize
if (!ble.begin()) {
    Serial.println("BLE init failed!");
    return;
}

// Start attack
ble.startSourApple();       // iOS spam
// OR
ble.startSamsungSpam();     // Samsung spam
// OR
ble.startGoogleSpam();      // Google Fast Pair
// OR
ble.startMicrosoftSpam();   // Swift Pair
// OR
ble.startSpamAll();         // Rotate all

// Execute in loop
while (ble.isRunning()) {
    ble.executeSourApple();  // Match with start* method
    delay(100);
    
    // Check stats
    Serial.printf("Packets: %d\n", ble.getPacketsSent());
}

// Stop
ble.stop();
ble.end();  // Power down BLE
```

**⚠️ Warning:** Use only in controlled environments for security research!

---

## RFID/NFC Tools

### RFID/NFC Multi-Tool

```cpp
#include "m5gotchi_rfid_nfc.h"

M5GotchiRFIDNFC rfid;

// Initialize
if (rfid.init()) {
    Serial.println("RFID initialized!");
    Serial.printf("Hardware: %s\n", rfid.getHardwareName().c_str());
}

// Scan for card
if (rfid.scanCard()) {
    CardInfo card = rfid.getLastCard();
    Serial.printf("UID: %s\n", card.uidString.c_str());
    Serial.printf("Type: %s\n", card.cardType.c_str());
    Serial.printf("Size: %d bytes\n", card.size);
}

// Read Mifare Classic
if (rfid.readMifareClassic()) {
    const uint8_t* data = rfid.getBlockData(4);  // Read block 4
    // Use data...
}

// Write block
uint8_t writeData[16] = {0x01, 0x02, ...};
if (rfid.writeMifareBlock(4, writeData)) {
    Serial.println("Write successful!");
}

// Clone card
rfid.cloneCard();  // 3-step interactive process

// Dictionary attack
rfid.performDictionaryAttack();  // Uses 8 default keys

// Modes:
rfid.setMode(MODE_SCANNER);   // Continuous scan
rfid.setMode(MODE_READER);    // Read card data
rfid.setMode(MODE_WRITER);    // Write to cards
rfid.setMode(MODE_CLONER);    // Clone cards
rfid.setMode(MODE_ANALYZER);  // Analyze card features
rfid.setMode(MODE_ATTACKS);   // Perform attacks
```

### Chameleon Ultra

```cpp
#include "m5gotchi_chameleon_ultra.h"

M5GotchiChameleonUltra chameleon;

// Initialize (Serial or BLE)
if (chameleon.initSerial()) {
    Serial.println("Connected via Serial!");
}
// OR
if (chameleon.initBLE()) {
    Serial.println("Connected via BLE!");
}

// Get device info
ChameleonDeviceInfo info = chameleon.getDeviceInfo();
Serial.printf("FW: %s\n", info.firmwareVersion.c_str());
Serial.printf("Battery: %d%%\n", info.batteryLevel);

// HF Slots (1-8)
chameleon.setActiveHFSlot(1);
chameleon.activateHFSlot(1);

String slotType = chameleon.getHFSlotType(1);  // e.g., "MIFARE_1K"

// Emulation mode
chameleon.enableEmulation();
chameleon.disableEmulation();

// Clone from PN532 to Chameleon
uint8_t uid[7] = {0x04, 0x12, 0x34, ...};
chameleon.cloneToSlot(1, SLOT_TYPE_MIFARE_1K, uid, 7);

// LF Slots (1-8)
chameleon.setActiveLFSlot(1);
chameleon.activateLFSlot(1);

// Settings
chameleon.setButtonMode(BUTTON_MODE_A);
chameleon.setLEDBrightness(128);  // 0-255
```

---

## Games

### Sushi SQL Injection Game

```cpp
#include "m5gotchi_sushi_sql_game.h"

M5GotchiSushiSQLGame game;

// Initialize with optional sound system
game.init(&nekoSounds);  // Pass nullptr if no sounds

// Main loop
while (game.isRunning()) {
    M5Cardputer.update();
    
    game.handleInput();
    game.update();
    game.render();
    
    delay(50);
}

// Get final score
int score = game.getCurrentScore();
int tablesAccessed = game.getTablesAccessed();

// Reset game
game.reset();
```

**Educational Topics Covered:**
- Authentication bypass (OR 1=1)
- UNION-based injection
- Blind SQL (time-based, boolean)
- Encoding bypasses (URL, hex, unicode)
- Stacked queries
- ORDER BY injection

---

## Kawaii Features

### Neko Sounds

```cpp
#include "m5gotchi_neko_sounds.h"

M5GotchiNekoSounds sounds;

// Initialize
sounds.init();
sounds.setVolume(128);  // 0-255
sounds.setEnabled(true);

// Play sounds
sounds.playSound(SOUND_MENU_SELECT);
sounds.playSound(SOUND_ACHIEVEMENT);
sounds.playSound(SOUND_ERROR);
sounds.playSound(SOUND_SUCCESS);
sounds.playSound(SOUND_NOTIFICATION);
sounds.playSound(SOUND_ATTACK);
sounds.playSound(SOUND_LEVEL_UP);
sounds.playSound(SOUND_PET_HAPPY);
sounds.playSound(SOUND_PET_HUNGRY);
sounds.playSound(SOUND_GAME_START);

// Personality (affects sound style)
sounds.setPersonality(PERSONALITY_KAWAII);      // Cute
sounds.setPersonality(PERSONALITY_HACKER);      // Techy
sounds.setPersonality(PERSONALITY_PROFESSIONAL); // Serious
sounds.setPersonality(PERSONALITY_CHAOTIC);     // Random
sounds.setPersonality(PERSONALITY_RETRO);       // 8-bit
sounds.setPersonality(PERSONALITY_CYBERPUNK);   // Futuristic
sounds.setPersonality(PERSONALITY_NINJA);       // Stealthy

// Text-to-speech (if enabled)
sounds.speak("Hello Neko!");
```

### Virtual Pet

```cpp
#include "m5gotchi_neko_virtual_pet.h"

M5GotchiNekoVirtualPet pet;

// Initialize
pet.init("NyanCat");

// Update (call in loop)
pet.update();

// Interact
pet.feed();
pet.play();
pet.pet();
pet.sleep();
pet.exercise();

// Check status
PetStats stats = pet.getStats();
Serial.printf("Hunger: %d/100\n", stats.hunger);
Serial.printf("Happiness: %d/100\n", stats.happiness);
Serial.printf("Energy: %d/100\n", stats.energy);
Serial.printf("Evolution: %d\n", stats.evolutionStage);  // 0-6

// Evolution stages:
// 0 = Egg
// 1 = Baby
// 2 = Child
// 3 = Teen
// 4 = Adult
// 5 = Master
// 6 = Legendary

// React to hacking activities
pet.onHackingActivity(ACTIVITY_WIFI_SCAN);   // Pet gets excited
pet.onHackingActivity(ACTIVITY_HANDSHAKE);   // Pet evolves
```

---

## Error Handling

### Common Patterns

```cpp
// Check return values
if (!module.init()) {
    Serial.println(F("Init failed!"));
    return;
}

// Memory checks
if (MemoryMonitor::isLowMemory()) {
    Serial.println(F("Low memory! Cleaning up..."));
    cleanup();
}

// Safe string operations
char buffer[64];
SafeString::copy(buffer, longString, sizeof(buffer));  // No overflow

// Error messages from PROGMEM
PRINTLN_P(ErrorStrings::ERR_OUT_OF_MEMORY);
PRINTLN_P(ErrorStrings::ERR_SD_CARD);
PRINTLN_P(ErrorStrings::ERR_TIMEOUT);
```

---

## Performance Tips

1. **Use PROGMEM for constant strings:**
   ```cpp
   PROGMEM_STRING(MY_MSG, "Save RAM!");
   PRINT_P(MY_MSG);
   ```

2. **Use ObjectPool for frequent allocations:**
   ```cpp
   ObjectPool<MyStruct> pool(20);
   auto* obj = pool.acquire();
   // Use...
   pool.release(obj);
   ```

3. **Use CircularBuffer instead of std::vector:**
   ```cpp
   CircularBuffer<int, 50> buffer;  // Fixed size, no heap
   buffer.push(value);
   ```

4. **Check memory before heavy operations:**
   ```cpp
   if (MemoryMonitor::getFreeHeap() < 20000) {
       return;  // Skip operation
   }
   ```

5. **Use DirtyFlag to skip unnecessary redraws:**
   ```cpp
   DirtyFlag dirty;
   if (dataChanged) {
       dirty.markDirty();
   }
   if (dirty.isDirty()) {
       redraw();
       dirty.clear();
   }
   ```

---

## Changelog

| Version | Date | Changes |
|---------|------|---------|
| 2.0 | 2025-10-26 | Achievement system refactor, Tutorial system, PROGMEM library, Memory utils, Full Doxygen docs |
| 1.5 | 2025-10-20 | RFID/NFC tools, Chameleon Ultra integration |
| 1.0 | 2025-10-01 | Initial release |

---

**For more information:**
- Architecture: [ARCHITECTURE.md](ARCHITECTURE.md)
- Memory optimization: [MEMORY_OPTIMIZATION.md](MEMORY_OPTIMIZATION.md)
- Contributing: [CONTRIBUTING.md](../CONTRIBUTING.md)

**Last Updated:** 2025-10-26  
**Version:** 2.0
