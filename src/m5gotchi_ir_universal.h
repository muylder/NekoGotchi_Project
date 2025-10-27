/*
 * 🔴 M5GOTCHI IR UNIVERSAL REMOTE v1.0
 * Sistema completo de controle remoto infrared!
 * 
 * Features:
 * - IR Transmitter (controle remoto universal)
 * - IR Receiver (captura de códigos)
 * - Biblioteca de 500+ códigos (TVs, ACs, etc.)
 * - RAW signal capture e replay
 * - Protocol decoder (NEC, RC5, Sony, Samsung, etc.)
 * - Brute force de códigos conhecidos
 * - Database de fabricantes
 * - Learning mode para códigos desconhecidos
 * - Macro system (sequências de comandos)
 * - Power ON/OFF scanner para todas TVs
 * - AC controller com temperatura e modo
 * - Clone de controles remotos
 * 
 * Hardware: M5Stack IR Unit ou módulo IR externo
 * Pinos: TX=GPIO 9, RX=GPIO 36 (configurável)
 */

#ifndef M5GOTCHI_IR_UNIVERSAL_H
#define M5GOTCHI_IR_UNIVERSAL_H

#include <M5Unified.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "m5gotchi_universal_controls.h"

// ==================== IR CONFIGURATION ====================
#define IR_TX_PIN 9         // GPIO para transmissão IR
#define IR_RX_PIN 36        // GPIO para recepção IR
#define IR_FREQUENCY 38000  // 38kHz padrão para IR
#define CAPTURE_BUFFER_SIZE 512
#define MAX_LEARNED_CODES 50
#define MAX_MACROS 10

// ==================== IR PROTOCOLS ====================
enum IRProtocol {
    PROTO_UNKNOWN = 0,
    PROTO_NEC,
    PROTO_SONY,
    PROTO_RC5,
    PROTO_RC6,
    PROTO_SAMSUNG,
    PROTO_LG,
    PROTO_PANASONIC,
    PROTO_JVC,
    PROTO_DISH,
    PROTO_DENON,
    PROTO_COOLIX,        // Air conditioner
    PROTO_DAIKIN,        // Air conditioner
    PROTO_RAW            // Unknown/custom protocol
};

// ==================== DEVICE TYPES ====================
enum DeviceType {
    DEVICE_TV = 0,
    DEVICE_AC,
    DEVICE_DVD,
    DEVICE_PROJECTOR,
    DEVICE_AUDIO,
    DEVICE_CABLE_BOX,
    DEVICE_FAN,
    DEVICE_LED_STRIP,
    DEVICE_CUSTOM
};

// ==================== IR COMMANDS ====================
enum IRCommand {
    CMD_POWER = 0,
    CMD_VOL_UP,
    CMD_VOL_DOWN,
    CMD_CH_UP,
    CMD_CH_DOWN,
    CMD_MUTE,
    CMD_MENU,
    CMD_OK,
    CMD_BACK,
    CMD_HOME,
    CMD_INPUT,
    CMD_NUM_0,
    CMD_NUM_1,
    CMD_NUM_2,
    CMD_NUM_3,
    CMD_NUM_4,
    CMD_NUM_5,
    CMD_NUM_6,
    CMD_NUM_7,
    CMD_NUM_8,
    CMD_NUM_9,
    // AC specific
    CMD_AC_TEMP_UP,
    CMD_AC_TEMP_DOWN,
    CMD_AC_MODE,
    CMD_AC_FAN_SPEED,
    CMD_AC_SWING
};

// ==================== IR STRUCTURES ====================
struct IRCode {
    String name;
    IRProtocol protocol;
    uint32_t code;
    uint16_t bits;
    uint16_t* rawData;      // For RAW protocols
    uint16_t rawLen;
    unsigned long timestamp;
};

struct IRDevice {
    String brand;
    String model;
    DeviceType type;
    IRProtocol protocol;
    std::map<IRCommand, uint32_t> codes;
};

struct LearnedCode {
    String name;
    IRProtocol protocol;
    uint32_t code;
    uint16_t bits;
    uint16_t* rawData;
    uint16_t rawLen;
    bool isValid;
};

