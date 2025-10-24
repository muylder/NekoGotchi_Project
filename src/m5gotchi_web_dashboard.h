/*
 * 🌐 M5GOTCHI WEB DASHBOARD v1.0
 * Interface web completa para controle remoto!
 * 
 * Features:
 * - Real-time network monitoring
 * - Remote attack control
 * - Live logs viewer
 * - Statistics dashboard
 * - File manager for captures
 * - Settings panel
 * - Mobile responsive design
 */

#ifndef M5GOTCHI_WEB_DASHBOARD_H
#define M5GOTCHI_WEB_DASHBOARD_H

#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <Update.h>

// ==================== WEB SERVER CONFIG ====================

WebServer webServer(80);
bool webDashboardActive = false;
String webPassword = "m5gotchi123"; // Default password
unsigned long lastUpdate = 0;
String currentActivity = "Idle";
int connectedClients = 0;

// Attack status tracking
struct AttackStatus {
    bool handshakeCapture = false;
    bool deauthAttack = false;
    bool evilPortal = false;
    bool bleSpam = false;
    int packetsCapture = 0;
    int deauthsSent = 0;
    int portalClients = 0;
    String targetSSID = "";
    String targetBSSID = "";
};

AttackStatus attackStatus;

// ==================== HTML TEMPLATES ====================

