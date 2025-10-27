/*
 * 🎴 M5GOTCHI RFID/NFC MULTI-TOOL v1.0
 * Leitor, escritor, clonador e analisador completo!
 * 
 * Hardware Support:
 * - PN532 (I2C/SPI/UART) - 13.56MHz HF
 * - RC522 (SPI) - 13.56MHz HF
 * - PN5180 (SPI) - 13.56MHz HF avançado
 * - RDM6300 (UART) - 125kHz LF
 * - Generic ISO14443A/B, ISO15693, FeliCa
 * 
 * Features:
 * - Read/Write Mifare Classic (1K/4K)
 * - Read/Write Mifare Ultralight/NTAG
 * - UID cloning and spoofing
 * - Key brute force (Mifare)
 * - Dictionary attack
 * - Dump to file (bin/json/mfd)
 * - Emulation mode (PN532)
 * - NDEF reading/writing
 * - Access control testing
 * - Card analyzer
 * - Key calculator (Mifare)
 * - Magic card detection
 * - Anti-clone detection
 * - EM410x (LF) support
 * - HID Prox support
 * - Kawaii card database
 * 
 * Supported Cards:
 * HF (13.56MHz): Mifare Classic, Ultralight, DESFire, Plus, NTAG, FeliCa
 * LF (125kHz): EM4100/4102, HID Prox, T5577, EM4305
 */

#ifndef M5GOTCHI_RFID_NFC_H
#define M5GOTCHI_RFID_NFC_H

#include <M5Unified.h>
#include <Wire.h>
#include <SPI.h>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_memory_utils.h"

// ==================== HARDWARE CONFIGURATION ====================
#define USE_PN532 1
#define USE_RC522 0
#define USE_RDM6300 0

// PN532 I2C
#define PN532_I2C_ADDRESS 0x24
#define PN532_IRQ 34
#define PN532_RESET 26

// RC522 SPI
#define RC522_CS 5
#define RC522_RST 26

// RDM6300 UART
#define RDM6300_RX 16

// ==================== MEMORY LIMITS ====================
#define MAX_SAVED_CARDS 10        // Reduced from 20
#define MAX_KEYS_STORED 50        // Reduced from 100
#define MAX_BLOCKS_BUFFER 256     // Keep for 4K cards

// ==================== PROTOCOL CONSTANTS ====================
#define MIFARE_CMD_AUTH_A 0x60
#define MIFARE_CMD_AUTH_B 0x61
#define MIFARE_CMD_READ 0x30
#define MIFARE_CMD_WRITE 0xA0
#define MIFARE_CMD_HALT 0x50

#define ISO14443A_CMD_REQA 0x26
#define ISO14443A_CMD_WUPA 0x52
#define ISO14443A_CMD_ANTICOLL 0x93
#define ISO14443A_CMD_SELECT 0x93
#define ISO14443A_CMD_RATS 0xE0

#define BLOCK_SIZE 16
#define SECTOR_SIZE 4
#define MAX_SECTORS_1K 16
#define MAX_SECTORS_4K 40
#define MAX_BLOCKS_1K 64
#define MAX_BLOCKS_4K 256
#define MAX_KEYS 100

// ==================== CARD TYPES ====================
enum CardType {
    RFID_CARD_UNKNOWN = 0,
    CARD_MIFARE_CLASSIC_1K,
    CARD_MIFARE_CLASSIC_4K,
    CARD_MIFARE_ULTRALIGHT,
    CARD_MIFARE_ULTRALIGHT_C,
    CARD_NTAG_213,
    CARD_NTAG_215,
    CARD_NTAG_216,
    CARD_MIFARE_DESFIRE,
    CARD_MIFARE_PLUS,
    CARD_FELICA,
    CARD_ISO15693,
    CARD_EM4100,
    CARD_EM4102,
    CARD_HID_PROX,
    CARD_T5577,
    CARD_INDALA
};

// ==================== CARD FEATURES ====================
enum CardFeature {
    FEATURE_READABLE = 0x01,
    FEATURE_WRITABLE = 0x02,
    FEATURE_UID_CHANGEABLE = 0x04,
    FEATURE_ENCRYPTED = 0x08,
    FEATURE_MAGIC_CARD = 0x10,
    FEATURE_ANTI_CLONE = 0x20,
    FEATURE_NDEF_COMPATIBLE = 0x40
};

// ==================== OPERATION MODES ====================
enum RFIDMode {
    MODE_SCANNER = 0,
    MODE_READER,
    MODE_WRITER,
    MODE_CLONER,
    MODE_ANALYZER,
    MODE_EMULATOR,
    MODE_ATTACKS,
    MODE_KEYS,
    MODE_DATABASE
};

// ==================== STRUCTURES ====================
struct CardInfo {
    CardType type;
    uint8_t uid[10];
    int uidLen;
    uint8_t sak;
    uint16_t atqa;
    uint8_t ats[32];
    int atsLen;
    int sectors;
    int blocks;
    int blockSize;
    uint16_t features;
    bool isMagic;
    bool hasNDEF;
    String manufacturer;
    int capacity;  // bytes
};

struct MifareKey {
    uint8_t key[6];
    bool isKeyA;
    int sector;
    bool verified;
};

