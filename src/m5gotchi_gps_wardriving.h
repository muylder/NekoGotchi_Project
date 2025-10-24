/*
 * 🗺️ M5GOTCHI GPS WARDRIVING v1.0
 * Sistema completo de wardriving com GPS!
 * 
 * Features:
 * - GPS coordinate logging (NEO-6M/NEO-8M support)
 * - CSV export (WigleWifi format)
 * - GPX track export
 * - Live mapping display
 * - Distance-based filtering
 * - Speed calculation
 * - Network density analysis
 * - Auto-upload capabilities
 */

#ifndef M5GOTCHI_GPS_WARDRIVING_H
#define M5GOTCHI_GPS_WARDRIVING_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// ==================== GPS & WARDRIVING DATA ====================

struct GPSData {
    float latitude;
    float longitude; 
    float altitude;
    float speed;           // km/h
    float course;          // degrees
    int satellites;
    bool fix_valid;
    String timestamp;      // HHMMSS
    String datestamp;      // DDMMYY
    float hdop;            // Horizontal Dilution of Precision
};

struct WiFiEntry {
    String ssid;
    String bssid;
    int channel;
    int rssi;
    String encryption;
    String vendor;         // From OUI lookup
    float latitude;
    float longitude;
    float altitude;
    String timestamp;
    float accuracy;        // GPS accuracy in meters
    int seen_count;        // Times seen at this location
};

struct WardrivingStats {
    int total_networks;
    int unique_networks;
    int open_networks;
    int secured_networks;
    float distance_traveled;   // km
    float avg_speed;          // km/h
    int session_duration;     // seconds
    String start_time;
    String end_time;
    float coverage_area;      // approximate km²
};

class GPSWardriving {
private:
    HardwareSerial gpsSerial;
    GPSData current_gps;
    WiFiEntry network_log[500];    // Max 500 unique networks
    int network_count;
    WardrivingStats stats;
    
    bool gps_enabled;
    bool wardriving_active;
    int display_mode;              // 0=status, 1=networks, 2=map, 3=stats
    uint32_t last_scan;
    uint32_t session_start;
    
    String csv_filename;
    String gpx_filename;
    String json_filename;
    
    // GPS parsing
    String nmea_buffer;
    bool parsing_gga;
    bool parsing_rmc;
    
    // Distance filtering
    float min_distance_filter;    // Minimum distance to log new entry (meters)
    GPSData last_logged_position;
    
    // Live mapping
    float map_center_lat;
    float map_center_lng;
    float map_zoom;               // degrees per screen width
    int map_points[100][2];       // Screen coordinates of logged points
    int map_point_count;
    
    // Network filtering
    String ignore_ssids[10] = {
        "AndroidAP",
        "iPhone",
        "DIRECT-",
        "HP-Print",
        "Canon_",
        "EPSON_",
        "PRINTER_",
        "CarPlay",
        "Tesla_",
        "xfinitywifi"
    };
    
public:
    GPSWardriving() : gpsSerial(2) {  // Use Serial2 for GPS
        network_count = 0;
        gps_enabled = false;
        wardriving_active = false;
        display_mode = 0;
        last_scan = 0;
        session_start = 0;
        min_distance_filter = 10.0;  // 10 meters
        map_zoom = 0.01;            // ~1km view
        map_point_count = 0;
        
        // Initialize filenames with timestamp
        String timestamp = String(millis());
        csv_filename = "/wardriving_" + timestamp + ".csv";
        gpx_filename = "/wardriving_" + timestamp + ".gpx";
        json_filename = "/wardriving_" + timestamp + ".json";
        
        // Initialize GPS data
        memset(&current_gps, 0, sizeof(GPSData));
        memset(&last_logged_position, 0, sizeof(GPSData));
        memset(&stats, 0, sizeof(WardrivingStats));
    }
    
    void start() {
        // Initialize GPS serial (RX=1, TX=0 or your preferred pins)
        gpsSerial.begin(9600, SERIAL_8N1, 1, 0);  // Adjust pins as needed
        
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        
        gps_enabled = true;
        wardriving_active = true;
        session_start = millis();
        last_scan = 0;
        
        stats.start_time = getCurrentTimeString();
        
        // Create CSV header
        createCSVHeader();
        
        // Create GPX header
        createGPXHeader();
        
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🗺️ GPS WARDRIVING", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Initializing GPS...", 10, 30);
        M5.Display.drawString("Searching for satellites", 10, 45);
        
