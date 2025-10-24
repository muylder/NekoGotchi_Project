/*
 * 📊 M5GOTCHI CHANNEL ANALYZER v1.0
 * Analisador de canal em tempo real com gráficos!
 * 
 * Features:
 * - Real-time packet density visualization
 * - Color-coded by frame type (MGMT/DATA/CTRL)
 * - Scalable view (1x, 2x, 4x, 8x)
 * - Channel statistics and recommendations
 * - Signal strength monitoring
 * - Best channel auto-detection
 */

#ifndef M5GOTCHI_CHANNEL_ANALYZER_H
#define M5GOTCHI_CHANNEL_ANALYZER_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ==================== ANALYZER DATA STRUCTURES ====================

struct ChannelData {
    int channel;
    uint32_t mgmt_count;      // Management frames
    uint32_t data_count;      // Data frames  
    uint32_t ctrl_count;      // Control frames
    uint32_t total_count;     // Total packets
    int avg_rssi;             // Average RSSI
    int max_rssi;             // Peak RSSI
    uint32_t last_update;     // Last packet timestamp
    float utilization;        // Channel utilization %
};

struct PacketStats {
    uint32_t beacon_count;
    uint32_t probe_req_count;
    uint32_t probe_resp_count;
    uint32_t assoc_count;
    uint32_t deauth_count;
    uint32_t eapol_count;
    uint32_t data_count;
    uint32_t qos_data_count;
};

class ChannelAnalyzer {
private:
    ChannelData channels[14];     // Channels 1-13 + channel 14
    PacketStats packet_stats;
    int current_channel;
    int display_mode;            // 0=bars, 1=stats, 2=packets, 3=signal
    int scale_factor;            // 1x, 2x, 4x, 8x
    bool is_scanning;
    uint32_t scan_start_time;
    uint32_t last_channel_hop;
    uint32_t channel_hop_interval;  // ms
    uint32_t total_packets;
    
    // Colors for different frame types
    uint16_t color_mgmt = 0x07E0;   // Green
    uint16_t color_data = 0x001F;   // Blue  
    uint16_t color_ctrl = 0xF800;   // Red
    uint16_t color_mixed = 0xFFE0;  // Yellow
    
public:
    ChannelAnalyzer() {
        current_channel = 1;
        display_mode = 0;
        scale_factor = 1;
        is_scanning = false;
        scan_start_time = 0;
        last_channel_hop = 0;
        channel_hop_interval = 250;  // 250ms per channel
        total_packets = 0;
        
        // Initialize channel data
        for (int i = 0; i < 14; i++) {
            channels[i] = {i + 1, 0, 0, 0, 0, 0, -100, 0, 0.0};
        }
        
        // Initialize packet stats
        memset(&packet_stats, 0, sizeof(PacketStats));
    }
    
    void start() {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        
        // Setup promiscuous mode
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);
        esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
        
