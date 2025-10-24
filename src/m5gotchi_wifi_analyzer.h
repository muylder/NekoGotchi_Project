/*
 * 📊 M5GOTCHI ADVANCED WIFI ANALYZER v1.0
 * Analisador WiFi profissional com gráficos!
 * 
 * Features:
 * - Channel graph visualization
 * - Signal strength heatmap
 * - Network density analysis
 * - Best channel recommendation
 * - Vendor detection (OUI)
 * - Encryption type statistics
 */

#ifndef M5GOTCHI_WIFI_ANALYZER_H
#define M5GOTCHI_WIFI_ANALYZER_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>

// ==================== ANALYZER DATA ====================

struct ChannelStats {
    int channel;
    int networkCount;
    int avgRSSI;
    int maxRSSI;
    int minRSSI;
    float utilization;
};

struct VendorStats {
    String vendor;
    int count;
};

struct EncryptionStats {
    int open;
    int wep;
    int wpa;
    int wpa2;
    int wpa3;
};

struct NetworkData {
    String ssid;
    String bssid;
    int channel;
    int rssi;
    int encryption;
    String vendor;
    unsigned long lastSeen;
};

class WiFiAnalyzer {
private:
    ChannelStats channels[14];
    NetworkData networks[50];
    int networkCount;
    int displayMode; // 0=graph, 1=list, 2=stats, 3=heatmap
    bool isScanning;
    unsigned long lastScan;
    EncryptionStats encStats;
    
    // Simple OUI database (top vendors)
    struct OUIEntry {
        String mac_prefix;
        String vendor;
    };
    
    OUIEntry oui_db[20] = {
        {"00:50:56", "VMware"},
        {"00:0C:29", "VMware"},
        {"08:00:27", "VirtualBox"},
        {"00:16:3E", "Xen"},
        {"52:54:00", "QEMU"},
        {"AC:DE:48", "Private"},
        {"00:50:C2", "IEEE"},
        {"3C:5A:B4", "Google"},
        {"DA:A1:19", "Google"},
        {"02:1A:11", "Google"},
        {"00:25:00", "Apple"},
        {"28:CD:C1", "Apple"},
        {"BC:52:B7", "Apple"},
        {"B4:18:D1", "Apple"},
        {"00:BB:3A", "Apple"},
        {"C8:69:CD", "Apple"},
        {"30:90:AB", "Apple"},
        {"00:17:F2", "Apple"},
        {"00:26:08", "Apple"},
        {"F0:98:9D", "Apple"}
    };
    
public:
    WiFiAnalyzer() {
        networkCount = 0;
        displayMode = 0;
        isScanning = false;
        lastScan = 0;
        
        // Initialize channel stats
        for (int i = 0; i < 14; i++) {
            channels[i] = {i + 1, 0, 0, -100, 0, 0.0};
        }
        
        // Initialize encryption stats
        encStats = {0, 0, 0, 0, 0};
    }
    
    void start() {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(2);
        M5.Display.drawString("📊 WIFI ANALYZER", 10, 10);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Starting scan...", 10, 40);
        
        delay(1000);
        isScanning = true;
    }
    
    void run() {
        if (!isScanning) {
            start();
            return;
        }
        
        // Auto-scan every 5 seconds
        if (millis() - lastScan > 5000) {
            scanNetworks();
            lastScan = millis();
        }
        
        // Handle input
        handleInput();
        
        // Display current mode
        switch (displayMode) {
            case 0: showChannelGraph(); break;
            case 1: showNetworkList(); break;
            case 2: showStatistics(); break;
            case 3: showHeatmap(); break;
        }
        
        delay(100);
    }
    
    void scanNetworks() {
        int n = WiFi.scanNetworks();
        if (n == 0) return;
        
        // Reset channel stats
        for (int i = 0; i < 14; i++) {
            channels[i].networkCount = 0;
            channels[i].avgRSSI = 0;
            channels[i].maxRSSI = -100;
            channels[i].minRSSI = 0;
        }
        
        // Reset encryption stats
        encStats = {0, 0, 0, 0, 0};
        
        networkCount = min(n, 50);
        
        for (int i = 0; i < networkCount; i++) {
            networks[i].ssid = WiFi.SSID(i);
            networks[i].bssid = WiFi.BSSIDstr(i);
            networks[i].channel = WiFi.channel(i);
            networks[i].rssi = WiFi.RSSI(i);
            networks[i].encryption = WiFi.encryptionType(i);
            networks[i].vendor = getVendor(networks[i].bssid);
            networks[i].lastSeen = millis();
            
            // Update channel stats
            int ch = networks[i].channel - 1;
            if (ch >= 0 && ch < 14) {
                channels[ch].networkCount++;
                channels[ch].avgRSSI += networks[i].rssi;
                if (networks[i].rssi > channels[ch].maxRSSI) {
                    channels[ch].maxRSSI = networks[i].rssi;
                }
                if (networks[i].rssi < channels[ch].minRSSI || channels[ch].minRSSI == 0) {
                    channels[ch].minRSSI = networks[i].rssi;
                }
            }
            
            // Update encryption stats
            switch (networks[i].encryption) {
                case WIFI_AUTH_OPEN: encStats.open++; break;
                case WIFI_AUTH_WEP: encStats.wep++; break;
                case WIFI_AUTH_WPA_PSK: encStats.wpa++; break;
                case WIFI_AUTH_WPA2_PSK: encStats.wpa2++; break;
                case WIFI_AUTH_WPA3_PSK: encStats.wpa3++; break;
            }
        }
        
        // Calculate average RSSI for channels
        for (int i = 0; i < 14; i++) {
            if (channels[i].networkCount > 0) {
                channels[i].avgRSSI /= channels[i].networkCount;
                channels[i].utilization = (float)channels[i].networkCount / networkCount * 100.0;
            }
        }
    }
    