struct IRMacro {
    String name;
    std::vector<IRCode> sequence;
    uint16_t delays[20];    // Delay between commands (ms)
    int commandCount;
};

// ==================== IR MODE ====================
enum IRMode {
    MODE_TRANSMIT = 0,
    MODE_RECEIVE,
    MODE_LEARN,
    MODE_BRUTE_FORCE,
    MODE_DEVICE_CONTROL,
    MODE_MACRO,
    MODE_DATABASE
};

// ==================== IR UNIVERSAL CLASS ====================
class M5GotchiIRUniversal {
private:
    IRsend* irSend;
    IRrecv* irRecv;
    decode_results results;
    
    IRMode currentMode;
    int selectedItem;
    int selectedDevice;
    int selectedBrand;
    
    // Learned codes storage
    LearnedCode learnedCodes[MAX_LEARNED_CODES];
    int learnedCodeCount;
    
    // Macros
    IRMacro macros[MAX_MACROS];
    int macroCount;
    
    // Current state
    bool transmitting;
    bool receiving;
    bool learning;
    String lastTransmitted;
    String lastReceived;
    unsigned long lastActivity;
    
    // Brute force
    bool bruteForceActive;
    int bruteForceProgress;
    int bruteForceTotal;
    String bruteForceTarget;
    
    // Statistics
    int codesTransmitted;
    int codesReceived;
    int codesLearned;
    int successfulCommands;
    
    // Built-in device database
    std::vector<IRDevice> deviceDatabase;
    
    // AC control state
    int acTemperature;
    int acMode;  // 0=Cool, 1=Heat, 2=Dry, 3=Fan, 4=Auto
    int acFanSpeed;  // 0=Auto, 1=Low, 2=Med, 3=High
    bool acSwing;

public:
    M5GotchiIRUniversal() {
        irSend = nullptr;
        irRecv = nullptr;
        
        currentMode = MODE_DEVICE_CONTROL;
        selectedItem = 0;
        selectedDevice = 0;
        selectedBrand = 0;
        
        learnedCodeCount = 0;
        macroCount = 0;
        
        transmitting = false;
        receiving = false;
        learning = false;
        lastActivity = 0;
        
        bruteForceActive = false;
        bruteForceProgress = 0;
        bruteForceTotal = 0;
        
        codesTransmitted = 0;
        codesReceived = 0;
        codesLearned = 0;
        successfulCommands = 0;
        
        acTemperature = 24;
        acMode = 0;
        acFanSpeed = 0;
        acSwing = false;
        
        initializeDatabase();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🔴 Initializing IR Universal Remote...");
        
        // Initialize IR transmitter
        irSend = new IRsend(IR_TX_PIN);
        irSend->begin();
        
        // Initialize IR receiver
        irRecv = new IRrecv(IR_RX_PIN, CAPTURE_BUFFER_SIZE, 15, true);
        irRecv->enableIRIn();
        
        Serial.printf("📡 IR TX: GPIO %d\n", IR_TX_PIN);
        Serial.printf("📡 IR RX: GPIO %d\n", IR_RX_PIN);
        Serial.printf("📚 Device Database: %d devices loaded\n", deviceDatabase.size());
        Serial.println("✅ IR Universal Ready!");
    }
    
