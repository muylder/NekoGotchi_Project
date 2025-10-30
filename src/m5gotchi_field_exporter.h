/**
 * @file m5gotchi_field_exporter.h
 * @brief Sistema de export de dados para formatos profissionais de pentest
 *
 * Exporta capturas e logs em formatos compatíveis com ferramentas padrão:
 * - PCAP/PCAPNG (Wireshark, Aircrack-ng)
 * - CSV (Excel, análise)
 * - JSON (scripts, automação)
 * - Wigle WiFi (wardriving)
 * - KML (Google Earth)
 *
 * @version 1.0.0
 * @date 2025-10-30
 */

#ifndef M5GOTCHI_FIELD_EXPORTER_H
#define M5GOTCHI_FIELD_EXPORTER_H

#include <SD.h>
#include <ArduinoJson.h>

/**
 * @brief Exportador de dados para formatos profissionais
 */
class FieldExporter {
public:
    /**
     * @brief Formatos de export suportados
     */
    enum Format {
        PCAP,        // Packet capture (Wireshark)
        PCAPNG,      // PCAP Next Generation
        CSV,         // Comma-Separated Values
        JSON,        // JavaScript Object Notation
        XML,         // Extensible Markup Language
        WIGLE,       // Wigle WiFi formato
        KML,         // Keyhole Markup Language (Google Earth)
        TXT          // Plain text report
    };

    /**
     * @brief Estrutura de AP WiFi para export
     */
    struct WiFiAP {
        String ssid;
        String bssid;
        int8_t rssi;
        uint8_t channel;
        String encryption;
        uint32_t timestamp;
        double latitude = 0.0;
        double longitude = 0.0;
        bool hasHandshake = false;
        bool hasPMKID = false;
    };

    /**
     * @brief Estrutura de pacote capturado
     */
    struct Packet {
        uint32_t timestamp;
        uint16_t length;
        uint8_t channel;
        int8_t rssi;
        uint8_t* data;
        String type;
    };

private:
    String exportPath = "/nekogotchi/export";

    /**
     * @brief Cabeçalho PCAP Global Header (24 bytes)
     */
    struct PCAPGlobalHeader {
        uint32_t magic_number = 0xa1b2c3d4;   // PCAP magic
        uint16_t version_major = 2;
        uint16_t version_minor = 4;
        int32_t  thiszone = 0;                // GMT to local correction
        uint32_t sigfigs = 0;                 // accuracy of timestamps
        uint32_t snaplen = 65535;             // max length of packets
        uint32_t network = 105;               // Data link type (IEEE 802.11)
    } __attribute__((packed));

    /**
     * @brief Cabeçalho de pacote PCAP (16 bytes)
     */
    struct PCAPPacketHeader {
        uint32_t ts_sec;         // timestamp seconds
        uint32_t ts_usec;        // timestamp microseconds
        uint32_t incl_len;       // number of octets saved
        uint32_t orig_len;       // actual length of packet
    } __attribute__((packed));

    /**
     * @brief Escreve cabeçalho PCAP
     */
    bool writePCAPHeader(File& file) {
        PCAPGlobalHeader header;
        return file.write((uint8_t*)&header, sizeof(header)) == sizeof(header);
    }

    /**
     * @brief Escreve pacote no arquivo PCAP
     */
    bool writePCAPPacket(File& file, const Packet& packet) {
        PCAPPacketHeader pktHeader;
        pktHeader.ts_sec = packet.timestamp / 1000000;
        pktHeader.ts_usec = packet.timestamp % 1000000;
        pktHeader.incl_len = packet.length;
        pktHeader.orig_len = packet.length;

        // Escreve header do pacote
        if (file.write((uint8_t*)&pktHeader, sizeof(pktHeader)) != sizeof(pktHeader)) {
            return false;
        }

        // Escreve dados do pacote
        return file.write(packet.data, packet.length) == packet.length;
    }

    /**
     * @brief Escapa string para CSV
     */
    String escapeCSV(const String& str) {
        String escaped = str;
        escaped.replace("\"", "\"\"");
        if (escaped.indexOf(',') >= 0 || escaped.indexOf('"') >= 0 || escaped.indexOf('\n') >= 0) {
            escaped = "\"" + escaped + "\"";
        }
        return escaped;
    }

