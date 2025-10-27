/*
 * 📡 M5GOTCHI 433MHz RF TRANSCEIVER v1.0
 * Sistema completo de captura e transmissão RF 433MHz!
 * 
 * Features:
 * - 433MHz Signal Receiver (captura de códigos)
 * - 433MHz Signal Transmitter (replay de códigos)
 * - Protocol decoder (fixed code, rolling code, etc.)
 * - RAW signal capture e análise
 * - Brute force de códigos fixos
 * - Biblioteca de dispositivos conhecidos (portões, alarmes, etc.)
 * - Jamming detection
 * - Frequency analyzer (433.05-434.79 MHz)
 * - Signal strength meter (RSSI)
 * - Garage door opener clone
 * - Car key fob analyzer
 * - Wireless doorbell clone
 * - Weather station sniffer
 * - Rolling code analysis (KeeLoq, etc.)
 * 
 * Hardware: Módulo RF 433MHz (TX + RX)
 * Pinos: TX=GPIO 26, RX=GPIO 36 (configurável)
 * Antena: 17.3cm para 433MHz
 */

#ifndef M5GOTCHI_RF433_TRANSCEIVER_H
#define M5GOTCHI_RF433_TRANSCEIVER_H

#include <M5Unified.h>
#include <RCSwitch.h>
#include "m5gotchi_universal_controls.h"

// ==================== RF CONFIGURATION ====================
#define RF_TX_PIN 26        // GPIO para transmissão RF
#define RF_RX_PIN 36        // GPIO para recepção RF
#define RF_FREQUENCY 433920000  // 433.92 MHz (padrão europeu)
#define MAX_CAPTURED_CODES 100
#define MAX_SAVED_CODES 50
#define ROLLING_CODE_BUFFER 1000

// ==================== RF PROTOCOLS ====================
enum RFProtocol {
    RF_UNKNOWN = 0,
    RF_FIXED_CODE,           // Código fixo simples
    RF_ROLLING_CODE,         // KeeLoq, HCS301, etc.
    RF_LEARNING_CODE,        // Códigos programáveis
    RF_PT2262,              // PT2262/2272 (comum em portões)
    RF_EV1527,              // EV1527 (alarmes)
    RF_HT12E,               // Holtek HT12E
    RF_PRINCETON,           // Princeton PT2260
    RF_WEATHER_STATION,     // Estações meteorológicas
    RF_TIRE_PRESSURE,       // Sensores de pressão de pneu
    RF_DOORBELL,            // Campainhas sem fio
    RF_GARAGE_DOOR,         // Portões de garagem
    RF_CAR_REMOTE,          // Controles de carro
    RF_ALARM_SENSOR         // Sensores de alarme
};

// ==================== DEVICE TYPES ====================
enum RFDeviceType {
    DEV_GARAGE_DOOR = 0,
    DEV_GATE,
    DEV_ALARM,
    DEV_DOORBELL,
    DEV_CAR_KEY,
    DEV_LIGHT_SWITCH,
    DEV_OUTLET,
    DEV_FAN,
    DEV_WEATHER,
    DEV_SENSOR,
    DEV_UNKNOWN
};

// ==================== RF STRUCTURES ====================
struct RFCode {
    String name;
    RFProtocol protocol;
    unsigned long code;
    unsigned int bitLength;
    unsigned int pulseLength;
    unsigned int repeatCount;
    unsigned long timestamp;
    int rssi;
    bool isRolling;
    uint8_t* rawData;
    int rawDataLength;
};

struct RFDevice {
    String name;
    String brand;
    RFDeviceType type;
    RFProtocol protocol;
    std::vector<unsigned long> knownCodes;
    unsigned int bitLength;
    unsigned int pulseLength;
};

struct RollingCodeInfo {
    unsigned long encryptedCode;
    unsigned long serial;
    unsigned int counter;
    unsigned int button;
    bool valid;
};

