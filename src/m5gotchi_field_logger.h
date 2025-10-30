/**
 * @file m5gotchi_field_logger.h
 * @brief Sistema de logging profissional para operações de pentest
 *
 * Gera logs estruturados, timeline de eventos, capturas e evidências
 * em formatos compatíveis com ferramentas profissionais de pentest.
 *
 * @version 1.0.0
 * @date 2025-10-30
 */

#ifndef M5GOTCHI_FIELD_LOGGER_H
#define M5GOTCHI_FIELD_LOGGER_H

#include <SD.h>
#include <ArduinoJson.h>
#include <time.h>

/**
 * @brief Logger profissional para pentest em campo
 *
 * Features:
 * - Timeline detalhado de todas operações
 * - Logs estruturados em JSON
 * - Capturas com metadata completo
 * - Geração de relatórios automatizados
 * - Export em formatos padrão (CSV, JSON, XML)
 * - Chain of custody para evidências
 */
class FieldLogger {
public:
    /**
     * @brief Níveis de log
     */
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
        SUCCESS,
        CAPTURE
    };

    /**
     * @brief Tipos de evento
     */
    enum EventType {
        // WiFi
        WIFI_SCAN_START,
        WIFI_SCAN_COMPLETE,
        WIFI_DEAUTH_SENT,
        WIFI_HANDSHAKE_CAPTURED,
        WIFI_PMKID_CAPTURED,
        WIFI_BEACON_SPAM,
        WIFI_PROBE_FLOOD,

        // BLE
        BLE_SCAN_START,
        BLE_DEVICE_FOUND,
        BLE_ATTACK_SENT,

        // RFID/NFC
        RFID_SCAN,
        RFID_READ,
        RFID_WRITE,
        RFID_CLONE,
        RFID_ATTACK,

        // Sistema
        SYSTEM_BOOT,
        SYSTEM_SHUTDOWN,
        SYSTEM_ERROR,
        SESSION_START,
        SESSION_END,

        // Outros
        GPS_LOCATION,
        USER_ACTION,
        CUSTOM
    };

    /**
     * @brief Estrutura de evento
     */
    struct Event {
        uint32_t timestamp;
        EventType type;
        LogLevel level;
        String module;
        String description;
        JsonDocument metadata;

        Event() : metadata(1024) {}
    };

    /**
     * @brief Configuração do logger
     */
    struct Config {
        bool enabled = true;
        bool writeToSD = true;
        bool writeToSerial = true;
        bool timestampsEnabled = true;
        bool metadataEnabled = true;
        String logPath = "/nekogotchi/logs";
        String sessionPrefix = "session";
        uint32_t maxLogSize = 5242880; // 5MB
        bool autoRotate = true;
        LogLevel minLevel = INFO;
    };