        is_scanning = true;
        scan_start_time = millis();
        last_channel_hop = millis();
        
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(2);
        M5.Display.drawString("📊 CHANNEL ANALYZER", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Initializing...", 10, 30);
        
        delay(1000);
    }
    
    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        // Get the global instance (you'd need to make this work properly)
        extern ChannelAnalyzer channelAnalyzer;
        channelAnalyzer.processPacket(buf, type);
    }
    
    void processPacket(void* buf, wifi_promiscuous_pkt_type_t type) {
        wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
        wifi_pkt_rx_ctrl_t ctrl = pkt->rx_ctrl;
        
        int ch = ctrl.channel;
        if (ch < 1 || ch > 13) return;
        
        int ch_idx = ch - 1;
        uint8_t* frame = pkt->payload;
        uint16_t frame_ctrl = *(uint16_t*)frame;
        uint8_t frame_type = (frame_ctrl & 0x0C) >> 2;
        uint8_t frame_subtype = (frame_ctrl & 0xF0) >> 4;
        
        // Update channel data
        channels[ch_idx].total_count++;
        channels[ch_idx].last_update = millis();
        
        // Update RSSI
        int rssi = ctrl.rssi;
        if (channels[ch_idx].avg_rssi == 0) {
            channels[ch_idx].avg_rssi = rssi;
        } else {
            channels[ch_idx].avg_rssi = (channels[ch_idx].avg_rssi + rssi) / 2;
        }
        
        if (rssi > channels[ch_idx].max_rssi) {
            channels[ch_idx].max_rssi = rssi;
        }
        
        // Classify frame type
        switch (frame_type) {
            case 0: // Management
                channels[ch_idx].mgmt_count++;
                
                // Detailed management frame stats
                switch (frame_subtype) {
                    case 8: packet_stats.beacon_count++; break;
                    case 4: packet_stats.probe_req_count++; break;
                    case 5: packet_stats.probe_resp_count++; break;
                    case 0: case 1: packet_stats.assoc_count++; break;
                    case 12: packet_stats.deauth_count++; break;
                }
                break;
                
            case 1: // Control
                channels[ch_idx].ctrl_count++;
                break;
                
            case 2: // Data
                channels[ch_idx].data_count++;
                
                // Check for EAPOL (4-way handshake)
                if (frame_subtype == 8) { // QoS Data
                    packet_stats.qos_data_count++;
                    // Simple EAPOL detection (would need more sophisticated parsing)
                    if (pkt->rx_ctrl.sig_len > 100) {
                        packet_stats.eapol_count++;
                    }
                } else {
                    packet_stats.data_count++;
                }
                break;
        }
        
        total_packets++;
    }
    
    void update() {
        if (!is_scanning) return;
        
        // Handle channel hopping
        if (millis() - last_channel_hop > channel_hop_interval) {
            current_channel++;
            if (current_channel > 13) current_channel = 1;
            
            esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
            last_channel_hop = millis();
        }
        
        // Handle input
        handleInput();
        
        // Update display based on mode
        switch (display_mode) {
            case 0: drawChannelBars(); break;
            case 1: drawChannelStats(); break;
            case 2: drawPacketStats(); break;
            case 3: drawSignalMonitor(); break;
        }
        
        delay(50);
    }
    
    void handleInput() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == "\t" || key == " ") { // TAB or SPACE - change mode
                display_mode = (display_mode + 1) % 4;
            } else if (key == "+" || key == "=") { // Scale up
                if (scale_factor < 8) scale_factor *= 2;
            } else if (key == "-") { // Scale down
                if (scale_factor > 1) scale_factor /= 2;
            } else if (key == "r") { // Reset stats
                resetStats();
            } else if (key == "h") { // Change hop speed
                if (channel_hop_interval == 250) channel_hop_interval = 500;
                else if (channel_hop_interval == 500) channel_hop_interval = 1000;
                else channel_hop_interval = 250;
            } else if (key == "`") { // ESC - exit
                stop();
                return;
            }
        }
    }
    
    void drawChannelBars() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📊 CHANNEL ANALYZER - BARS", 5, 5);
        
        // Mode info
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 1/4 | Scale: " + String(scale_factor) + "x | Hop: " + String(channel_hop_interval) + "ms", 5, 15);
        M5.Display.drawString("TAB: Mode  +/-: Scale  H: Hop  R: Reset  ESC: Exit", 5, 125);
        
        // Current channel indicator
        M5.Display.setTextColor(0xFFE0);
        M5.Display.drawString("Scanning Ch: " + String(current_channel), 5, 25);
        
        // Find max count for scaling
        uint32_t max_count = 1;
        for (int i = 0; i < 13; i++) {
            if (channels[i].total_count > max_count) {
                max_count = channels[i].total_count;
            }
        }
        
        // Draw channel bars
        int bar_width = 15;
        int bar_spacing = 18;
        int max_height = 70;
        int base_y = 100;
        
        for (int i = 0; i < 13; i++) {
            int x = 5 + (i * bar_spacing);
            int ch = i + 1;
            
            // Calculate bar heights for each frame type
            uint32_t total = channels[i].total_count;
            if (total == 0) continue;
            
            int total_height = map(total * scale_factor, 0, max_count * scale_factor, 0, max_height);
            
            // Calculate proportional heights
            int mgmt_height = (channels[i].mgmt_count * total_height) / total;
            int data_height = (channels[i].data_count * total_height) / total;  
            int ctrl_height = (channels[i].ctrl_count * total_height) / total;
            
            // Draw stacked bars
            int y_pos = base_y;
            
            // Management (green) - bottom
            if (mgmt_height > 0) {
                M5.Display.fillRect(x, y_pos - mgmt_height, bar_width, mgmt_height, color_mgmt);
                y_pos -= mgmt_height;
            }
            
            // Data (blue) - middle
            if (data_height > 0) {
                M5.Display.fillRect(x, y_pos - data_height, bar_width, data_height, color_data);
                y_pos -= data_height;
            }
            
            // Control (red) - top
            if (ctrl_height > 0) {
                M5.Display.fillRect(x, y_pos - ctrl_height, bar_width, ctrl_height, color_ctrl);
            }
            
            // Channel number
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.drawString(String(ch), x + 3, base_y + 5);
            
            // Packet count (if fits)
            if (total < 1000) {
                M5.Display.setTextColor(0x4208);
                M5.Display.drawString(String(total), x - 2, 35);
            }
            
            // Current channel highlight
            if (ch == current_channel) {
                M5.Display.drawRect(x - 1, base_y - max_height - 1, bar_width + 2, max_height + 15, 0xFFE0);
            }
        }
        
        // Legend
        M5.Display.fillRect(180, 40, 8, 6, color_mgmt);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("MGMT", 195, 40);
        
        M5.Display.fillRect(180, 50, 8, 6, color_data);
        M5.Display.drawString("DATA", 195, 50);
        
        M5.Display.fillRect(180, 60, 8, 6, color_ctrl);
        M5.Display.drawString("CTRL", 195, 60);
        
        // Stats
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Total: " + String(total_packets), 180, 75);
        M5.Display.drawString("Time: " + String((millis() - scan_start_time) / 1000) + "s", 180, 85);
        
        // Best channel recommendation
        int best_ch = getBestChannel();
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("Best: Ch " + String(best_ch), 180, 95);
    }
    
    void drawChannelStats() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📊 CHANNEL ANALYZER - STATS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 2/4 - TAB: Next Mode  ESC: Exit", 5, 125);
        
        // Channel stats table
        M5.Display.drawString("CH  TOTAL  MGMT  DATA  CTRL  RSSI", 5, 25);
        M5.Display.drawLine(5, 35, 235, 35, 0x4208);
        
        for (int i = 0; i < 13; i++) {
            int y = 40 + (i * 8);
            if (y > 115) break;
            
            int ch = i + 1;
            uint16_t color = (ch == current_channel) ? 0xFFE0 : TFT_WHITE;
            
            M5.Display.setTextColor(color);
            
            // Channel
            M5.Display.drawString(String(ch).length() == 1 ? " " + String(ch) : String(ch), 5, y);
            
            // Total packets
            String total_str = String(channels[i].total_count);
            if (total_str.length() < 5) {
                total_str = String("     ").substring(0, 5 - total_str.length()) + total_str;
            }
            M5.Display.drawString(total_str, 25, y);
            
            // MGMT packets
            String mgmt_str = String(channels[i].mgmt_count);
            if (mgmt_str.length() < 4) {
                mgmt_str = String("    ").substring(0, 4 - mgmt_str.length()) + mgmt_str;
            }
            M5.Display.drawString(mgmt_str, 70, y);
            
            // DATA packets  
            String data_str = String(channels[i].data_count);
            if (data_str.length() < 4) {
                data_str = String("    ").substring(0, 4 - data_str.length()) + data_str;
            }
            M5.Display.drawString(data_str, 110, y);
            
            // CTRL packets
            String ctrl_str = String(channels[i].ctrl_count);
            if (ctrl_str.length() < 4) {
                ctrl_str = String("    ").substring(0, 4 - ctrl_str.length()) + ctrl_str;
            }
            M5.Display.drawString(ctrl_str, 150, y);
            
            // Average RSSI
            if (channels[i].avg_rssi < 0) {
                M5.Display.drawString(String(channels[i].avg_rssi), 190, y);
            } else {
                M5.Display.drawString("--", 190, y);
            }
        }
        
        // Summary at bottom
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Scanning: Ch " + String(current_channel) + " | " + 
                             String(total_packets) + " total packets", 5, 15);
    }
    
    void drawPacketStats() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📊 PACKET STATISTICS", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 3/4 - TAB: Next Mode  ESC: Exit", 5, 125);
        
        // Packet type breakdown
        int y = 30;
        
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("Management Frames:", 10, y);
        y += 12;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("  Beacons:      " + String(packet_stats.beacon_count), 10, y); y += 10;
        M5.Display.drawString("  Probe Req:    " + String(packet_stats.probe_req_count), 10, y); y += 10;
        M5.Display.drawString("  Probe Resp:   " + String(packet_stats.probe_resp_count), 10, y); y += 10;
        M5.Display.drawString("  Assoc:        " + String(packet_stats.assoc_count), 10, y); y += 10;
        M5.Display.drawString("  Deauth:       " + String(packet_stats.deauth_count), 10, y); y += 12;
        
        M5.Display.setTextColor(0x001F);
        M5.Display.drawString("Data Frames:", 10, y); y += 12;
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("  Data:         " + String(packet_stats.data_count), 10, y); y += 10;
        M5.Display.drawString("  QoS Data:     " + String(packet_stats.qos_data_count), 10, y); y += 10;
        M5.Display.drawString("  EAPOL:        " + String(packet_stats.eapol_count), 10, y); y += 12;
        
        // Rate calculation
        uint32_t elapsed = (millis() - scan_start_time) / 1000;
        if (elapsed > 0) {
            float rate = (float)total_packets / elapsed;
            M5.Display.setTextColor(0x07FF);
            M5.Display.drawString("Rate: " + String(rate, 1) + " pkt/s", 10, y);
        }
    }
    
    void drawSignalMonitor() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Header
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("📊 SIGNAL STRENGTH", 5, 5);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Mode 4/4 - TAB: Next Mode  ESC: Exit", 5, 125);
        
        // Current channel signal
        M5.Display.setTextColor(0xFFE0);
        M5.Display.drawString("Current Channel: " + String(current_channel), 10, 25);
        
        int current_idx = current_channel - 1;
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("Avg RSSI: " + String(channels[current_idx].avg_rssi) + " dBm", 10, 35);
        M5.Display.drawString("Max RSSI: " + String(channels[current_idx].max_rssi) + " dBm", 10, 45);
        M5.Display.drawString("Packets:  " + String(channels[current_idx].total_count), 10, 55);
        
        // Signal strength bars for all channels
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("All Channels Signal Strength:", 10, 70);
        
        for (int i = 0; i < 13; i++) {
            int x = 10 + (i * 17);
            int y = 85;
            int ch = i + 1;
            
            // Convert RSSI to bar height (assume -100 to -30 dBm range)
            int rssi = channels[i].avg_rssi;
            if (rssi == 0) rssi = -100;  // No signal
            
            int bar_height = map(rssi, -100, -30, 1, 25);
            if (bar_height < 1) bar_height = 1;
            
            // Color coding for signal strength
            uint16_t signal_color;
            if (rssi > -50) signal_color = 0x07E0;      // Green - Strong
            else if (rssi > -70) signal_color = 0xFFE0; // Yellow - Medium  
            else signal_color = 0xF800;                 // Red - Weak
            
            // Draw signal bar
            M5.Display.fillRect(x, y + 25 - bar_height, 14, bar_height, signal_color);
            M5.Display.drawRect(x, y, 14, 25, 0x4208);
            
            // Channel number
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.drawString(String(ch), x + 3, y + 27);
            
            // Highlight current channel
            if (ch == current_channel) {
                M5.Display.drawRect(x - 1, y - 1, 16, 35, 0xFFE0);
            }
        }
    }
    
    int getBestChannel() {
        // Find channel with least total packets (best for new AP)
        int best_ch = 1;
        uint32_t min_packets = 0xFFFFFFFF;
        
        // Prefer channels 1, 6, 11 (non-overlapping)
        int preferred[] = {1, 6, 11};
        for (int i = 0; i < 3; i++) {
            int ch = preferred[i];
            int idx = ch - 1;
            if (channels[idx].total_count < min_packets) {
                min_packets = channels[idx].total_count;
                best_ch = ch;
            }
        }
        
        return best_ch;
    }
    
    void resetStats() {
        for (int i = 0; i < 14; i++) {
            channels[i].mgmt_count = 0;
            channels[i].data_count = 0;
            channels[i].ctrl_count = 0;
            channels[i].total_count = 0;
            channels[i].avg_rssi = 0;
            channels[i].max_rssi = -100;
        }
        
        memset(&packet_stats, 0, sizeof(PacketStats));
        total_packets = 0;
        scan_start_time = millis();
    }
    
    void stop() {
        esp_wifi_set_promiscuous(false);
        is_scanning = false;
        WiFi.mode(WIFI_OFF);
    }
    
    bool isActive() {
        return is_scanning;
    }
};

// Global instance
ChannelAnalyzer channelAnalyzer;

#endif // M5GOTCHI_CHANNEL_ANALYZER_H