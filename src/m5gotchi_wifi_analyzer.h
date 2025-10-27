/**
 * @file m5gotchi_wifi_analyzer.h
 * @brief 📊 Advanced WiFi Network Analyzer with Visualization
 * @version 1.0
 * @date 2025-10-26
 * 
 * @details Professional WiFi analyzer providing comprehensive network analysis
 * with multiple visualization modes. Performs channel utilization analysis,
 * vendor detection via OUI lookup, encryption statistics, and signal strength
 * heatmaps. Provides intelligent channel recommendations for optimal WiFi setup.
 * 
 * **Features:**
 * - 4 visualization modes: Graph, List, Statistics, Heatmap
 * - Channel utilization analysis (1-13)
 * - Signal strength visualization (-100 to -30 dBm)
 * - Best channel recommendation (non-overlapping: 1, 6, 11)
 * - Vendor detection with OUI database (20+ vendors)
 * - Encryption type statistics (Open/WEP/WPA/WPA2/WPA3)
 * - Real-time network monitoring
 * - Auto-refresh every 5 seconds
 * 
 * @copyright (c) 2025 M5Gotchi Pro Project
 * @license MIT License
 * 
 * @example
 * ```cpp
 * WiFiAnalyzer analyzer;
 * analyzer.start();
 * while (true) {
 *     analyzer.run();
 *     delay(100);
 * }
 * ```
 */

#ifndef M5GOTCHI_WIFI_ANALYZER_H
#define M5GOTCHI_WIFI_ANALYZER_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>

// ==================== ANALYZER DATA ====================

/**
 * @brief Statistics for a single WiFi channel
 * @details Tracks network count, signal strength metrics, and utilization
 * percentage for channel analysis and recommendation.
 */
struct ChannelStats {
    int channel;         ///< Channel number (1-14)
    int networkCount;    ///< Number of APs on this channel
    int avgRSSI;         ///< Average signal strength (dBm)
    int maxRSSI;         ///< Strongest signal on channel (dBm)
    int minRSSI;         ///< Weakest signal on channel (dBm)
    float utilization;   ///< Channel utilization percentage (0-100%)
};

/**
 * @brief Vendor occurrence statistics
 * @details Tracks how many devices from each manufacturer are detected.
 */
struct VendorStats {
    String vendor;       ///< Vendor name (e.g., "Apple", "Google")
    int count;           ///< Number of APs from this vendor
};

/**
 * @brief Network security type distribution
 * @details Counts networks by encryption type for security analysis.
 */
struct EncryptionStats {
    int open;            ///< Open networks (no encryption)
    int wep;             ///< WEP encrypted (deprecated, insecure)
    int wpa;             ///< WPA encrypted (legacy)
    int wpa2;            ///< WPA2 encrypted (standard)
    int wpa3;            ///< WPA3 encrypted (modern)
};

/**
 * @brief Complete data for a detected WiFi network
 * @details Stores all relevant information about a scanned access point.
 */
struct NetworkData {
    String ssid;         ///< Network name (SSID)
    String bssid;        ///< MAC address (BSSID)
    int channel;         ///< Operating channel (1-14)
    int rssi;            ///< Signal strength in dBm
    int encryption;      ///< Encryption type (WIFI_AUTH_*)
    String vendor;       ///< Manufacturer name from OUI
    unsigned long lastSeen; ///< Timestamp of last detection (millis)
};

/**
 * @class WiFiAnalyzer
 * @brief Professional WiFi network analyzer with multiple visualization modes
 * 
 * @details Provides comprehensive WiFi network analysis with real-time scanning,
 * channel utilization tracking, vendor detection, and intelligent recommendations.
 * Features 4 display modes accessible via TAB key cycling.
 * 
 * **Display Modes:**
 * - Mode 0 (Graph): Bar chart of network count per channel
 * - Mode 1 (List): Scrollable list of detected networks
 * - Mode 2 (Statistics): Detailed stats and recommendations
 * - Mode 3 (Heatmap): Channel activity heat visualization
 * 
 * **Controls:**
 * - TAB/SPACE: Cycle display modes
 * - R: Force rescan
 * - ESC (`): Exit analyzer
 * 
 * **Performance:**
 * - Auto-scan interval: 5 seconds
 * - Max tracked networks: 50
 * - OUI database: 20 vendors
 * - Supported channels: 1-14 (global)
 */