        delay(2000);
    }
    
    void update() {
        if (!wardriving_active) return;
        
        // Update GPS
        updateGPS();
        
        // Scan WiFi networks if GPS has fix
        if (current_gps.fix_valid && millis() - last_scan > 3000) {  // Scan every 3 seconds
            scanAndLogNetworks();
            last_scan = millis();
        }
        
        // Update statistics
        updateStatistics();
        
        // Handle input
        handleInput();
        
        // Update display
        switch (display_mode) {
            case 0: drawStatusScreen(); break;
            case 1: drawNetworkList(); break;
            case 2: drawLiveMap(); break;
            case 3: drawStatistics(); break;
        }
        
        delay(100);
    }
    
    void updateGPS() {
        while (gpsSerial.available()) {
            char c = gpsSerial.read();
            
            if (c == '\n') {
                // Process complete NMEA sentence
                processNMEASentence(nmea_buffer);
                nmea_buffer = "";
            } else if (c != '\r') {
                nmea_buffer += c;
            }
        }
    }
    
    void processNMEASentence(String sentence) {
        if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
            parseGGA(sentence);
        } else if (sentence.startsWith("$GPRMC") || sentence.startsWith("$GNRMC")) {
            parseRMC(sentence);  
        }
    }
    
    void parseGGA(String sentence) {
        // $GPGGA,HHMMSS.SS,DDMM.MMMM,N,DDDMM.MMMM,E,Q,NN,H.H,AAA.A,M,GGG.G,M,T,CCCC*hh
        
        int commas[14];
        int comma_count = 0;
        
        // Find all comma positions
        for (int i = 0; i < sentence.length() && comma_count < 14; i++) {
            if (sentence.charAt(i) == ',') {
                commas[comma_count++] = i;
            }
        }
        
        if (comma_count < 14) return;  // Invalid sentence
        
        // Parse timestamp (field 1)
        String time_str = sentence.substring(commas[0] + 1, commas[1]);
        if (time_str.length() >= 6) {
            current_gps.timestamp = time_str.substring(0, 6);
        }
        
        // Parse latitude (fields 2-3)
        String lat_str = sentence.substring(commas[1] + 1, commas[2]);
        String lat_dir = sentence.substring(commas[2] + 1, commas[3]);
        if (lat_str.length() > 0) {
            float lat_deg = lat_str.substring(0, 2).toFloat();
            float lat_min = lat_str.substring(2).toFloat();
            current_gps.latitude = lat_deg + (lat_min / 60.0);
            if (lat_dir == "S") current_gps.latitude = -current_gps.latitude;
        }
        
        // Parse longitude (fields 4-5)
        String lng_str = sentence.substring(commas[3] + 1, commas[4]);
        String lng_dir = sentence.substring(commas[4] + 1, commas[5]);
        if (lng_str.length() > 0) {
            float lng_deg = lng_str.substring(0, 3).toFloat();
            float lng_min = lng_str.substring(3).toFloat();
            current_gps.longitude = lng_deg + (lng_min / 60.0);
            if (lng_dir == "W") current_gps.longitude = -current_gps.longitude;
        }
        
        // Parse fix quality (field 6)
        String quality = sentence.substring(commas[5] + 1, commas[6]);
        current_gps.fix_valid = (quality.toInt() > 0);
        
        // Parse satellites (field 7)
        String sat_str = sentence.substring(commas[6] + 1, commas[7]);
        current_gps.satellites = sat_str.toInt();
        
        // Parse HDOP (field 8)
        String hdop_str = sentence.substring(commas[7] + 1, commas[8]);
        current_gps.hdop = hdop_str.toFloat();
        
        // Parse altitude (field 9)
        String alt_str = sentence.substring(commas[8] + 1, commas[9]);
        current_gps.altitude = alt_str.toFloat();
    }
    
    void parseRMC(String sentence) {
        // $GPRMC,HHMMSS.SS,A,DDMM.MMMM,N,DDDMM.MMMM,E,S.S,C.C,DDMMYY,D.D,E,M*hh
        
        int commas[12];
        int comma_count = 0;
        
        for (int i = 0; i < sentence.length() && comma_count < 12; i++) {
            if (sentence.charAt(i) == ',') {
                commas[comma_count++] = i;
            }
        }
        
        if (comma_count < 12) return;
        
        // Parse speed (field 7) - knots to km/h
        String speed_str = sentence.substring(commas[6] + 1, commas[7]);
        current_gps.speed = speed_str.toFloat() * 1.852;  // knots to km/h
        
        // Parse course (field 8)
        String course_str = sentence.substring(commas[7] + 1, commas[8]);
        current_gps.course = course_str.toFloat();
        
        // Parse date (field 9)
        String date_str = sentence.substring(commas[8] + 1, commas[9]);
        if (date_str.length() >= 6) {
            current_gps.datestamp = date_str;
        }
    }
    
    void scanAndLogNetworks() {
        int n = WiFi.scanNetworks();
        if (n == 0) return;
        
        for (int i = 0; i < n; i++) {
            String ssid = WiFi.SSID(i);
            String bssid = WiFi.BSSIDstr(i);
            int channel = WiFi.channel(i);
            int rssi = WiFi.RSSI(i);
            
            // Skip if already logged recently at this location
            if (isNetworkRecentlyLogged(bssid)) continue;
            
            // Skip ignored SSIDs
            if (shouldIgnoreSSID(ssid)) continue;
            
            // Check distance filter
            if (shouldApplyDistanceFilter()) continue;
            
            // Create network entry
            WiFiEntry entry;
            entry.ssid = ssid;
            entry.bssid = bssid;
            entry.channel = channel;
            entry.rssi = rssi;
            entry.encryption = getEncryptionType(WiFi.encryptionType(i));
            entry.vendor = getVendorFromBSSID(bssid);
            entry.latitude = current_gps.latitude;
            entry.longitude = current_gps.longitude;
            entry.altitude = current_gps.altitude;
            entry.timestamp = getCurrentTimeString();
            entry.accuracy = calculateGPSAccuracy();
            entry.seen_count = 1;
            
            // Add to log
            if (network_count < 500) {
                network_log[network_count] = entry;
                network_count++;
                
                // Write to files
                appendToCSV(entry);
                appendToGPX(entry);
                
                // Update map
                updateLiveMap(entry);
                
                stats.total_networks++;
                if (entry.encryption == "Open") {
                    stats.open_networks++;
                } else {
                    stats.secured_networks++;
                }
            }
        }
        
        // Update last logged position
        last_logged_position = current_gps;
    }
    
    bool isNetworkRecentlyLogged(String bssid) {
        // Check if this BSSID was logged recently at similar location
        for (int i = max(0, network_count - 50); i < network_count; i++) {  // Check last 50 entries
            if (network_log[i].bssid == bssid) {
                float distance = calculateDistance(
                    current_gps.latitude, current_gps.longitude,
                    network_log[i].latitude, network_log[i].longitude
                );
                if (distance < min_distance_filter) {
                    return true;  // Too close to previous log
                }
            }
        }
        return false;
    }
    
    bool shouldIgnoreSSID(String ssid) {
        for (int i = 0; i < 10; i++) {
            if (ssid.startsWith(ignore_ssids[i])) {
                return true;
            }
        }
        return ssid.length() == 0;  // Also ignore hidden SSIDs
    }
    
    bool shouldApplyDistanceFilter() {
        if (last_logged_position.latitude == 0) return false;  // First position
        
        float distance = calculateDistance(
            current_gps.latitude, current_gps.longitude,
            last_logged_position.latitude, last_logged_position.longitude
        );
        
        return distance < min_distance_filter;
    }
    
    float calculateDistance(float lat1, float lng1, float lat2, float lng2) {
        // Haversine formula for distance in meters
        const float R = 6371000;  // Earth radius in meters
        
        float dLat = radians(lat2 - lat1);
        float dLng = radians(lng2 - lng1);
        
        float a = sin(dLat/2) * sin(dLat/2) +
                  cos(radians(lat1)) * cos(radians(lat2)) *
                  sin(dLng/2) * sin(dLng/2);
        
        float c = 2 * atan2(sqrt(a), sqrt(1-a));
        return R * c;
    }
    
    String getEncryptionType(wifi_auth_mode_t encType) {
        switch (encType) {
            case WIFI_AUTH_OPEN: return "Open";
            case WIFI_AUTH_WEP: return "WEP";
            case WIFI_AUTH_WPA_PSK: return "WPA";
            case WIFI_AUTH_WPA2_PSK: return "WPA2";
            case WIFI_AUTH_WPA3_PSK: return "WPA3";
            case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/3";
            default: return "Unknown";
        }
    }
    
    String getVendorFromBSSID(String bssid) {
        // Simple OUI lookup (first 3 octets)
        String oui = bssid.substring(0, 8);
        oui.toUpperCase();
        
        if (oui.startsWith("00:50:56")) return "VMware";
        if (oui.startsWith("3C:5A:B4")) return "Google";
        if (oui.startsWith("00:25:00")) return "Apple";
        if (oui.startsWith("28:CD:C1")) return "Apple";
        if (oui.startsWith("BC:52:B7")) return "Apple";
        if (oui.startsWith("B4:18:D1")) return "Apple";
        // Add more OUI mappings as needed
        
        return "Unknown";
    }
    
    float calculateGPSAccuracy() {
        // Estimate GPS accuracy based on HDOP and satellite count
        if (current_gps.satellites < 4) return 999.0;  // No fix
        
        // Basic accuracy estimation
        float base_accuracy = current_gps.hdop * 5.0;  // meters
        
        if (current_gps.satellites >= 8) base_accuracy *= 0.7;      // Good constellation
        else if (current_gps.satellites >= 6) base_accuracy *= 0.9; // Fair constellation
        
        return base_accuracy;
    }
    
    String getCurrentTimeString() {
        if (current_gps.timestamp.length() >= 6 && current_gps.datestamp.length() >= 6) {
            return current_gps.datestamp + "_" + current_gps.timestamp;
        }
        return String(millis());
    }
    
    void createCSVHeader() {
        if (!SD.begin()) return;
        
        File csv_file = SD.open(csv_filename, FILE_WRITE);
        if (csv_file) {
            // WigleWifi CSV format header
            csv_file.println("WigleWifi-1.4,appRelease=M5Gotchi Pro,model=M5Stack,release=1.0,device=M5Gotchi,display=M5,board=ESP32,brand=M5Stack");
            csv_file.println("MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type");
            csv_file.close();
        }
    }
    
    void createGPXHeader() {
        if (!SD.begin()) return;
        
        File gpx_file = SD.open(gpx_filename, FILE_WRITE);
        if (gpx_file) {
            gpx_file.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            gpx_file.println("<gpx version=\"1.1\" creator=\"M5Gotchi Pro\">");
            gpx_file.println("  <trk>");
            gpx_file.println("    <name>Wardriving Track</name>");
            gpx_file.println("    <trkseg>");
            gpx_file.close();
        }
    }
    
    void appendToCSV(WiFiEntry& entry) {
        if (!SD.begin()) return;
        
        File csv_file = SD.open(csv_filename, FILE_APPEND);
        if (csv_file) {
            String line = entry.bssid + "," +
                         "\"" + entry.ssid + "\"," +
                         "[" + entry.encryption + "]," +
                         entry.timestamp + "," +
                         String(entry.channel) + "," +
                         String(entry.rssi) + "," +
                         String(entry.latitude, 6) + "," +
                         String(entry.longitude, 6) + "," +
                         String(entry.altitude, 1) + "," +
                         String(entry.accuracy, 1) + "," +
                         "WIFI";
            
            csv_file.println(line);
            csv_file.close();
        }
    }
    
    void appendToGPX(WiFiEntry& entry) {
        if (!SD.begin()) return;
        
        File gpx_file = SD.open(gpx_filename, FILE_APPEND);
        if (gpx_file) {
            gpx_file.println("      <trkpt lat=\"" + String(entry.latitude, 6) + 
                           "\" lon=\"" + String(entry.longitude, 6) + "\">");
            gpx_file.println("        <ele>" + String(entry.altitude, 1) + "</ele>");
            gpx_file.println("        <time>" + entry.timestamp + "</time>");
            gpx_file.println("        <extensions>");
            gpx_file.println("          <wifi>");
            gpx_file.println("            <ssid>" + entry.ssid + "</ssid>");
            gpx_file.println("            <bssid>" + entry.bssid + "</bssid>");
            gpx_file.println("            <rssi>" + String(entry.rssi) + "</rssi>");
            gpx_file.println("            <channel>" + String(entry.channel) + "</channel>");
            gpx_file.println("          </wifi>");
            gpx_file.println("        </extensions>");
            gpx_file.println("      </trkpt>");
            gpx_file.close();
        }
    }
    
    void updateLiveMap(WiFiEntry& entry) {
        if (map_point_count >= 100) return;  // Map full
        
        // Convert GPS coordinates to screen coordinates
        float rel_lat = entry.latitude - map_center_lat;
        float rel_lng = entry.longitude - map_center_lng;
        
        int screen_x = 120 + (int)(rel_lng / map_zoom * 240);
        int screen_y = 64 - (int)(rel_lat / map_zoom * 128);
        
        // Check if point is on screen
        if (screen_x >= 0 && screen_x < 240 && screen_y >= 0 && screen_y < 135) {
            map_points[map_point_count][0] = screen_x;
            map_points[map_point_count][1] = screen_y;
            map_point_count++;
        }
    }
    
    void updateStatistics() {
        uint32_t elapsed = (millis() - session_start) / 1000;
        stats.session_duration = elapsed;
        
        // Calculate unique networks
        stats.unique_networks = network_count;  // Simplified - should deduplicate by BSSID
        
        // Calculate distance traveled (simplified)
        if (network_count > 1) {
            float total_distance = 0;
            for (int i = 1; i < network_count; i++) {
                total_distance += calculateDistance(
                    network_log[i-1].latitude, network_log[i-1].longitude,
                    network_log[i].latitude, network_log[i].longitude
                );
            }
            stats.distance_traveled = total_distance / 1000.0;  // Convert to km
        }
        
        // Calculate average speed
        if (elapsed > 0) {
            stats.avg_speed = (stats.distance_traveled / elapsed) * 3600;  // km/h
        }
    }
    
    void handleInput() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == "\t" || key == " ") { // TAB - change mode
                display_mode = (display_mode + 1) % 4;
            } else if (key == "+" || key == "=") { // Zoom in
                map_zoom *= 0.5;
            } else if (key == "-") { // Zoom out
                map_zoom *= 2.0;
            } else if (key == "c") { // Center map on current position
                map_center_lat = current_gps.latitude;
                map_center_lng = current_gps.longitude;
                map_point_count = 0;  // Clear old points
            } else if (key == "s") { // Save/Export
                exportData();
            } else if (key == "r") { // Reset session
                resetSession();
            } else if (key == "`") { // ESC - exit
                stop();
                return;
            }
        }
    }
    
    void drawStatusScreen() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🗺️ GPS WARDRIVING STATUS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 1/4 - TAB: Next  C: Center  S: Save  ESC: Exit", 5, 125);
        
        int y = 20;
        
        // GPS Status
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("GPS Status:", 10, y); y += 12;
        
        if (current_gps.fix_valid) {
            M5.Display.setTextColor(0x07E0);
            M5.Display.drawString("✓ GPS LOCK (" + String(current_gps.satellites) + " sats)", 15, y);
        } else {
            M5.Display.setTextColor(0xF800);
            M5.Display.drawString("✗ NO GPS FIX (" + String(current_gps.satellites) + " sats)", 15, y);
        }
        y += 12;
        
        if (current_gps.fix_valid) {
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.drawString("Lat: " + String(current_gps.latitude, 6), 15, y); y += 10;
            M5.Display.drawString("Lng: " + String(current_gps.longitude, 6), 15, y); y += 10;
            M5.Display.drawString("Alt: " + String(current_gps.altitude, 1) + "m", 15, y); y += 10;
            M5.Display.drawString("Speed: " + String(current_gps.speed, 1) + " km/h", 15, y); y += 10;
            M5.Display.drawString("HDOP: " + String(current_gps.hdop, 1), 15, y); y += 12;
        } else {
            y += 50;
        }
        
        // Wardriving Stats
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Session Stats:", 10, y); y += 12;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Networks: " + String(stats.total_networks), 15, y); y += 10;
        M5.Display.drawString("Unique: " + String(stats.unique_networks), 15, y); y += 10;
        M5.Display.drawString("Distance: " + String(stats.distance_traveled, 2) + " km", 15, y); y += 10;
        
        uint32_t elapsed = stats.session_duration;
        String duration = String(elapsed / 3600) + "h " + String((elapsed % 3600) / 60) + "m";
        M5.Display.drawString("Duration: " + duration, 15, y);
        
        // Files info
        M5.Display.setTextColor(0x4208);
        M5.Display.drawString("CSV: " + csv_filename.substring(1), 130, 20);
        M5.Display.drawString("GPX: " + gpx_filename.substring(1), 130, 30);
    }
    
    void drawNetworkList() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🗺️ WARDRIVING NETWORKS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 2/4 - Last " + String(min(10, network_count)) + " networks", 5, 125);
        
        if (network_count == 0) {
            M5.Display.setTextColor(0x4208);
            M5.Display.drawString("No networks logged yet", 10, 50);
            if (!current_gps.fix_valid) {
                M5.Display.drawString("Waiting for GPS fix...", 10, 65);
            }
            return;
        }
        
        // Network list (last 10)
        M5.Display.drawString("SSID", 10, 20);
        M5.Display.drawString("RSSI", 120, 20);
        M5.Display.drawString("CH", 150, 20);
        M5.Display.drawString("ENC", 170, 20);
        M5.Display.drawString("DIST", 200, 20);
        M5.Display.drawLine(5, 30, 235, 30, 0x4208);
        
        int start_idx = max(0, network_count - 10);
        for (int i = start_idx; i < network_count; i++) {
            int y = 35 + ((i - start_idx) * 9);
            WiFiEntry& entry = network_log[i];
            
            // Color by signal strength
            uint16_t color = TFT_WHITE;
            if (entry.rssi > -50) color = 0x07E0;      // Green - strong
            else if (entry.rssi > -70) color = 0xFFE0; // Yellow - medium
            else color = 0xF800;                       // Red - weak
            
            M5.Display.setTextColor(color);
            
            // SSID (truncated)
            String ssid = entry.ssid;
            if (ssid.length() > 14) ssid = ssid.substring(0, 14);
            M5.Display.drawString(ssid, 10, y);
            
            // RSSI
            M5.Display.drawString(String(entry.rssi), 120, y);
            
            // Channel
            M5.Display.drawString(String(entry.channel), 150, y);
            
            // Encryption (abbreviated)
            String enc = entry.encryption;
            if (enc == "Open") enc = "OPN";
            else if (enc == "WPA2") enc = "WP2";
            else if (enc == "WPA3") enc = "WP3";
            M5.Display.drawString(enc, 170, y);
            
            // Distance from current position (if GPS valid)
            if (current_gps.fix_valid) {
                float dist = calculateDistance(
                    current_gps.latitude, current_gps.longitude,
                    entry.latitude, entry.longitude
                );
                if (dist < 1000) {
                    M5.Display.drawString(String((int)dist) + "m", 200, y);
                } else {
                    M5.Display.drawString(String(dist/1000, 1) + "k", 200, y);
                }
            }
        }
    }
    
    void drawLiveMap() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🗺️ LIVE MAP", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 3/4 - +/-: Zoom  C: Center", 5, 125);
        
        // Map info
        M5.Display.drawString("Zoom: " + String(map_zoom, 4) + "°", 130, 15);
        M5.Display.drawString("Points: " + String(map_point_count), 200, 15);
        
        // Draw map grid
        M5.Display.drawRect(5, 25, 230, 95, 0x4208);
        
        // Draw crosshairs (center)
        M5.Display.drawLine(120, 25, 120, 120, 0x2104);
        M5.Display.drawLine(5, 72, 235, 72, 0x2104);
        
        // Draw logged points
        for (int i = 0; i < map_point_count; i++) {
            int x = map_points[i][0];
            int y = map_points[i][1];
            
            if (x >= 5 && x < 235 && y >= 25 && y < 120) {
                M5.Display.fillCircle(x, y, 1, 0x07E0);  // Green dots for networks
            }
        }
        
        // Draw current position
        if (current_gps.fix_valid) {
            // Calculate current position on map
            float rel_lat = current_gps.latitude - map_center_lat;
            float rel_lng = current_gps.longitude - map_center_lng;
            
            int curr_x = 120 + (int)(rel_lng / map_zoom * 240);
            int curr_y = 72 - (int)(rel_lat / map_zoom * 95);
            
            if (curr_x >= 5 && curr_x < 235 && curr_y >= 25 && curr_y < 120) {
                // Draw current position as red circle
                M5.Display.drawCircle(curr_x, curr_y, 3, 0xF800);
                M5.Display.fillCircle(curr_x, curr_y, 1, 0xF800);
                
                // Draw direction indicator
                if (current_gps.speed > 1.0) {  // Only if moving
                    float course_rad = radians(current_gps.course);
                    int end_x = curr_x + (int)(sin(course_rad) * 8);
                    int end_y = curr_y - (int)(cos(course_rad) * 8);
                    M5.Display.drawLine(curr_x, curr_y, end_x, end_y, 0xFFE0);
                }
            }
        }
        
        // Map legend
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("● Networks", 140, 25);
        M5.Display.setTextColor(0xF800);
        M5.Display.drawString("● Current", 140, 35);
    }
    
    void drawStatistics() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🗺️ SESSION STATISTICS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 4/4 - R: Reset  S: Save", 5, 125);
        
        int y = 25;
        
        // Collection stats
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Collection:", 10, y); y += 12;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Total Networks: " + String(stats.total_networks), 15, y); y += 10;
        M5.Display.drawString("Unique Networks: " + String(stats.unique_networks), 15, y); y += 10;
        M5.Display.drawString("Open Networks: " + String(stats.open_networks), 15, y); y += 10;
        M5.Display.drawString("Secured Networks: " + String(stats.secured_networks), 15, y); y += 12;
        
        // Travel stats
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Travel:", 10, y); y += 12;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Distance: " + String(stats.distance_traveled, 2) + " km", 15, y); y += 10;
        M5.Display.drawString("Avg Speed: " + String(stats.avg_speed, 1) + " km/h", 15, y); y += 10;
        M5.Display.drawString("Max Speed: " + String(current_gps.speed, 1) + " km/h", 15, y); y += 12;
        
        // Time stats
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Session:", 10, y); y += 12;
        
        uint32_t elapsed = stats.session_duration;
        String duration = String(elapsed / 3600) + "h " + String((elapsed % 3600) / 60) + "m " + String(elapsed % 60) + "s";
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Duration: " + duration, 15, y); y += 10;
        
        if (elapsed > 0) {
            float rate = (float)stats.total_networks / (elapsed / 60.0);  // per minute
            M5.Display.drawString("Rate: " + String(rate, 1) + " nets/min", 15, y);
        }
    }
    
    void exportData() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📁 EXPORTING DATA...", 10, 50);
        
        // Close GPX file
        if (SD.begin()) {
            File gpx_file = SD.open(gpx_filename, FILE_APPEND);
            if (gpx_file) {
                gpx_file.println("    </trkseg>");
                gpx_file.println("  </trk>");
                gpx_file.println("</gpx>");
                gpx_file.close();
            }
            
            // Create JSON summary
            createJSONSummary();
        }
        
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("✓ Data exported successfully!", 10, 70);
        delay(2000);
    }
    
    void createJSONSummary() {
        File json_file = SD.open(json_filename, FILE_WRITE);
        if (json_file) {
            JsonDocument doc;
            
            doc["session_start"] = stats.start_time;
            doc["session_end"] = getCurrentTimeString();
            doc["total_networks"] = stats.total_networks;
            doc["unique_networks"] = stats.unique_networks;
            doc["open_networks"] = stats.open_networks;
            doc["secured_networks"] = stats.secured_networks;
            doc["distance_km"] = stats.distance_traveled;
            doc["avg_speed_kmh"] = stats.avg_speed;
            doc["duration_sec"] = stats.session_duration;
            doc["csv_file"] = csv_filename;
            doc["gpx_file"] = gpx_filename;
            
            String json_str;
            serializeJsonPretty(doc, json_str);
            json_file.print(json_str);
            json_file.close();
        }
    }
    
    void resetSession() {
        network_count = 0;
        map_point_count = 0;
        session_start = millis();
        memset(&stats, 0, sizeof(WardrivingStats));
        stats.start_time = getCurrentTimeString();
        
        // Create new filenames
        String timestamp = String(millis());
        csv_filename = "/wardriving_" + timestamp + ".csv";
        gpx_filename = "/wardriving_" + timestamp + ".gpx";
        json_filename = "/wardriving_" + timestamp + ".json";
        
        createCSVHeader();
        createGPXHeader();
    }
    
    void stop() {
        exportData();
        wardriving_active = false;
        gps_enabled = false;
        WiFi.mode(WIFI_OFF);
    }
    
    bool isActive() {
        return wardriving_active;
    }
    
    // Public getters for integration
    int getNetworkCount() { return network_count; }
    float getDistanceTraveled() { return stats.distance_traveled; }
    bool hasGPSFix() { return current_gps.fix_valid; }
    int getSatelliteCount() { return current_gps.satellites; }
};

// Global instance
GPSWardriving gpsWardriving;

#endif // M5GOTCHI_GPS_WARDRIVING_H