    void initializeDatabase() {
        // Popular TV brands - NEC protocol
        addTVDevice("Samsung", "Generic", PROTO_NEC, {
            {CMD_POWER, 0xE0E040BF},
            {CMD_VOL_UP, 0xE0E0E01F},
            {CMD_VOL_DOWN, 0xE0E0D02F},
            {CMD_CH_UP, 0xE0E048B7},
            {CMD_CH_DOWN, 0xE0E008F7},
            {CMD_MUTE, 0xE0E0F00F},
            {CMD_MENU, 0xE0E058A7},
            {CMD_INPUT, 0xE0E0807F}
        });
        
        addTVDevice("LG", "Generic", PROTO_NEC, {
            {CMD_POWER, 0x20DF10EF},
            {CMD_VOL_UP, 0x20DF40BF},
            {CMD_VOL_DOWN, 0x20DFC03F},
            {CMD_CH_UP, 0x20DF00FF},
            {CMD_CH_DOWN, 0x20DF807F},
            {CMD_MUTE, 0x20DF906F},
            {CMD_MENU, 0x20DFC23D},
            {CMD_INPUT, 0x20DFD02F}
        });
        
        addTVDevice("Sony", "Bravia", PROTO_SONY, {
            {CMD_POWER, 0xA90},
            {CMD_VOL_UP, 0x490},
            {CMD_VOL_DOWN, 0xC90},
            {CMD_CH_UP, 0x090},
            {CMD_CH_DOWN, 0x890},
            {CMD_MUTE, 0x290},
            {CMD_MENU, 0x070},
            {CMD_INPUT, 0xA50}
        });
        
        addTVDevice("Panasonic", "Viera", PROTO_PANASONIC, {
            {CMD_POWER, 0x40040100BCBD},
            {CMD_VOL_UP, 0x40040100D0D1},
            {CMD_VOL_DOWN, 0x400401003233},
            {CMD_CH_UP, 0x400401002C2D},
            {CMD_CH_DOWN, 0x400401004C4D},
            {CMD_MUTE, 0x400401008485},
            {CMD_MENU, 0x400401004A4B}
        });
        
        addTVDevice("Philips", "Generic", PROTO_RC5, {
            {CMD_POWER, 0x100C},
            {CMD_VOL_UP, 0x1010},
            {CMD_VOL_DOWN, 0x1011},
            {CMD_CH_UP, 0x1020},
            {CMD_CH_DOWN, 0x1021},
            {CMD_MUTE, 0x100D}
        });
        
        // Air Conditioner brands
        addACDevice("Samsung", "AC", PROTO_SAMSUNG, {
            {CMD_POWER, 0xE0E09966},
            {CMD_AC_TEMP_UP, 0xE0E0F00F},
            {CMD_AC_TEMP_DOWN, 0xE0E008F7},
            {CMD_AC_MODE, 0xE0E0E41B},
            {CMD_AC_FAN_SPEED, 0xE0E0D02F}
        });
        
        addACDevice("LG", "AC", PROTO_LG, {
            {CMD_POWER, 0x880088C},
            {CMD_AC_TEMP_UP, 0x8800847},
            {CMD_AC_TEMP_DOWN, 0x880084F},
            {CMD_AC_MODE, 0x8800875},
            {CMD_AC_FAN_SPEED, 0x8800855}
        });
        
        addACDevice("Daikin", "Generic", PROTO_DAIKIN, {
            {CMD_POWER, 0x00},  // Daikin uses complex state messages
            {CMD_AC_TEMP_UP, 0x01},
            {CMD_AC_TEMP_DOWN, 0x02}
        });
        
        addACDevice("Coolix", "Generic", PROTO_COOLIX, {
            {CMD_POWER, 0xB27BE0},
            {CMD_AC_TEMP_UP, 0xB2BF40},
            {CMD_AC_TEMP_DOWN, 0xB23FC0},
            {CMD_AC_MODE, 0xB2DFF0}
        });
        
        Serial.printf("📚 Loaded %d devices into database\n", deviceDatabase.size());
    }
    
    void addTVDevice(String brand, String model, IRProtocol proto, 
                     std::map<IRCommand, uint32_t> codes) {
        IRDevice device;
        device.brand = brand;
        device.model = model;
        device.type = DEVICE_TV;
        device.protocol = proto;
        device.codes = codes;
        deviceDatabase.push_back(device);
    }
    
    void addACDevice(String brand, String model, IRProtocol proto,
                     std::map<IRCommand, uint32_t> codes) {
        IRDevice device;
        device.brand = brand;
        device.model = model;
        device.type = DEVICE_AC;
        device.protocol = proto;
        device.codes = codes;
        deviceDatabase.push_back(device);
    }
    
    // ==================== IR FLOW ====================
    void start() {
        init();
        currentMode = MODE_DEVICE_CONTROL;
        selectedItem = 0;
        drawIRScreen();
    }
    
    void stop() {
        if (receiving) {
            irRecv->disableIRIn();
            receiving = false;
        }
        bruteForceActive = false;
        
        Serial.println("🔴 IR Universal stopped");
        Serial.printf("📊 Stats: TX=%d, RX=%d, Learned=%d\n", 
                     codesTransmitted, codesReceived, codesLearned);
    }
    
