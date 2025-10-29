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
    MODE_SETTINGS
};

// Estado WiFi
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

// Estado Evil Portal
struct PortalState {
    bool isRunning = false;
    int capturedCredentials = 0;
    int connectedClients = 0;
    String portalSSID = "Free WiFi";
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

// Estado GPS
struct GPSState {
    bool isActive = false;
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;
    int satellites = 0;
    unsigned long lastUpdate = 0;
    std::vector<String> capturedNetworks;
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
