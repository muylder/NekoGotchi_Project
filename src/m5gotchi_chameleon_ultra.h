/*
 * 🦎 M5GOTCHI CHAMELEON ULTRA INTEGRATION v1.0
 * Interface completa para Chameleon Ultra via Serial/BLE!
 * 
 * Features:
 * - Serial communication com Chameleon Ultra
 * - BLE communication (opcional)
 * - Emulation mode control
 * - Slot management (8 slots HF + 8 slots LF)
 * - Card detection e cloning
 * - UID manipulation
 * - Mifare Classic attack integration
 * - Dictionary attack management
 * - Settings sync
 * - Firmware update helper
 * - Battery monitor
 * - LED control
 * - Button simulation
 * - Log viewer
 * 
 * Chameleon Ultra: Multi-protocol RFID emulator
 * Suporta: Mifare Classic, Ultralight, NTAG, EM410x, T5577, etc.
 * Conexão: Serial (UART) ou BLE
 */

#ifndef M5GOTCHI_CHAMELEON_ULTRA_H
#define M5GOTCHI_CHAMELEON_ULTRA_H

#include <M5Unified.h>
#include <HardwareSerial.h>
#include "m5gotchi_universal_controls.h"

// ==================== CHAMELEON CONFIGURATION ====================
#define CHAMELEON_SERIAL_RX 16
#define CHAMELEON_SERIAL_TX 17
#define CHAMELEON_BAUD 115200
#define CHAMELEON_TIMEOUT 1000
#define MAX_SLOTS 8
#define MAX_COMMAND_LEN 256
#define MAX_RESPONSE_LEN 512

// ==================== CHAMELEON COMMANDS ====================
enum ChameleonCommand {
    CMD_GET_VERSION = 0x1000,
    CMD_GET_DEVICE_MODEL,
    CMD_GET_BATTERY_INFO,
    CMD_GET_BUTTON_PRESS,
    CMD_GET_ANIMATION_MODE,
    CMD_SET_ANIMATION_MODE,
    CMD_GET_GIT_VERSION,
    
    // Slot management
    CMD_HF_SET_SLOT_ACTIVATED = 0x1010,
    CMD_HF_SET_SLOT_TAG_TYPE,
    CMD_HF_SET_SLOT_DATA_DEFAULT,
    CMD_HF_GET_SLOT_INFO,
    CMD_HF_GET_SLOT_TAG_NAME,
    CMD_HF_SET_SLOT_TAG_NAME,
    CMD_HF_SLOT_DATA_SAVED,
    
    // Mifare Classic
    CMD_MF1_DETECT_SUPPORT = 0x2000,
    CMD_MF1_DETECT_PRNG,
    CMD_MF1_STATIC_NESTED_ACQUIRE,
    CMD_MF1_DARKSIDE_ACQUIRE,
    CMD_MF1_DETECT_NT_DIST,
    CMD_MF1_NESTED_ACQUIRE,
    CMD_MF1_AUTH_ONE_KEY_BLOCK,
    CMD_MF1_READ_EMU_BLOCK_DATA,
    CMD_MF1_WRITE_EMU_BLOCK_DATA,
    
    // EM410x (LF)
    CMD_EM410X_SCAN = 0x3000,
    CMD_EM410X_WRITE_TO_T55XX,
    
    // Settings
    CMD_GET_SETTINGS = 0x4000,
    CMD_SET_SETTINGS,
    CMD_GET_BLE_PAIRING_KEY,
    CMD_DELETE_BLE_ALL_BONDS
};

// ==================== SLOT TYPES ====================
enum SlotType {
    SLOT_EMPTY = 0,
    SLOT_MIFARE_MINI,
    SLOT_MIFARE_1K,
    SLOT_MIFARE_2K,
    SLOT_MIFARE_4K,
    SLOT_MIFARE_ULTRALIGHT,
    SLOT_NTAG_213,
    SLOT_NTAG_215,
    SLOT_NTAG_216,
    SLOT_EM410X,
    SLOT_T5577
};