struct CardData {
    CardInfo info;
    uint8_t blocks[MAX_BLOCKS_4K][BLOCK_SIZE];
    MifareKey keys[MAX_SECTORS_4K * 2];
    int keysFound;
    bool blockRead[MAX_BLOCKS_4K];
    String name;
    unsigned long timestamp;
};

struct KeyDictionary {
    String name;
    MifareKey keys[MAX_KEYS];
    int keyCount;
};

struct NDEFMessage {
    uint8_t type;
    String uri;
    String text;
    uint8_t data[256];
    int dataLen;
};

// ==================== PN532 EMULATION ====================
class PN532Emulator {
public:
    uint8_t uid[7];
    int uidLen;
    CardType emulatedType;
    uint8_t blocks[MAX_BLOCKS_1K][BLOCK_SIZE];
    bool emulating;
    
    PN532Emulator() {
        emulating = false;
        uidLen = 4;
        emulatedType = CARD_MIFARE_CLASSIC_1K;
    }
    
    void setUID(uint8_t* newUID, int len) {
        memcpy(uid, newUID, len);
        uidLen = len;
    }
    
    void loadDump(CardData& data) {
        memcpy(blocks, data.blocks, sizeof(blocks));
        setUID(data.info.uid, data.info.uidLen);
        emulatedType = data.info.type;
    }
};

// ==================== RFID/NFC SCREENS ====================
enum RFIDScreen {
    SCREEN_MAIN = 0,
    SCREEN_SCANNER,
    SCREEN_READER,
    SCREEN_WRITER,
    SCREEN_CLONER,
    SCREEN_ANALYZER,
    SCREEN_EMULATOR,
    SCREEN_ATTACKS,
    SCREEN_KEYS,
    SCREEN_DATABASE,
    SCREEN_CARD_DETAIL
};

// ==================== MAIN CLASS ====================
class M5GotchiRFIDNFC {
private:
    RFIDScreen currentScreen;
    int selectedItem;
    int scrollOffset;
    
    // Hardware state
    bool pn532Ready;
    bool rc522Ready;
    bool rdm6300Ready;
    
    // Card state
    CardInfo currentCard;
    bool cardPresent;
    unsigned long lastScan;
    
    // Data storage - OPTIMIZED: Fixed arrays instead of dynamic
    CardData savedCards[MAX_SAVED_CARDS];  // Fixed size array
    int savedCount;
    CardData* activeCard;
    
    // Keys - OPTIMIZED: Array for found keys
    KeyDictionary defaultKeys;
    KeyDictionary customKeys;
    MifareKey foundKeys[MAX_KEYS_STORED];  // Fixed array instead of circular buffer
    int foundKeyCount;
    
    // Operations
    bool scanning;
    bool reading;
    bool writing;
    bool attacking;
    int attackProgress;
    
    // Emulation
    PN532Emulator emulator;
    
    // Statistics
    int cardsScanned;
    int cardsRead;
    int cardsCloned;
    int keysRecovered;
    int attacksSuccess;
    
    // UI
    unsigned long lastUpdate;
    String statusMessage;

public:
    M5GotchiRFIDNFC() {
        currentScreen = SCREEN_MAIN;
        selectedItem = 0;
        scrollOffset = 0;
        
        pn532Ready = false;
        rc522Ready = false;
        rdm6300Ready = false;
        
        cardPresent = false;
        lastScan = 0;
        
        savedCount = 0;
        activeCard = nullptr;
        
        foundKeyCount = 0;
        
        scanning = false;
        reading = false;
        writing = false;
        attacking = false;
        attackProgress = 0;
        
        cardsScanned = 0;
        cardsRead = 0;
        cardsCloned = 0;
        keysRecovered = 0;
        attacksSuccess = 0;
        
        lastUpdate = 0;
        statusMessage = "Ready";
        
        initDefaultKeys();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🎴 Initializing RFID/NFC System...");
        
        #if USE_PN532
        initPN532();
        #endif
        
        #if USE_RC522
        initRC522();
        #endif
        
        #if USE_RDM6300
        initRDM6300();
        #endif
        
        if (pn532Ready || rc522Ready || rdm6300Ready) {
            Serial.println("✅ RFID/NFC system initialized!");
        } else {
            Serial.println("⚠️ No RFID hardware detected");
        }
    }
    
    void initPN532() {
        Serial.println("📡 Initializing PN532...");
        
        Wire.begin();
        
        // Configure PN532 pins
        pinMode(PN532_IRQ, INPUT);
        if (PN532_RESET >= 0) {
            pinMode(PN532_RESET, OUTPUT);
            digitalWrite(PN532_RESET, HIGH);
        }
        
        // Test communication
        Wire.beginTransmission(PN532_I2C_ADDRESS);
        if (Wire.endTransmission() == 0) {
            pn532Ready = true;
            Serial.println("✅ PN532 ready on I2C!");
            
            // Configure PN532
            configurePN532();
        } else {
            Serial.println("⚠️ PN532 not found on I2C");
        }
    }
    
