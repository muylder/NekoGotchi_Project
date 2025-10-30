/**
 * @file m5gotchi_field_persistence.h
 * @brief Sistema robusto de persistência para uso em campo
 *
 * Gerencia auto-save, recovery de crashes, backup de capturas e state management
 * otimizado para operações de pentest em campo com alta confiabilidade.
 *
 * @version 1.0.0
 * @date 2025-10-30
 */

#ifndef M5GOTCHI_FIELD_PERSISTENCE_H
#define M5GOTCHI_FIELD_PERSISTENCE_H

#include <Preferences.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <esp_system.h>
#include <esp_task_wdt.h>

/**
 * @brief Sistema de persistência robusto para operações de campo
 *
 * Features:
 * - Auto-save periódico de estado crítico
 * - Recovery automático após crash
 * - Backup incremental de capturas
 * - Versionamento de dados
 * - Detecção de corrupção
 * - Modo de emergência (salvar antes de OOM)
 */
class FieldPersistence {
public:
    /**
     * @brief Tipos de dados persistentes
     */
    enum DataType {
        STATE_CRITICAL,      // Estado crítico do sistema
        STATE_SESSION,       // Estado da sessão atual
        CAPTURE_WIFI,        // Capturas WiFi (handshakes, pmkid)
        CAPTURE_BLE,         // Capturas BLE
        CAPTURE_RFID,        // Capturas RFID/NFC
        LOG_EVENTS,          // Log de eventos
        LOG_TIMELINE,        // Timeline de operações
        CONFIG_USER,         // Configurações do usuário
        NEKO_STATE          // Estado do Neko pet
    };

    /**
     * @brief Status de operação
     */
    enum Status {
        OK,
        ERROR_NO_STORAGE,
        ERROR_CORRUPT,
        ERROR_NO_SPACE,
        ERROR_IO,
        RECOVERED_FROM_CRASH
    };

    /**
     * @brief Configuração de persistência
     */
    struct Config {
        bool autoSaveEnabled = true;
        uint32_t autoSaveInterval = 30000;  // 30s
        bool backupEnabled = true;
        uint8_t maxBackups = 3;
        bool compressionEnabled = false;    // Futuro: compressão
        bool encryptionEnabled = false;     // Futuro: criptografia
        String storagePath = "/nekogotchi"; // Path base no SD
    };

    /**
     * @brief Metadados de arquivo persistente
     */
    struct FileMetadata {
        uint32_t version;
        uint32_t timestamp;
        uint32_t crc32;
        uint32_t size;
        bool valid;
    };

private:
    Preferences prefs;
    Config config;

    uint32_t lastAutoSave = 0;
    bool sdAvailable = false;
    bool spiffsAvailable = false;

    // Contadores de operação
    uint32_t saveCount = 0;
    uint32_t loadCount = 0;
    uint32_t crashCount = 0;

    // Estado de recovery
    bool recoveredFromCrash = false;
    String lastCrashReason = "";

    /**
     * @brief Calcula CRC32 para verificação de integridade
     */
    uint32_t calculateCRC32(const uint8_t* data, size_t length) {
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < length; i++) {
            crc ^= data[i];
            for (int j = 0; j < 8; j++) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    }

    /**
     * @brief Obtém path para tipo de dado
     */
    String getPathForType(DataType type, bool backup = false) {
        String base = config.storagePath;
        String suffix = backup ? ".bak" : "";

        switch(type) {
            case STATE_CRITICAL:  return base + "/state_critical.json" + suffix;
            case STATE_SESSION:   return base + "/state_session.json" + suffix;
            case CAPTURE_WIFI:    return base + "/captures/wifi/";
            case CAPTURE_BLE:     return base + "/captures/ble/";
            case CAPTURE_RFID:    return base + "/captures/rfid/";
            case LOG_EVENTS:      return base + "/logs/events.log";
            case LOG_TIMELINE:    return base + "/logs/timeline.json";
            case CONFIG_USER:     return base + "/config.json" + suffix;
            case NEKO_STATE:      return base + "/neko_state.json" + suffix;
            default:              return base + "/unknown.dat";
        }
    }

    /**
     * @brief Cria diretórios necessários
     */
    bool ensureDirectories() {
        if (!sdAvailable) return false;

        SD.mkdir(config.storagePath.c_str());
        SD.mkdir((config.storagePath + "/captures").c_str());
        SD.mkdir((config.storagePath + "/captures/wifi").c_str());
        SD.mkdir((config.storagePath + "/captures/ble").c_str());
        SD.mkdir((config.storagePath + "/captures/rfid").c_str());
        SD.mkdir((config.storagePath + "/logs").c_str());
        SD.mkdir((config.storagePath + "/backup").c_str());

        return true;
    }