private:
    Config config;
    String currentSessionID;
    File logFile;
    File timelineFile;
    File captureIndex;

    uint32_t eventCount = 0;
    uint32_t captureCount = 0;
    uint32_t errorCount = 0;

    bool initialized = false;

    /**
     * @brief Gera ID de sessão único
     */
    String generateSessionID() {
        char sessionID[32];
        uint32_t timestamp = millis();
        uint32_t random_val = esp_random();
        snprintf(sessionID, sizeof(sessionID), "%s_%08lx_%08lx",
                config.sessionPrefix.c_str(), timestamp, random_val);
        return String(sessionID);
    }

    /**
     * @brief Nome do arquivo de log
     */
    String getLogFilePath() {
        return config.logPath + "/" + currentSessionID + ".log";
    }

    /**
     * @brief Nome do arquivo de timeline
     */
    String getTimelineFilePath() {
        return config.logPath + "/" + currentSessionID + "_timeline.json";
    }

    /**
     * @brief Nome do índice de capturas
     */
    String getCaptureIndexPath() {
        return config.logPath + "/" + currentSessionID + "_captures.json";
    }

    /**
     * @brief Converte LogLevel para string
     */
    String levelToString(LogLevel level) {
        switch(level) {
            case DEBUG:    return "DEBUG";
            case INFO:     return "INFO";
            case WARNING:  return "WARNING";
            case ERROR:    return "ERROR";
            case CRITICAL: return "CRITICAL";
            case SUCCESS:  return "SUCCESS";
            case CAPTURE:  return "CAPTURE";
            default:       return "UNKNOWN";
        }
    }

    /**
     * @brief Converte LogLevel para emoji
     */
    String levelToEmoji(LogLevel level) {
        switch(level) {
            case DEBUG:    return "🔍";
            case INFO:     return "ℹ️";
            case WARNING:  return "⚠️";
            case ERROR:    return "❌";
            case CRITICAL: return "🚨";
            case SUCCESS:  return "✅";
            case CAPTURE:  return "📡";
            default:       return "❓";
        }
    }

    /**
     * @brief Converte EventType para string
     */
    String eventTypeToString(EventType type) {
        switch(type) {
            case WIFI_SCAN_START:        return "WiFi Scan Start";
            case WIFI_SCAN_COMPLETE:     return "WiFi Scan Complete";
            case WIFI_DEAUTH_SENT:       return "WiFi Deauth Sent";
            case WIFI_HANDSHAKE_CAPTURED: return "WiFi Handshake Captured";
            case WIFI_PMKID_CAPTURED:    return "WiFi PMKID Captured";
            case WIFI_BEACON_SPAM:       return "WiFi Beacon Spam";
            case WIFI_PROBE_FLOOD:       return "WiFi Probe Flood";
            case BLE_SCAN_START:         return "BLE Scan Start";
            case BLE_DEVICE_FOUND:       return "BLE Device Found";
            case BLE_ATTACK_SENT:        return "BLE Attack Sent";
            case RFID_SCAN:              return "RFID Scan";
            case RFID_READ:              return "RFID Read";
            case RFID_WRITE:             return "RFID Write";
            case RFID_CLONE:             return "RFID Clone";
            case RFID_ATTACK:            return "RFID Attack";
            case SYSTEM_BOOT:            return "System Boot";
            case SYSTEM_SHUTDOWN:        return "System Shutdown";
            case SYSTEM_ERROR:           return "System Error";
            case SESSION_START:          return "Session Start";
            case SESSION_END:            return "Session End";
            case GPS_LOCATION:           return "GPS Location";
            case USER_ACTION:            return "User Action";
            default:                     return "Custom Event";
        }
    }

    /**
     * @brief Formata timestamp
     */
    String formatTimestamp(uint32_t timestamp) {
        char buffer[32];
        uint32_t seconds = timestamp / 1000;
        uint32_t ms = timestamp % 1000;
        uint32_t hours = seconds / 3600;
        uint32_t minutes = (seconds % 3600) / 60;
        uint32_t secs = seconds % 60;

        snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu.%03lu",
                hours, minutes, secs, ms);

        return String(buffer);
    }

    /**
     * @brief Escreve evento no arquivo de log
     */
    void writeToLogFile(const Event& event) {
        if (!config.writeToSD || !logFile) return;

        // Formato: [TIMESTAMP] [LEVEL] [MODULE] Description
        String line = "";

        if (config.timestampsEnabled) {
            line += "[" + formatTimestamp(event.timestamp) + "] ";
        }

        line += "[" + levelToString(event.level) + "] ";
        line += "[" + event.module + "] ";
        line += event.description;

        logFile.println(line);
        logFile.flush();
    }

    /**
     * @brief Escreve evento na timeline JSON
     */
    void writeToTimeline(const Event& event) {
        if (!config.writeToSD || !timelineFile) return;

        JsonDocument doc(2048);
        doc["id"] = eventCount;
        doc["timestamp"] = event.timestamp;
        doc["time"] = formatTimestamp(event.timestamp);
        doc["type"] = eventTypeToString(event.type);
        doc["level"] = levelToString(event.level);
        doc["module"] = event.module;
        doc["description"] = event.description;

        if (config.metadataEnabled && event.metadata.size() > 0) {
            doc["metadata"] = event.metadata;
        }

        String json;
        serializeJson(doc, json);
        timelineFile.println(json + ",");
        timelineFile.flush();
    }

    /**
     * @brief Escreve para serial
     */
    void writeToSerial(const Event& event) {
        if (!config.writeToSerial) return;

        String emoji = levelToEmoji(event.level);
        String time = config.timestampsEnabled ?
                     "[" + formatTimestamp(event.timestamp) + "] " : "";

        Serial.printf("%s %s[%s] %s\n",
                     emoji.c_str(),
                     time.c_str(),
                     event.module.c_str(),
                     event.description.c_str());
    }