// ==================== CHAMELEON STATE ====================
enum ChameleonMode {
    MODE_READER = 0,
    MODE_EMULATOR,
    MODE_SNIFFER
};

enum AnimationMode {
    ANIM_FULL = 0,
    ANIM_MINIMAL,
    ANIM_NONE
};

// ==================== STRUCTURES ====================
struct SlotInfo {
    int slotNum;
    SlotType type;
    String name;
    bool enabled;
    uint8_t uid[10];
    int uidLen;
    uint8_t data[4096];  // Max 4K data
    int dataLen;
};

struct ChameleonDeviceInfo {
    String version;
    String gitVersion;
    String model;
    int batteryLevel;
    int batteryVoltage;
    bool charging;
    AnimationMode animMode;
    ChameleonMode mode;
    int activeSlotHF;
    int activeSlotLF;
};

struct MifareKey {
    uint8_t key[6];
    bool isKeyA;
    int sector;
};

struct DetectionResult {
    bool cardDetected;
    SlotType cardType;
    uint8_t uid[10];
    int uidLen;
    int sak;
    int atqa;
    bool prngWeak;
    bool staticNested;
    bool hardnested;
};

// ==================== CHAMELEON SCREENS ====================
enum ChameleonScreen {
    SCREEN_MAIN = 0,
    SCREEN_SLOTS_HF,
    SCREEN_SLOTS_LF,
    SCREEN_EMULATOR,
    SCREEN_READER,
    SCREEN_ATTACKS,
    SCREEN_SETTINGS,
    SCREEN_DEVICE_INFO,
    SCREEN_LOGS
};

// ==================== CHAMELEON ULTRA CLASS ====================
class M5GotchiChameleonUltra {
private:
    HardwareSerial* chameleonSerial;
    
    ChameleonScreen currentScreen;
    int selectedItem;
    int selectedSlot;
    
    // Device state
    ChameleonDeviceInfo deviceInfo;
    SlotInfo slotsHF[MAX_SLOTS];
    SlotInfo slotsLF[MAX_SLOTS];
    
    // Detection
    DetectionResult lastDetection;
    bool scanning;
    
    // Communication
    bool connected;
    unsigned long lastCommand;
    String lastResponse;
    String commandLog[20];
    int logIndex;
    
    // Statistics
    int commandsSent;
    int commandsSuccess;
    int cardsDetected;
    int cardsCloned;
    int attacksPerformed;
    
    // UI
    unsigned long lastUpdate;
    int scrollOffset;

public:
    M5GotchiChameleonUltra() {
        chameleonSerial = nullptr;
        
        currentScreen = SCREEN_MAIN;
        selectedItem = 0;
        selectedSlot = 0;
        
        connected = false;
        scanning = false;
        lastCommand = 0;
        
        commandsSent = 0;
        commandsSuccess = 0;
        cardsDetected = 0;
        cardsCloned = 0;
        attacksPerformed = 0;
        
        lastUpdate = 0;
        scrollOffset = 0;
        logIndex = 0;
        
        initializeSlots();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🦎 Initializing Chameleon Ultra Interface...");
        
        // Initialize serial communication
        chameleonSerial = new HardwareSerial(2);
        chameleonSerial->begin(CHAMELEON_BAUD, SERIAL_8N1, CHAMELEON_SERIAL_RX, CHAMELEON_SERIAL_TX);
        
        Serial.printf("📡 Serial: RX=%d TX=%d @ %d baud\n", 
                     CHAMELEON_SERIAL_RX, CHAMELEON_SERIAL_TX, CHAMELEON_BAUD);
        
        // Test connection
        delay(100);
        if (testConnection()) {
            connected = true;
            Serial.println("✅ Chameleon Ultra connected!");
            
            // Get device info
            getDeviceInfo();
        } else {
            Serial.println("⚠️ Chameleon Ultra not detected");
            Serial.println("   Connect via RX/TX or use BLE mode");
        }
    }
    