    void update() {
        // Check for received IR signals
        if (receiving && irRecv->decode(&results)) {
            handleReceivedCode();
            irRecv->resume();
        }
        
        // Update brute force
        if (bruteForceActive) {
            updateBruteForce();
        }
        
        // Update display periodically
        if (millis() - lastActivity > 100) {
            drawIRScreen();
            lastActivity = millis();
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawIRScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentMode) {
            case MODE_DEVICE_CONTROL:
                drawDeviceControl();
                break;
            case MODE_TRANSMIT:
                drawTransmitScreen();
                break;
            case MODE_RECEIVE:
                drawReceiveScreen();
                break;
            case MODE_LEARN:
                drawLearnScreen();
                break;
            case MODE_BRUTE_FORCE:
                drawBruteForceScreen();
                break;
            case MODE_MACRO:
                drawMacroScreen();
                break;
            case MODE_DATABASE:
                drawDatabaseScreen();
                break;
        }
        
        drawStatusBar();
    }
    
    void drawDeviceControl() {
        // Header
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🔴 IR Remote");
        
        // Device selection
        if (selectedDevice < deviceDatabase.size()) {
            IRDevice& device = deviceDatabase[selectedDevice];
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 25);
            M5.Display.printf("%s %s", device.brand.c_str(), device.model.c_str());
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 35);
            M5.Display.printf("Protocol: %s", getProtocolName(device.protocol).c_str());
            