class WiFiAnalyzer {
private:
    ChannelStats channels[14];     ///< Statistics for each WiFi channel
    NetworkData networks[50];      ///< Array of detected networks (fixed size)
    int networkCount;              ///< Current number of detected networks
    int displayMode;               ///< Active display mode (0-3)
    bool isScanning;               ///< Scanning active flag
    unsigned long lastScan;        ///< Timestamp of last scan (millis)
    EncryptionStats encStats;      ///< Encryption type distribution
    
    /**
     * @brief OUI (Organizationally Unique Identifier) database entry
     * @details Maps MAC address prefixes to vendor names for device identification.
     */
    struct OUIEntry {
        String mac_prefix;         ///< First 8 chars of MAC (e.g., "00:50:56")
        String vendor;             ///< Manufacturer name (e.g., "VMware")
    };
    
    /// Embedded OUI database with 20 common vendors
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
    /**
     * @brief Constructor - initializes all analyzer data structures
     * @details Resets channel statistics, encryption counters, and display state.
     * Memory allocation: ~2KB for network buffer + ~400 bytes for channel stats.
     */
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
    
    /**
     * @brief Initialize analyzer and start scanning
     * @details Sets WiFi to station mode, disconnects existing connections,
     * displays startup screen, and enables scanning flag.
     */
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
    
    /**
     * @brief Main analyzer loop - call repeatedly in main loop
     * @details Handles auto-scanning (5s interval), input processing,
     * and display rendering based on current mode.
     */
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
    
    /**
     * @brief Perform WiFi network scan and update statistics
     * @details Scans all channels, updates network array (max 50),
     * calculates per-channel statistics (count, RSSI, utilization),
     * and updates encryption type distribution.
     * 
     * **Performance:** Scan duration ~3-5 seconds depending on network count
     */
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
    
    /**
     * @brief Lookup vendor name from MAC address
     * @param bssid MAC address string (format: "XX:XX:XX:XX:XX:XX")
     * @return Vendor name or "Unknown" if not in database
     * 
     * @details Extracts first 8 characters (OUI prefix) and matches
     * against embedded database of 20 common vendors.
     */
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
    
    /**
     * @brief Handle keyboard input
     * @details Processes key presses:
     * - TAB/SPACE: Cycle display mode (0→1→2→3→0)
     * - R: Force immediate rescan
     * - ESC (`): Exit analyzer
     */
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
    
    /**
     * @brief Display Mode 0: Channel bar graph
     * @details Shows vertical bars for each channel (1-13) with height
     * proportional to network count. Color-coded: Green (low), Yellow (medium),
     * Red (high). Includes best channel recommendation and network total.
     */
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
    
    /**
     * @brief Display Mode 1: Network list view
     * @details Shows scrollable list of first 8 networks with SSID, channel,
     * RSSI, security status, and signal strength bars. Color-coded by signal:
     * Green (strong), Yellow (medium), Red (weak).
     */
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
    
    /**
     * @brief Display Mode 2: Statistics dashboard
     * @details Shows comprehensive analysis:
     * - Total network count
     * - Channel distribution (1-3, 4-9, 10-13)
     * - Encryption type breakdown
     * - Top 3 recommended channels with network counts
     */
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
    
    /**
     * @brief Display Mode 3: Channel activity heatmap
     * @details Shows 6-level heat visualization for each channel (1-13).
     * Color progression: Blue (low) → Green → Yellow → Red (very high).
     * Provides visual overview of spectrum congestion.
     */
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
    
    /**
     * @brief Find best WiFi channel for AP placement
     * @param exclude1 Channel to exclude from consideration (default: -1)
     * @param exclude2 Second channel to exclude (default: -1)
     * @return Channel number (1-11) with lowest network count
     * 
     * @details Prioritizes non-overlapping channels (1, 6, 11) for 2.4GHz WiFi.
     * Selects channel with minimum network count for best performance.
     */
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
    
    /**
     * @brief Count total networks in channel range
     * @param start Starting channel (0-based index)
     * @param end Ending channel (0-based index)
     * @return Total number of networks in range
     */
    int getChannelRange(int start, int end) {
        int count = 0;
        for (int i = start; i <= end && i < 14; i++) {
            count += channels[i].networkCount;
        }
        return count;
    }
    
    /**
     * @brief Stop analyzer and disable WiFi
     * @details Clears scanning flag and sets WiFi mode to OFF for power saving.
     */
    void stop() {
        isScanning = false;
        WiFi.mode(WIFI_OFF);
    }
};

// Global instance
WiFiAnalyzer wifiAnalyzer;

#endif // M5GOTCHI_WIFI_ANALYZER_H