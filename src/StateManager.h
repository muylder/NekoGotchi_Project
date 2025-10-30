/*
 * StateManager.h
 * Gerencia estado da aplicação de forma centralizada
 * Substitui variáveis static espalhadas
 */

#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <WiFi.h>

// Forward declarations
class M5GotchiRFIDNFC;
class BluetoothAttacks;
class NekoVirtualPet;
class TutorialSystem;
class AchievementManager;

// Modos de operação
enum OperationMode {
    MODE_MENU,
    MODE_DASHBOARD,
    MODE_WIFI_SCAN,
    MODE_DEAUTH,
    MODE_BEACON_SPAM,
    MODE_PROBE_FLOOD,
    MODE_EVIL_PORTAL,
    MODE_HANDSHAKE,
    MODE_CHANNEL_ANALYZER,
    MODE_PACKET_MONITOR,
    MODE_FILE_MANAGER,
    MODE_GPS_WARDRIVING,
    MODE_STATISTICS,
    MODE_WPS_ATTACK,
    MODE_RFID_READER,
    MODE_RFID_WRITER,
    MODE_RFID_EMULATE,
    MODE_BLE_SCAN,
    MODE_BLE_SPAM,
    MODE_IR_REMOTE,
    MODE_RF_REMOTE,
    MODE_LORA_SCANNER,
    MODE_NEKO_PET,
    MODE_ACHIEVEMENTS,
    MODE_TUTORIAL,
    MODE_SETTINGS,
    // Legacy compatibility aliases
    MODE_RF433 = MODE_RF_REMOTE,
    MODE_RFID_NFC = MODE_RFID_READER,
    MODE_BLUETOOTH = MODE_BLE_SPAM,
    MODE_LOG_VIEWER = MODE_SETTINGS + 1
};

// Estado WiFi (expandido)
struct WiFiState {
    std::vector<String> networkNames;
    std::vector<int> networkRSSI;
    std::vector<int> networkChannels;
    std::vector<wifi_auth_mode_t> networkAuth;
    std::vector<uint8_t*> networkBSSID;
    int selectedNetwork = 0;
    int scrollOffset = 0;
    int networkCount = 0;
    bool isScanning = false;
    unsigned long lastScanTime = 0;
    String customSSID = "FREE_WiFi";
};

// Estado de animação
struct AnimationState {
    int wifiAnimFrame = 0;
    int barAnimHeight[11] = {0};
    int flyingPackets[5][3] = {0};
    unsigned long lastAnimUpdate = 0;
    unsigned long lastBlink = 0;
    bool blinkState = false;
};

// Estado de ataques
struct AttackState {
    bool isAttacking = false;
    unsigned long attackStartTime = 0;
    int packetsInjected = 0;
    int packetsCaptured = 0;
    int currentChannel = 1;
    String targetSSID = "";
    uint8_t targetBSSID[6] = {0};
};

// Estado Deauth
struct DeauthState {
    bool isDeauthing = false;
    unsigned long lastDeauth = 0;
    unsigned long deauthCount = 0;
};

// Estado Beacon Spam
struct BeaconSpamState {
    bool isBeaconSpamming = false;
    unsigned long lastBeacon = 0;
    unsigned long beaconCount = 0;
};

// Estado Probe Flood
struct ProbeFloodState {
    bool isProbeFlooding = false;
    unsigned long lastProbe = 0;
    unsigned long probeCount = 0;
};

// Estado Handshake Capture
struct HandshakeState {
    bool isCapturingHandshake = false;
    int eapolPackets = 0;
    unsigned long captureStartTime = 0;
    bool handshakeComplete = false;
    String targetSSID = "";
    uint8_t targetBSSID[6] = {0};
};

// Estado Packet Monitor
struct PacketMonitorState {
    unsigned long beaconPackets = 0;
    unsigned long dataPackets = 0;
    unsigned long mgmtPackets = 0;
    unsigned long totalPackets = 0;
    unsigned long lastPacketTime = 0;
    int packetsPerSecond = 0;
    bool isMonitoring = false;
};

// Estado Channel Analyzer
struct ChannelAnalyzerState {
    int channelCount[14] = {0};
    int channelMaxRSSI[14] = {-100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};
    int bestChannel = 1;
    bool isAnalyzing = false;
    unsigned long analyzeStartTime = 0;
};

// Estado WPS Attack
struct WPSState {
    bool isWPSAttacking = false;
    unsigned long wpsStartTime = 0;
    int wpsPinsTried = 0;
    int wpsVulnerableAPs = 0;
    String wpsTargetSSID = "";
    uint8_t wpsTargetBSSID[6] = {0};
    unsigned long wpsLastAttempt = 0;
};

// Estado IR Remote
struct IRState {
    bool irAvailable = false;
    int irCodeCount = 0;
    int selectedIRCode = 0;
    bool isLearningIR = false;
    bool isTransmittingIR = false;
    String irMode = "TV"; // TV, AC, DVD, etc
};

// Estado RF 433MHz
struct RFState {
    bool rf433Available = false;
    int rfCodeCount = 0;
    int selectedRFCode = 0;
    bool isCapturingRF = false;
    bool isReplayingRF = false;
};

// Estado LoRa
struct LoRaState {
    bool loraAvailable = false;
    int loraDeviceCount = 0;
    int selectedLoRaDevice = 0;
    bool isScanningLoRa = false;
    unsigned long lastLoRaScan = 0;
    int loraPacketsReceived = 0;
};

// Estado de Logs
struct LogState {
    int logCount = 0;
    int logViewScroll = 0;
    bool autoSaveLogsEnabled = true;
};