const char* html_header = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>M5Gotchi Pro Dashboard</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Courier New', monospace; 
            background: linear-gradient(135deg, #0c0c0c 0%, #1a1a2e 50%, #16213e 100%);
            color: #00ff88; 
            min-height: 100vh;
        }
        .container { max-width: 1200px; margin: 0 auto; padding: 20px; }
        .header { 
            text-align: center; 
            margin-bottom: 30px; 
            border-bottom: 2px solid #00ff88;
            padding-bottom: 20px;
        }
        .header h1 { 
            font-size: 2.5em; 
            text-shadow: 0 0 20px #00ff88;
            animation: glow 2s ease-in-out infinite alternate;
        }
        @keyframes glow {
            from { text-shadow: 0 0 20px #00ff88; }
            to { text-shadow: 0 0 30px #00ff88, 0 0 40px #00ff88; }
        }
        .nav { 
            display: flex; 
            justify-content: center; 
            margin-bottom: 30px; 
            flex-wrap: wrap;
        }
        .nav-btn { 
            background: #1a1a2e; 
            color: #00ff88; 
            border: 2px solid #00ff88; 
            padding: 10px 20px; 
            margin: 5px; 
            cursor: pointer; 
            border-radius: 5px; 
            transition: all 0.3s;
            font-family: inherit;
        }
        .nav-btn:hover, .nav-btn.active { 
            background: #00ff88; 
            color: #1a1a2e; 
            box-shadow: 0 0 15px #00ff88;
        }
        .grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); 
            gap: 20px; 
        }
        .card { 
            background: rgba(26, 26, 46, 0.8); 
            border: 2px solid #00ff88; 
            border-radius: 10px; 
            padding: 20px; 
            backdrop-filter: blur(10px);
        }
        .card h3 { 
            color: #00ff88; 
            margin-bottom: 15px; 
            font-size: 1.3em;
        }
        .status-indicator { 
            display: inline-block; 
            width: 12px; 
            height: 12px; 
            border-radius: 50%; 
            margin-right: 8px; 
        }
        .status-active { background: #00ff88; box-shadow: 0 0 10px #00ff88; }
        .status-inactive { background: #ff4444; }
        .btn { 
            background: #1a1a2e; 
            color: #00ff88; 
            border: 2px solid #00ff88; 
            padding: 8px 16px; 
            cursor: pointer; 
            border-radius: 5px; 
            margin: 5px; 
            transition: all 0.3s;
            font-family: inherit;
        }
        .btn:hover { 
            background: #00ff88; 
            color: #1a1a2e; 
        }
        .btn-danger { 
            border-color: #ff4444; 
            color: #ff4444; 
        }
        .btn-danger:hover { 
            background: #ff4444; 
            color: white; 
        }
        .log-container { 
            background: #0c0c0c; 
            border: 1px solid #00ff88; 
            border-radius: 5px; 
            padding: 10px; 
            height: 200px; 
            overflow-y: auto; 
            font-size: 0.9em;
        }
        .log-entry { 
            margin-bottom: 5px; 
            padding: 2px 0;
        }
        .log-info { color: #00ff88; }
        .log-warning { color: #ffaa00; }
        .log-error { color: #ff4444; }
        .input-group { 
            margin-bottom: 15px; 
        }
        .input-group label { 
            display: block; 
            margin-bottom: 5px; 
            color: #00ff88; 
        }
        .input-group input, .input-group select { 
            width: 100%; 
            padding: 8px; 
            background: #1a1a2e; 
            border: 2px solid #00ff88; 
            border-radius: 5px; 
            color: #00ff88; 
            font-family: inherit;
        }
        .stats-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(120px, 1fr)); 
            gap: 10px; 
        }
        .stat-box { 
            text-align: center; 
            padding: 15px; 
            background: #0c0c0c; 
            border-radius: 5px; 
            border: 1px solid #00ff88;
        }
        .stat-number { 
            font-size: 1.8em; 
            font-weight: bold; 
            color: #00ff88; 
        }
        .stat-label { 
            font-size: 0.9em; 
            color: #aaa; 
        }
        .hidden { display: none; }
        @media (max-width: 768px) {
            .container { padding: 10px; }
            .grid { grid-template-columns: 1fr; }
            .nav { flex-direction: column; align-items: center; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🎯 M5Gotchi Pro Dashboard</h1>
            <p>Advanced WiFi Security Testing Platform</p>
            <div style="margin-top: 10px;">
                <span class="status-indicator" id="deviceStatus"></span>
                <span id="deviceStatusText">Connected</span> | 
                <span style="color: #aaa;">Last Update: <span id="lastUpdate">--:--:--</span></span>
            </div>
        </div>
)";

const char* html_footer = R"(
    </div>
    <script>
        let currentView = 'dashboard';
        let refreshInterval;
        
        function showView(view) {
            document.querySelectorAll('.view').forEach(v => v.classList.add('hidden'));
            document.getElementById(view + 'View').classList.remove('hidden');
            
            document.querySelectorAll('.nav-btn').forEach(btn => btn.classList.remove('active'));
            document.querySelector(`[onclick="showView('${view}')"]`).classList.add('active');
            
            currentView = view;
            
            if (view === 'dashboard' || view === 'logs') {
                startAutoRefresh();
            } else {
                stopAutoRefresh();
            }
        }
        
        function startAutoRefresh() {
            stopAutoRefresh();
            refreshData();
            refreshInterval = setInterval(refreshData, 2000);
        }
        
        function stopAutoRefresh() {
            if (refreshInterval) {
                clearInterval(refreshInterval);
                refreshInterval = null;
            }
        }
        
        async function refreshData() {
            try {
                const response = await fetch('/api/status');
                const data = await response.json();
                
                // Update status indicators
                updateStatusIndicators(data);
                
                // Update stats if dashboard view
                if (currentView === 'dashboard') {
                    updateStats(data);
                }
                
                // Update logs if logs view
                if (currentView === 'logs') {
                    updateLogs();
                }
                
                // Update last update time
                document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
                document.getElementById('deviceStatus').className = 'status-indicator status-active';
                document.getElementById('deviceStatusText').textContent = 'Connected';
                
            } catch (error) {
                document.getElementById('deviceStatus').className = 'status-indicator status-inactive';
                document.getElementById('deviceStatusText').textContent = 'Disconnected';
            }
        }
        
        function updateStatusIndicators(data) {
            const indicators = {
                'handshake-status': data.handshakeCapture,
                'deauth-status': data.deauthAttack,
                'portal-status': data.evilPortal,
                'ble-status': data.bleSpam
            };
            
            Object.entries(indicators).forEach(([id, active]) => {
                const element = document.getElementById(id);
                if (element) {
                    element.className = 'status-indicator ' + (active ? 'status-active' : 'status-inactive');
                }
            });
        }
        
        function updateStats(data) {
            const stats = {
                'packets-captured': data.packetsCapture || 0,
                'deauths-sent': data.deauthsSent || 0,
                'portal-clients': data.portalClients || 0,
                'networks-found': data.networksFound || 0
            };
            
            Object.entries(stats).forEach(([id, value]) => {
                const element = document.getElementById(id);
                if (element) {
                    element.textContent = value;
                }
            });
            
            // Update target info
            if (data.targetSSID) {
                document.getElementById('target-ssid').textContent = data.targetSSID;
                document.getElementById('target-bssid').textContent = data.targetBSSID || 'Unknown';
            }
        }
        
        async function updateLogs() {
            try {
                const response = await fetch('/api/logs');
                const logs = await response.json();
                
                const logContainer = document.getElementById('log-entries');
                logContainer.innerHTML = '';
                
                logs.forEach(log => {
                    const entry = document.createElement('div');
                    entry.className = 'log-entry log-' + log.level;
                    entry.textContent = `[${log.time}] ${log.message}`;
                    logContainer.appendChild(entry);
                });
                
                logContainer.scrollTop = logContainer.scrollHeight;
            } catch (error) {
                console.error('Failed to update logs:', error);
            }
        }
        
        async function sendCommand(command, params = {}) {
            try {
                const response = await fetch('/api/command', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ command, ...params })
                });
                
                const result = await response.json();
                if (result.success) {
                    addLogEntry('info', `Command executed: ${command}`);
                } else {
                    addLogEntry('error', `Command failed: ${result.error}`);
                }
            } catch (error) {
                addLogEntry('error', `Network error: ${error.message}`);
            }
        }
        
        function addLogEntry(level, message) {
            const logContainer = document.getElementById('log-entries');
            const entry = document.createElement('div');
            entry.className = 'log-entry log-' + level;
            entry.textContent = `[${new Date().toLocaleTimeString()}] ${message}`;
            logContainer.appendChild(entry);
            logContainer.scrollTop = logContainer.scrollHeight;
        }
        
        // Initialize
        document.addEventListener('DOMContentLoaded', function() {
            showView('dashboard');
        });
        
        // Attack control functions
        function startHandshakeCapture() {
            const ssid = prompt('Enter target SSID (or leave empty for all):');
            sendCommand('start_handshake', { ssid: ssid || '' });
        }
        
        function stopHandshakeCapture() {
            sendCommand('stop_handshake');
        }
        
        function startDeauth() {
            const ssid = prompt('Enter target SSID:');
            if (ssid) {
                sendCommand('start_deauth', { ssid: ssid });
            }
        }
        
        function stopDeauth() {
            sendCommand('stop_deauth');
        }
        
        function startEvilPortal() {
            const ssid = prompt('Enter portal SSID:', 'Free WiFi');
            if (ssid) {
                sendCommand('start_portal', { ssid: ssid });
            }
        }
        
        function stopEvilPortal() {
            sendCommand('stop_portal');
        }
        
        function startBLESpam() {
            sendCommand('start_ble_spam');
        }
        
        function stopBLESpam() {
            sendCommand('stop_ble_spam');
        }
        
        function rebootDevice() {
            if (confirm('Are you sure you want to reboot the device?')) {
                sendCommand('reboot');
            }
        }
        
        function formatSD() {
            if (confirm('Are you sure you want to format the SD card? This will delete all files!')) {
                sendCommand('format_sd');
            }
        }
    </script>
</body>
</html>
)";