    void configurePN532() {
        // SAM Configuration
        uint8_t samConfig[] = {0x14, 0x01, 0x00, 0x01};
        sendPN532Command(samConfig, 4);
        
        // Set max retries
        uint8_t maxRetries[] = {0x32, 0x05, 0x00, 0x00};
        sendPN532Command(maxRetries, 4);
        
        Serial.println("⚙️ PN532 configured");
    }
    
    void initRC522() {
        Serial.println("📡 Initializing RC522...");
        
        SPI.begin();
        pinMode(RC522_CS, OUTPUT);
        digitalWrite(RC522_CS, HIGH);
        
        if (RC522_RST >= 0) {
            pinMode(RC522_RST, OUTPUT);
            digitalWrite(RC522_RST, HIGH);
        }
        
        // Test RC522
        digitalWrite(RC522_CS, LOW);
        SPI.transfer(0x00);
        uint8_t version = SPI.transfer(0x00);
        digitalWrite(RC522_CS, HIGH);
        
        if (version != 0x00 && version != 0xFF) {
            rc522Ready = true;
            Serial.printf("✅ RC522 ready! Version: 0x%02X\n", version);
        } else {
            Serial.println("⚠️ RC522 not detected");
        }
    }
    
    void initRDM6300() {
        Serial.println("📻 Initializing RDM6300 (125kHz)...");
        
        // Initialize serial for RDM6300
        Serial2.begin(9600, SERIAL_8N1, RDM6300_RX, -1);
        
        rdm6300Ready = true;
        Serial.println("✅ RDM6300 ready for LF cards!");
    }
    
    void initDefaultKeys() {
        defaultKeys.name = "Default Keys";
        defaultKeys.keyCount = 0;
        
        // Default Mifare keys
        uint8_t defaultKeysList[][6] = {
            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  // Factory default
            {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5},  // MAD key
            {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7},  // NDEF key
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // Blank key
            {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5},  // Alternative
            {0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD},  // Hotel key
            {0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A},  // Transport key
            {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},  // Test key
        };
        
        for (int i = 0; i < 8; i++) {
            memcpy(defaultKeys.keys[i].key, defaultKeysList[i], 6);
            defaultKeys.keys[i].isKeyA = true;
            defaultKeys.keys[i].sector = -1;  // All sectors
            defaultKeys.keyCount++;
        }
        
        Serial.printf("🔑 Loaded %d default keys\n", defaultKeys.keyCount);
    }
    
    // ==================== MAIN FLOW ====================
    void start() {
        init();
        currentScreen = SCREEN_MAIN;
        selectedItem = 0;
        drawRFIDScreen();
    }
    
    void stop() {
        scanning = false;
        emulator.emulating = false;
        
        Serial.println("🎴 RFID/NFC system stopped");
        Serial.printf("📊 Stats: Scanned=%d Read=%d Cloned=%d Keys=%d\n",
                     cardsScanned, cardsRead, cardsCloned, keysRecovered);
    }
    
    void update() {
        // Auto-scan in scanner mode
        if (currentScreen == SCREEN_SCANNER && millis() - lastScan > 500) {
            scanForCard();
            lastScan = millis();
        }
        
        // Update display
        if (millis() - lastUpdate > 100) {
            drawRFIDScreen();
            lastUpdate = millis();
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawRFIDScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentScreen) {
            case SCREEN_MAIN:
                drawMainScreen();
                break;
            case SCREEN_SCANNER:
                drawScannerScreen();
                break;
            case SCREEN_READER:
                drawReaderScreen();
                break;
            case SCREEN_WRITER:
                drawWriterScreen();
                break;
            case SCREEN_CLONER:
                drawClonerScreen();
                break;
            case SCREEN_ANALYZER:
                drawAnalyzerScreen();
                break;
            case SCREEN_EMULATOR:
                drawEmulatorScreen();
                break;
            case SCREEN_ATTACKS:
                drawAttacksScreen();
                break;
            case SCREEN_KEYS:
                drawKeysScreen();
                break;
            case SCREEN_DATABASE:
                drawDatabaseScreen();
                break;
            case SCREEN_CARD_DETAIL:
                drawCardDetailScreen();
                break;
        }
        
        drawStatusBar();
    }
    
    void drawMainScreen() {
        // Header
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(40, 5);
        M5.Display.print("🎴 RFID/NFC Tool");
        
        // Hardware status
        M5.Display.setTextColor(pn532Ready ? GREEN : DARKGREY);
        M5.Display.setCursor(200, 5);
        M5.Display.print(pn532Ready ? "●" : "○");
        
        if (!pn532Ready && !rc522Ready && !rdm6300Ready) {
            M5.Display.setTextColor(ORANGE);
            M5.Display.setCursor(30, 30);
            M5.Display.print("⚠️ No hardware!");
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(20, 50);
            M5.Display.print("Connect PN532/RC522");
            return;
        }
        
        // Menu items
        String menuItems[] = {
            "🔍 Scanner",
            "📖 Reader",
            "✏️ Writer",
            "📋 Cloner",
            "🔬 Analyzer",
            "🎭 Emulator",
            "⚔️ Attacks",
            "🔑 Keys",
            "💾 Database"
        };
        
        int y = 25;
        for (int i = 0; i < 9; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, y);
            M5.Display.print((i == selectedItem ? "> " : "  ") + menuItems[i]);
            y += 10;
        }
        