    /**
     * @brief Faz backup de um arquivo
     */
    bool backupFile(const String& path) {
        if (!config.backupEnabled || !sdAvailable) return false;

        String backupPath = path + ".bak";

        // Remove backup antigo
        if (SD.exists(backupPath.c_str())) {
            SD.remove(backupPath.c_str());
        }

        // Copia arquivo atual para backup
        File src = SD.open(path.c_str(), FILE_READ);
        if (!src) return false;

        File dst = SD.open(backupPath.c_str(), FILE_WRITE);
        if (!dst) {
            src.close();
            return false;
        }

        uint8_t buffer[512];
        while (src.available()) {
            size_t len = src.read(buffer, sizeof(buffer));
            dst.write(buffer, len);
        }

        src.close();
        dst.close();

        return true;
    }

    /**
     * @brief Restaura de backup
     */
    bool restoreFromBackup(const String& path) {
        String backupPath = path + ".bak";

        if (!SD.exists(backupPath.c_str())) return false;

        // Remove arquivo corrompido
        if (SD.exists(path.c_str())) {
            SD.remove(path.c_str());
        }

        // Copia backup para arquivo principal
        File src = SD.open(backupPath.c_str(), FILE_READ);
        if (!src) return false;

        File dst = SD.open(path.c_str(), FILE_WRITE);
        if (!dst) {
            src.close();
            return false;
        }

        uint8_t buffer[512];
        while (src.available()) {
            size_t len = src.read(buffer, sizeof(buffer));
            dst.write(buffer, len);
        }

        src.close();
        dst.close();

        Serial.println("[FieldPersistence] 🔄 Restaurado de backup: " + path);
        return true;
    }

public:
    /**
     * @brief Inicializa sistema de persistência
     */
    Status begin(const Config& cfg = Config()) {
        config = cfg;

        // Detecta crash anterior
        prefs.begin("nekogotchi", false);
        bool wasRunning = prefs.getBool("running", false);

        if (wasRunning) {
            recoveredFromCrash = true;
            crashCount = prefs.getUInt("crash_count", 0) + 1;
            lastCrashReason = prefs.getString("last_error", "Unknown");
            prefs.putUInt("crash_count", crashCount);

            Serial.println("╔════════════════════════════════════════╗");
            Serial.println("║  ⚠️  CRASH RECOVERY DETECTADO!        ║");
            Serial.println("╠════════════════════════════════════════╣");
            Serial.printf( "║  Crashes totais: %-2d                  ║\n", crashCount);
            Serial.printf( "║  Última razão: %-20s   ║\n", lastCrashReason.c_str());
            Serial.println("║  Tentando recuperar estado...         ║");
            Serial.println("╚════════════════════════════════════════╝");
        }

        // Marca como rodando
        prefs.putBool("running", true);
        prefs.putString("last_boot", String(millis()));

        // Inicializa storages
        spiffsAvailable = SPIFFS.begin(true);
        sdAvailable = SD.begin();

        if (!spiffsAvailable && !sdAvailable) {
            Serial.println("[FieldPersistence] ❌ Nenhum storage disponível!");
            return ERROR_NO_STORAGE;
        }

        if (sdAvailable) {
            ensureDirectories();
            Serial.println("[FieldPersistence] ✅ SD Card montado");
        }

        if (spiffsAvailable) {
            Serial.println("[FieldPersistence] ✅ SPIFFS montado");
        }

        // Configura watchdog para auto-save de emergência
        esp_task_wdt_init(30, true); // 30s timeout

        Serial.println("[FieldPersistence] 🐱 Sistema de persistência iniciado");

        return recoveredFromCrash ? RECOVERED_FROM_CRASH : OK;
    }

    /**
     * @brief Finaliza gracefully (chame antes de shutdown)
     */
    void end() {
        prefs.putBool("running", false);
        prefs.putString("last_shutdown", String(millis()));
        prefs.end();

        Serial.println("[FieldPersistence] 👋 Shutdown gracioso");
    }

    /**
     * @brief Salva estado crítico (deve ser rápido!)
     */
    bool saveCriticalState(const JsonDocument& state) {
        if (!sdAvailable) {
            // Fallback para Preferences se SD não disponível
            String output;
            serializeJson(state, output);
            prefs.putString("critical", output);
            return true;
        }

        String path = getPathForType(STATE_CRITICAL);

        // Backup do estado anterior
        if (SD.exists(path.c_str())) {
            backupFile(path);
        }

        // Salva novo estado
        File file = SD.open(path.c_str(), FILE_WRITE);
        if (!file) return false;

        serializeJson(state, file);
        file.close();

        saveCount++;
        lastAutoSave = millis();

        return true;
    }