// ==================== WEB DASHBOARD CLASS ====================

class WebDashboard {
private:
    String logBuffer = "";
    int logCount = 0;
    
public:
    void start() {
        // Create AP for dashboard access
        WiFi.softAP("M5Gotchi-Dashboard", webPassword.c_str());
        
        setupRoutes();
        webServer.begin();
        webDashboardActive = true;
        
        addLog("info", "Web Dashboard started on " + WiFi.softAPIP().toString());
        
        // Show on screen
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🌐 WEB DASHBOARD", 10, 10);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Access Point: M5Gotchi-Dashboard", 10, 40);
        M5.Display.drawString("Password: " + webPassword, 10, 55);
        M5.Display.drawString("URL: http://" + WiFi.softAPIP().toString(), 10, 70);
        M5.Display.drawString("Connected clients: 0", 10, 85);
        M5.Display.drawString("Press ESC to stop", 10, 110);
    }
    
    void setupRoutes() {
        // Main dashboard page
        webServer.on("/", HTTP_GET, [this]() {
            String html = String(html_header);
            html += generateDashboardHTML();
            html += String(html_footer);
            webServer.send(200, "text/html", html);
        });
        
        // API endpoints
        webServer.on("/api/status", HTTP_GET, [this]() {
            handleStatusAPI();
        });
        
        webServer.on("/api/logs", HTTP_GET, [this]() {
            handleLogsAPI();
        });
        
        webServer.on("/api/command", HTTP_POST, [this]() {
            handleCommandAPI();
        });
        
        webServer.on("/api/files", HTTP_GET, [this]() {
            handleFilesAPI();
        });
        
        webServer.on("/api/download", HTTP_GET, [this]() {
            handleDownloadAPI();
        });
        
        // 404 handler
        webServer.onNotFound([this]() {
            webServer.send(404, "text/plain", "404: Not Found");
        });
    }
    
    String generateDashboardHTML() {
        String html = R"(
        <div class="nav">
            <button class="nav-btn active" onclick="showView('dashboard')">📊 Dashboard</button>
            <button class="nav-btn" onclick="showView('attacks')">⚔️ Attacks</button>
            <button class="nav-btn" onclick="showView('logs')">📋 Logs</button>
            <button class="nav-btn" onclick="showView('files')">📁 Files</button>
            <button class="nav-btn" onclick="showView('settings')">⚙️ Settings</button>
        </div>
        
        <!-- Dashboard View -->
        <div id="dashboardView" class="view">
            <div class="grid">
                <div class="card">
                    <h3>📊 Current Status</h3>
                    <div style="margin-bottom: 10px;">
                        <span class="status-indicator" id="handshake-status"></span>
                        Handshake Capture
                    </div>
                    <div style="margin-bottom: 10px;">
                        <span class="status-indicator" id="deauth-status"></span>
                        Deauth Attack
                    </div>
                    <div style="margin-bottom: 10px;">
                        <span class="status-indicator" id="portal-status"></span>
                        Evil Portal
                    </div>
                    <div style="margin-bottom: 10px;">
                        <span class="status-indicator" id="ble-status"></span>
                        BLE Spam
                    </div>
                </div>
                
                <div class="card">
                    <h3>📈 Statistics</h3>
                    <div class="stats-grid">
                        <div class="stat-box">
                            <div class="stat-number" id="packets-captured">0</div>
                            <div class="stat-label">Packets</div>
                        </div>
                        <div class="stat-box">
                            <div class="stat-number" id="deauths-sent">0</div>
                            <div class="stat-label">Deauths</div>
                        </div>
                        <div class="stat-box">
                            <div class="stat-number" id="portal-clients">0</div>
                            <div class="stat-label">Portal Clients</div>
                        </div>
                        <div class="stat-box">
                            <div class="stat-number" id="networks-found">0</div>
                            <div class="stat-label">Networks</div>
                        </div>
                    </div>
                </div>
                
                <div class="card">
                    <h3>🎯 Current Target</h3>
                    <div><strong>SSID:</strong> <span id="target-ssid">None</span></div>
                    <div><strong>BSSID:</strong> <span id="target-bssid">None</span></div>
                    <div><strong>Activity:</strong> <span id="current-activity">)" + currentActivity + R"(</span></div>
                </div>
            </div>
        </div>
        
        <!-- Attacks View -->
        <div id="attacksView" class="view hidden">
            <div class="grid">
                <div class="card">
                    <h3>📡 WiFi Attacks</h3>
                    <div style="margin-bottom: 15px;">
                        <button class="btn" onclick="startHandshakeCapture()">Start Handshake Capture</button>
                        <button class="btn btn-danger" onclick="stopHandshakeCapture()">Stop</button>
                    </div>
                    <div style="margin-bottom: 15px;">
                        <button class="btn" onclick="startDeauth()">Start Deauth Attack</button>
                        <button class="btn btn-danger" onclick="stopDeauth()">Stop</button>
                    </div>
                    <div style="margin-bottom: 15px;">
                        <button class="btn" onclick="startEvilPortal()">Start Evil Portal</button>
                        <button class="btn btn-danger" onclick="stopEvilPortal()">Stop</button>
                    </div>
                </div>
                
                <div class="card">
                    <h3>📲 Bluetooth Attacks</h3>
                    <div style="margin-bottom: 15px;">
                        <button class="btn" onclick="startBLESpam()">Start BLE Spam</button>
                        <button class="btn btn-danger" onclick="stopBLESpam()">Stop</button>
                    </div>
                    <p style="color: #aaa; font-size: 0.9em;">
                        BLE spam attacks target nearby devices with advertisement packets.
                    </p>
                </div>
                
                <div class="card">
                    <h3>⚠️ Safety Notice</h3>
                    <p style="color: #ffaa00; font-size: 0.9em;">
                        These tools are for authorized security testing only. 
                        Ensure you have proper permission before testing any networks.
                    </p>
                </div>
            </div>
        </div>
        
        <!-- Logs View -->
        <div id="logsView" class="view hidden">
            <div class="card">
                <h3>📋 Live Logs</h3>
                <div class="log-container">
                    <div id="log-entries"></div>
                </div>
                <div style="margin-top: 10px;">
                    <button class="btn" onclick="updateLogs()">Refresh</button>
                    <button class="btn" onclick="document.getElementById('log-entries').innerHTML=''">Clear</button>
                </div>
            </div>
        </div>
        
        <!-- Files View -->
        <div id="filesView" class="view hidden">
            <div class="card">
                <h3>📁 File Manager</h3>
                <div id="file-list">
                    <p>Loading files...</p>
                </div>
                <div style="margin-top: 15px;">
                    <button class="btn" onclick="refreshFiles()">Refresh</button>
                    <button class="btn btn-danger" onclick="formatSD()">Format SD Card</button>
                </div>
            </div>
        </div>
        
        <!-- Settings View -->
        <div id="settingsView" class="view hidden">
            <div class="grid">
                <div class="card">
                    <h3>⚙️ Device Settings</h3>
                    <div class="input-group">
                        <label>Dashboard Password:</label>
                        <input type="password" id="new-password" placeholder="Enter new password">
                        <button class="btn" onclick="changePassword()">Change Password</button>
                    </div>
                    <div class="input-group">
                        <label>Device Name:</label>
                        <input type="text" id="device-name" value="M5Gotchi Pro" placeholder="Device name">
                        <button class="btn" onclick="changeDeviceName()">Update Name</button>
                    </div>
                </div>
                
                <div class="card">
                    <h3>🔧 System Actions</h3>
                    <div style="margin-bottom: 15px;">
                        <button class="btn" onclick="rebootDevice()">Reboot Device</button>
                    </div>
                    <div style="margin-bottom: 15px;">
                        <button class="btn btn-danger" onclick="formatSD()">Format SD Card</button>
                    </div>
                    <p style="color: #aaa; font-size: 0.9em;">
                        Use these actions carefully. Device will disconnect during reboot.
                    </p>
                </div>
                
                <div class="card">
                    <h3>ℹ️ Device Info</h3>
                    <div><strong>Firmware:</strong> M5Gotchi Pro v1.0</div>
                    <div><strong>Chip:</strong> ESP32-S3</div>
                    <div><strong>Flash Size:</strong> 16MB</div>
                    <div><strong>Free Heap:</strong> <span id="free-heap">--</span> KB</div>
                    <div><strong>Uptime:</strong> <span id="uptime">--</span></div>
                </div>
            </div>
        </div>
        )";
        
        return html;
    }
    
    void handleStatusAPI() {
        JsonDocument doc;
        
        doc["handshakeCapture"] = attackStatus.handshakeCapture;
        doc["deauthAttack"] = attackStatus.deauthAttack;
        doc["evilPortal"] = attackStatus.evilPortal;
        doc["bleSpam"] = attackStatus.bleSpam;
        doc["packetsCapture"] = attackStatus.packetsCapture;
        doc["deauthsSent"] = attackStatus.deauthsSent;
        doc["portalClients"] = attackStatus.portalClients;
        doc["targetSSID"] = attackStatus.targetSSID;
        doc["targetBSSID"] = attackStatus.targetBSSID;
        doc["currentActivity"] = currentActivity;
        doc["freeHeap"] = ESP.getFreeHeap() / 1024;
        doc["uptime"] = millis() / 1000;
        doc["networksFound"] = WiFi.scanComplete();
        
        String response;
        serializeJson(doc, response);
        webServer.send(200, "application/json", response);
    }
    
    void handleLogsAPI() {
        JsonDocument doc;
        JsonArray logs = doc.createNestedArray("logs");
        
        // Parse log buffer and create JSON array
        String tempBuffer = logBuffer;
        int startPos = 0;
        int endPos = tempBuffer.indexOf('\n');
        
        while (endPos != -1 && logs.size() < 50) { // Limit to last 50 logs
            String logLine = tempBuffer.substring(startPos, endPos);
            if (logLine.length() > 0) {
                JsonObject logEntry = logs.createNestedObject();
                
                // Parse log format: [level] timestamp message
                int levelEnd = logLine.indexOf(']');
                if (levelEnd > 0) {
                    String level = logLine.substring(1, levelEnd);
                    String rest = logLine.substring(levelEnd + 2);
                    
                    int timeEnd = rest.indexOf(']');
                    if (timeEnd > 0) {
                        String time = rest.substring(1, timeEnd);
                        String message = rest.substring(timeEnd + 2);
                        
                        logEntry["level"] = level;
                        logEntry["time"] = time;
                        logEntry["message"] = message;
                    }
                }
            }
            
            startPos = endPos + 1;
            endPos = tempBuffer.indexOf('\n', startPos);
        }
        
        String response;
        serializeJson(doc, response);
        webServer.send(200, "application/json", response);
    }
    
    void handleCommandAPI() {
        if (!webServer.hasArg("plain")) {
            webServer.send(400, "application/json", "{\"success\":false,\"error\":\"No JSON body\"}");
            return;
        }
        
        JsonDocument doc;
        deserializeJson(doc, webServer.arg("plain"));
        
        String command = doc["command"];
        bool success = processCommand(command, doc);
        
        JsonDocument response;
        response["success"] = success;
        if (!success) {
            response["error"] = "Command failed";
        }
        
        String responseStr;
        serializeJson(response, responseStr);
        webServer.send(200, "application/json", responseStr);
    }
    
    void handleFilesAPI() {
        JsonDocument doc;
        JsonArray files = doc.createNestedArray("files");
        
        // List files on SD card
        if (SD.begin()) {
            File root = SD.open("/");
            File file = root.openNextFile();
            
            while (file && files.size() < 50) {
                if (!file.isDirectory()) {
                    JsonObject fileObj = files.createNestedObject();
                    fileObj["name"] = String(file.name());
                    fileObj["size"] = file.size();
                }
                file = root.openNextFile();
            }
        }
        
        String response;
        serializeJson(doc, response);
        webServer.send(200, "application/json", response);
    }
    
    void handleDownloadAPI() {
        String filename = webServer.arg("file");
        if (filename.length() == 0) {
            webServer.send(400, "text/plain", "No file specified");
            return;
        }
        
        if (SD.begin()) {
            File file = SD.open("/" + filename);
            if (file) {
                webServer.streamFile(file, "application/octet-stream");
                file.close();
                return;
            }
        }
        
        webServer.send(404, "text/plain", "File not found");
    }
    
    bool processCommand(String command, JsonDocument& params) {
        addLog("info", "Received command: " + command);
        
        if (command == "start_handshake") {
            attackStatus.handshakeCapture = true;
            attackStatus.targetSSID = params["ssid"].as<String>();
            currentActivity = "Handshake Capture";
            return true;
        }
        else if (command == "stop_handshake") {
            attackStatus.handshakeCapture = false;
            currentActivity = "Idle";
            return true;
        }
        else if (command == "start_deauth") {
            attackStatus.deauthAttack = true;
            attackStatus.targetSSID = params["ssid"].as<String>();
            currentActivity = "Deauth Attack";
            return true;
        }
        else if (command == "stop_deauth") {
            attackStatus.deauthAttack = false;
            currentActivity = "Idle";
            return true;
        }
        else if (command == "start_portal") {
            attackStatus.evilPortal = true;
            attackStatus.targetSSID = params["ssid"].as<String>();
            currentActivity = "Evil Portal";
            return true;
        }
        else if (command == "stop_portal") {
            attackStatus.evilPortal = false;
            currentActivity = "Idle";
            return true;
        }
        else if (command == "start_ble_spam") {
            attackStatus.bleSpam = true;
            currentActivity = "BLE Spam";
            return true;
        }
        else if (command == "stop_ble_spam") {
            attackStatus.bleSpam = false;
            currentActivity = "Idle";
            return true;
        }
        else if (command == "reboot") {
            addLog("warning", "Device reboot requested");
            delay(1000);
            ESP.restart();
            return true;
        }
        else if (command == "format_sd") {
            addLog("warning", "SD card format requested");
            // Note: SD.format() is not available in standard ESP32 SD library
            // You would need a custom implementation
            return false;
        }
        
        return false;
    }
    
    void addLog(String level, String message) {
        String timestamp = String(millis() / 1000);
        String logLine = "[" + level + "] [" + timestamp + "] " + message + "\n";
        
        logBuffer += logLine;
        logCount++;
        
        // Keep only last 100 log entries
        if (logCount > 100) {
            int firstNewline = logBuffer.indexOf('\n');
            if (firstNewline > 0) {
                logBuffer = logBuffer.substring(firstNewline + 1);
                logCount--;
            }
        }
    }
    
    void update() {
        if (webDashboardActive) {
            webServer.handleClient();
            
            // Update connected clients count
            int clients = WiFi.softAPgetStationNum();
            if (clients != connectedClients) {
                connectedClients = clients;
                
                // Update display
                M5.Display.fillRect(10, 85, 200, 15, TFT_BLACK);
                M5.Display.setTextColor(TFT_WHITE);
                M5.Display.setTextSize(1);
                M5.Display.drawString("Connected clients: " + String(connectedClients), 10, 85);
            }
        }
    }
    
    void stop() {
        webServer.stop();
        WiFi.softAPdisconnect(true);
        webDashboardActive = false;
        addLog("info", "Web Dashboard stopped");
    }
    
    bool isActive() {
        return webDashboardActive;
    }
    
    // Public methods to update attack status
    void updateHandshakeStatus(bool active, int packets = 0) {
        attackStatus.handshakeCapture = active;
        attackStatus.packetsCapture = packets;
    }
    
    void updateDeauthStatus(bool active, int sent = 0) {
        attackStatus.deauthAttack = active;
        attackStatus.deauthsSent = sent;
    }
    
    void updatePortalStatus(bool active, int clients = 0) {
        attackStatus.evilPortal = active;
        attackStatus.portalClients = clients;
    }
    
    void updateBLEStatus(bool active) {
        attackStatus.bleSpam = active;
    }
    
    void setTarget(String ssid, String bssid = "") {
        attackStatus.targetSSID = ssid;
        attackStatus.targetBSSID = bssid;
    }
    
    void setActivity(String activity) {
        currentActivity = activity;
    }
};

// Global instance
WebDashboard webDashboard;

#endif // M5GOTCHI_WEB_DASHBOARD_H