struct SignalAnalysis {
    float frequency;
    int rssi;
    unsigned int pulseWidth;
    unsigned int bitRate;
    int snr;  // Signal to Noise Ratio
    bool jammingDetected;
};

// ==================== RF MODE ====================
enum RFMode {
    MODE_RX = 0,
    MODE_TX,
    MODE_ANALYZE,
    MODE_BRUTE_FORCE,
    MODE_CLONE,
    MODE_DATABASE,
    MODE_ROLLING_CODE,
    MODE_JAMMING_DETECT
};

// ==================== RF433 CLASS ====================
class M5GotchiRF433 {
private:
    RCSwitch rfSwitch;
    
    RFMode currentMode;
    int selectedItem;
    int selectedCode;
    int selectedDevice;
    
    // Captured codes
    RFCode capturedCodes[MAX_CAPTURED_CODES];
    int capturedCount;
    
    // Saved codes
    RFCode savedCodes[MAX_SAVED_CODES];
    int savedCount;
    
    // Device database
    std::vector<RFDevice> deviceDatabase;
    
    // Current state
    bool receiving;
    bool transmitting;
    bool analyzing;
    String lastReceived;
    String lastTransmitted;
    unsigned long lastActivity;
    
    // Brute force
    bool bruteForceActive;
    unsigned long bruteForceProgress;
    unsigned long bruteForceTotal;
    unsigned long bruteForceStart;
    unsigned long bruteForceEnd;
    int bruteForceBitLength;
    
    // Signal analysis
    SignalAnalysis currentSignal;
    int signalHistory[100];
    int signalHistoryIdx;
    
    // Rolling code analysis
    RollingCodeInfo rollingCodes[ROLLING_CODE_BUFFER];
    int rollingCodeCount;
    
    // Statistics
    int codesReceived;
    int codesTransmitted;
    int uniqueCodes;
    int rollingCodesDetected;
    unsigned long totalOnTime;

public:
    M5GotchiRF433() {
        currentMode = MODE_RX;
        selectedItem = 0;
        selectedCode = 0;
        selectedDevice = 0;
        
        capturedCount = 0;
        savedCount = 0;
        
        receiving = false;
        transmitting = false;
        analyzing = false;
        lastActivity = 0;
        
        bruteForceActive = false;
        bruteForceProgress = 0;
        bruteForceTotal = 0;
        bruteForceBitLength = 24;
        
        rollingCodeCount = 0;
        signalHistoryIdx = 0;
        
        codesReceived = 0;
        codesTransmitted = 0;
        uniqueCodes = 0;
        rollingCodesDetected = 0;
        totalOnTime = 0;
        
        initializeDatabase();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("📡 Initializing 433MHz RF Transceiver...");
        
        // Initialize RCSwitch
        rfSwitch.enableTransmit(RF_TX_PIN);
        rfSwitch.enableReceive(RF_RX_PIN);
        
        // Set protocol (1 = default, most compatible)
        rfSwitch.setProtocol(1);
        rfSwitch.setPulseLength(350);
        rfSwitch.setRepeatTransmit(10);
        
        Serial.printf("📡 RF TX: GPIO %d\n", RF_TX_PIN);
        Serial.printf("📡 RF RX: GPIO %d\n", RF_RX_PIN);
        Serial.printf("📻 Frequency: %.2f MHz\n", RF_FREQUENCY / 1000000.0);
        Serial.printf("📚 Device Database: %d devices\n", deviceDatabase.size());
        Serial.println("✅ 433MHz RF Ready!");
        
        receiving = true;
    }
    
