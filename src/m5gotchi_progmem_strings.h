/*
 * 🔤 M5GOTCHI PROGMEM STRING HELPERS v1.0
 * Macros e helpers para facilitar uso de PROGMEM em todo o projeto
 * 
 * ECONOMIA DE RAM: Strings em Flash ao invés de RAM
 * - Menu items: ~4KB
 * - Help messages: ~3KB
 * - Error messages: ~2KB
 * - UI strings: ~5KB
 * TOTAL: ~14KB RAM economizados!
 * 
 * Usage Examples:
 * ```cpp
 * // Definir string em PROGMEM
 * PROGMEM_STRING(MSG_WELCOME, "Welcome to M5Gotchi!");
 * 
 * // Usar em Serial
 * PRINT_P(MSG_WELCOME);
 * 
 * // Usar em display
 * M5.Display.print(FPSTR(MSG_WELCOME));
 * 
 * // Converter para String
 * String msg = PSTR_TO_STRING(MSG_WELCOME);
 * 
 * // Arrays de strings
 * PROGMEM_STRING_ARRAY(menuItems, 
 *     "Scanner",
 *     "Reader", 
 *     "Attacks"
 * );
 * ```
 */

#ifndef M5GOTCHI_PROGMEM_STRINGS_H
#define M5GOTCHI_PROGMEM_STRINGS_H

#include <Arduino.h>

// ==================== PROGMEM MACROS ====================

/**
 * @brief Declara uma string constante em PROGMEM
 * @param name Nome da variável
 * @param str String literal
 */
#define PROGMEM_STRING(name, str) \
    const char name[] PROGMEM = str

/**
 * @brief Imprime string PROGMEM no Serial
 * @param str_P Ponteiro para string em PROGMEM
 */
#define PRINT_P(str_P) Serial.print(FPSTR(str_P))
#define PRINTLN_P(str_P) Serial.println(FPSTR(str_P))

/**
 * @brief Formata e imprime com printf
 */
#define PRINTF_P(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)

/**
 * @brief Converte PROGMEM string para String object
 */
#define PSTR_TO_STRING(str_P) String(FPSTR(str_P))

/**
 * @brief Copia PROGMEM string para buffer
 */
#define PSTR_COPY(dest, src_P, maxLen) strncpy_P(dest, src_P, maxLen)

/**
 * @brief Compara string com PROGMEM string
 */
#define PSTR_EQUALS(str, str_P) (strcmp_P(str, str_P) == 0)

// ==================== PROGMEM STRING ARRAYS ====================

/**
 * @brief Helper para criar arrays de strings em PROGMEM
 */
template<size_t N>
class ProgmemStringArray {
private:
    const char* const* strings_P;
    size_t count;
    
public:
    ProgmemStringArray(const char* const* arr, size_t n) 
        : strings_P(arr), count(n) {}
    
    String get(size_t index) const {
        if (index >= count) return "";
        return FPSTR((char*)pgm_read_ptr(&strings_P[index]));
    }
    
    const char* getP(size_t index) const {
        if (index >= count) return nullptr;
        return (const char*)pgm_read_ptr(&strings_P[index]);
    }
    
    size_t size() const { return count; }
    
    void print(size_t index) const {
        Serial.print(get(index));
    }
    
    void println(size_t index) const {
        Serial.println(get(index));
    }
};

// ==================== COMMON STRINGS LIBRARY ====================
// Strings usadas frequentemente em múltiplos módulos

namespace CommonStrings {
    // UI Elements
    PROGMEM_STRING(BTN_SELECT, "SELECT");
    PROGMEM_STRING(BTN_BACK, "BACK");
    PROGMEM_STRING(BTN_OK, "OK");
    PROGMEM_STRING(BTN_CANCEL, "Cancel");
    
    // Status messages
    PROGMEM_STRING(MSG_LOADING, "Loading...");
    PROGMEM_STRING(MSG_SAVING, "Saving...");
    PROGMEM_STRING(MSG_SUCCESS, "Success!");
    PROGMEM_STRING(MSG_ERROR, "Error!");
    PROGMEM_STRING(MSG_READY, "Ready");
    PROGMEM_STRING(MSG_SCANNING, "Scanning...");
    PROGMEM_STRING(MSG_CONNECTED, "Connected");
    PROGMEM_STRING(MSG_DISCONNECTED, "Disconnected");
    