// Estado de Estatísticas
struct StatisticsState {
    unsigned long sessionStartTime = 0;
    unsigned long totalScans = 0;
    unsigned long totalNetworksFound = 0;
    unsigned long totalDeauthSent = 0;
    unsigned long totalBeaconsSent = 0;
    unsigned long totalProbesSent = 0;
    unsigned long totalHandshakesCaptured = 0;
    unsigned long totalCredentialsCaptured = 0;
    unsigned long totalAPsLogged = 0;
};

// Estado de Configuração
struct ConfigState {
    String configFile = "/config.json";
    String sessionFile = "/session.json";
    String statsFile = "/stats.json";
    String logFile = "/system.log";
    bool autoSaveEnabled = true;
    unsigned long lastAutoSave = 0;
};

// Estado Evil Portal (expandido)
struct PortalState {
    bool isRunning = false;
    int capturedCredentials = 0;
    int connectedClients = 0;
    String portalSSID = "Free WiFi";
    String capturedUsername = "";
    String capturedPassword = "";
    unsigned long portalStartTime = 0;
    String customPortalHTML = "";
    bool useCustomPortal = false;
};

// Estado do File Manager
struct FileManagerState {
    String currentPath = "/";
    std::vector<String> files;
    std::vector<String> directories;
    int selectedItem = 0;
    int scrollOffset = 0;
    bool showHidden = false;
};

// Estado GPS (expandido)
struct GPSState {
    bool isActive = false;
    bool gpsAvailable = false;
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;
    float speed = 0.0;
    int satellites = 0;
    bool gpsFixed = false;
    float hdop = 99.99;
    unsigned long lastUpdate = 0;
    std::vector<String> capturedNetworks;
    int wifiLogged = 0;
    bool isWardriving = false;
    unsigned long wardrivingStartTime = 0;
    String wardrivingFile = "/wardriving.csv";
    String gpxTrackFile = "/gps_track.gpx";
    int gpxPointCount = 0;
};

// Estado do menu
struct MenuState {
    int selectedItem = 0;
    int scrollOffset = 0;
    int menuLevel = 0;  // 0=main, 1=submenu, etc
};

// Estado global da aplicação
class StateManager {
public:
    // Singleton instance
    static StateManager& getInstance() {
        static StateManager instance;
        return instance;
    }

    // Proíbe cópia
    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;

    // Estado atual
    OperationMode currentMode = MODE_MENU;
    OperationMode previousMode = MODE_MENU;

    // Estados específicos
    WiFiState wifi;
    AnimationState animation;
    AttackState attack;
    DeauthState deauth;
    BeaconSpamState beaconSpam;
    ProbeFloodState probeFlood;
    HandshakeState handshake;
    PacketMonitorState packetMonitor;
    ChannelAnalyzerState channelAnalyzer;
    WPSState wps;
    IRState ir;
    RFState rf;
    LoRaState lora;
    LogState log;
    StatisticsState statistics;
    ConfigState config;
    PortalState portal;
    FileManagerState fileManager;
    GPSState gps;
    MenuState menu;

    // Flags globais
    bool sdCardAvailable = false;
    bool wifiInitialized = false;
    bool gpsInitialized = false;
    bool loraInitialized = false;
    bool irInitialized = false;
    bool rfInitialized = false;

    // Módulos (ponteiros gerenciados externamente por enquanto)
    M5GotchiRFIDNFC* rfidModule = nullptr;
    BluetoothAttacks* bleModule = nullptr;
    NekoVirtualPet* nekoModule = nullptr;
    TutorialSystem* tutorialModule = nullptr;
    AchievementManager* achievementModule = nullptr;

    // Timing
    unsigned long lastInputTime = 0;
    unsigned long lastScreenUpdate = 0;
    unsigned long lastStatusUpdate = 0;

    // Métodos auxiliares
    void changeMode(OperationMode newMode) {
        previousMode = currentMode;
        currentMode = newMode;

        // Reset estados específicos ao mudar de modo
        resetModeState(newMode);
    }

    void returnToPreviousMode() {
        currentMode = previousMode;
    }

    void resetModeState(OperationMode mode) {
        // Reseta estados específicos baseado no modo
        switch(mode) {
            case MODE_WIFI_SCAN:
                wifi.selectedNetwork = 0;
                wifi.scrollOffset = 0;
                break;

            case MODE_FILE_MANAGER:
                fileManager.selectedItem = 0;
                fileManager.scrollOffset = 0;
                break;

            case MODE_MENU:
                menu.selectedItem = 0;
                menu.scrollOffset = 0;
                break;

            default:
                break;
        }
    }

    void resetAttack() {
        attack.isAttacking = false;
        attack.attackStartTime = 0;
        attack.packetsInjected = 0;
        attack.packetsCaptured = 0;
    }

    void resetAnimation() {
        animation.wifiAnimFrame = 0;
        animation.lastAnimUpdate = 0;
        animation.lastBlink = 0;
        animation.blinkState = false;

        for(int i = 0; i < 11; i++) {
            animation.barAnimHeight[i] = 0;
        }
    }

    void clearWiFiData() {
        wifi.networkNames.clear();
        wifi.networkRSSI.clear();
        wifi.networkChannels.clear();
        wifi.networkAuth.clear();

        // Libera BSSIDs se alocados
        for(auto bssid : wifi.networkBSSID) {
            if(bssid != nullptr) {
                delete[] bssid;
            }
        }
        wifi.networkBSSID.clear();

        wifi.networkCount = 0;
        wifi.selectedNetwork = 0;
        wifi.scrollOffset = 0;
    }

private:
    // Construtor privado para singleton
    StateManager() = default;
};

// Macro para acesso rápido ao StateManager
#define STATE StateManager::getInstance()

#endif // STATE_MANAGER_H