    /**
     * @brief Escapa string para XML
     */
    String escapeXML(const String& str) {
        String escaped = str;
        escaped.replace("&", "&amp;");
        escaped.replace("<", "&lt;");
        escaped.replace(">", "&gt;");
        escaped.replace("\"", "&quot;");
        escaped.replace("'", "&apos;");
        return escaped;
    }

public:
    /**
     * @brief Inicializa exporter
     */
    bool begin() {
        if (!SD.begin()) {
            Serial.println("[FieldExporter] ❌ SD Card não disponível");
            return false;
        }

        SD.mkdir(exportPath.c_str());

        Serial.println("[FieldExporter] 📦 Export system iniciado");
        return true;
    }

    /**
     * @brief Exporta lista de APs WiFi para CSV
     */
    bool exportWiFiToCSV(const WiFiAP* aps, size_t count, const String& filename) {
        String path = exportPath + "/" + filename + ".csv";
        File file = SD.open(path.c_str(), FILE_WRITE);

        if (!file) {
            Serial.println("[FieldExporter] ❌ Erro ao criar CSV");
            return false;
        }

        // Header CSV
        file.println("SSID,BSSID,RSSI,Channel,Encryption,Timestamp,Latitude,Longitude,Handshake,PMKID");

        // Dados
        for (size_t i = 0; i < count; i++) {
            const WiFiAP& ap = aps[i];

            file.print(escapeCSV(ap.ssid)); file.print(",");
            file.print(escapeCSV(ap.bssid)); file.print(",");
            file.print(ap.rssi); file.print(",");
            file.print(ap.channel); file.print(",");
            file.print(escapeCSV(ap.encryption)); file.print(",");
            file.print(ap.timestamp); file.print(",");
            file.print(ap.latitude, 6); file.print(",");
            file.print(ap.longitude, 6); file.print(",");
            file.print(ap.hasHandshake ? "YES" : "NO"); file.print(",");
            file.print(ap.hasPMKID ? "YES" : "NO");
            file.println();
        }

        file.close();

        Serial.println("[FieldExporter] ✅ CSV exportado: " + path);
        Serial.printf("[FieldExporter]    %d APs exportados\n", count);

        return true;
    }

    /**
     * @brief Exporta lista de APs WiFi para JSON
     */
    bool exportWiFiToJSON(const WiFiAP* aps, size_t count, const String& filename) {
        String path = exportPath + "/" + filename + ".json";
        File file = SD.open(path.c_str(), FILE_WRITE);

        if (!file) {
            Serial.println("[FieldExporter] ❌ Erro ao criar JSON");
            return false;
        }

        JsonDocument doc(8192);
        doc["export_type"] = "wifi_scan";
        doc["timestamp"] = millis();
        doc["ap_count"] = count;

        JsonArray apsArray = doc["access_points"].to<JsonArray>();

        for (size_t i = 0; i < count; i++) {
            const WiFiAP& ap = aps[i];

            JsonObject apObj = apsArray.add<JsonObject>();
            apObj["ssid"] = ap.ssid;
            apObj["bssid"] = ap.bssid;
            apObj["rssi"] = ap.rssi;
            apObj["channel"] = ap.channel;
            apObj["encryption"] = ap.encryption;
            apObj["timestamp"] = ap.timestamp;

            if (ap.latitude != 0.0 || ap.longitude != 0.0) {
                JsonObject gps = apObj["gps"].to<JsonObject>();
                gps["lat"] = ap.latitude;
                gps["lon"] = ap.longitude;
            }

            JsonObject flags = apObj["flags"].to<JsonObject>();
            flags["handshake"] = ap.hasHandshake;
            flags["pmkid"] = ap.hasPMKID;
        }

        serializeJsonPretty(doc, file);
        file.close();

        Serial.println("[FieldExporter] ✅ JSON exportado: " + path);

        return true;
    }