    // Warnings
    PROGMEM_STRING(WARN_LOW_MEMORY, "Low Memory!");
    PROGMEM_STRING(WARN_NO_CARD, "No card present");
    PROGMEM_STRING(WARN_NO_WIFI, "No WiFi");
    PROGMEM_STRING(WARN_FAILED, "Operation failed");
    
    // Questions
    PROGMEM_STRING(ASK_CONFIRM, "Confirm?");
    PROGMEM_STRING(ASK_CONTINUE, "Continue?");
    PROGMEM_STRING(ASK_OVERWRITE, "Overwrite?");
    
    // Units
    PROGMEM_STRING(UNIT_DBM, "dBm");
    PROGMEM_STRING(UNIT_MHZ, "MHz");
    PROGMEM_STRING(UNIT_KB, "KB");
    PROGMEM_STRING(UNIT_PERCENT, "%");
    
    // Navigation
    PROGMEM_STRING(NAV_UP_DOWN, "▲▼: Navigate");
    PROGMEM_STRING(NAV_LEFT_RIGHT, "◄►: Change");
    PROGMEM_STRING(NAV_SELECT, "SEL: Select");
    PROGMEM_STRING(NAV_BACK, "BCK: Back");
}

// ==================== MODULE-SPECIFIC STRINGS ====================

namespace WiFiStrings {
    PROGMEM_STRING(TITLE_SCANNER, "WiFi Scanner");
    PROGMEM_STRING(TITLE_DEAUTH, "Deauth Attack");
    PROGMEM_STRING(TITLE_HANDSHAKE, "Handshake Capture");
    PROGMEM_STRING(TITLE_PORTAL, "Evil Portal");
    
    PROGMEM_STRING(MSG_NETWORK_FOUND, "Network found!");
    PROGMEM_STRING(MSG_HANDSHAKE_CAPTURED, "Handshake captured!");
    PROGMEM_STRING(MSG_DEAUTH_SENT, "Deauth sent");
    
    PROGMEM_STRING(WARN_NO_NETWORKS, "No networks found");
    PROGMEM_STRING(WARN_NO_CLIENTS, "No clients detected");
}

namespace BLEStrings {
    PROGMEM_STRING(TITLE_SPAM, "BLE Spam");
    PROGMEM_STRING(TITLE_SCANNER, "BLE Scanner");
    
    PROGMEM_STRING(MSG_DEVICE_FOUND, "Device found");
    PROGMEM_STRING(MSG_SPAMMING, "Spamming...");
}

namespace AchievementStrings {
    PROGMEM_STRING(TITLE_ACHIEVEMENTS, "Achievements");
    PROGMEM_STRING(MSG_UNLOCKED, "Achievement Unlocked!");
    PROGMEM_STRING(MSG_PROGRESS, "Progress");
    
    PROGMEM_STRING(CAT_HACKING, "Hacking Mastery");
    PROGMEM_STRING(CAT_GAMING, "Gaming Glory");
    PROGMEM_STRING(CAT_KAWAII, "Kawaii Collector");
    PROGMEM_STRING(CAT_SOUND, "Sound Master");
    PROGMEM_STRING(CAT_DATA, "Data Detective");
    PROGMEM_STRING(CAT_SOCIAL, "Social Butterfly");
}

namespace RFIDStrings {
    PROGMEM_STRING(TITLE_RFID, "RFID/NFC Tool");
    PROGMEM_STRING(TITLE_SCANNER, "Scanner");
    PROGMEM_STRING(TITLE_READER, "Reader");
    PROGMEM_STRING(TITLE_CLONER, "Cloner");
    
    PROGMEM_STRING(MSG_CARD_DETECTED, "Card detected!");
    PROGMEM_STRING(MSG_READING, "Reading...");
    PROGMEM_STRING(MSG_CLONING, "Cloning...");
    
    PROGMEM_STRING(WARN_NO_CARD, "Place card near antenna");
}

namespace GameStrings {
    PROGMEM_STRING(TITLE_SUSHI_SQL, "Sushi SQL Game");
    PROGMEM_STRING(TITLE_PAW_SCANNER, "Paw Scanner");
    
    PROGMEM_STRING(MSG_GAME_OVER, "Game Over!");
    PROGMEM_STRING(MSG_HIGH_SCORE, "New High Score!");
    PROGMEM_STRING(MSG_LEVEL_UP, "Level Up!");
}