    String getVendor(String bssid) {
        String prefix = bssid.substring(0, 8);
        prefix.toUpperCase();
        
        for (int i = 0; i < 20; i++) {
            if (prefix.startsWith(oui_db[i].mac_prefix)) {
                return oui_db[i].vendor;
            }
        }
        return "Unknown";
    }
    
    void handleInput() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == "\t" || key == " ") { // TAB or SPACE
                displayMode = (displayMode + 1) % 4;
            } else if (key == "r") {
                // Reset and rescan
                scanNetworks();
            } else if (key == "`") { // ESC
                isScanning = false;
            }
        }
    }
    
    void showChannelGraph() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📊 CHANNEL GRAPH", 10, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 1/4 - TAB: Next  R: Rescan  ESC: Exit", 10, 125);
        
        // Draw channel bars
        int barWidth = 15;
        int barSpacing = 17;
        int maxHeight = 80;
        
        for (int i = 0; i < 13; i++) {
            int x = 10 + (i * barSpacing);
            int height = map(channels[i].networkCount, 0, 10, 0, maxHeight);
            
            // Color coding
            uint16_t color = 0x07E0; // Green
            if (channels[i].networkCount > 3) color = 0xFFE0; // Yellow
            if (channels[i].networkCount > 6) color = 0xF800; // Red
            
            // Draw bar
            M5.Display.fillRect(x, 100 - height, barWidth, height, color);
            M5.Display.drawRect(x, 20, barWidth, maxHeight, 0x4208);
            
            // Channel number
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.drawString(String(i + 1), x + 2, 105);
            
            // Network count
            if (channels[i].networkCount > 0) {
                M5.Display.setTextColor(color);
                M5.Display.drawString(String(channels[i].networkCount), x + 2, 15);
            }
        }
        
        // Legend
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("Low", 200, 30);
        M5.Display.setTextColor(0xFFE0);
        M5.Display.drawString("Med", 200, 45);
        M5.Display.setTextColor(0xF800);
        M5.Display.drawString("High", 200, 60);
        
        // Best channel recommendation
        int bestChannel = getBestChannel();
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Best: Ch " + String(bestChannel), 200, 80);
        
        // Total networks
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Total: " + String(networkCount) + " APs", 200, 95);
    }
    
    void showNetworkList() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📋 NETWORK LIST", 10, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 2/4 - TAB: Next  R: Rescan  ESC: Exit", 10, 125);
        
        // Header
        M5.Display.drawString("SSID", 10, 20);
        M5.Display.drawString("CH", 120, 20);
        M5.Display.drawString("RSSI", 140, 20);
        M5.Display.drawString("SEC", 180, 20);
        M5.Display.drawLine(10, 30, 230, 30, 0x4208);
        
        // Show first 8 networks
        int maxDisplay = min(8, networkCount);
        for (int i = 0; i < maxDisplay; i++) {
            int y = 35 + (i * 11);
            
            // Color by signal strength
            uint16_t color = 0xF800; // Red (weak)
            if (networks[i].rssi > -70) color = 0xFFE0; // Yellow (medium)
            if (networks[i].rssi > -50) color = 0x07E0; // Green (strong)
            
            M5.Display.setTextColor(color);
            
            // SSID (truncated)
            String ssid = networks[i].ssid;
            if (ssid.length() > 14) ssid = ssid.substring(0, 14);
            M5.Display.drawString(ssid, 10, y);
            
            // Channel
            M5.Display.drawString(String(networks[i].channel), 120, y);
            
            // RSSI
            M5.Display.drawString(String(networks[i].rssi), 140, y);
            
            // Security
            String sec = "OPEN";
            if (networks[i].encryption != WIFI_AUTH_OPEN) sec = "SEC";
            M5.Display.drawString(sec, 180, y);
            
            // Signal bars
            int bars = map(networks[i].rssi, -100, -30, 1, 4);
            for (int b = 0; b < bars; b++) {
                M5.Display.fillRect(210 + (b * 3), y + 5 - b, 2, b + 1, color);
            }
        }
        
        if (networkCount > 8) {
            M5.Display.setTextColor(0x4208);
            M5.Display.drawString("+" + String(networkCount - 8) + " more...", 10, 120);
        }
    }
    
    void showStatistics() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📈 STATISTICS", 10, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 3/4 - TAB: Next  R: Rescan  ESC: Exit", 10, 125);
        
        // Network count
        M5.Display.drawString("Total Networks: " + String(networkCount), 10, 25);
        
        // Channel distribution
        M5.Display.drawString("Channel Distribution:", 10, 40);
        M5.Display.drawString("Ch 1-3: " + String(channels[0].networkCount + channels[1].networkCount + channels[2].networkCount), 10, 50);
        M5.Display.drawString("Ch 4-9: " + String(getChannelRange(3, 8)), 10, 60);
        M5.Display.drawString("Ch 10-13: " + String(getChannelRange(9, 12)), 10, 70);
        
        // Encryption stats
        M5.Display.drawString("Security Types:", 130, 40);
        M5.Display.setTextColor(0xF800);
        M5.Display.drawString("Open: " + String(encStats.open), 130, 50);
        M5.Display.setTextColor(0xFFE0);
        M5.Display.drawString("WEP: " + String(encStats.wep), 130, 60);
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("WPA2: " + String(encStats.wpa2), 130, 70);
        
        // Best channels
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Recommended Channels:", 10, 85);
        M5.Display.setTextColor(TFT_WHITE);
        
        int best1 = getBestChannel();
        int best2 = getBestChannel(best1);
        int best3 = getBestChannel(best1, best2);
        
        M5.Display.drawString("1st: Ch " + String(best1) + " (" + String(channels[best1-1].networkCount) + " APs)", 10, 95);
        M5.Display.drawString("2nd: Ch " + String(best2) + " (" + String(channels[best2-1].networkCount) + " APs)", 10, 105);
        M5.Display.drawString("3rd: Ch " + String(best3) + " (" + String(channels[best3-1].networkCount) + " APs)", 10, 115);
    }
    
    void showHeatmap() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("🌡️ CHANNEL HEATMAP", 10, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 4/4 - TAB: Next  R: Rescan  ESC: Exit", 10, 125);
        
        // Draw heatmap grid
        for (int ch = 0; ch < 13; ch++) {
            int x = 15 + (ch * 16);
            int networks = channels[ch].networkCount;
            
            // Draw channel column
            for (int level = 0; level < 6; level++) {
                int y = 100 - (level * 12);
                
                uint16_t color = TFT_BLACK;
                if (networks > level) {
                    // Heat color based on level
                    if (level < 2) color = 0x001F;      // Blue (low)
                    else if (level < 4) color = 0x07E0; // Green (medium)
                    else if (level < 5) color = 0xFFE0; // Yellow (high)
                    else color = 0xF800;                // Red (very high)
                }
                
                M5.Display.fillRect(x, y, 14, 10, color);
                M5.Display.drawRect(x, y, 14, 10, 0x4208);
            }
            
            // Channel label
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.drawString(String(ch + 1), x + 2, 110);
        }
        
        // Legend
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Activity Level:", 10, 20);
        
        M5.Display.fillRect(10, 30, 8, 6, 0x001F);
        M5.Display.drawString("Low", 25, 30);
        
        M5.Display.fillRect(10, 40, 8, 6, 0x07E0);
        M5.Display.drawString("Med", 25, 40);
        
        M5.Display.fillRect(10, 50, 8, 6, 0xFFE0);
        M5.Display.drawString("High", 25, 50);
        
        M5.Display.fillRect(10, 60, 8, 6, 0xF800);
        M5.Display.drawString("Max", 25, 60);
    }
    
    int getBestChannel(int exclude1 = -1, int exclude2 = -1) {
        int bestCh = 1;
        int minNetworks = 999;
        
        // Prefer channels 1, 6, 11 (non-overlapping)
        int preferredChannels[] = {1, 6, 11};
        
        for (int i = 0; i < 3; i++) {
            int ch = preferredChannels[i];
            if (ch == exclude1 || ch == exclude2) continue;
            
            if (channels[ch - 1].networkCount < minNetworks) {
                minNetworks = channels[ch - 1].networkCount;
                bestCh = ch;
            }
        }
        
        return bestCh;
    }
    
    int getChannelRange(int start, int end) {
        int count = 0;
        for (int i = start; i <= end && i < 14; i++) {
            count += channels[i].networkCount;
        }
        return count;
    }
    
    void stop() {
        isScanning = false;
        WiFi.mode(WIFI_OFF);
    }
};

// Global instance
WiFiAnalyzer wifiAnalyzer;

#endif // M5GOTCHI_WIFI_ANALYZER_H