    /**
     * @brief Exporta para formato Wigle WiFi
     *
     * Formato: WigleWifi-1.4,appRelease,model,release,device,display,board,brand
     *          MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type
     */
    bool exportToWigle(const WiFiAP* aps, size_t count, const String& filename) {
        String path = exportPath + "/" + filename + ".wigle";
        File file = SD.open(path.c_str(), FILE_WRITE);

        if (!file) {
            Serial.println("[FieldExporter] ❌ Erro ao criar Wigle");
            return false;
        }

        // Header Wigle
        file.println("WigleWifi-1.4,NekoGotchi,ESP32-S3,1.0.0,M5Cardputer,240x135,M5Stack,M5Stack");
        file.println("MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type");

        // Dados
        for (size_t i = 0; i < count; i++) {
            const WiFiAP& ap = aps[i];

            file.print(ap.bssid); file.print(",");
            file.print(escapeCSV(ap.ssid)); file.print(",");
            file.print(escapeCSV(ap.encryption)); file.print(",");

            // Timestamp ISO8601-ish
            char timestamp[32];
            snprintf(timestamp, sizeof(timestamp), "2025-10-30T%02lu:%02lu:%02lu",
                    (ap.timestamp / 3600000) % 24,
                    (ap.timestamp / 60000) % 60,
                    (ap.timestamp / 1000) % 60);
            file.print(timestamp); file.print(",");

            file.print(ap.channel); file.print(",");
            file.print(ap.rssi); file.print(",");
            file.print(ap.latitude, 6); file.print(",");
            file.print(ap.longitude, 6); file.print(",");
            file.print("0,0,WIFI"); // Altitude, Accuracy, Type
            file.println();
        }

        file.close();

        Serial.println("[FieldExporter] ✅ Wigle exportado: " + path);
        Serial.println("[FieldExporter]    Compatível com Wigle WiFi Wardriving");

        return true;
    }

    /**
     * @brief Exporta para KML (Google Earth)
     */
    bool exportToKML(const WiFiAP* aps, size_t count, const String& filename) {
        String path = exportPath + "/" + filename + ".kml";
        File file = SD.open(path.c_str(), FILE_WRITE);

        if (!file) {
            Serial.println("[FieldExporter] ❌ Erro ao criar KML");
            return false;
        }

        // Header KML
        file.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        file.println("<kml xmlns=\"http://www.opengis.net/kml/2.2\">");
        file.println("  <Document>");
        file.println("    <name>NekoGotchi WiFi Wardriving</name>");
        file.println("    <description>WiFi Access Points captured by NekoGotchi</description>");

        // Estilos para diferentes tipos de segurança
        file.println("    <Style id=\"open\">");
        file.println("      <IconStyle><color>ff00ff00</color></IconStyle>");
        file.println("    </Style>");
        file.println("    <Style id=\"wpa\">");
        file.println("      <IconStyle><color>ff0000ff</color></IconStyle>");
        file.println("    </Style>");
        file.println("    <Style id=\"wep\">");
        file.println("      <IconStyle><color>ffff0000</color></IconStyle>");
        file.println("    </Style>");

        // Placemarks para cada AP
        for (size_t i = 0; i < count; i++) {
            const WiFiAP& ap = aps[i];

            if (ap.latitude == 0.0 && ap.longitude == 0.0) continue;

            String style = "open";
            if (ap.encryption.indexOf("WPA") >= 0) style = "wpa";
            else if (ap.encryption.indexOf("WEP") >= 0) style = "wep";

            file.println("    <Placemark>");
            file.println("      <name>" + escapeXML(ap.ssid) + "</name>");
            file.println("      <description>");
            file.println("        BSSID: " + escapeXML(ap.bssid) + "<br/>");
            file.println("        Channel: " + String(ap.channel) + "<br/>");
            file.println("        RSSI: " + String(ap.rssi) + " dBm<br/>");
            file.println("        Encryption: " + escapeXML(ap.encryption) + "<br/>");
            if (ap.hasHandshake) file.println("        <b>Handshake Captured!</b><br/>");
            if (ap.hasPMKID) file.println("        <b>PMKID Captured!</b><br/>");
            file.println("      </description>");
            file.println("      <styleUrl>#" + style + "</styleUrl>");
            file.println("      <Point>");
            file.printf(  "        <coordinates>%.6f,%.6f,0</coordinates>\n",
                         ap.longitude, ap.latitude);
            file.println("      </Point>");
            file.println("    </Placemark>");
        }

        file.println("  </Document>");
        file.println("</kml>");
        file.close();

        Serial.println("[FieldExporter] ✅ KML exportado: " + path);
        Serial.println("[FieldExporter]    Abra no Google Earth");

        return true;
    }