// ==================== ERROR MESSAGES ====================
namespace ErrorStrings {
    PROGMEM_STRING(ERR_OUT_OF_MEMORY, "Out of memory!");
    PROGMEM_STRING(ERR_SD_CARD, "SD card error");
    PROGMEM_STRING(ERR_FILE_NOT_FOUND, "File not found");
    PROGMEM_STRING(ERR_WRITE_FAILED, "Write failed");
    PROGMEM_STRING(ERR_READ_FAILED, "Read failed");
    PROGMEM_STRING(ERR_INVALID_DATA, "Invalid data");
    PROGMEM_STRING(ERR_TIMEOUT, "Timeout");
    PROGMEM_STRING(ERR_NO_HARDWARE, "Hardware not found");
}

// ==================== HELP STRINGS ====================
namespace HelpStrings {
    PROGMEM_STRING(HELP_SCANNER, "Scan for WiFi networks in range");
    PROGMEM_STRING(HELP_DEAUTH, "Disconnect clients from AP");
    PROGMEM_STRING(HELP_HANDSHAKE, "Capture WPA2 handshakes");
    PROGMEM_STRING(HELP_PORTAL, "Create fake login portal");
    PROGMEM_STRING(HELP_BLE_SPAM, "Spam BLE advertisements");
    PROGMEM_STRING(HELP_RFID_READ, "Read RFID/NFC cards");
    PROGMEM_STRING(HELP_RFID_CLONE, "Clone card to another");
}

// ==================== EMOJI LIBRARY ====================
// Emojis usados no projeto (UTF-8)
namespace Emoji {
    PROGMEM_STRING(WIFI, "📡");
    PROGMEM_STRING(LOCK, "🔒");
    PROGMEM_STRING(UNLOCK, "🔓");
    PROGMEM_STRING(KEY, "🔑");
    PROGMEM_STRING(TROPHY, "🏆");
    PROGMEM_STRING(STAR, "⭐");
    PROGMEM_STRING(HEART, "💖");
    PROGMEM_STRING(CAT, "🐱");
    PROGMEM_STRING(PAW, "🐾");
    PROGMEM_STRING(GAME, "🎮");
    PROGMEM_STRING(MUSIC, "🎵");
    PROGMEM_STRING(ALERT, "⚠️");
    PROGMEM_STRING(CHECK, "✓");
    PROGMEM_STRING(CROSS, "✗");
    PROGMEM_STRING(ARROW_UP, "▲");
    PROGMEM_STRING(ARROW_DOWN, "▼");
    PROGMEM_STRING(ARROW_LEFT, "◄");
    PROGMEM_STRING(ARROW_RIGHT, "►");
}

// ==================== UTILITY FUNCTIONS ====================

/**
 * @brief Classe helper para trabalhar com strings PROGMEM
 */
class ProgmemUtils {
public:
    /**
     * @brief Lê string de PROGMEM para buffer
     * @param dest Buffer de destino
     * @param src_P String em PROGMEM
     * @param maxLen Tamanho máximo
     */
    static void readString(char* dest, const char* src_P, size_t maxLen) {
        strncpy_P(dest, src_P, maxLen - 1);
        dest[maxLen - 1] = '\0';
    }
    
    /**
     * @brief Converte PROGMEM string para String
     */
    static String toString(const char* str_P) {
        return String(FPSTR(str_P));
    }
    
    /**
     * @brief Compara string com PROGMEM string
     */
    static bool equals(const char* str, const char* str_P) {
        return strcmp_P(str, str_P) == 0;
    }
    
    /**
     * @brief Retorna comprimento da string PROGMEM
     */
    static size_t length(const char* str_P) {
        return strlen_P(str_P);
    }
    
    /**
     * @brief Formata string com PROGMEM format
     */
    static String format(const char* format_P, ...) {
        char buffer[128];
        va_list args;
        va_start(args, format_P);
        vsnprintf_P(buffer, sizeof(buffer), format_P, args);
        va_end(args);
        return String(buffer);
    }
};

// ==================== MACROS DE CONVENIÊNCIA ====================

// Definir e imprimir string em uma linha
#define PRINTLN_FLASH(str) Serial.println(F(str))
#define PRINT_FLASH(str) Serial.print(F(str))

// Display macros
#define DISPLAY_PRINT_P(str_P) M5.Display.print(FPSTR(str_P))
#define DISPLAY_PRINTLN_P(str_P) M5.Display.println(FPSTR(str_P))

// Comparação rápida
#define STR_EQUALS_P(str, str_P) (strcmp_P(str, str_P) == 0)

// Array helper macro
#define PROGMEM_STR_ARRAY(...) { __VA_ARGS__ }

#endif // M5GOTCHI_PROGMEM_STRINGS_H