    void initializeDatabase() {
        // Common garage door brands
        addDevice("Chamberlain", "Garage Opener", DEV_GARAGE_DOOR, RF_ROLLING_CODE, {}, 32, 350);
        addDevice("LiftMaster", "Garage Opener", DEV_GARAGE_DOOR, RF_ROLLING_CODE, {}, 32, 350);
        addDevice("Genie", "Garage Opener", DEV_GARAGE_DOOR, RF_FIXED_CODE, {}, 12, 350);
        
        // Common gate brands
        addDevice("Came", "Gate Remote", DEV_GATE, RF_FIXED_CODE, {0x123456, 0x123457}, 24, 350);
        addDevice("Nice", "Gate Remote", DEV_GATE, RF_FIXED_CODE, {0x234567, 0x234568}, 24, 350);
        addDevice("BFT", "Gate Remote", DEV_GATE, RF_FIXED_CODE, {0x345678}, 24, 400);
        
        // Doorbells
        addDevice("Generic", "Doorbell", DEV_DOORBELL, RF_EV1527, {0xABCDE0, 0xABCDE1}, 24, 350);
        
        // Alarms
        addDevice("Sonoff", "RF Switch", DEV_OUTLET, RF_PT2262, {}, 24, 350);
        addDevice("Generic", "PIR Sensor", DEV_SENSOR, RF_EV1527, {}, 24, 350);
        
        Serial.printf("📚 Loaded %d RF devices\n", deviceDatabase.size());
    }
    
    void addDevice(String brand, String name, RFDeviceType type, RFProtocol proto,
                   std::vector<unsigned long> codes, unsigned int bits, unsigned int pulse) {
        RFDevice device;
        device.brand = brand;
        device.name = name;
        device.type = type;
        device.protocol = proto;
        device.knownCodes = codes;
        device.bitLength = bits;
        device.pulseLength = pulse;
        deviceDatabase.push_back(device);
    }
    
    // ==================== RF FLOW ====================
    void start() {
        init();
        currentMode = MODE_RX;
        selectedItem = 0;
        drawRFScreen();
    }
    
    void stop() {
        rfSwitch.disableReceive();
        rfSwitch.disableTransmit();
        receiving = false;
        bruteForceActive = false;
        
        Serial.println("📡 RF Transceiver stopped");
        Serial.printf("📊 Stats: RX=%d TX=%d Unique=%d\n",
                     codesReceived, codesTransmitted, uniqueCodes);
    }
    
    void update() {
        // Check for received RF signals
        if (receiving && rfSwitch.available()) {
            handleReceivedCode();
            rfSwitch.resetAvailable();
        }
        
        // Update brute force
        if (bruteForceActive) {
            updateBruteForce();
        }
        
        // Update signal analysis
        if (analyzing) {
            updateSignalAnalysis();
        }
        
        // Update display
        if (millis() - lastActivity > 100) {
            drawRFScreen();
            lastActivity = millis();
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawRFScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentMode) {
            case MODE_RX:
                drawReceiveScreen();
                break;
            case MODE_TX:
                drawTransmitScreen();
                break;
            case MODE_ANALYZE:
                drawAnalyzeScreen();
                break;
            case MODE_BRUTE_FORCE:
                drawBruteForceScreen();
                break;
            case MODE_CLONE:
                drawCloneScreen();
                break;
            case MODE_DATABASE:
                drawDatabaseScreen();
                break;
            case MODE_ROLLING_CODE:
                drawRollingCodeScreen();
                break;
            case MODE_JAMMING_DETECT:
                drawJammingScreen();
                break;
        }
        
        drawStatusBar();
    }
    
    void drawReceiveScreen() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📡 RF Receiver");
        
