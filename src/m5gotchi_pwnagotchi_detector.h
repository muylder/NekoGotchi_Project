/*
 * 🤖 M5GOTCHI PWNAGOTCHI DETECTOR v1.0
 * Detecta e interage com Pwnagotchis na área!
 * 
 * Features:
 * - Auto-detection via beacon payload analysis
 * - Parse JSON data (name, pwnd count, version)
 * - Friendship system with other Pwnagotchis
 * - Monitor mode for tracking multiple units
 * - Display detailed stats and info
 * - Save encounters to SD card log
 */

#ifndef M5GOTCHI_PWNAGOTCHI_DETECTOR_H
#define M5GOTCHI_PWNAGOTCHI_DETECTOR_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include <SD.h>

// ==================== PWNAGOTCHI DATA STRUCTURES ====================

struct PwnagotchiInfo {
    String name;
    String identity;          // Unique identity hash
    uint8_t bssid[6];        // MAC address
    int pwnd_tot;            // Total handshakes captured
    int pwnd_run;            // Handshakes this session
    String version;          // Pwnagotchi version
    bool deauth;             // Currently deauthing
    int uptime;              // Uptime in seconds
    int channel;             // WiFi channel
    int rssi;                // Signal strength
    String policy;           // AI policy
    String state;            // Current state
    uint32_t last_seen;      // Last detection timestamp
    bool is_friend;          // Friend status
    int encounter_count;     // Number of times encountered
};

struct DetectionStats {
    int total_detected;
    int unique_units;
    int friends_found;
    int active_units;        // Seen in last 5 minutes
    uint32_t first_detection;
    uint32_t last_detection;
};

class PwnagotchiDetector {
private:
    PwnagotchiInfo detected_units[10];  // Max 10 concurrent Pwnagotchis
    int unit_count;
    DetectionStats stats;
    bool is_scanning;
    int display_mode;        // 0=list, 1=details, 2=monitor, 3=stats
    int selected_unit;
    uint32_t scan_start_time;
    String log_filename;
    
    // Pwnagotchi detection signatures
    uint8_t pwn_oui[3] = {0xDE, 0xAD, 0xBE};  // Custom OUI for detection
    String pwn_ssids[5] = {
        "pwnagotchi",
        "pwned",
        "unit7", 
        "unit42",
        "pwny"
    };
    
public:
    PwnagotchiDetector() {
        unit_count = 0;
        is_scanning = false;
        display_mode = 0;
        selected_unit = 0;
        scan_start_time = 0;
        log_filename = "/pwnagotchi_log.txt";
        
        // Initialize stats
        memset(&stats, 0, sizeof(DetectionStats));
        
        // Initialize units array
        for (int i = 0; i < 10; i++) {
            detected_units[i] = {"", "", {0}, 0, 0, "", false, 0, 0, 0, "", "", 0, false, 0};
        }
    }
    
    void start() {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        
        // Setup promiscuous mode for beacon analysis
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);
        