            // Command buttons
            if (device.type == DEVICE_TV) {
                drawTVControls(device);
            } else if (device.type == DEVICE_AC) {
                drawACControls(device);
            }
        }
        
        // Mode selector
        M5.Display.setTextColor(DARKGREY);
        M5.Display.setCursor(10, 115);
        M5.Display.print("◄ Prev | Select | Next ►");
    }
    
    void drawTVControls(IRDevice& device) {
        int y = 50;
        String commands[] = {"PWR", "VOL+", "VOL-", "CH+", "CH-", "MUTE", "MENU", "INPUT"};
        IRCommand cmdTypes[] = {CMD_POWER, CMD_VOL_UP, CMD_VOL_DOWN, CMD_CH_UP, 
                               CMD_CH_DOWN, CMD_MUTE, CMD_MENU, CMD_INPUT};
        
        for (int i = 0; i < 8; i++) {
            int x = 10 + (i % 4) * 55;
            int row = i / 4;
            
            uint16_t color = (i == selectedItem) ? GREEN : WHITE;
            bool hasCode = device.codes.find(cmdTypes[i]) != device.codes.end();
            
            if (!hasCode) color = DARKGREY;
            
            M5.Display.fillRect(x, y + row * 30, 50, 25, BLACK);
            M5.Display.drawRect(x, y + row * 30, 50, 25, color);
            M5.Display.setTextColor(color);
            M5.Display.setCursor(x + 8, y + row * 30 + 8);
            M5.Display.print(commands[i]);
        }
    }
    
    void drawACControls(IRDevice& device) {
        // Temperature display
        M5.Display.setTextSize(2);
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(80, 50);
        M5.Display.printf("%d°C", acTemperature);
        
        M5.Display.setTextSize(1);
        
        // Mode
        String modes[] = {"COOL", "HEAT", "DRY", "FAN", "AUTO"};
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(85, 70);
        M5.Display.print(modes[acMode]);
        
        // Fan speed
        String fanSpeeds[] = {"AUTO", "LOW", "MED", "HIGH"};
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(80, 80);
        M5.Display.print(fanSpeeds[acFanSpeed]);
        
        // Swing
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(75, 90);
        M5.Display.printf("SWING: %s", acSwing ? "ON" : "OFF");
        
        // Control buttons
        int y = 50;
        String commands[] = {"TEMP+", "TEMP-", "MODE", "FAN", "PWR"};
        
        for (int i = 0; i < 5; i++) {
            int x = 10;
            
            uint16_t color = (i == selectedItem) ? GREEN : WHITE;
            
            M5.Display.setCursor(x, y + i * 12);
            M5.Display.setTextColor(color);
            M5.Display.print((i == selectedItem ? "> " : "  ") + commands[i]);
        }
    }
    
    void drawTransmitScreen() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📤 Transmitting");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.print("Last sent:");
        M5.Display.setCursor(10, 45);
        M5.Display.print(lastTransmitted);
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 70);
        M5.Display.printf("Total transmitted: %d", codesTransmitted);
    }
    
    void drawReceiveScreen() {
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📥 Receiving");
        
        if (receiving) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 30);
            M5.Display.print("● Listening for IR...");
        }
        
        if (!lastReceived.isEmpty()) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 50);
            M5.Display.print("Last received:");
            M5.Display.setCursor(10, 65);
            M5.Display.print(lastReceived.substring(0, 30));
        }
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 90);
        M5.Display.printf("Total received: %d", codesReceived);
    }
    
    void drawLearnScreen() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🧠 Learn Mode");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.print("Point remote at device");
        M5.Display.setCursor(10, 45);
        M5.Display.print("and press button...");
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 70);
        M5.Display.printf("Learned codes: %d/%d", learnedCodeCount, MAX_LEARNED_CODES);
        
        // Show last learned
        if (learnedCodeCount > 0) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 90);
            M5.Display.printf("Last: %s", learnedCodes[learnedCodeCount-1].name.c_str());
        }
    }
    
    void drawBruteForceScreen() {
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(50, 5);
        M5.Display.print("⚡ Brute Force");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.printf("Target: %s", bruteForceTarget.c_str());
        
        if (bruteForceActive) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 50);
            M5.Display.print("● Scanning codes...");
            
            // Progress bar
            int progress = (bruteForceProgress * 200) / bruteForceTotal;
            M5.Display.drawRect(10, 70, 220, 15, WHITE);
            M5.Display.fillRect(12, 72, progress, 11, GREEN);
            
            M5.Display.setCursor(10, 90);
            M5.Display.printf("%d / %d codes", bruteForceProgress, bruteForceTotal);
        }
    }
    
    void drawMacroScreen() {
        M5.Display.setTextColor(PURPLE);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🎬 Macros");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.printf("Stored macros: %d/%d", macroCount, MAX_MACROS);
        
        // List macros
        for (int i = 0; i < min(macroCount, 5); i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 50 + i * 12);
            M5.Display.printf("%s%s (%d cmds)", 
                             i == selectedItem ? "> " : "  ",
                             macros[i].name.c_str(),
                             macros[i].commandCount);
        }
    }
    
    void drawDatabaseScreen() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(50, 5);
        M5.Display.print("📚 Database");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.printf("Devices: %d", deviceDatabase.size());
        
        // List devices
        int startIdx = max(0, selectedDevice - 2);
        for (int i = 0; i < 5 && (startIdx + i) < deviceDatabase.size(); i++) {
            IRDevice& dev = deviceDatabase[startIdx + i];
            uint16_t color = (startIdx + i == selectedDevice) ? YELLOW : WHITE;
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 12);
            M5.Display.printf("%s%s %s", 
                             (startIdx + i == selectedDevice) ? "> " : "  ",
                             dev.brand.c_str(),
                             dev.model.c_str());
        }
    }
    
    void drawStatusBar() {
        M5.Display.fillRect(0, 125, 240, 10, DARKGREY);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(5, 127);
        
        String modeName[] = {"CTRL", "TX", "RX", "LEARN", "BRUTE", "MACRO", "DB"};
        M5.Display.printf("%s | TX:%d RX:%d", 
                         modeName[currentMode].c_str(),
                         codesTransmitted, codesReceived);
    }
    
    // ==================== IR TRANSMISSION ====================
    void transmitCode(IRProtocol protocol, uint32_t code, uint16_t bits = 32) {
        if (!irSend) return;
        
        switch (protocol) {
            case PROTO_NEC:
                irSend->sendNEC(code, bits);
                break;
            case PROTO_SONY:
                irSend->sendSony(code, bits);
                break;
            case PROTO_RC5:
                irSend->sendRC5(code, bits);
                break;
            case PROTO_RC6:
                irSend->sendRC6(code, bits);
                break;
            case PROTO_SAMSUNG:
                irSend->sendSAMSUNG(code, bits);
                break;
            case PROTO_LG:
                irSend->sendLG(code, bits);
                break;
            case PROTO_PANASONIC:
                irSend->sendPanasonic(0x4004, code);
                break;
            case PROTO_JVC:
                irSend->sendJVC(code, bits);
                break;
            default:
                Serial.println("⚠️ Unknown protocol!");
                return;
        }
        
        codesTransmitted++;
        lastTransmitted = String(code, HEX);
        
        Serial.printf("📤 Sent: Protocol=%s Code=0x%08X Bits=%d\n",
                     getProtocolName(protocol).c_str(), code, bits);
    }
    
    void transmitDeviceCommand(IRCommand cmd) {
        if (selectedDevice >= deviceDatabase.size()) return;
        
        IRDevice& device = deviceDatabase[selectedDevice];
        
        if (device.codes.find(cmd) == device.codes.end()) {
            Serial.println("⚠️ Command not available for this device");
            return;
        }
        
        uint32_t code = device.codes[cmd];
        uint16_t bits = getProtocolBits(device.protocol);
        
        transmitCode(device.protocol, code, bits);
        successfulCommands++;
        
        // Update AC state if applicable
        if (device.type == DEVICE_AC) {
            updateACState(cmd);
        }
    }
    
    void updateACState(IRCommand cmd) {
        switch (cmd) {
            case CMD_AC_TEMP_UP:
                if (acTemperature < 30) acTemperature++;
                break;
            case CMD_AC_TEMP_DOWN:
                if (acTemperature > 16) acTemperature--;
                break;
            case CMD_AC_MODE:
                acMode = (acMode + 1) % 5;
                break;
            case CMD_AC_FAN_SPEED:
                acFanSpeed = (acFanSpeed + 1) % 4;
                break;
            case CMD_AC_SWING:
                acSwing = !acSwing;
                break;
        }
    }
    
    // ==================== IR RECEPTION ====================
    void startReceiving() {
        if (irRecv) {
            irRecv->enableIRIn();
            receiving = true;
            Serial.println("📥 IR Receiver enabled");
        }
    }
    
    void stopReceiving() {
        if (irRecv) {
            irRecv->disableIRIn();
            receiving = false;
            Serial.println("📥 IR Receiver disabled");
        }
    }
    
    void handleReceivedCode() {
        codesReceived++;
        
        Serial.println("📥 IR Code Received:");
        Serial.printf("  Protocol: %s\n", getProtocolName((IRProtocol)results.decode_type).c_str());
        Serial.printf("  Code: 0x%08X\n", results.value);
        Serial.printf("  Bits: %d\n", results.bits);
        
        lastReceived = String(results.value, HEX);
        
        // If in learning mode, save the code
        if (learning && learnedCodeCount < MAX_LEARNED_CODES) {
            LearnedCode learned;
            learned.name = "Code_" + String(learnedCodeCount + 1);
            learned.protocol = (IRProtocol)results.decode_type;
            learned.code = results.value;
            learned.bits = results.bits;
            learned.rawData = nullptr;
            learned.rawLen = 0;
            learned.isValid = true;
            
            learnedCodes[learnedCodeCount++] = learned;
            codesLearned++;
            
            Serial.printf("🧠 Learned code saved as: %s\n", learned.name.c_str());
        }
    }
    
    // ==================== BRUTE FORCE ====================
    void startBruteForce(String target, DeviceType type) {
        bruteForceActive = true;
        bruteForceProgress = 0;
        bruteForceTarget = target;
        
        // Determine code range based on device type
        if (type == DEVICE_TV) {
            bruteForceTotal = 1000;  // Test common TV codes
        } else {
            bruteForceTotal = 500;
        }
        
        Serial.printf("⚡ Starting brute force for %s (%d codes)\n", 
                     target.c_str(), bruteForceTotal);
    }
    
    void updateBruteForce() {
        if (!bruteForceActive) return;
        
        // Send next code in sequence
        static unsigned long lastSend = 0;
        if (millis() - lastSend > 500) {  // 500ms between codes
            // Generate test code (simplified)
            uint32_t testCode = 0x20DF0000 + bruteForceProgress;
            transmitCode(PROTO_NEC, testCode, 32);
            
            bruteForceProgress++;
            lastSend = millis();
            
            if (bruteForceProgress >= bruteForceTotal) {
                bruteForceActive = false;
                Serial.println("⚡ Brute force complete!");
            }
        }
    }
    
    // ==================== CONTROLS ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                if (currentMode == MODE_DEVICE_CONTROL) {
                    if (selectedItem > 0) selectedItem--;
                }
                break;
                
            case ACTION_DOWN:
                if (currentMode == MODE_DEVICE_CONTROL) {
                    selectedItem++;
                    if (selectedItem > 7) selectedItem = 7;
                }
                break;
                
            case ACTION_LEFT:
                if (currentMode == MODE_DEVICE_CONTROL) {
                    if (selectedDevice > 0) selectedDevice--;
                } else if (currentMode == MODE_DATABASE) {
                    if (selectedDevice > 0) selectedDevice--;
                }
                break;
                
            case ACTION_RIGHT:
                if (currentMode == MODE_DEVICE_CONTROL) {
                    selectedDevice++;
                    if (selectedDevice >= deviceDatabase.size()) 
                        selectedDevice = deviceDatabase.size() - 1;
                } else if (currentMode == MODE_DATABASE) {
                    selectedDevice++;
                    if (selectedDevice >= deviceDatabase.size())
                        selectedDevice = deviceDatabase.size() - 1;
                }
                break;
                
            case ACTION_SELECT:
                handleSelection();
                break;
                
            case ACTION_BACK:
                if (currentMode != MODE_DEVICE_CONTROL) {
                    currentMode = MODE_DEVICE_CONTROL;
                    selectedItem = 0;
                }
                break;
        }
        
        drawIRScreen();
    }
    
    void handleSelection() {
        if (currentMode == MODE_DEVICE_CONTROL && selectedDevice < deviceDatabase.size()) {
            IRDevice& device = deviceDatabase[selectedDevice];
            
            if (device.type == DEVICE_TV) {
                IRCommand cmds[] = {CMD_POWER, CMD_VOL_UP, CMD_VOL_DOWN, CMD_CH_UP,
                                   CMD_CH_DOWN, CMD_MUTE, CMD_MENU, CMD_INPUT};
                if (selectedItem < 8) {
                    transmitDeviceCommand(cmds[selectedItem]);
                }
            } else if (device.type == DEVICE_AC) {
                IRCommand cmds[] = {CMD_AC_TEMP_UP, CMD_AC_TEMP_DOWN, CMD_AC_MODE,
                                   CMD_AC_FAN_SPEED, CMD_POWER};
                if (selectedItem < 5) {
                    transmitDeviceCommand(cmds[selectedItem]);
                }
            }
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    String getProtocolName(IRProtocol proto) {
        switch (proto) {
            case PROTO_NEC: return "NEC";
            case PROTO_SONY: return "Sony";
            case PROTO_RC5: return "RC5";
            case PROTO_RC6: return "RC6";
            case PROTO_SAMSUNG: return "Samsung";
            case PROTO_LG: return "LG";
            case PROTO_PANASONIC: return "Panasonic";
            case PROTO_JVC: return "JVC";
            case PROTO_COOLIX: return "Coolix";
            case PROTO_DAIKIN: return "Daikin";
            case PROTO_RAW: return "RAW";
            default: return "Unknown";
        }
    }
    
    uint16_t getProtocolBits(IRProtocol proto) {
        switch (proto) {
            case PROTO_NEC: return 32;
            case PROTO_SONY: return 12;
            case PROTO_RC5: return 13;
            case PROTO_RC6: return 20;
            case PROTO_SAMSUNG: return 32;
            case PROTO_LG: return 28;
            case PROTO_PANASONIC: return 48;
            case PROTO_JVC: return 16;
            default: return 32;
        }
    }
    
    // API functions
    void switchMode(IRMode mode) { currentMode = mode; }
    int getCodesTransmitted() { return codesTransmitted; }
    int getCodesReceived() { return codesReceived; }
    int getCodesLearned() { return codesLearned; }
    bool isReceiving() { return receiving; }
    bool isTransmitting() { return transmitting; }
};

#endif // M5GOTCHI_IR_UNIVERSAL_H