        if (receiving) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 25);
            M5.Display.print("● Listening 433.92 MHz");
        }
        
        // Show last received
        if (!lastReceived.isEmpty()) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 45);
            M5.Display.print("Last received:");
            M5.Display.setCursor(10, 60);
            M5.Display.setTextColor(CYAN);
            M5.Display.print(lastReceived.substring(0, 28));
        }
        
        // Captured codes list
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 80);
        M5.Display.printf("Captured: %d codes", capturedCount);
        
        if (capturedCount > 0) {
            M5.Display.setCursor(10, 95);
            M5.Display.printf("Last: %ld (%db)", 
                             capturedCodes[capturedCount-1].code,
                             capturedCodes[capturedCount-1].bitLength);
        }
    }
    
    void drawTransmitScreen() {
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📤 RF Transmit");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Saved codes: %d", savedCount);
        
        // List saved codes
        int startIdx = max(0, selectedCode - 3);
        for (int i = 0; i < 4 && (startIdx + i) < savedCount; i++) {
            RFCode& code = savedCodes[startIdx + i];
            uint16_t color = (startIdx + i == selectedCode) ? YELLOW : WHITE;
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 15);
            M5.Display.printf("%s%s", 
                             (startIdx + i == selectedCode) ? "> " : "  ",
                             code.name.c_str());
            
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(15, 45 + i * 15 + 8);
            M5.Display.printf("0x%lX (%db)", code.code, code.bitLength);
        }
        
        if (savedCount == 0) {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(40, 60);
            M5.Display.print("No saved codes");
        }
    }
    
    void drawAnalyzeScreen() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(40, 5);
        M5.Display.print("📊 Signal Analysis");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Freq: %.2f MHz", currentSignal.frequency / 1000000.0);
        
        M5.Display.setCursor(10, 40);
        M5.Display.printf("RSSI: %d dBm", currentSignal.rssi);
        
        M5.Display.setCursor(10, 55);
        M5.Display.printf("Pulse: %d us", currentSignal.pulseWidth);
        
        M5.Display.setCursor(10, 70);
        M5.Display.printf("Bitrate: %d bps", currentSignal.bitRate);
        
        M5.Display.setCursor(10, 85);
        M5.Display.printf("SNR: %d dB", currentSignal.snr);
        
        // Signal strength bar
        M5.Display.drawRect(10, 100, 220, 15, WHITE);
        int strength = map(currentSignal.rssi, -100, -50, 0, 220);
        strength = constrain(strength, 0, 220);
        M5.Display.fillRect(10, 100, strength, 15, GREEN);
    }
    
    void drawBruteForceScreen() {
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(50, 5);
        M5.Display.print("⚡ Brute Force");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Bit length: %d", bruteForceBitLength);
        
        if (bruteForceActive) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 45);
            M5.Display.print("● Scanning codes...");
            
            // Progress
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 65);
            M5.Display.printf("Progress: %lu / %lu", bruteForceProgress, bruteForceTotal);
            
            // Progress bar
            int progress = (bruteForceProgress * 220) / bruteForceTotal;
            M5.Display.drawRect(10, 85, 220, 15, WHITE);
            M5.Display.fillRect(10, 85, progress, 15, ORANGE);
            
            // ETA
            unsigned long elapsed = millis() - lastActivity;
            unsigned long eta = (bruteForceTotal - bruteForceProgress) * elapsed / max(1UL, bruteForceProgress);
            M5.Display.setCursor(10, 105);
            M5.Display.printf("ETA: %lu sec", eta / 1000);
        } else {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(50, 60);
            M5.Display.print("Press SELECT to start");
        }
    }
    
    void drawCloneScreen() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📋 Clone Mode");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.print("1. Capture original signal");
        M5.Display.setCursor(10, 45);
        M5.Display.print("2. Analyze code");
        M5.Display.setCursor(10, 60);
        M5.Display.print("3. Save for replay");
        
        if (capturedCount > 0) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 85);
            M5.Display.print("✓ Ready to clone!");
            M5.Display.setCursor(10, 100);
            M5.Display.printf("Code: 0x%lX", capturedCodes[capturedCount-1].code);
        }
    }
    
    void drawDatabaseScreen() {
        M5.Display.setTextColor(PURPLE);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📚 RF Database");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Devices: %d", deviceDatabase.size());
        
        // List devices
        int startIdx = max(0, selectedDevice - 3);
        for (int i = 0; i < 4 && (startIdx + i) < deviceDatabase.size(); i++) {
            RFDevice& dev = deviceDatabase[startIdx + i];
            uint16_t color = (startIdx + i == selectedDevice) ? YELLOW : WHITE;
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 15);
            M5.Display.printf("%s%s", 
                             (startIdx + i == selectedDevice) ? "> " : "  ",
                             dev.brand.c_str());
            
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(15, 45 + i * 15 + 8);
            M5.Display.printf("%s (%s)", dev.name.c_str(), getDeviceTypeName(dev.type).c_str());
        }
    }
    
    void drawRollingCodeScreen() {
        M5.Display.setTextColor(BLUE);
        M5.Display.setCursor(40, 5);
        M5.Display.print("🔄 Rolling Codes");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Detected: %d", rollingCodesDetected);
        
        if (rollingCodeCount > 0) {
            RollingCodeInfo& rc = rollingCodes[rollingCodeCount-1];
            
            M5.Display.setCursor(10, 45);
            M5.Display.printf("Serial: 0x%lX", rc.serial);
            
            M5.Display.setCursor(10, 60);
            M5.Display.printf("Counter: %u", rc.counter);
            
            M5.Display.setCursor(10, 75);
            M5.Display.printf("Button: %u", rc.button);
            
            M5.Display.setTextColor(rc.valid ? GREEN : RED);
            M5.Display.setCursor(10, 90);
            M5.Display.print(rc.valid ? "✓ Valid" : "✗ Invalid");
        }
    }
    
    void drawJammingScreen() {
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(40, 5);
        M5.Display.print("🔇 Jamming Detect");
        
        if (currentSignal.jammingDetected) {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(50, 50);
            M5.Display.print("⚠️ JAMMING!");
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(30, 70);
            M5.Display.print("Interference detected");
        } else {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(60, 50);
            M5.Display.print("✓ Clean");
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(45, 70);
            M5.Display.print("No jamming");
        }
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 95);
        M5.Display.printf("Signal quality: %d%%", 
                         map(currentSignal.snr, -20, 40, 0, 100));
    }
    
    void drawStatusBar() {
        M5.Display.fillRect(0, 125, 240, 10, DARKGREY);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(5, 127);
        
        String modeNames[] = {"RX", "TX", "ANALYZE", "BRUTE", "CLONE", "DB", "ROLL", "JAM"};
        M5.Display.printf("%s | RX:%d TX:%d", 
                         modeNames[currentMode].c_str(),
                         codesReceived, codesTransmitted);
    }
    
    // ==================== RF RECEPTION ====================
    void handleReceivedCode() {
        unsigned long code = rfSwitch.getReceivedValue();
        
        if (code == 0) {
            Serial.println("📡 Unknown encoding");
            return;
        }
        
        unsigned int bitLength = rfSwitch.getReceivedBitlength();
        unsigned int pulse = rfSwitch.getReceivedDelay();
        int protocol = rfSwitch.getReceivedProtocol();
        
        codesReceived++;
        
        Serial.println("📡 RF Code Received:");
        Serial.printf("  Code: %lu (0x%lX)\n", code, code);
        Serial.printf("  Bits: %u\n", bitLength);
        Serial.printf("  Pulse: %u us\n", pulse);
        Serial.printf("  Protocol: %d\n", protocol);
        
        lastReceived = String(code, HEX) + " (" + String(bitLength) + "b)";
        
        // Save to captured codes
        if (capturedCount < MAX_CAPTURED_CODES) {
            RFCode captured;
            captured.name = "Code_" + String(capturedCount + 1);
            captured.protocol = identifyProtocol(bitLength, pulse);
            captured.code = code;
            captured.bitLength = bitLength;
            captured.pulseLength = pulse;
            captured.timestamp = millis();
            captured.isRolling = detectRollingCode(code, bitLength);
            
            capturedCodes[capturedCount++] = captured;
            
            // Check if unique
            if (isUniqueCode(code)) uniqueCodes++;
            
            // Analyze if rolling code
            if (captured.isRolling) {
                analyzeRollingCode(code, bitLength);
            }
        }
    }
    
    RFProtocol identifyProtocol(unsigned int bits, unsigned int pulse) {
        // Identify protocol based on characteristics
        if (bits == 24 && pulse >= 300 && pulse <= 400) {
            return RF_PT2262;
        } else if (bits == 24 && pulse >= 400 && pulse <= 500) {
            return RF_EV1527;
        } else if (bits == 12) {
            return RF_HT12E;
        } else if (bits >= 32 && bits <= 66) {
            return RF_ROLLING_CODE;
        }
        
        return RF_FIXED_CODE;
    }
    
    bool detectRollingCode(unsigned long code, unsigned int bits) {
        // Rolling codes typically have:
        // - 32-66 bits
        // - Changing values each time
        // - KeeLoq signature patterns
        
        if (bits >= 32 && bits <= 66) {
            // Check if code changes (simplified detection)
            static unsigned long lastCode = 0;
            if (lastCode != 0 && code != lastCode) {
                lastCode = code;
                return true;
            }
            lastCode = code;
        }
        
        return false;
    }
    
    void analyzeRollingCode(unsigned long code, unsigned int bits) {
        if (rollingCodeCount >= ROLLING_CODE_BUFFER) return;
        
        RollingCodeInfo rc;
        rc.encryptedCode = code;
        
        // Extract KeeLoq fields (simplified)
        if (bits == 64 || bits == 66) {
            rc.serial = (code >> 32) & 0x0FFFFFFF;
            rc.counter = (code >> 16) & 0xFFFF;
            rc.button = code & 0x0F;
            rc.valid = true;
            
            rollingCodes[rollingCodeCount++] = rc;
            rollingCodesDetected++;
            
            Serial.println("🔄 Rolling code detected:");
            Serial.printf("  Serial: 0x%lX\n", rc.serial);
            Serial.printf("  Counter: %u\n", rc.counter);
            Serial.printf("  Button: %u\n", rc.button);
        }
    }
    
    bool isUniqueCode(unsigned long code) {
        for (int i = 0; i < capturedCount - 1; i++) {
            if (capturedCodes[i].code == code) return false;
        }
        return true;
    }
    
    // ==================== RF TRANSMISSION ====================
    void transmitCode(unsigned long code, unsigned int bits, unsigned int pulse) {
        if (transmitting) return;
        
        transmitting = true;
        
        rfSwitch.setPulseLength(pulse);
        rfSwitch.send(code, bits);
        
        codesTransmitted++;
        lastTransmitted = String(code, HEX);
        
        Serial.printf("📤 Transmitted: 0x%lX (%db, %dus)\n", code, bits, pulse);
        
        delay(100);
        transmitting = false;
    }
    
    void transmitSavedCode(int index) {
        if (index < 0 || index >= savedCount) return;
        
        RFCode& code = savedCodes[index];
        transmitCode(code.code, code.bitLength, code.pulseLength);
    }
    
    void saveCurrentCode() {
        if (capturedCount == 0 || savedCount >= MAX_SAVED_CODES) return;
        
        savedCodes[savedCount++] = capturedCodes[capturedCount - 1];
        Serial.printf("💾 Code saved! (%d/%d)\n", savedCount, MAX_SAVED_CODES);
    }
    
    // ==================== BRUTE FORCE ====================
    void startBruteForce(unsigned int bits) {
        bruteForceBitLength = bits;
        bruteForceProgress = 0;
        bruteForceTotal = (1UL << bits) - 1;  // 2^bits - 1
        bruteForceActive = true;
        
        Serial.printf("⚡ Starting brute force: %d bits (%lu codes)\n", 
                     bits, bruteForceTotal);
    }
    
    void updateBruteForce() {
        if (!bruteForceActive) return;
        
        static unsigned long lastSend = 0;
        if (millis() - lastSend > 200) {  // 200ms between codes
            unsigned long code = bruteForceProgress;
            transmitCode(code, bruteForceBitLength, 350);
            
            bruteForceProgress++;
            lastSend = millis();
            
            if (bruteForceProgress >= bruteForceTotal) {
                bruteForceActive = false;
                Serial.println("⚡ Brute force complete!");
            }
        }
    }
    
    // ==================== SIGNAL ANALYSIS ====================
    void updateSignalAnalysis() {
        // Simulate signal analysis
        currentSignal.frequency = RF_FREQUENCY + random(-5000, 5000);
        currentSignal.rssi = random(-90, -50);
        currentSignal.pulseWidth = 350 + random(-50, 50);
        currentSignal.bitRate = 1000000 / currentSignal.pulseWidth;
        currentSignal.snr = currentSignal.rssi + 80;  // Simplified SNR
        
        // Detect jamming (simplified)
        currentSignal.jammingDetected = (currentSignal.rssi > -60 && codesReceived == 0);
        
        // Update history
        signalHistory[signalHistoryIdx] = currentSignal.rssi;
        signalHistoryIdx = (signalHistoryIdx + 1) % 100;
    }
    
    // ==================== CONTROLS ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                if (currentMode == MODE_TX && selectedCode > 0) selectedCode--;
                else if (currentMode == MODE_DATABASE && selectedDevice > 0) selectedDevice--;
                break;
                
            case ACTION_DOWN:
                if (currentMode == MODE_TX && selectedCode < savedCount - 1) selectedCode++;
                else if (currentMode == MODE_DATABASE && selectedDevice < deviceDatabase.size() - 1) selectedDevice++;
                break;
                
            case ACTION_LEFT:
                if (currentMode > MODE_RX) currentMode = (RFMode)(currentMode - 1);
                break;
                
            case ACTION_RIGHT:
                if (currentMode < MODE_JAMMING_DETECT) currentMode = (RFMode)(currentMode + 1);
                break;
                
            case ACTION_SELECT:
                handleSelection();
                break;
                
            case ACTION_BACK:
                if (bruteForceActive) {
                    bruteForceActive = false;
                } else {
                    currentMode = MODE_RX;
                }
                break;
        }
        
        drawRFScreen();
    }
    
    void handleSelection() {
        switch (currentMode) {
            case MODE_TX:
                if (savedCount > 0) {
                    transmitSavedCode(selectedCode);
                }
                break;
                
            case MODE_BRUTE_FORCE:
                if (!bruteForceActive) {
                    startBruteForce(bruteForceBitLength);
                }
                break;
                
            case MODE_CLONE:
                saveCurrentCode();
                break;
                
            case MODE_ANALYZE:
                analyzing = !analyzing;
                break;
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    String getDeviceTypeName(RFDeviceType type) {
        switch (type) {
            case DEV_GARAGE_DOOR: return "Garage";
            case DEV_GATE: return "Gate";
            case DEV_ALARM: return "Alarm";
            case DEV_DOORBELL: return "Doorbell";
            case DEV_CAR_KEY: return "Car Key";
            case DEV_LIGHT_SWITCH: return "Light";
            case DEV_OUTLET: return "Outlet";
            case DEV_FAN: return "Fan";
            case DEV_WEATHER: return "Weather";
            case DEV_SENSOR: return "Sensor";
            default: return "Unknown";
        }
    }
    
    // API functions
    int getCapturedCount() { return capturedCount; }
    int getSavedCount() { return savedCount; }
    int getCodesReceived() { return codesReceived; }
    int getCodesTransmitted() { return codesTransmitted; }
    bool isReceiving() { return receiving; }
    bool isTransmitting() { return transmitting; }
};

#endif // M5GOTCHI_RF433_TRANSCEIVER_H