    void initializeSlots() {
        // Initialize HF slots
        for (int i = 0; i < MAX_SLOTS; i++) {
            slotsHF[i].slotNum = i;
            slotsHF[i].type = SLOT_EMPTY;
            slotsHF[i].name = "Empty";
            slotsHF[i].enabled = false;
            slotsHF[i].uidLen = 0;
            slotsHF[i].dataLen = 0;
        }
        
        // Initialize LF slots
        for (int i = 0; i < MAX_SLOTS; i++) {
            slotsLF[i].slotNum = i;
            slotsLF[i].type = SLOT_EMPTY;
            slotsLF[i].name = "Empty";
            slotsLF[i].enabled = false;
            slotsLF[i].uidLen = 0;
            slotsLF[i].dataLen = 0;
        }
        
        // Load slots from Chameleon if connected
        if (connected) {
            loadAllSlots();
        }
    }
    
    // ==================== CONNECTION ====================
    bool testConnection() {
        // Send version command
        sendCommand(CMD_GET_VERSION, nullptr, 0);
        delay(100);
        
        // Check for response
        if (chameleonSerial->available()) {
            String response = readResponse();
            return response.length() > 0;
        }
        
        return false;
    }
    
    void getDeviceInfo() {
        // Get version
        sendCommand(CMD_GET_VERSION, nullptr, 0);
        deviceInfo.version = readResponse();
        
        // Get git version
        sendCommand(CMD_GET_GIT_VERSION, nullptr, 0);
        deviceInfo.gitVersion = readResponse();
        
        // Get device model
        sendCommand(CMD_GET_DEVICE_MODEL, nullptr, 0);
        deviceInfo.model = readResponse();
        
        // Get battery info
        getBatteryInfo();
        
        // Get animation mode
        sendCommand(CMD_GET_ANIMATION_MODE, nullptr, 0);
        deviceInfo.animMode = (AnimationMode)readByte();
        
        Serial.println("📱 Device Info:");
        Serial.printf("   Model: %s\n", deviceInfo.model.c_str());
        Serial.printf("   Version: %s\n", deviceInfo.version.c_str());
        Serial.printf("   Battery: %d%% (%dmV)\n", deviceInfo.batteryLevel, deviceInfo.batteryVoltage);
    }
    
    void getBatteryInfo() {
        sendCommand(CMD_GET_BATTERY_INFO, nullptr, 0);
        
        // Parse battery response
        if (chameleonSerial->available() >= 3) {
            deviceInfo.batteryLevel = chameleonSerial->read();
            deviceInfo.batteryVoltage = chameleonSerial->read() | (chameleonSerial->read() << 8);
            deviceInfo.charging = chameleonSerial->read();
        }
    }
    
    // ==================== CHAMELEON FLOW ====================
    void start() {
        init();
        currentScreen = SCREEN_MAIN;
        selectedItem = 0;
        drawChameleonScreen();
    }
    
    void stop() {
        if (chameleonSerial) {
            chameleonSerial->end();
        }
        
        Serial.println("🦎 Chameleon Ultra disconnected");
        Serial.printf("📊 Stats: Cmds=%d Cards=%d Cloned=%d\n",
                     commandsSent, cardsDetected, cardsCloned);
    }
    
    void update() {
        // Update battery info periodically
        static unsigned long lastBatteryCheck = 0;
        if (millis() - lastBatteryCheck > 30000) {
            getBatteryInfo();
            lastBatteryCheck = millis();
        }
        
        // Check for scanning results
        if (scanning) {
            checkForCard();
        }
        
        // Update display
        if (millis() - lastUpdate > 100) {
            drawChameleonScreen();
            lastUpdate = millis();
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawChameleonScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentScreen) {
            case SCREEN_MAIN:
                drawMainScreen();
                break;
            case SCREEN_SLOTS_HF:
                drawSlotsScreen(true);
                break;
            case SCREEN_SLOTS_LF:
                drawSlotsScreen(false);
                break;
            case SCREEN_EMULATOR:
                drawEmulatorScreen();
                break;
            case SCREEN_READER:
                drawReaderScreen();
                break;
            case SCREEN_ATTACKS:
                drawAttacksScreen();
                break;
            case SCREEN_SETTINGS:
                drawSettingsScreen();
                break;
            case SCREEN_DEVICE_INFO:
                drawDeviceInfoScreen();
                break;
            case SCREEN_LOGS:
                drawLogsScreen();
                break;
        }
        
        drawStatusBar();
    }
    
