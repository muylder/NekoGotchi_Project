/*
 * ConfigManager.h
 * Centraliza todas as configurações e constantes do projeto
 * Evita magic numbers e facilita manutenção
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

namespace Config {

    // ==================== DISPLAY ====================
    namespace Display {
        constexpr int WIDTH = 240;
        constexpr int HEIGHT = 135;
        constexpr int HEADER_HEIGHT = 16;
        constexpr int FOOTER_HEIGHT = 12;
        constexpr int MENU_ITEM_HEIGHT = 20;
        constexpr int CONTENT_START_Y = HEADER_HEIGHT;
        constexpr int CONTENT_END_Y = HEIGHT - FOOTER_HEIGHT;
        constexpr int SCROLLBAR_WIDTH = 4;
    }

    // ==================== SD CARD ====================
    namespace SDCard {
        constexpr int SCK_PIN = 40;
        constexpr int MISO_PIN = 39;
        constexpr int MOSI_PIN = 14;
        constexpr int CS_PIN = 12;
    }

    // ==================== WIFI ====================
    namespace WiFi {
        constexpr int MAX_NETWORKS = 20;
        constexpr int SCAN_TIMEOUT_MS = 5000;
        constexpr int CHANNEL_MIN = 1;
        constexpr int CHANNEL_MAX = 14;

        // Evil Portal
        constexpr int DNS_PORT = 53;
        constexpr int HTTP_PORT = 80;
        constexpr int MAX_CLIENTS = 4;

        // Handshake Capture
        constexpr int EAPOL_FRAME = 0x888E;
        constexpr int SNAP_LEN = 2324;
    }

    // ==================== FILE PATHS ====================
    namespace Paths {
        constexpr const char* HANDSHAKE_DIR = "/handshakes";
        constexpr const char* PORTAL_DIR = "/portals";
        constexpr const char* CREDS_FILE = "/credentials.txt";
        constexpr const char* CAPTURES_DIR = "/captures";
        constexpr const char* CONFIG_FILE = "/config.json";
        constexpr const char* LOG_DIR = "/logs";
    }

    // ==================== IR REMOTE ====================
    namespace IR {
        constexpr int TX_PIN = 9;
        constexpr int RX_PIN = 36;
        constexpr int FREQUENCY = 38000;
        constexpr int MAX_CODES = 50;
        constexpr int BUFFER_SIZE = 1024;
    }

    // ==================== RF 433MHz ====================
    namespace RF {
        constexpr int TX_PIN = 26;
        constexpr int RX_PIN = 36;
        constexpr int MAX_CODES = 50;
        constexpr int PROTOCOL = 1;
        constexpr int PULSE_LENGTH = 350;
    }

    // ==================== LORA ====================
    namespace LoRa {
        constexpr int SCK_PIN = 18;
        constexpr int MISO_PIN = 19;
        constexpr int MOSI_PIN = 23;
        constexpr int CS_PIN = 5;
        constexpr int RST_PIN = 14;
        constexpr int IRQ_PIN = 2;
        constexpr long FREQUENCY = 915E6;  // 915 MHz (US) - mudar para 868E6 (EU) ou 433E6
        constexpr int MAX_DEVICES = 30;
        constexpr int SYNC_WORD = 0x12;
        constexpr int SPREADING_FACTOR = 7;
        constexpr long SIGNAL_BANDWIDTH = 125E3;
    }

    // ==================== GPS ====================
    namespace GPS {
        constexpr int RX_PIN = 1;
        constexpr int TX_PIN = 2;
        constexpr int BAUD_RATE = 9600;
        constexpr int UPDATE_INTERVAL_MS = 1000;
        constexpr int TIMEOUT_MS = 5000;
    }

    // ==================== BLUETOOTH ====================
    namespace Bluetooth {
        constexpr int SCAN_DURATION_SEC = 5;
        constexpr int MAX_DEVICES = 50;
        constexpr int RSSI_THRESHOLD = -100;
    }

    // ==================== RFID/NFC ====================
    namespace RFID {
        constexpr int MAX_TAGS = 20;
        constexpr int READ_TIMEOUT_MS = 1000;
    }

    // ==================== ANIMATION ====================
    namespace Animation {
        constexpr int DEFAULT_FPS = 30;
        constexpr int FRAME_DELAY_MS = 1000 / DEFAULT_FPS;
        constexpr int SCROLL_SPEED = 2;
        constexpr int FADE_STEPS = 10;
    }

    // ==================== TIMING ====================
    namespace Timing {
        constexpr int INPUT_DEBOUNCE_MS = 50;
        constexpr int SCREEN_REFRESH_MS = 100;
        constexpr int STATUS_UPDATE_MS = 500;
        constexpr int AUTO_SAVE_INTERVAL_MS = 30000;  // 30 seconds
    }

    // ==================== NEKO PET ====================
    namespace NekoPet {
        constexpr int MAX_HUNGER = 100;
        constexpr int MAX_HAPPINESS = 100;
        constexpr int MAX_ENERGY = 100;
        constexpr int HUNGER_DECAY_RATE = 1;  // per minute
        constexpr int HAPPINESS_DECAY_RATE = 1;  // per minute
        constexpr int ENERGY_DECAY_RATE = 2;  // per minute
        constexpr int ANIMATION_FRAMES = 4;
        constexpr int FRAME_DURATION_MS = 200;
    }

    // ==================== ACHIEVEMENTS ====================
    namespace Achievements {
        constexpr int MAX_ACHIEVEMENTS = 50;
        constexpr int NOTIFICATION_DURATION_MS = 3000;
    }

    // ==================== MEMORY ====================
    namespace Memory {
        constexpr int PACKET_BUFFER_SIZE = 512;
        constexpr int STRING_BUFFER_SIZE = 256;
        constexpr int NETWORK_LIST_SIZE = 50;
    }

    // ==================== VERSIONING ====================
    namespace Version {
        constexpr const char* VERSION = "2.0.0";
        constexpr const char* BUILD_DATE = __DATE__;
        constexpr const char* BUILD_TIME = __TIME__;
        constexpr const char* DEVICE = "M5Stack Cardputer";
    }

} // namespace Config

#endif // CONFIG_MANAGER_H