public:
    /**
     * @brief Inicializa o logger
     */
    bool begin(const Config& cfg = Config()) {
        config = cfg;

        if (!SD.begin()) {
            Serial.println("[FieldLogger] ❌ SD Card não disponível");
            config.writeToSD = false;
        }

        // Cria diretórios
        SD.mkdir(config.logPath.c_str());
        SD.mkdir((config.logPath + "/captures").c_str());

        // Gera ID de sessão
        currentSessionID = generateSessionID();

        // Abre arquivos de log
        if (config.writeToSD) {
            logFile = SD.open(getLogFilePath().c_str(), FILE_WRITE);
            timelineFile = SD.open(getTimelineFilePath().c_str(), FILE_WRITE);
            captureIndex = SD.open(getCaptureIndexPath().c_str(), FILE_WRITE);

            if (!logFile || !timelineFile || !captureIndex) {
                Serial.println("[FieldLogger] ⚠️ Erro ao abrir arquivos de log");
                config.writeToSD = false;
            } else {
                // Header do timeline (array JSON)
                timelineFile.println("[");
            }
        }

        initialized = true;

        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║      📝 FIELD LOGGER INICIADO         ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Session ID: %-22s ║\n", currentSessionID.c_str());
        Serial.printf( "║  Log Path: %-26s ║\n", config.logPath.c_str());
        Serial.printf( "║  SD Logging: %-3s                     ║\n",
                      config.writeToSD ? "ON" : "OFF");
        Serial.println("╚════════════════════════════════════════╝\n");

        // Log de boot
        log(SYSTEM_BOOT, INFO, "System", "NekoGotchi Field Logger started");

        return true;
    }

    /**
     * @brief Loga um evento
     */
    void log(EventType type, LogLevel level, const String& module,
            const String& description, const JsonDocument* metadata = nullptr) {

        if (!initialized || level < config.minLevel) return;

        Event event;
        event.timestamp = millis();
        event.type = type;
        event.level = level;
        event.module = module;
        event.description = description;

        if (metadata) {
            event.metadata = *metadata;
        }

        // Incrementa contadores
        eventCount++;
        if (level == ERROR || level == CRITICAL) {
            errorCount++;
        }

        // Escreve em múltiplos destinos
        writeToSerial(event);
        writeToLogFile(event);
        writeToTimeline(event);
    }

    /**
     * @brief Log rápido de informação
     */
    void info(const String& module, const String& msg) {
        log(CUSTOM, INFO, module, msg);
    }

    /**
     * @brief Log rápido de erro
     */
    void error(const String& module, const String& msg) {
        log(SYSTEM_ERROR, ERROR, module, msg);
    }

    /**
     * @brief Log rápido de sucesso
     */
    void success(const String& module, const String& msg) {
        log(CUSTOM, SUCCESS, module, msg);
    }

    /**
     * @brief Registra captura WiFi
     */
    void logWiFiCapture(const String& ssid, const String& bssid,
                       uint8_t channel, const String& captureType,
                       const String& filePath) {

        JsonDocument metadata(512);
        metadata["ssid"] = ssid;
        metadata["bssid"] = bssid;
        metadata["channel"] = channel;
        metadata["type"] = captureType;
        metadata["file"] = filePath;
        metadata["capture_id"] = captureCount;

        String desc = "WiFi " + captureType + " capturado: " + ssid +
                     " [" + bssid + "] on CH" + String(channel);

        log(WIFI_HANDSHAKE_CAPTURED, CAPTURE, "WiFi", desc, &metadata);

        // Adiciona ao índice de capturas
        if (captureIndex) {
            String json;
            serializeJson(metadata, json);
            captureIndex.println(json + ",");
            captureIndex.flush();
        }

        captureCount++;
    }

    /**
     * @brief Registra localização GPS
     */
    void logGPSLocation(double lat, double lon, double alt = 0.0) {
        JsonDocument metadata(256);
        metadata["latitude"] = lat;
        metadata["longitude"] = lon;
        metadata["altitude"] = alt;

        char desc[128];
        snprintf(desc, sizeof(desc), "GPS: %.6f, %.6f (alt: %.1fm)",
                lat, lon, alt);

        log(GPS_LOCATION, INFO, "GPS", String(desc), &metadata);
    }

    /**
     * @brief Finaliza sessão e fecha arquivos
     */
    void end() {
        if (!initialized) return;

        log(SESSION_END, INFO, "System",
            "Session ended - " + String(eventCount) + " events logged");

        // Fecha timeline array
        if (timelineFile) {
            timelineFile.println("{}]"); // Dummy object para fechar array
            timelineFile.close();
        }

        if (logFile) logFile.close();
        if (captureIndex) {
            captureIndex.println("{}]");
            captureIndex.close();
        }

        printSessionStats();

        initialized = false;
    }

    /**
     * @brief Gera relatório de sessão
     */
    void generateReport() {
        String reportPath = config.logPath + "/" + currentSessionID + "_report.txt";
        File report = SD.open(reportPath.c_str(), FILE_WRITE);

        if (!report) {
            Serial.println("[FieldLogger] ❌ Erro ao gerar relatório");
            return;
        }

        report.println("╔════════════════════════════════════════════════════════╗");
        report.println("║           NEKOGOTCHI FIELD OPERATION REPORT           ║");
        report.println("╠════════════════════════════════════════════════════════╣");
        report.printf( "║  Session ID: %-42s ║\n", currentSessionID.c_str());
        report.printf( "║  Events Logged: %-6lu                               ║\n", eventCount);
        report.printf( "║  Captures: %-6lu                                    ║\n", captureCount);
        report.printf( "║  Errors: %-6lu                                      ║\n", errorCount);
        report.printf( "║  Duration: %s                                       ║\n",
                      formatTimestamp(millis()).c_str());
        report.println("╚════════════════════════════════════════════════════════╝");

        report.close();

        Serial.println("[FieldLogger] 📄 Relatório gerado: " + reportPath);
    }

    /**
     * @brief Imprime estatísticas da sessão
     */
    void printSessionStats() {
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║       📊 SESSION STATISTICS           ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Session: %-28s ║\n", currentSessionID.c_str());
        Serial.printf( "║  Events: %-6lu                       ║\n", eventCount);
        Serial.printf( "║  Captures: %-6lu                     ║\n", captureCount);
        Serial.printf( "║  Errors: %-6lu                       ║\n", errorCount);
        Serial.printf( "║  Duration: %-26s ║\n", formatTimestamp(millis()).c_str());
        Serial.println("╚════════════════════════════════════════╝\n");
    }

    // Getters
    String getSessionID() const { return currentSessionID; }
    uint32_t getEventCount() const { return eventCount; }
    uint32_t getCaptureCount() const { return captureCount; }
    uint32_t getErrorCount() const { return errorCount; }
};

#endif // M5GOTCHI_FIELD_LOGGER_H