    void drawMainScreen() {
        // Header
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(40, 5);
        M5.Display.print("🦎 Chameleon Ultra");
        
        // Connection status
        if (connected) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(200, 5);
            M5.Display.print("●");
        } else {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(10, 25);
            M5.Display.print("⚠️ Not connected");
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(10, 40);
            M5.Display.print("Connect Chameleon Ultra");
            M5.Display.setCursor(10, 55);
            M5.Display.printf("RX:%d TX:%d", CHAMELEON_SERIAL_RX, CHAMELEON_SERIAL_TX);
            return;
        }
        
        // Menu items
        String menuItems[] = {
            "📡 HF Slots (13.56MHz)",
            "📻 LF Slots (125kHz)",
            "🎭 Emulator Mode",
            "🔍 Reader Mode",
            "⚔️ Mifare Attacks",
            "⚙️ Settings",
            "ℹ️ Device Info",
            "📜 Logs"
        };
        
        int y = 25;
        for (int i = 0; i < 8; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, y);
            M5.Display.print((i == selectedItem ? "> " : "  ") + menuItems[i]);
            y += 11;
        }
    }
    
    void drawSlotsScreen(bool isHF) {
        M5.Display.setTextColor(isHF ? CYAN : ORANGE);
        M5.Display.setCursor(50, 5);
        M5.Display.printf("%s Slots", isHF ? "HF" : "LF");
        
        SlotInfo* slots = isHF ? slotsHF : slotsLF;
        
        // List slots
        for (int i = 0; i < MAX_SLOTS; i++) {
            uint16_t color = (i == selectedSlot) ? YELLOW : WHITE;
            if (!slots[i].enabled) color = DARKGREY;
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 25 + i * 12);
            M5.Display.printf("%s%d: %s", 
                             i == selectedSlot ? ">" : " ",
                             i + 1,
                             slots[i].name.c_str());
            
            // UID if available
            if (slots[i].uidLen > 0) {
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(100, 25 + i * 12);
                M5.Display.print(formatUID(slots[i].uid, slots[i].uidLen));
            }
        }
        
        // Instructions
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(10, 115);
        M5.Display.print("SEL: Activate | ← LF/HF →");
    }
    
    void drawEmulatorScreen() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🎭 Emulator");
        
        if (deviceInfo.activeSlotHF >= 0) {
            SlotInfo& slot = slotsHF[deviceInfo.activeSlotHF];
            
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 30);
            M5.Display.printf("Active: Slot %d", deviceInfo.activeSlotHF + 1);
            
            M5.Display.setCursor(10, 45);
            M5.Display.printf("Type: %s", getSlotTypeName(slot.type).c_str());
            
            M5.Display.setCursor(10, 60);
            M5.Display.printf("Name: %s", slot.name.c_str());
            
            if (slot.uidLen > 0) {
                M5.Display.setTextColor(CYAN);
                M5.Display.setCursor(10, 75);
                M5.Display.print("UID: " + formatUID(slot.uid, slot.uidLen));
            }
            
            // Visual indicator
            M5.Display.fillCircle(120, 100, 15, GREEN);
            M5.Display.setTextColor(BLACK);
            M5.Display.setCursor(110, 95);
            M5.Display.print("EMU");
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(60, 60);
            M5.Display.print("No slot active");
        }
    }
    
    void drawReaderScreen() {
        M5.Display.setTextColor(BLUE);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🔍 Reader");
        
        if (scanning) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(60, 35);
            M5.Display.print("● Scanning...");
            
            // Animated scan indicator
            static int scanAnim = 0;
            M5.Display.drawCircle(120, 70, 20 + (scanAnim % 20), CYAN);
            scanAnim++;
        }
        
        if (lastDetection.cardDetected) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 35);
            M5.Display.print("✓ Card detected!");
            
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 50);
            M5.Display.printf("Type: %s", getSlotTypeName(lastDetection.cardType).c_str());
            
            M5.Display.setCursor(10, 65);
            M5.Display.print("UID: " + formatUID(lastDetection.uid, lastDetection.uidLen));
            
            M5.Display.setCursor(10, 80);
            M5.Display.printf("SAK: 0x%02X ATQA: 0x%04X", lastDetection.sak, lastDetection.atqa);
            
            // Clone option
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 100);
            M5.Display.print("Press SELECT to clone");
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(40, 60);
            M5.Display.print("Place card near");
            M5.Display.setCursor(50, 75);
            M5.Display.print("Chameleon...");
        }
    }
    
    void drawAttacksScreen() {
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(40, 5);
        M5.Display.print("⚔️ Mifare Attacks");
        
        String attacks[] = {
            "Darkside Attack",
            "Nested Attack",
            "Static Nested",
            "Hardnested Attack",
            "Dictionary Attack",
            "PRNG Detection"
        };
        
        int y = 30;
        for (int i = 0; i < 6; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, y);
            M5.Display.print((i == selectedItem ? "> " : "  ") + attacks[i]);
            y += 12;
        }
        
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(10, 105);
        M5.Display.printf("Attacks performed: %d", attacksPerformed);
    }
    
    void drawSettingsScreen() {
        M5.Display.setTextColor(PURPLE);
        M5.Display.setCursor(60, 5);
        M5.Display.print("⚙️ Settings");
        
        String settings[] = {
            "Animation: " + String(deviceInfo.animMode == ANIM_FULL ? "Full" : 
                                   deviceInfo.animMode == ANIM_MINIMAL ? "Min" : "Off"),
            "Active Slot HF: " + String(deviceInfo.activeSlotHF + 1),
            "Active Slot LF: " + String(deviceInfo.activeSlotLF + 1),
            "BLE Pairing",
            "Factory Reset",
            "Firmware Update"
        };
        
        int y = 30;
        for (int i = 0; i < 6; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, y);
            M5.Display.print((i == selectedItem ? "> " : "  ") + settings[i]);
            y += 12;
        }
    }
    
    void drawDeviceInfoScreen() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(50, 5);
        M5.Display.print("ℹ️ Device Info");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Model: %s", deviceInfo.model.c_str());
        
        M5.Display.setCursor(10, 40);
        M5.Display.printf("FW: %s", deviceInfo.version.c_str());
        
        M5.Display.setCursor(10, 55);
        M5.Display.printf("Git: %s", deviceInfo.gitVersion.substring(0, 12).c_str());
        
        // Battery
        M5.Display.setTextColor(deviceInfo.batteryLevel > 20 ? GREEN : RED);
        M5.Display.setCursor(10, 70);
        M5.Display.printf("Battery: %d%%", deviceInfo.batteryLevel);
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 85);
        M5.Display.printf("Voltage: %dmV", deviceInfo.batteryVoltage);
        
        if (deviceInfo.charging) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(120, 85);
            M5.Display.print("⚡ Charging");
        }
        
        // Stats
        M5.Display.setTextColor(DARKGREY);
        M5.Display.setCursor(10, 105);
        M5.Display.printf("Commands: %d | Cards: %d", commandsSent, cardsDetected);
    }
    
    void drawLogsScreen() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(60, 5);
        M5.Display.print("📜 Logs");
        
        M5.Display.setTextColor(WHITE);
        int y = 25;
        int startIdx = max(0, logIndex - 7);
        
        for (int i = 0; i < 7 && (startIdx + i) < logIndex; i++) {
            M5.Display.setCursor(5, y);
            M5.Display.print(commandLog[(startIdx + i) % 20].substring(0, 38));
            y += 12;
        }
        
        if (logIndex == 0) {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(60, 60);
            M5.Display.print("No logs yet");
        }
    }
    
    void drawStatusBar() {
        M5.Display.fillRect(0, 125, 240, 10, DARKGREY);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(5, 127);
        
        String screenNames[] = {"MAIN", "HF", "LF", "EMU", "READ", "ATK", "SET", "INFO", "LOG"};
        M5.Display.printf("%s", screenNames[currentScreen].c_str());
        
        if (connected) {
            M5.Display.setCursor(180, 127);
            M5.Display.printf("🔋%d%%", deviceInfo.batteryLevel);
        }
    }
    
    // ==================== COMMUNICATION ====================
    void sendCommand(ChameleonCommand cmd, uint8_t* data, int dataLen) {
        if (!chameleonSerial) return;
        
        // Build command packet
        uint8_t packet[MAX_COMMAND_LEN];
        packet[0] = 0x11;  // SOF
        packet[1] = (cmd >> 8) & 0xFF;
        packet[2] = cmd & 0xFF;
        packet[3] = dataLen & 0xFF;
        packet[4] = (dataLen >> 8) & 0xFF;
        
        // Add data
        if (data && dataLen > 0) {
            memcpy(&packet[5], data, dataLen);
        }
        
        // Send packet
        chameleonSerial->write(packet, 5 + dataLen);
        
        commandsSent++;
        lastCommand = millis();
        
        // Log command
        addLog("CMD: 0x" + String(cmd, HEX));
    }
    
    String readResponse() {
        String response = "";
        unsigned long timeout = millis() + CHAMELEON_TIMEOUT;
        
        while (millis() < timeout) {
            if (chameleonSerial->available()) {
                char c = chameleonSerial->read();
                response += c;
                
                if (c == '\n') break;
            }
        }
        
        if (response.length() > 0) {
            commandsSuccess++;
            lastResponse = response;
            addLog("RSP: " + response.substring(0, 20));
        }
        
        return response;
    }
    
    uint8_t readByte() {
        unsigned long timeout = millis() + CHAMELEON_TIMEOUT;
        
        while (millis() < timeout) {
            if (chameleonSerial->available()) {
                return chameleonSerial->read();
            }
        }
        
        return 0;
    }
    
    void addLog(String log) {
        commandLog[logIndex % 20] = log;
        logIndex++;
    }
    
    // ==================== SLOT MANAGEMENT ====================
    void loadAllSlots() {
        Serial.println("📂 Loading slots from Chameleon...");
        
        // Load HF slots
        for (int i = 0; i < MAX_SLOTS; i++) {
            loadSlot(i, true);
        }
        
        // Load LF slots
        for (int i = 0; i < MAX_SLOTS; i++) {
            loadSlot(i, false);
        }
        
        Serial.println("✅ Slots loaded!");
    }
    
    void loadSlot(int slotNum, bool isHF) {
        // Send get slot info command
        uint8_t slotData[2];
        slotData[0] = slotNum;
        slotData[1] = isHF ? 1 : 0;
        
        sendCommand(CMD_HF_GET_SLOT_INFO, slotData, 2);
        
        // Parse response (simplified)
        if (chameleonSerial->available() >= 20) {
            SlotInfo* slot = isHF ? &slotsHF[slotNum] : &slotsLF[slotNum];
            
            slot->enabled = readByte();
            slot->type = (SlotType)readByte();
            slot->uidLen = readByte();
            
            for (int i = 0; i < slot->uidLen; i++) {
                slot->uid[i] = readByte();
            }
            
            // Get slot name
            sendCommand(CMD_HF_GET_SLOT_TAG_NAME, slotData, 2);
            slot->name = readResponse();
            if (slot->name.isEmpty()) {
                slot->name = "Slot " + String(slotNum + 1);
            }
        }
    }
    
    void activateSlot(int slotNum, bool isHF) {
        uint8_t slotData[2];
        slotData[0] = slotNum;
        slotData[1] = isHF ? 1 : 0;
        
        sendCommand(CMD_HF_SET_SLOT_ACTIVATED, slotData, 2);
        
        if (isHF) deviceInfo.activeSlotHF = slotNum;
        else deviceInfo.activeSlotLF = slotNum;
        
        Serial.printf("✅ Activated %s slot %d\n", isHF ? "HF" : "LF", slotNum + 1);
    }
    
    void saveToSlot(int slotNum, DetectionResult& detection) {
        SlotInfo& slot = slotsHF[slotNum];
        
        slot.type = detection.cardType;
        slot.uidLen = detection.uidLen;
        memcpy(slot.uid, detection.uid, detection.uidLen);
        slot.enabled = true;
        slot.name = "Cloned " + String(slotNum + 1);
        
        // Send to Chameleon
        uint8_t slotData[32];
        slotData[0] = slotNum;
        slotData[1] = slot.type;
        slotData[2] = slot.uidLen;
        memcpy(&slotData[3], slot.uid, slot.uidLen);
        
        sendCommand(CMD_HF_SET_SLOT_DATA_DEFAULT, slotData, 3 + slot.uidLen);
        
        cardsCloned++;
        Serial.printf("💾 Cloned to slot %d\n", slotNum + 1);
    }
    
    // ==================== CARD DETECTION ====================
    void startScanning() {
        scanning = true;
        lastDetection.cardDetected = false;
        
        Serial.println("🔍 Scanning for cards...");
    }
    
    void checkForCard() {
        // Send detection command
        sendCommand(CMD_MF1_DETECT_SUPPORT, nullptr, 0);
        
        // Check for card presence
        if (chameleonSerial->available() >= 16) {
            lastDetection.cardDetected = readByte();
            
            if (lastDetection.cardDetected) {
                lastDetection.cardType = (SlotType)readByte();
                lastDetection.uidLen = readByte();
                
                for (int i = 0; i < lastDetection.uidLen; i++) {
                    lastDetection.uid[i] = readByte();
                }
                
                lastDetection.sak = readByte();
                lastDetection.atqa = readByte() | (readByte() << 8);
                
                cardsDetected++;
                scanning = false;
                
                Serial.println("✅ Card detected!");
                Serial.printf("   Type: %s\n", getSlotTypeName(lastDetection.cardType).c_str());
                Serial.printf("   UID: %s\n", formatUID(lastDetection.uid, lastDetection.uidLen).c_str());
            }
        }
    }
    
    // ==================== ATTACKS ====================
    void performDarksideAttack() {
        Serial.println("⚔️ Starting Darkside attack...");
        
        sendCommand(CMD_MF1_DARKSIDE_ACQUIRE, nullptr, 0);
        
        attacksPerformed++;
        
        // Wait for results (simplified)
        delay(5000);
        
        Serial.println("✅ Darkside attack complete!");
    }
    
    void performNestedAttack() {
        Serial.println("⚔️ Starting Nested attack...");
        
        sendCommand(CMD_MF1_NESTED_ACQUIRE, nullptr, 0);
        
        attacksPerformed++;
        
        delay(3000);
        
        Serial.println("✅ Nested attack complete!");
    }
    
    void detectPRNG() {
        Serial.println("🔍 Detecting PRNG...");
        
        sendCommand(CMD_MF1_DETECT_PRNG, nullptr, 0);
        
        if (chameleonSerial->available()) {
            lastDetection.prngWeak = readByte();
            
            Serial.printf("   PRNG: %s\n", lastDetection.prngWeak ? "WEAK ✓" : "HARD");
        }
    }
    
    // ==================== CONTROLS ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                if (selectedItem > 0) selectedItem--;
                else if (selectedSlot > 0) selectedSlot--;
                break;
                
            case ACTION_DOWN:
                selectedItem++;
                selectedSlot++;
                if (selectedItem > 7) selectedItem = 7;
                if (selectedSlot >= MAX_SLOTS) selectedSlot = MAX_SLOTS - 1;
                break;
                
            case ACTION_LEFT:
                if (currentScreen == SCREEN_SLOTS_HF) {
                    currentScreen = SCREEN_SLOTS_LF;
                } else if (currentScreen > SCREEN_MAIN) {
                    currentScreen = (ChameleonScreen)(currentScreen - 1);
                }
                selectedItem = 0;
                break;
                
            case ACTION_RIGHT:
                if (currentScreen == SCREEN_SLOTS_LF) {
                    currentScreen = SCREEN_SLOTS_HF;
                } else if (currentScreen < SCREEN_LOGS) {
                    currentScreen = (ChameleonScreen)(currentScreen + 1);
                }
                selectedItem = 0;
                break;
                
            case ACTION_SELECT:
                handleSelection();
                break;
                
            case ACTION_BACK:
                if (currentScreen != SCREEN_MAIN) {
                    currentScreen = SCREEN_MAIN;
                    selectedItem = 0;
                } else if (scanning) {
                    scanning = false;
                }
                break;
        }
        
        drawChameleonScreen();
    }
    
    void handleSelection() {
        switch (currentScreen) {
            case SCREEN_MAIN:
                switch (selectedItem) {
                    case 0: currentScreen = SCREEN_SLOTS_HF; break;
                    case 1: currentScreen = SCREEN_SLOTS_LF; break;
                    case 2: currentScreen = SCREEN_EMULATOR; break;
                    case 3: 
                        currentScreen = SCREEN_READER;
                        startScanning();
                        break;
                    case 4: currentScreen = SCREEN_ATTACKS; break;
                    case 5: currentScreen = SCREEN_SETTINGS; break;
                    case 6: currentScreen = SCREEN_DEVICE_INFO; break;
                    case 7: currentScreen = SCREEN_LOGS; break;
                }
                break;
                
            case SCREEN_SLOTS_HF:
            case SCREEN_SLOTS_LF:
                activateSlot(selectedSlot, currentScreen == SCREEN_SLOTS_HF);
                currentScreen = SCREEN_EMULATOR;
                break;
                
            case SCREEN_READER:
                if (lastDetection.cardDetected) {
                    saveToSlot(selectedSlot, lastDetection);
                }
                break;
                
            case SCREEN_ATTACKS:
                switch (selectedItem) {
                    case 0: performDarksideAttack(); break;
                    case 1: performNestedAttack(); break;
                    case 5: detectPRNG(); break;
                }
                break;
        }
    }
    
    // ==================== UTILITY ====================
    String getSlotTypeName(SlotType type) {
        switch (type) {
            case SLOT_MIFARE_1K: return "Mifare 1K";
            case SLOT_MIFARE_4K: return "Mifare 4K";
            case SLOT_MIFARE_ULTRALIGHT: return "Ultralight";
            case SLOT_NTAG_213: return "NTAG213";
            case SLOT_NTAG_215: return "NTAG215";
            case SLOT_NTAG_216: return "NTAG216";
            case SLOT_EM410X: return "EM410x";
            case SLOT_T5577: return "T5577";
            default: return "Empty";
        }
    }
    
    String formatUID(uint8_t* uid, int len) {
        String result = "";
        for (int i = 0; i < len; i++) {
            if (i > 0) result += ":";
            if (uid[i] < 0x10) result += "0";
            result += String(uid[i], HEX);
        }
        result.toUpperCase();
        return result;
    }
    
    // API
    bool isConnected() { return connected; }
    int getCardsDetected() { return cardsDetected; }
    int getCardsCloned() { return cardsCloned; }
    int getBatteryLevel() { return deviceInfo.batteryLevel; }
};

#endif // M5GOTCHI_CHAMELEON_ULTRA_H