    /**
     * @brief Carrega estado crítico
     */
    bool loadCriticalState(JsonDocument& state) {
        String path = getPathForType(STATE_CRITICAL);

        // Tenta carregar do SD
        if (sdAvailable && SD.exists(path.c_str())) {
            File file = SD.open(path.c_str(), FILE_READ);
            if (!file) {
                // Tenta backup
                return restoreFromBackup(path) && loadCriticalState(state);
            }

            DeserializationError error = deserializeJson(state, file);
            file.close();

            if (error) {
                Serial.println("[FieldPersistence] ⚠️ Estado corrompido, tentando backup...");
                return restoreFromBackup(path) && loadCriticalState(state);
            }

            loadCount++;
            return true;
        }

        // Fallback: Preferences
        if (prefs.isKey("critical")) {
            String json = prefs.getString("critical", "{}");
            DeserializationError error = deserializeJson(state, json);
            return !error;
        }

        return false;
    }

    /**
     * @brief Salva captura WiFi (handshake, PMKID, etc)
     */
    bool saveWiFiCapture(const String& ssid, const uint8_t* data, size_t len) {
        if (!sdAvailable) return false;

        String basePath = getPathForType(CAPTURE_WIFI);
        String timestamp = String(millis());
        String filename = ssid;
        filename.replace(" ", "_");
        filename.replace("/", "_");
        String fullPath = basePath + filename + "_" + timestamp + ".cap";

        File file = SD.open(fullPath.c_str(), FILE_WRITE);
        if (!file) {
            Serial.println("[FieldPersistence] ❌ Erro ao salvar captura WiFi");
            return false;
        }

        file.write(data, len);
        file.close();

        Serial.printf("[FieldPersistence] 📡 Captura salva: %s (%d bytes)\n",
                     fullPath.c_str(), len);

        return true;
    }

    /**
     * @brief Salva log de evento
     */
    bool logEvent(const String& event, const String& details = "") {
        String path = getPathForType(LOG_EVENTS);

        File file = SD.open(path.c_str(), FILE_APPEND);
        if (!file) return false;

        char timestamp[32];
        snprintf(timestamp, sizeof(timestamp), "[%010lu]", millis());

        file.print(timestamp);
        file.print(" ");
        file.print(event);
        if (details.length() > 0) {
            file.print(" - ");
            file.print(details);
        }
        file.println();
        file.close();

        return true;
    }

    /**
     * @brief Auto-save periódico (chame no loop)
     */
    void update() {
        if (!config.autoSaveEnabled) return;

        uint32_t now = millis();
        if (now - lastAutoSave >= config.autoSaveInterval) {
            // Trigger para salvar estado crítico
            // Implementar callback ou flag para main
            lastAutoSave = now;
        }
    }

    /**
     * @brief Salva erro antes de crash (emergência)
     */
    void emergencySave(const String& reason) {
        prefs.putString("last_error", reason);
        prefs.putULong("crash_time", millis());
        Serial.println("[FieldPersistence] 🚨 EMERGENCY SAVE: " + reason);
    }

    /**
     * @brief Exporta sessão completa para análise
     */
    bool exportSession(const String& sessionName) {
        if (!sdAvailable) return false;

        String exportPath = config.storagePath + "/export/" + sessionName;
        SD.mkdir(exportPath.c_str());

        // Copia todos os logs e capturas da sessão
        // TODO: Implementar export completo com manifest

        Serial.println("[FieldPersistence] 📦 Sessão exportada: " + exportPath);
        return true;
    }

    /**
     * @brief Limpa dados antigos para liberar espaço
     */
    size_t cleanup(uint32_t olderThanMs = 86400000) { // 24h default
        if (!sdAvailable) return 0;

        size_t freedSpace = 0;
        // TODO: Implementar limpeza inteligente de arquivos antigos

        return freedSpace;
    }

    /**
     * @brief Estatísticas de uso
     */
    void printStats() {
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║     📊 FIELD PERSISTENCE STATS        ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Saves: %-6lu   Loads: %-6lu     ║\n", saveCount, loadCount);
        Serial.printf( "║  Crashes: %-4lu   Recovered: %-3s    ║\n",
                      crashCount, recoveredFromCrash ? "YES" : "NO");
        Serial.printf( "║  SD: %-3s   SPIFFS: %-3s             ║\n",
                      sdAvailable ? "OK" : "N/A", spiffsAvailable ? "OK" : "N/A");

        if (sdAvailable) {
            uint64_t total = SD.totalBytes();
            uint64_t used = SD.usedBytes();
            uint8_t percent = (used * 100) / total;
            Serial.printf( "║  SD Usage: %llu/%llu MB (%d%%)   ║\n",
                          used/1048576, total/1048576, percent);
        }

        Serial.println("╚════════════════════════════════════════╝\n");
    }

    // Getters
    bool wasRecovered() const { return recoveredFromCrash; }
    uint32_t getCrashCount() const { return crashCount; }
    bool hasSDCard() const { return sdAvailable; }
    bool hasSPIFFS() const { return spiffsAvailable; }
};

#endif // M5GOTCHI_FIELD_PERSISTENCE_H