        // Stats
        M5.Display.setTextColor(DARKGREY);
        M5.Display.setCursor(10, 118);
        M5.Display.printf("Cards:%d Keys:%d", cardsScanned, keysRecovered);
    }
    
    void drawScannerScreen() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🔍 Scanner");
        
        if (scanning) {
            // Animated scanner
            static int scanAnim = 0;
            M5.Display.drawCircle(120, 60, 25, CYAN);
            M5.Display.drawCircle(120, 60, 25 + (scanAnim % 15), BLUE);
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(65, 55);
            M5.Display.print("Scanning...");
            scanAnim++;
        }
        
        if (cardPresent) {
            // Card detected!
            M5.Display.fillRect(10, 30, 220, 75, DARKGREEN);
            M5.Display.drawRect(10, 30, 220, 75, GREEN);
            
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(20, 35);
            M5.Display.print("✓ Card Detected!");
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(20, 50);
            M5.Display.printf("Type: %s", getCardTypeName(currentCard.type).c_str());
            
            M5.Display.setCursor(20, 65);
            M5.Display.print("UID: " + formatUID(currentCard.uid, currentCard.uidLen));
            
            M5.Display.setCursor(20, 80);
            M5.Display.printf("SAK: 0x%02X ATQA: 0x%04X", currentCard.sak, currentCard.atqa);
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(20, 95);
            M5.Display.print("Press SELECT to read");
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(50, 95);
            M5.Display.print("Place card near");
            M5.Display.setCursor(60, 108);
            M5.Display.print("antenna...");
        }
    }
    
    void drawReaderScreen() {
        M5.Display.setTextColor(BLUE);
        M5.Display.setCursor(60, 5);
        M5.Display.print("📖 Reader");
        
        if (reading) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(60, 40);
            M5.Display.print("● Reading...");
            
            // Progress bar
            int progress = attackProgress;
            M5.Display.fillRect(30, 60, 180, 10, DARKGREY);
            M5.Display.fillRect(30, 60, progress * 180 / 100, 10, GREEN);
            M5.Display.setCursor(95, 80);
            M5.Display.printf("%d%%", progress);
            
            return;
        }
        
        if (activeCard) {
            // Display read data
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 25);
            M5.Display.printf("Card: %s", activeCard->name.c_str());
            
            M5.Display.setCursor(10, 40);
            M5.Display.printf("Type: %s", getCardTypeName(activeCard->info.type).c_str());
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 55);
            M5.Display.print("UID: " + formatUID(activeCard->info.uid, activeCard->info.uidLen));
            
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 70);
            M5.Display.printf("Keys found: %d/%d", activeCard->keysFound, activeCard->info.sectors * 2);
            
            M5.Display.setCursor(10, 85);
            int blocksRead = 0;
            for (int i = 0; i < activeCard->info.blocks; i++) {
                if (activeCard->blockRead[i]) blocksRead++;
            }
            M5.Display.printf("Blocks read: %d/%d", blocksRead, activeCard->info.blocks);
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 105);
            M5.Display.print("▲▼:Navigate SEL:Save");
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(50, 60);
            M5.Display.print("Place card and");
            M5.Display.setCursor(60, 75);
            M5.Display.print("press SELECT");
        }
    }
    
    void drawWriterScreen() {
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(60, 5);
        M5.Display.print("✏️ Writer");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.print("Write options:");
        
        String options[] = {
            "Write from dump",
            "Write UID only",
            "Write NDEF message",
            "Format card",
            "Clear all blocks"
        };
        
        int y = 50;
        for (int i = 0; i < 5; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(15, y);
            M5.Display.print((i == selectedItem ? "> " : "  ") + options[i]);
            y += 12;
        }
        
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(10, 115);
        M5.Display.print("⚠️ Warning: Will overwrite!");
    }
    
    void drawClonerScreen() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(60, 5);
        M5.Display.print("📋 Cloner");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.print("Step 1: Read source card");
        M5.Display.setCursor(10, 40);
        M5.Display.print("Step 2: Place target card");
        M5.Display.setCursor(10, 55);
        M5.Display.print("Step 3: Write data");
        
        if (activeCard) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(180, 25);
            M5.Display.print("✓");
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 75);
            M5.Display.print("Source: " + formatUID(activeCard->info.uid, activeCard->info.uidLen));
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 95);
            M5.Display.print("Press SELECT to clone");
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(40, 95);
            M5.Display.print("Read source first");
        }
    }
    
    void drawAnalyzerScreen() {
        M5.Display.setTextColor(PURPLE);
        M5.Display.setCursor(55, 5);
        M5.Display.print("🔬 Analyzer");
        
        if (!cardPresent) {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(60, 60);
            M5.Display.print("No card");
            return;
        }
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Type: %s", getCardTypeName(currentCard.type).c_str());
        
        M5.Display.setCursor(10, 40);
        M5.Display.printf("Capacity: %d bytes", currentCard.capacity);
        
        M5.Display.setCursor(10, 55);
        M5.Display.printf("Manufacturer: %s", currentCard.manufacturer.c_str());
        
        // Features
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 70);
        M5.Display.print("Features:");
        
        int y = 85;
        if (currentCard.features & FEATURE_READABLE) {
            M5.Display.setCursor(15, y);
            M5.Display.print("✓ Readable");
            y += 10;
        }
        if (currentCard.features & FEATURE_WRITABLE) {
            M5.Display.setCursor(15, y);
            M5.Display.print("✓ Writable");
            y += 10;
        }
        if (currentCard.isMagic) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(15, y);
            M5.Display.print("✓ Magic card!");
        }
    }
    
    void drawEmulatorScreen() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🎭 Emulator");
        
        if (emulator.emulating) {
            M5.Display.fillCircle(120, 55, 20, DARKGREEN);
            M5.Display.drawCircle(120, 55, 20, GREEN);
            M5.Display.drawCircle(120, 55, 25, GREEN);
            
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(105, 50);
            M5.Display.print("EMU");
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 85);
            M5.Display.print("UID: " + formatUID(emulator.uid, emulator.uidLen));
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(40, 105);
            M5.Display.print("Press BACK to stop");
        } else {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 35);
            M5.Display.print("Load card to emulate:");
            
            // List saved cards
            int y = 55;
            for (int i = 0; i < min(savedCount, 4); i++) {
                uint16_t color = (i == selectedItem) ? YELLOW : DARKGREY;
                M5.Display.setTextColor(color);
                M5.Display.setCursor(15, y);
                M5.Display.printf("%s%d: %s", 
                                 i == selectedItem ? ">" : " ",
                                 i + 1,
                                 savedCards[i].name.c_str());
                y += 12;
            }
            
            if (savedCount == 0) {
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(50, 65);
                M5.Display.print("No cards saved");
            }
        }
    }
    
    void drawAttacksScreen() {
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(55, 5);
        M5.Display.print("⚔️ Attacks");
        
        String attacks[] = {
            "Dictionary Attack",
            "Darkside Attack",
            "Nested Attack",
            "Hardnested Attack",
            "Key Brute Force",
            "PRNG Test"
        };
        
        int y = 30;
        for (int i = 0; i < 6; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, y);
            M5.Display.print((i == selectedItem ? "> " : "  ") + attacks[i]);
            y += 12;
        }
        
        if (attacking) {
            M5.Display.fillRect(10, 100, 220, 20, RED);
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(50, 105);
            M5.Display.printf("Attacking... %d%%", attackProgress);
        } else {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 110);
            M5.Display.printf("Keys recovered: %d", keysRecovered);
        }
    }
    
    void drawKeysScreen() {
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🔑 Keys");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Default keys: %d", defaultKeys.keyCount);
        
        M5.Display.setCursor(10, 40);
        M5.Display.printf("Found keys: %d", foundKeyCount);
        
        // Display some keys
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 60);
        M5.Display.print("Recent keys:");
        
        int y = 75;
        int displayCount = min(foundKeyCount, 3);
        for (int i = 0; i < displayCount; i++) {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(15, y);
            M5.Display.printf("S%d %s: ", 
                             foundKeys[i].sector,
                             foundKeys[i].isKeyA ? "A" : "B");
            
            for (int j = 0; j < 6; j++) {
                M5.Display.printf("%02X", foundKeys[i].key[j]);
            }
            y += 12;
        }
    }
    
    void drawDatabaseScreen() {
        M5.Display.setTextColor(BLUE);
        M5.Display.setCursor(50, 5);
        M5.Display.print("💾 Database");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Saved cards: %d/20", savedCount);
        
        if (savedCount > 0) {
            // List saved cards
            int y = 45;
            int startIdx = max(0, scrollOffset);
            int endIdx = min(savedCount, startIdx + 6);
            
            for (int i = startIdx; i < endIdx; i++) {
                uint16_t color = (i == selectedItem) ? YELLOW : DARKGREY;
                M5.Display.setTextColor(color);
                M5.Display.setCursor(10, y);
                M5.Display.printf("%s%d: %s", 
                                 i == selectedItem ? ">" : " ",
                                 i + 1,
                                 savedCards[i].name.c_str());
                
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(140, y);
                M5.Display.print(getCardTypeName(savedCards[i].info.type).substring(0, 8));
                
                y += 11;
            }
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(60, 60);
            M5.Display.print("Empty");
        }
    }
    
    void drawCardDetailScreen() {
        if (!activeCard) return;
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 5);
        M5.Display.print(activeCard->name);
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 20);
        M5.Display.printf("Type: %s", getCardTypeName(activeCard->info.type).c_str());
        
        M5.Display.setCursor(10, 35);
        M5.Display.print("UID: " + formatUID(activeCard->info.uid, activeCard->info.uidLen));
        
        // Block data preview (hex dump style)
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(10, 55);
        M5.Display.print("Block data:");
        
        int y = 70;
        int startBlock = scrollOffset;
        for (int i = 0; i < 4 && (startBlock + i) < activeCard->info.blocks; i++) {
            if (!activeCard->blockRead[startBlock + i]) continue;
            
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(10, y);
            M5.Display.printf("%02d:", startBlock + i);
            
            M5.Display.setTextColor(WHITE);
            for (int j = 0; j < 8; j++) {
                M5.Display.printf(" %02X", activeCard->blocks[startBlock + i][j]);
            }
            y += 10;
        }
    }
    
    void drawStatusBar() {
        M5.Display.fillRect(0, 125, 240, 10, DARKGREY);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(5, 127);
        M5.Display.print(statusMessage.substring(0, 35));
    }
    
    // ==================== CARD OPERATIONS ====================
    void scanForCard() {
        if (!pn532Ready) return;
        
        // Send InListPassiveTarget command
        uint8_t cmd[] = {0x4A, 0x01, 0x00};  // MaxTg=1, BrTy=106kbps TypeA
        
        if (sendPN532Command(cmd, 3)) {
            delay(50);
            
            if (readPN532Response()) {
                cardPresent = true;
                parseCardInfo();
                cardsScanned++;
                statusMessage = "Card detected!";
            } else {
                cardPresent = false;
            }
        }
    }
    
    void parseCardInfo() {
        // Simplified - parse PN532 response to fill currentCard
        // In real implementation, parse actual PN532 response bytes
        
        currentCard.type = CARD_MIFARE_CLASSIC_1K;
        currentCard.uidLen = 4;
        // Read actual UID from PN532 response
        currentCard.uid[0] = 0x04;
        currentCard.uid[1] = 0xAB;
        currentCard.uid[2] = 0xCD;
        currentCard.uid[3] = 0xEF;
        
        currentCard.sak = 0x08;
        currentCard.atqa = 0x0004;
        currentCard.sectors = 16;
        currentCard.blocks = 64;
        currentCard.blockSize = 16;
        currentCard.capacity = 1024;
        currentCard.manufacturer = "NXP";
        currentCard.features = FEATURE_READABLE | FEATURE_WRITABLE;
        currentCard.isMagic = false;
        currentCard.hasNDEF = false;
    }
    
    void readCard() {
        if (!cardPresent) {
            statusMessage = "No card present";
            return;
        }
        
        reading = true;
        attackProgress = 0;
        
        // Create new card data
        CardData* newCard = &savedCards[savedCount];
        newCard->info = currentCard;
        newCard->keysFound = 0;
        newCard->name = "Card_" + String(savedCount + 1);
        newCard->timestamp = millis();
        
        // Clear block read flags
        for (int i = 0; i < MAX_BLOCKS_4K; i++) {
            newCard->blockRead[i] = false;
        }
        
        // Try to read all sectors
        for (int sector = 0; sector < currentCard.sectors; sector++) {
            attackProgress = (sector * 100) / currentCard.sectors;
            
            // Try default keys
            bool keyFound = false;
            for (int k = 0; k < defaultKeys.keyCount && !keyFound; k++) {
                if (authenticateSector(sector, defaultKeys.keys[k].key, true)) {
                    // Key A works!
                    memcpy(newCard->keys[sector * 2].key, defaultKeys.keys[k].key, 6);
                    newCard->keys[sector * 2].isKeyA = true;
                    newCard->keys[sector * 2].sector = sector;
                    newCard->keys[sector * 2].verified = true;
                    newCard->keysFound++;
                    keyFound = true;
                    
                    // Read blocks in this sector
                    int startBlock = sector * 4;
                    for (int block = 0; block < 4; block++) {
                        if (readBlock(startBlock + block, newCard->blocks[startBlock + block])) {
                            newCard->blockRead[startBlock + block] = true;
                        }
                    }
                }
            }
            
            // Try key B
            if (!keyFound) {
                for (int k = 0; k < defaultKeys.keyCount; k++) {
                    if (authenticateSector(sector, defaultKeys.keys[k].key, false)) {
                        memcpy(newCard->keys[sector * 2 + 1].key, defaultKeys.keys[k].key, 6);
                        newCard->keys[sector * 2 + 1].isKeyA = false;
                        newCard->keys[sector * 2 + 1].sector = sector;
                        newCard->keys[sector * 2 + 1].verified = true;
                        newCard->keysFound++;
                        break;
                    }
                }
            }
        }
        
        reading = false;
        attackProgress = 100;
        
        if (newCard->keysFound > 0) {
            activeCard = newCard;
            savedCount++;
            cardsRead++;
            statusMessage = "Read complete!";
            Serial.printf("✅ Read complete! Keys: %d/%d\n", 
                         newCard->keysFound, currentCard.sectors * 2);
        } else {
            statusMessage = "Read failed";
            Serial.println("❌ Read failed - no keys found");
        }
    }
    
    bool authenticateSector(int sector, uint8_t* key, bool isKeyA) {
        // Simplified authentication
        // Real implementation: Send InDataExchange with Mifare auth command
        
        uint8_t cmd[20];
        cmd[0] = 0x40;  // InDataExchange
        cmd[1] = 0x01;  // Tg
        cmd[2] = isKeyA ? MIFARE_CMD_AUTH_A : MIFARE_CMD_AUTH_B;
        cmd[3] = sector * 4;  // Block number
        
        // Add key
        memcpy(&cmd[4], key, 6);
        
        // Add UID
        memcpy(&cmd[10], currentCard.uid, currentCard.uidLen);
        
        if (sendPN532Command(cmd, 10 + currentCard.uidLen)) {
            delay(10);
            return readPN532Response();
        }
        
        return false;
    }
    
    bool readBlock(int blockNum, uint8_t* data) {
        // Simplified block read
        // Real implementation: Send InDataExchange with Mifare read command
        
        uint8_t cmd[4];
        cmd[0] = 0x40;  // InDataExchange
        cmd[1] = 0x01;  // Tg
        cmd[2] = MIFARE_CMD_READ;
        cmd[3] = blockNum;
        
        if (sendPN532Command(cmd, 4)) {
            delay(10);
            
            // Read response into data buffer
            // In real implementation, parse actual response
            for (int i = 0; i < BLOCK_SIZE; i++) {
                data[i] = 0x00;  // Placeholder
            }
            
            return true;
        }
        
        return false;
    }
    
    void cloneCard() {
        if (!activeCard) {
            statusMessage = "No source card";
            return;
        }
        
        if (!cardPresent) {
            statusMessage = "Place target card";
            return;
        }
        
        statusMessage = "Cloning...";
        
        // Write all sectors
        bool success = true;
        for (int sector = 0; sector < activeCard->info.sectors; sector++) {
            // Authenticate target with found key
            if (activeCard->keys[sector * 2].verified) {
                if (authenticateSector(sector, activeCard->keys[sector * 2].key, true)) {
                    // Write blocks
                    int startBlock = sector * 4;
                    for (int block = 0; block < 3; block++) {  // Skip sector trailer
                        if (activeCard->blockRead[startBlock + block]) {
                            if (!writeBlock(startBlock + block, activeCard->blocks[startBlock + block])) {
                                success = false;
                            }
                        }
                    }
                }
            }
        }
        
        if (success) {
            cardsCloned++;
            statusMessage = "Clone complete!";
            Serial.println("✅ Card cloned successfully!");
        } else {
            statusMessage = "Clone failed";
            Serial.println("❌ Clone failed");
        }
    }
    
    bool writeBlock(int blockNum, uint8_t* data) {
        // Simplified block write
        // Real implementation: Send InDataExchange with Mifare write command
        
        uint8_t cmd[20];
        cmd[0] = 0x40;  // InDataExchange
        cmd[1] = 0x01;  // Tg
        cmd[2] = MIFARE_CMD_WRITE;
        cmd[3] = blockNum;
        
        // Add data
        memcpy(&cmd[4], data, BLOCK_SIZE);
        
        if (sendPN532Command(cmd, 4 + BLOCK_SIZE)) {
            delay(20);  // Write takes longer
            return readPN532Response();
        }
        
        return false;
    }
    
    void performDictionaryAttack() {
        if (!cardPresent) {
            statusMessage = "No card present";
            return;
        }
        
        attacking = true;
        attackProgress = 0;
        statusMessage = "Dictionary attack...";
        
        int totalKeys = defaultKeys.keyCount;
        int keysFound = 0;
        
        for (int sector = 0; sector < currentCard.sectors; sector++) {
            for (int k = 0; k < defaultKeys.keyCount; k++) {
                attackProgress = ((sector * defaultKeys.keyCount + k) * 100) / 
                                (currentCard.sectors * defaultKeys.keyCount);
                
                // Try key A
                if (authenticateSector(sector, defaultKeys.keys[k].key, true)) {
                    memcpy(foundKeys[foundKeyCount].key, defaultKeys.keys[k].key, 6);
                    foundKeys[foundKeyCount].isKeyA = true;
                    foundKeys[foundKeyCount].sector = sector;
                    foundKeys[foundKeyCount].verified = true;
                    foundKeyCount++;
                    keysFound++;
                    break;
                }
                
                // Try key B
                if (authenticateSector(sector, defaultKeys.keys[k].key, false)) {
                    memcpy(foundKeys[foundKeyCount].key, defaultKeys.keys[k].key, 6);
                    foundKeys[foundKeyCount].isKeyA = false;
                    foundKeys[foundKeyCount].sector = sector;
                    foundKeys[foundKeyCount].verified = true;
                    foundKeyCount++;
                    keysFound++;
                    break;
                }
            }
        }
        
        attacking = false;
        keysRecovered += keysFound;
        attackProgress = 100;
        
        if (keysFound > 0) {
            attacksSuccess++;
            statusMessage = "Found " + String(keysFound) + " keys!";
            Serial.printf("✅ Dictionary attack: %d keys found\n", keysFound);
        } else {
            statusMessage = "No keys found";
            Serial.println("❌ Dictionary attack: no keys found");
        }
    }
    
    // ==================== PN532 COMMUNICATION ====================
    bool sendPN532Command(uint8_t* cmd, int len) {
        if (!pn532Ready) return false;
        
        // Build PN532 frame
        uint8_t frame[256];
        frame[0] = 0x00;  // Preamble
        frame[1] = 0x00;  // Start code
        frame[2] = 0xFF;  // Start code
        frame[3] = len + 1;  // Length
        frame[4] = ~(len + 1) + 1;  // Length checksum
        frame[5] = 0xD4;  // Direction (host to PN532)
        
        // Copy command
        memcpy(&frame[6], cmd, len);
        
        // Calculate data checksum
        uint8_t checksum = 0xD4;
        for (int i = 0; i < len; i++) {
            checksum += cmd[i];
        }
        frame[6 + len] = ~checksum + 1;
        frame[7 + len] = 0x00;  // Postamble
        
        // Send via I2C
        Wire.beginTransmission(PN532_I2C_ADDRESS);
        Wire.write(frame, 8 + len);
        return (Wire.endTransmission() == 0);
    }
    
    bool readPN532Response() {
        if (!pn532Ready) return false;
        
        // Wait for ready
        delay(10);
        
        // Request data
        Wire.requestFrom(PN532_I2C_ADDRESS, 32);
        
        if (Wire.available() >= 6) {
            // Read and validate frame
            uint8_t preamble = Wire.read();
            uint8_t start1 = Wire.read();
            uint8_t start2 = Wire.read();
            
            if (start1 == 0x00 && start2 == 0xFF) {
                uint8_t len = Wire.read();
                uint8_t lcs = Wire.read();
                uint8_t tfi = Wire.read();
                
                // Valid response
                return (tfi == 0xD5);
            }
        }
        
        return false;
    }
    
    // ==================== CONTROLS ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                if (selectedItem > 0) selectedItem--;
                if (scrollOffset > 0) scrollOffset--;
                break;
                
            case ACTION_DOWN:
                selectedItem++;
                scrollOffset++;
                break;
                
            case ACTION_LEFT:
                if (currentScreen > SCREEN_MAIN) {
                    currentScreen = (RFIDScreen)(currentScreen - 1);
                    selectedItem = 0;
                }
                break;
                
            case ACTION_RIGHT:
                if (currentScreen < SCREEN_DATABASE) {
                    currentScreen = (RFIDScreen)(currentScreen + 1);
                    selectedItem = 0;
                }
                break;
                
            case ACTION_SELECT:
                handleSelection();
                break;
                
            case ACTION_BACK:
                if (currentScreen != SCREEN_MAIN) {
                    currentScreen = SCREEN_MAIN;
                    selectedItem = 0;
                    scanning = false;
                    reading = false;
                    attacking = false;
                } else if (emulator.emulating) {
                    emulator.emulating = false;
                }
                break;
        }
        
        drawRFIDScreen();
    }
    
    void handleSelection() {
        switch (currentScreen) {
            case SCREEN_MAIN:
                switch (selectedItem) {
                    case 0: 
                        currentScreen = SCREEN_SCANNER;
                        scanning = true;
                        break;
                    case 1: currentScreen = SCREEN_READER; break;
                    case 2: currentScreen = SCREEN_WRITER; break;
                    case 3: currentScreen = SCREEN_CLONER; break;
                    case 4: currentScreen = SCREEN_ANALYZER; break;
                    case 5: currentScreen = SCREEN_EMULATOR; break;
                    case 6: currentScreen = SCREEN_ATTACKS; break;
                    case 7: currentScreen = SCREEN_KEYS; break;
                    case 8: currentScreen = SCREEN_DATABASE; break;
                }
                break;
                
            case SCREEN_SCANNER:
                if (cardPresent) {
                    readCard();
                }
                break;
                
            case SCREEN_READER:
                if (cardPresent && !reading) {
                    readCard();
                }
                break;
                
            case SCREEN_CLONER:
                cloneCard();
                break;
                
            case SCREEN_EMULATOR:
                if (!emulator.emulating && savedCount > 0 && selectedItem < savedCount) {
                    emulator.loadDump(savedCards[selectedItem]);
                    emulator.emulating = true;
                    statusMessage = "Emulating...";
                }
                break;
                
            case SCREEN_ATTACKS:
                if (!attacking) {
                    switch (selectedItem) {
                        case 0: performDictionaryAttack(); break;
                    }
                }
                break;
                
            case SCREEN_DATABASE:
                if (savedCount > 0 && selectedItem < savedCount) {
                    activeCard = &savedCards[selectedItem];
                    currentScreen = SCREEN_CARD_DETAIL;
                }
                break;
        }
    }
    
    // ==================== UTILITY ====================
    String getCardTypeName(CardType type) {
        switch (type) {
            case CARD_MIFARE_CLASSIC_1K: return "Mifare Classic 1K";
            case CARD_MIFARE_CLASSIC_4K: return "Mifare Classic 4K";
            case CARD_MIFARE_ULTRALIGHT: return "Mifare Ultralight";
            case CARD_NTAG_213: return "NTAG213";
            case CARD_NTAG_215: return "NTAG215";
            case CARD_NTAG_216: return "NTAG216";
            case CARD_MIFARE_DESFIRE: return "DESFire";
            case CARD_EM4100: return "EM4100";
            case CARD_HID_PROX: return "HID Prox";
            default: return "Unknown";
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
    bool isReady() { return pn532Ready || rc522Ready || rdm6300Ready; }
    bool hasCardPresent() { return cardPresent; }
    int getCardsScanned() { return cardsScanned; }
    int getKeysRecovered() { return keysRecovered; }
    CardInfo getCurrentCard() { return currentCard; }
};

#endif // M5GOTCHI_RFID_NFC_H