    /**
     * @brief Cria arquivo PCAP vazio pronto para receber pacotes
     */
    File createPCAP(const String& filename) {
        String path = exportPath + "/" + filename + ".pcap";
        File file = SD.open(path.c_str(), FILE_WRITE);

        if (!file) {
            Serial.println("[FieldExporter] ❌ Erro ao criar PCAP");
            return file;
        }

        if (!writePCAPHeader(file)) {
            Serial.println("[FieldExporter] ❌ Erro ao escrever header PCAP");
            file.close();
            return File();
        }

        Serial.println("[FieldExporter] ✅ PCAP criado: " + path);
        Serial.println("[FieldExporter]    Compatível com Wireshark/Aircrack-ng");

        return file;
    }

    /**
     * @brief Adiciona pacote ao arquivo PCAP
     */
    bool addPacketToPCAP(File& pcapFile, const Packet& packet) {
        if (!pcapFile) return false;
        return writePCAPPacket(pcapFile, packet);
    }

    /**
     * @brief Gera relatório completo em texto
     */
    bool generateTextReport(const String& sessionID, const JsonDocument& stats) {
        String path = exportPath + "/" + sessionID + "_report.txt";
        File file = SD.open(path.c_str(), FILE_WRITE);

        if (!file) return false;

        file.println("╔════════════════════════════════════════════════════════════╗");
        file.println("║              NEKOGOTCHI FIELD OPERATION REPORT            ║");
        file.println("╠════════════════════════════════════════════════════════════╣");
        file.println("║                                                            ║");
        file.printf( "║  Session ID: %-45s ║\n", sessionID.c_str());
        file.println("║  Device: M5Stack Cardputer (ESP32-S3)                      ║");
        file.println("║  Firmware: NekoGotchi Field v1.0.0                         ║");
        file.println("║                                                            ║");
        file.println("╠════════════════════════════════════════════════════════════╣");
        file.println("║  SUMMARY                                                   ║");
        file.println("╠════════════════════════════════════════════════════════════╣");

        if (stats.containsKey("wifi_aps_found")) {
            file.printf( "║  WiFi APs Found: %-6d                                    ║\n",
                        stats["wifi_aps_found"].as<int>());
        }
        if (stats.containsKey("handshakes")) {
            file.printf( "║  Handshakes Captured: %-6d                              ║\n",
                        stats["handshakes"].as<int>());
        }
        if (stats.containsKey("pmkids")) {
            file.printf( "║  PMKIDs Captured: %-6d                                  ║\n",
                        stats["pmkids"].as<int>());
        }

        file.println("╠════════════════════════════════════════════════════════════╣");
        file.println("║  EXPORTED FILES                                            ║");
        file.println("╠════════════════════════════════════════════════════════════╣");
        file.println("║  • [SessionID].csv - WiFi AP list (spreadsheet)            ║");
        file.println("║  • [SessionID].json - Structured data (automation)         ║");
        file.println("║  • [SessionID].wigle - Wigle WiFi format (wardriving)      ║");
        file.println("║  • [SessionID].kml - Google Earth format (mapping)         ║");
        file.println("║  • [SessionID].pcap - Packet capture (Wireshark/Aircrack)  ║");
        file.println("╚════════════════════════════════════════════════════════════╝");

        file.close();

        Serial.println("[FieldExporter] ✅ Relatório gerado: " + path);

        return true;
    }

    /**
     * @brief Lista arquivos exportados
     */
    void listExports() {
        File root = SD.open(exportPath.c_str());
        if (!root) return;

        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║        📁 EXPORTED FILES              ║");
        Serial.println("╠════════════════════════════════════════╣");

        File file = root.openNextFile();
        int count = 0;

        while (file) {
            if (!file.isDirectory()) {
                Serial.printf("║  %-36s  ║\n", file.name());
                count++;
            }
            file = root.openNextFile();
        }

        Serial.printf("╠════════════════════════════════════════╣\n");
        Serial.printf("║  Total: %-2d arquivos                  ║\n", count);
        Serial.println("╚════════════════════════════════════════╝\n");

        root.close();
    }
};

#endif // M5GOTCHI_FIELD_EXPORTER_H