        is_scanning = true;
        scan_start_time = millis();
        stats.first_detection = millis();
        
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🤖 PWNAGOTCHI HUNT", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Scanning for fellow Pwnagotchis...", 10, 30);
        M5.Display.drawString("This may take a few moments", 10, 45);
        
        delay(2000);
    }
    
    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        extern PwnagotchiDetector pwnagotchiDetector;
        pwnagotchiDetector.processPacket(buf, type);
    }
    
    void processPacket(void* buf, wifi_promiscuous_pkt_type_t type) {
        if (type != WIFI_PKT_MGMT) return;
        
        wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
        wifi_pkt_rx_ctrl_t ctrl = pkt->rx_ctrl;
        uint8_t* frame = pkt->payload;
        
        // Check if it's a beacon frame
        uint16_t frame_ctrl = *(uint16_t*)frame;
        uint8_t frame_type = (frame_ctrl & 0x0C) >> 2;
        uint8_t frame_subtype = (frame_ctrl & 0xF0) >> 4;
        
        if (frame_type != 0 || frame_subtype != 8) return;  // Not a beacon
        
        // Extract BSSID (transmitter address)
        uint8_t* bssid = frame + 16;
        
        // Check for Pwnagotchi signatures
        if (isPwnagotchiBeacon(frame, ctrl.sig_len, bssid, ctrl.channel, ctrl.rssi)) {
            // Found a potential Pwnagotchi!
            processPwnagotchiBeacon(frame, ctrl.sig_len, bssid, ctrl.channel, ctrl.rssi);
        }
    }
    
    bool isPwnagotchiBeacon(uint8_t* frame, int length, uint8_t* bssid, int channel, int rssi) {
        // Method 1: Check for custom OUI in BSSID
        if (memcmp(bssid, pwn_oui, 3) == 0) {
            return true;
        }
        
        // Method 2: Parse SSID for Pwnagotchi patterns
        // Beacon format: Fixed parameters (12 bytes) + Variable elements
        uint8_t* ssid_element = frame + 36;  // Skip to first IE
        
        if (ssid_element[0] == 0x00) {  // SSID element ID
            uint8_t ssid_len = ssid_element[1];
            if (ssid_len > 0 && ssid_len < 32) {
                String ssid = "";
                for (int i = 0; i < ssid_len; i++) {
                    ssid += (char)ssid_element[2 + i];
                }
                
                // Check against known Pwnagotchi SSIDs
                for (int i = 0; i < 5; i++) {
                    if (ssid.indexOf(pwn_ssids[i]) >= 0) {
                        return true;
                    }
                }
                
                // Check for JSON-like patterns in SSID (sometimes they leak data here)
                if (ssid.indexOf("{") >= 0 || ssid.indexOf("pwnd") >= 0) {
                    return true;
                }
            }
        }
        
        // Method 3: Look for JSON payload in vendor-specific IEs
        uint8_t* ie_ptr = frame + 36;  // Start of Information Elements
        int remaining = length - 36;
        
        while (remaining > 2) {
            uint8_t ie_id = ie_ptr[0];
            uint8_t ie_len = ie_ptr[1];
            
            if (ie_len == 0 || ie_len > remaining - 2) break;
            
            // Vendor-specific IE (0xDD) often contains Pwnagotchi JSON
            if (ie_id == 0xDD && ie_len > 10) {
                // Look for JSON patterns
                uint8_t* payload = ie_ptr + 2;
                String payload_str = "";
                
                for (int i = 0; i < min(ie_len, 50); i++) {
                    if (payload[i] >= 32 && payload[i] <= 126) {  // Printable ASCII
                        payload_str += (char)payload[i];
                    }
                }
                
                if (payload_str.indexOf("pwnd") >= 0 || 
                    payload_str.indexOf("name") >= 0 ||
                    payload_str.indexOf("version") >= 0) {
                    return true;
                }
            }
            
            ie_ptr += 2 + ie_len;
            remaining -= 2 + ie_len;
        }
        
        return false;
    }
    
    void processPwnagotchiBeacon(uint8_t* frame, int length, uint8_t* bssid, int channel, int rssi) {
        // Try to extract JSON data from the beacon
        PwnagotchiInfo info;
        memcpy(info.bssid, bssid, 6);
        info.channel = channel;
        info.rssi = rssi;
        info.last_seen = millis();
        
        // Create BSSID string for identity
        char bssid_str[18];
        sprintf(bssid_str, "%02X:%02X:%02X:%02X:%02X:%02X", 
                bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        info.identity = String(bssid_str);
        
        // Extract data from beacon (simplified - real implementation would be more complex)
        if (extractPwnagotchiData(frame, length, info)) {
            addOrUpdateUnit(info);
        } else {
            // Even if we can't extract full data, mark as detected Pwnagotchi
            info.name = "Unknown Pwnagotchi";
            info.version = "Unknown";
            info.pwnd_tot = -1;  // Unknown
            addOrUpdateUnit(info);
        }
    }
    
    bool extractPwnagotchiData(uint8_t* frame, int length, PwnagotchiInfo& info) {
        // Look for JSON data in vendor-specific IEs
        uint8_t* ie_ptr = frame + 36;
        int remaining = length - 36;
        
        while (remaining > 2) {
            uint8_t ie_id = ie_ptr[0];
            uint8_t ie_len = ie_ptr[1];
            
            if (ie_len == 0 || ie_len > remaining - 2) break;
            
            if (ie_id == 0xDD && ie_len > 20) {  // Vendor-specific with enough data
                uint8_t* payload = ie_ptr + 2;
                
                // Try to parse as JSON (simplified)
                String json_str = "";
                bool found_json = false;
                
                for (int i = 0; i < ie_len; i++) {
                    if (payload[i] == '{') {
                        found_json = true;
                    }
                    if (found_json && payload[i] >= 32 && payload[i] <= 126) {
                        json_str += (char)payload[i];
                        if (payload[i] == '}') break;
                    }
                }
                
                if (json_str.length() > 10) {
                    return parseJSON(json_str, info);
                }
            }
            
            ie_ptr += 2 + ie_len;
            remaining -= 2 + ie_len;
        }
        
        return false;
    }
    
    bool parseJSON(String json_str, PwnagotchiInfo& info) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json_str);
        
        if (error) {
            // Fallback: try to extract key values manually
            if (json_str.indexOf("name") >= 0) {
                int name_start = json_str.indexOf("name") + 7;
                int name_end = json_str.indexOf("\"", name_start);
                if (name_end > name_start) {
                    info.name = json_str.substring(name_start, name_end);
                }
            }
            
            if (json_str.indexOf("pwnd_tot") >= 0) {
                int pwnd_start = json_str.indexOf("pwnd_tot") + 10;
                int pwnd_end = json_str.indexOf(",", pwnd_start);
                if (pwnd_end == -1) pwnd_end = json_str.indexOf("}", pwnd_start);
                if (pwnd_end > pwnd_start) {
                    info.pwnd_tot = json_str.substring(pwnd_start, pwnd_end).toInt();
                }
            }
            
            return info.name.length() > 0;
        }
        
        // Successfully parsed JSON
        if (doc.containsKey("name")) {
            info.name = doc["name"].as<String>();
        }
        if (doc.containsKey("pwnd_tot")) {
            info.pwnd_tot = doc["pwnd_tot"];
        }
        if (doc.containsKey("pwnd_run")) {
            info.pwnd_run = doc["pwnd_run"];
        }
        if (doc.containsKey("version")) {
            info.version = doc["version"].as<String>();
        }
        if (doc.containsKey("deauth")) {
            info.deauth = doc["deauth"];
        }
        if (doc.containsKey("uptime")) {
            info.uptime = doc["uptime"];
        }
        if (doc.containsKey("policy")) {
            info.policy = doc["policy"].as<String>();
        }
        if (doc.containsKey("state")) {
            info.state = doc["state"].as<String>();
        }
        
        return true;
    }
    
    void addOrUpdateUnit(PwnagotchiInfo& new_info) {
        // Check if we already know this unit
        for (int i = 0; i < unit_count; i++) {
            if (detected_units[i].identity == new_info.identity) {
                // Update existing unit
                detected_units[i].last_seen = new_info.last_seen;
                detected_units[i].rssi = new_info.rssi;
                detected_units[i].channel = new_info.channel;
                detected_units[i].encounter_count++;
                
                // Update data if we got better info
                if (new_info.name.length() > 0 && detected_units[i].name.length() == 0) {
                    detected_units[i].name = new_info.name;
                }
                if (new_info.pwnd_tot >= 0) {
                    detected_units[i].pwnd_tot = new_info.pwnd_tot;
                    detected_units[i].pwnd_run = new_info.pwnd_run;
                }
                if (new_info.version.length() > 0) {
                    detected_units[i].version = new_info.version;
                }
                
                stats.last_detection = millis();
                return;
            }
        }
        
        // New unit - add to list
        if (unit_count < 10) {
            new_info.encounter_count = 1;
            detected_units[unit_count] = new_info;
            unit_count++;
            stats.total_detected++;
            stats.unique_units++;
            stats.last_detection = millis();
            
            // Log the discovery
            logDiscovery(new_info);
        }
    }
    
    void logDiscovery(PwnagotchiInfo& info) {
        if (!SD.begin()) return;
        
        File log_file = SD.open(log_filename, FILE_APPEND);
        if (log_file) {
            String log_entry = String(millis()) + "," + 
                              info.identity + "," + 
                              info.name + "," + 
                              String(info.pwnd_tot) + "," + 
                              info.version + "," + 
                              String(info.rssi) + "," + 
                              String(info.channel) + "\n";
            log_file.print(log_entry);
            log_file.close();
        }
    }
    
    void update() {
        if (!is_scanning) return;
        
        handleInput();
        updateStats();
        
        switch (display_mode) {
            case 0: drawUnitList(); break;
            case 1: drawUnitDetails(); break;
            case 2: drawMonitorMode(); break;
            case 3: drawDetectionStats(); break;
        }
        
        delay(100);
    }
    
    void handleInput() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == "\t" || key == " ") { // TAB - change mode
                display_mode = (display_mode + 1) % 4;
            } else if (key == "k" || key.indexOf("Up") >= 0) { // Up
                if (selected_unit > 0) selected_unit--;
            } else if (key == "j" || key.indexOf("Down") >= 0) { // Down
                if (selected_unit < unit_count - 1) selected_unit++;
            } else if (key == "f") { // Toggle friend
                if (unit_count > 0 && selected_unit < unit_count) {
                    detected_units[selected_unit].is_friend = !detected_units[selected_unit].is_friend;
                    updateFriendStats();
                }
            } else if (key == "r") { // Reset
                resetDetection();
            } else if (key == "`") { // ESC - exit
                stop();
                return;
            }
        }
    }
    
    void updateStats() {
        // Count active units (seen in last 5 minutes)
        stats.active_units = 0;
        uint32_t cutoff = millis() - (5 * 60 * 1000);  // 5 minutes ago
        
        for (int i = 0; i < unit_count; i++) {
            if (detected_units[i].last_seen > cutoff) {
                stats.active_units++;
            }
        }
    }
    
    void updateFriendStats() {
        stats.friends_found = 0;
        for (int i = 0; i < unit_count; i++) {
            if (detected_units[i].is_friend) {
                stats.friends_found++;
            }
        }
    }
    
    void drawUnitList() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🤖 PWNAGOTCHI DETECTOR", 5, 5);
        M5.Display.setTextColor(TFT_WHITE); 
        M5.Display.drawString("Mode 1/4 - Found: " + String(unit_count) + " units", 5, 15);
        M5.Display.drawString("↑↓: Select  F: Friend  TAB: Mode  ESC: Exit", 5, 125);
        
        if (unit_count == 0) {
            M5.Display.setTextColor(0x4208);
            M5.Display.drawString("No Pwnagotchis detected yet...", 10, 50);
            M5.Display.drawString("Keep scanning!", 10, 65);
            return;
        }
        
        // Draw unit list
        int start_y = 30;
        for (int i = 0; i < min(8, unit_count); i++) {
            int y = start_y + (i * 12);
            PwnagotchiInfo& unit = detected_units[i];
            
            // Highlight selected unit
            if (i == selected_unit) {
                M5.Display.fillRect(5, y - 2, 230, 11, 0x2104);
            }
            
            // Friend indicator
            uint16_t color = unit.is_friend ? 0x07E0 : TFT_WHITE;
            if (unit.is_friend) {
                M5.Display.setTextColor(0x07E0);
                M5.Display.drawString("♥", 8, y);
            }
            
            M5.Display.setTextColor(color);
            
            // Name
            String display_name = unit.name.length() > 0 ? unit.name : "Unknown";
            if (display_name.length() > 12) display_name = display_name.substring(0, 12);
            M5.Display.drawString(display_name, 20, y);
            
            // Pwnd count
            if (unit.pwnd_tot >= 0) {
                M5.Display.drawString("Pwnd:" + String(unit.pwnd_tot), 100, y);
            }
            
            // Signal strength
            M5.Display.drawString("RSSI:" + String(unit.rssi), 150, y);
            
            // Channel
            M5.Display.drawString("Ch" + String(unit.channel), 200, y);
            
            // Activity indicator
            uint32_t age = (millis() - unit.last_seen) / 1000;
            if (age < 30) {
                M5.Display.fillCircle(225, y + 4, 2, 0x07E0);  // Green - active
            } else if (age < 300) {
                M5.Display.fillCircle(225, y + 4, 2, 0xFFE0);  // Yellow - recent
            } else {
                M5.Display.fillCircle(225, y + 4, 2, 0xF800);  // Red - old
            }
        }
        
        // Show more indicator
        if (unit_count > 8) {
            M5.Display.setTextColor(0x4208);
            M5.Display.drawString("+" + String(unit_count - 8) + " more units...", 10, 120);
        }
    }
    
    void drawUnitDetails() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🤖 UNIT DETAILS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 2/4 - Unit " + String(selected_unit + 1) + "/" + String(unit_count), 5, 125);
        
        if (unit_count == 0 || selected_unit >= unit_count) {
            M5.Display.setTextColor(0x4208);
            M5.Display.drawString("No unit selected", 10, 50);
            return;
        }
        
        PwnagotchiInfo& unit = detected_units[selected_unit];
        
        // Unit info
        int y = 25;
        
        // Name
        M5.Display.setTextColor(unit.is_friend ? 0x07E0 : 0x07FF);
        String display_name = unit.name.length() > 0 ? unit.name : "Unknown Unit";
        M5.Display.drawString("Name: " + display_name, 10, y); y += 12;
        
        // Friend status
        if (unit.is_friend) {
            M5.Display.setTextColor(0x07E0);
            M5.Display.drawString("Status: ♥ FRIEND ♥", 10, y);
        } else {
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.drawString("Status: Stranger", 10, y);
        }
        y += 15;
        
        // Identity
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("BSSID: " + unit.identity, 10, y); y += 12;
        
        // Stats
        if (unit.pwnd_tot >= 0) {
            M5.Display.setTextColor(0xFFE0);
            M5.Display.drawString("Total Pwnd: " + String(unit.pwnd_tot), 10, y); y += 10;
            M5.Display.drawString("Session Pwnd: " + String(unit.pwnd_run), 10, y); y += 12;
        }
        
        // Technical info
        M5.Display.setTextColor(0x4208);
        M5.Display.drawString("Version: " + (unit.version.length() > 0 ? unit.version : "Unknown"), 10, y); y += 10;
        M5.Display.drawString("Channel: " + String(unit.channel), 10, y); y += 10;
        M5.Display.drawString("RSSI: " + String(unit.rssi) + " dBm", 10, y); y += 10;
        M5.Display.drawString("Encounters: " + String(unit.encounter_count), 10, y); y += 12;
        
        // Activity
        uint32_t age = (millis() - unit.last_seen) / 1000;
        String age_str;
        if (age < 60) age_str = String(age) + "s ago";
        else if (age < 3600) age_str = String(age / 60) + "m ago";
        else age_str = String(age / 3600) + "h ago";
        
        if (age < 30) M5.Display.setTextColor(0x07E0);
        else if (age < 300) M5.Display.setTextColor(0xFFE0);
        else M5.Display.setTextColor(0xF800);
        
        M5.Display.drawString("Last Seen: " + age_str, 10, y);
        
        // Current state
        if (unit.state.length() > 0) {
            y += 12;
            M5.Display.setTextColor(0x07FF);
            M5.Display.drawString("State: " + unit.state, 10, y);
        }
    }
    
    void drawMonitorMode() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🤖 LIVE MONITOR", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 3/4 - Real-time activity", 5, 125);
        
        // Activity summary
        int y = 20;
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Active Units: " + String(stats.active_units) + "/" + String(unit_count), 10, y); y += 15;
        
        // Active units list
        uint32_t cutoff = millis() - (5 * 60 * 1000);  // 5 minutes
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Recently Active:", 10, y); y += 12;
        
        int shown = 0;
        for (int i = 0; i < unit_count && shown < 6; i++) {
            if (detected_units[i].last_seen > cutoff) {
                PwnagotchiInfo& unit = detected_units[i];
                
                uint16_t color = unit.is_friend ? 0x07E0 : TFT_WHITE;
                M5.Display.setTextColor(color);
                
                String name = unit.name.length() > 0 ? unit.name : "Unknown";
                if (name.length() > 10) name = name.substring(0, 10);
                
                uint32_t age = (millis() - unit.last_seen) / 1000;
                String activity = String(age) + "s";
                
                M5.Display.drawString("• " + name + " (" + activity + ")", 15, y);
                
                // Show pwnd count if available
                if (unit.pwnd_tot >= 0) {
                    M5.Display.setTextColor(0xFFE0);
                    M5.Display.drawString(String(unit.pwnd_tot), 180, y);
                }
                
                y += 10;
                shown++;
            }
        }
        
        if (shown == 0) {
            M5.Display.setTextColor(0x4208);
            M5.Display.drawString("  No recent activity", 15, y);
        }
    }
    
    void drawDetectionStats() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🤖 DETECTION STATS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 4/4 - Overall statistics", 5, 125);
        
        // Statistics
        int y = 25;
        
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Detection Summary:", 10, y); y += 15;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Total Detected: " + String(stats.total_detected), 15, y); y += 10;
        M5.Display.drawString("Unique Units: " + String(stats.unique_units), 15, y); y += 10;
        M5.Display.drawString("Friends Made: " + String(stats.friends_found), 15, y); y += 10;
        M5.Display.drawString("Currently Active: " + String(stats.active_units), 15, y); y += 15;
        
        // Timing
        uint32_t scan_time = (millis() - scan_start_time) / 1000;
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Timing:", 10, y); y += 12;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Scan Duration: " + formatTime(scan_time), 15, y); y += 10;
        
        if (stats.last_detection > 0) {
            uint32_t last_detect = (millis() - stats.last_detection) / 1000;
            M5.Display.drawString("Last Detection: " + formatTime(last_detect) + " ago", 15, y);
            y += 10;
        }
        
        // Detection rate
        if (scan_time > 0) {
            float rate = (float)stats.total_detected / (scan_time / 60.0);  // per minute
            M5.Display.drawString("Rate: " + String(rate, 1) + " detections/min", 15, y);
        }
    }
    
    String formatTime(uint32_t seconds) {
        if (seconds < 60) {
            return String(seconds) + "s";
        } else if (seconds < 3600) {
            return String(seconds / 60) + "m " + String(seconds % 60) + "s";
        } else {
            return String(seconds / 3600) + "h " + String((seconds % 3600) / 60) + "m";
        }
    }
    
    void resetDetection() {
        unit_count = 0;
        selected_unit = 0;
        memset(&stats, 0, sizeof(DetectionStats));
        stats.first_detection = millis();
        scan_start_time = millis();
        
        // Clear units array
        for (int i = 0; i < 10; i++) {
            detected_units[i] = {"", "", {0}, 0, 0, "", false, 0, 0, 0, "", "", 0, false, 0};
        }
    }
    
    void stop() {
        esp_wifi_set_promiscuous(false);
        is_scanning = false;
        WiFi.mode(WIFI_OFF);
    }
    
    bool isActive() {
        return is_scanning;
    }
    
    // Public methods for integration
    int getDetectedCount() { return unit_count; }
    int getFriendsCount() { return stats.friends_found; }
    String getLastDetectedName() {
        if (unit_count > 0) {
            return detected_units[unit_count - 1].name;
        }
        return "";
    }
};

// Global instance
PwnagotchiDetector pwnagotchiDetector;

#endif // M5GOTCHI_PWNAGOTCHI_DETECTOR_H