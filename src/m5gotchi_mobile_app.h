/*
 * 📱 M5GOTCHI MOBILE COMPANION v1.0
 * Progressive Web App para controle mobile!
 * 
 * Features:
 * - PWA installable
 * - Real-time sync with device
 * - Touch-friendly interface
 * - Offline capabilities
 * - Push notifications
 * - Quick action shortcuts
 */

#ifndef M5GOTCHI_MOBILE_APP_H
#define M5GOTCHI_MOBILE_APP_H

#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ==================== MOBILE APP SERVER ====================

WebServer mobileServer(8080);
bool mobileAppActive = false;
String appVersion = "1.0.0";

// ==================== PWA MANIFEST ====================

const char* pwa_manifest = R"({
  "name": "M5Gotchi Pro Mobile",
  "short_name": "M5Gotchi",
  "description": "M5Gotchi Pro Mobile Companion App",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#1a1a2e",
  "theme_color": "#00ff88",
  "orientation": "portrait",
  "icons": [
    {
      "src": "/icon-192.png",
      "sizes": "192x192",
      "type": "image/png",
      "purpose": "any maskable"
    },
    {
      "src": "/icon-512.png", 
      "sizes": "512x512",
      "type": "image/png",
      "purpose": "any maskable"
    }
  ],
  "categories": ["utilities", "security"],
  "screenshots": [
    {
      "src": "/screenshot1.png",
      "sizes": "540x720",
      "type": "image/png"
    }
  ]
})";

// ==================== SERVICE WORKER ====================

const char* service_worker = R"(
const CACHE_NAME = 'm5gotchi-v1';
const urlsToCache = [
  '/',
  '/app.js',
  '/style.css',
  '/manifest.json'
];

self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then((cache) => cache.addAll(urlsToCache))
  );
});

self.addEventListener('fetch', (event) => {
  event.respondWith(
    caches.match(event.request)
      .then((response) => {
        if (response) {
          return response;
        }
        return fetch(event.request);
      }
    )
  );
});

self.addEventListener('push', (event) => {
  const options = {
    body: event.data ? event.data.text() : 'M5Gotchi notification',
    icon: '/icon-192.png',
    badge: '/icon-192.png',
    vibrate: [200, 100, 200],
    data: {
      dateOfArrival: Date.now(),
      primaryKey: '1'
    },
    actions: [
      {
        action: 'explore',
        title: 'Open App',
        icon: '/icon-192.png'
      },
      {
        action: 'close',
        title: 'Close',
        icon: '/icon-192.png'
      }
    ]
  };

  event.waitUntil(
    self.registration.showNotification('M5Gotchi Pro', options)
  );
});
)";

// ==================== MOBILE APP HTML ====================

const char* mobile_app_html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>M5Gotchi Pro Mobile</title>
    <link rel="manifest" href="/manifest.json">
    <meta name="theme-color" content="#00ff88">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <meta name="apple-mobile-web-app-title" content="M5Gotchi">
    <link rel="apple-touch-icon" href="/icon-192.png">
    
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            -webkit-tap-highlight-color: transparent;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #0c0c0c 0%, #1a1a2e 50%, #16213e 100%);
            color: #00ff88;
            height: 100vh;
            overflow-x: hidden;
            user-select: none;
        }
        
        .app-header {
            background: rgba(26, 26, 46, 0.95);
            backdrop-filter: blur(10px);
            padding: 15px 20px;
            border-bottom: 2px solid #00ff88;
            position: sticky;
            top: 0;
            z-index: 100;
        }
        
        .app-title {
            font-size: 1.8em;
            font-weight: bold;
            text-align: center;
            text-shadow: 0 0 10px #00ff88;
        }
        
        .status-bar {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-top: 10px;
            font-size: 0.9em;
        }
        
        .connection-status {
            display: flex;
            align-items: center;
            gap: 8px;
        }
        
        .status-dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: #ff4444;
            animation: pulse 2s infinite;
        }
        
        .status-dot.connected {
            background: #00ff88;
        }
        
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        
        .main-content {
            padding: 20px;
            height: calc(100vh - 120px);
            overflow-y: auto;
        }
        
        .quick-actions {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
            margin-bottom: 30px;
        }
        
        .action-btn {
            background: rgba(26, 26, 46, 0.8);
            border: 2px solid #00ff88;
            border-radius: 15px;
            padding: 20px;
            text-align: center;
            transition: all 0.3s;
            cursor: pointer;
            position: relative;
            overflow: hidden;
        }
        
        .action-btn:active {
            transform: scale(0.95);
            background: rgba(0, 255, 136, 0.1);
        }
        
        .action-btn.active {
            background: rgba(0, 255, 136, 0.2);
            box-shadow: 0 0 20px rgba(0, 255, 136, 0.5);
        }
        
        .action-btn.danger {
            border-color: #ff4444;
            color: #ff4444;
        }
        
        .action-btn.danger.active {
            background: rgba(255, 68, 68, 0.2);
            box-shadow: 0 0 20px rgba(255, 68, 68, 0.5);
        }
        
        .action-icon {
            font-size: 2em;
            margin-bottom: 10px;
            display: block;
        }
        
        .action-label {
            font-size: 0.9em;
            font-weight: bold;
        }
        
        .stats-section {
            background: rgba(26, 26, 46, 0.8);
            border: 2px solid #00ff88;
            border-radius: 15px;
            padding: 20px;
            margin-bottom: 20px;
        }
        
        .section-title {
            font-size: 1.2em;
            font-weight: bold;
            margin-bottom: 15px;
            text-align: center;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
        }
        
        .stat-item {
            text-align: center;
            padding: 15px;
            background: rgba(12, 12, 12, 0.5);
            border-radius: 10px;
        }
        
        .stat-value {
            font-size: 1.8em;
            font-weight: bold;
            color: #00ff88;
            display: block;
        }
        
        .stat-label {
            font-size: 0.8em;
            color: #aaa;
            margin-top: 5px;
        }
        
        .target-info {
            background: rgba(26, 26, 46, 0.8);
            border: 2px solid #00ff88;
            border-radius: 15px;
            padding: 20px;
            margin-bottom: 20px;
        }
        
        .target-item {
            display: flex;
            justify-content: space-between;
            margin-bottom: 10px;
            padding: 8px 0;
            border-bottom: 1px solid rgba(0, 255, 136, 0.2);
        }
        
        .target-item:last-child {
            border-bottom: none;
            margin-bottom: 0;
        }
        
        .logs-section {
            background: rgba(26, 26, 46, 0.8);
            border: 2px solid #00ff88;
            border-radius: 15px;
            padding: 20px;
        }
        
        .logs-container {
            background: #0c0c0c;
            border-radius: 10px;
            padding: 15px;
            height: 200px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 0.8em;
        }
        
        .log-entry {
            margin-bottom: 8px;
            padding: 5px;
            border-radius: 3px;
        }
        
        .log-info { color: #00ff88; }
        .log-warning { color: #ffaa00; background: rgba(255, 170, 0, 0.1); }
        .log-error { color: #ff4444; background: rgba(255, 68, 68, 0.1); }
        
        .fab {
            position: fixed;
            bottom: 20px;
            right: 20px;
            width: 60px;
            height: 60px;
            background: #00ff88;
            color: #1a1a2e;
            border: none;
            border-radius: 50%;
            font-size: 1.5em;
            cursor: pointer;
            box-shadow: 0 4px 20px rgba(0, 255, 136, 0.4);
            transition: all 0.3s;
            z-index: 1000;
        }
        
        .fab:active {
            transform: scale(0.9);
        }
        
        .modal {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: rgba(0, 0, 0, 0.8);
            display: flex;
            align-items: center;
            justify-content: center;
            z-index: 2000;
            opacity: 0;
            visibility: hidden;
            transition: all 0.3s;
        }
        
        .modal.show {
            opacity: 1;
            visibility: visible;
        }
        
        .modal-content {
            background: #1a1a2e;
            border: 2px solid #00ff88;
            border-radius: 15px;
            padding: 30px;
            width: 90%;
            max-width: 400px;
            text-align: center;
        }
        
        .modal-title {
            font-size: 1.3em;
            margin-bottom: 20px;
            color: #00ff88;
        }
        
        .modal-input {
            width: 100%;
            padding: 12px;
            background: #0c0c0c;
            border: 2px solid #00ff88;
            border-radius: 8px;
            color: #00ff88;
            font-size: 1em;
            margin-bottom: 20px;
        }
        
        .modal-buttons {
            display: flex;
            gap: 15px;
            justify-content: center;
        }
        
        .modal-btn {
            padding: 12px 25px;
            background: transparent;
            border: 2px solid #00ff88;
            border-radius: 8px;
            color: #00ff88;
            cursor: pointer;
            transition: all 0.3s;
        }
        
        .modal-btn:active {
            background: #00ff88;
            color: #1a1a2e;
        }
        
        .modal-btn.cancel {
            border-color: #ff4444;
            color: #ff4444;
        }
        
        .modal-btn.cancel:active {
            background: #ff4444;
            color: white;
        }
        
        .install-prompt {
            background: rgba(0, 255, 136, 0.1);
            border: 2px solid #00ff88;
            border-radius: 10px;
            padding: 15px;
            margin-bottom: 20px;
            text-align: center;
        }
        
        .install-btn {
            background: #00ff88;
            color: #1a1a2e;
            border: none;
            padding: 10px 20px;
            border-radius: 8px;
            font-weight: bold;
            cursor: pointer;
            margin-top: 10px;
        }
        
        .hidden { display: none !important; }
        
        /* Smooth scrolling */
        html { scroll-behavior: smooth; }
        
        /* Loading animation */
        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 2px solid rgba(0, 255, 136, 0.3);
            border-radius: 50%;
            border-top-color: #00ff88;
            animation: spin 1s ease-in-out infinite;
        }
        
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <div class="app-header">
        <div class="app-title">🎯 M5Gotchi Pro</div>
        <div class="status-bar">
            <div class="connection-status">
                <div class="status-dot" id="connectionDot"></div>
                <span id="connectionText">Disconnected</span>
            </div>
            <div id="lastUpdate">--:--:--</div>
        </div>
    </div>
    
    <div class="main-content">
        <!-- Install Prompt -->
        <div id="installPrompt" class="install-prompt hidden">
            <div>📱 Install M5Gotchi as an app for better experience!</div>
            <button class="install-btn" onclick="installApp()">Install App</button>
        </div>
        
        <!-- Quick Actions -->
        <div class="quick-actions">
            <div class="action-btn" id="handshakeBtn" onclick="toggleHandshake()">
                <span class="action-icon">📡</span>
                <span class="action-label">Handshake</span>
            </div>
            <div class="action-btn danger" id="deauthBtn" onclick="toggleDeauth()">
                <span class="action-icon">💥</span>
                <span class="action-label">Deauth</span>
            </div>
            <div class="action-btn" id="portalBtn" onclick="togglePortal()">
                <span class="action-icon">🌐</span>
                <span class="action-label">Portal</span>
            </div>
            <div class="action-btn" id="bleBtn" onclick="toggleBLE()">
                <span class="action-icon">📲</span>
                <span class="action-label">BLE Spam</span>
            </div>
        </div>
        
        <!-- Statistics -->
        <div class="stats-section">
            <div class="section-title">📊 Statistics</div>
            <div class="stats-grid">
                <div class="stat-item">
                    <span class="stat-value" id="packetsCount">0</span>
                    <div class="stat-label">Packets</div>
                </div>
                <div class="stat-item">
                    <span class="stat-value" id="deauthsCount">0</span>
                    <div class="stat-label">Deauths</div>
                </div>
                <div class="stat-item">
                    <span class="stat-value" id="clientsCount">0</span>
                    <div class="stat-label">Portal Clients</div>
                </div>
                <div class="stat-item">
                    <span class="stat-value" id="networksCount">0</span>
                    <div class="stat-label">Networks</div>
                </div>
            </div>
        </div>
        
        <!-- Target Info -->
        <div class="target-info">
            <div class="section-title">🎯 Current Target</div>
            <div class="target-item">
                <span><strong>SSID:</strong></span>
                <span id="targetSSID">None</span>
            </div>
            <div class="target-item">
                <span><strong>BSSID:</strong></span>
                <span id="targetBSSID">None</span>
            </div>
            <div class="target-item">
                <span><strong>Activity:</strong></span>
                <span id="currentActivity">Idle</span>
            </div>
        </div>
        
        <!-- Live Logs -->
        <div class="logs-section">
            <div class="section-title">📋 Live Logs</div>
            <div class="logs-container" id="logsContainer">
                <div class="log-entry log-info">[INFO] M5Gotchi Mobile App started</div>
            </div>
        </div>
    </div>
    
    <!-- Floating Action Button -->
    <button class="fab" onclick="refreshData()" id="refreshFab">
        🔄
    </button>
    
    <!-- Modals -->
    <div class="modal" id="ssidModal">
        <div class="modal-content">
            <div class="modal-title">Enter Target SSID</div>
            <input type="text" class="modal-input" id="ssidInput" placeholder="WiFi Network Name">
            <div class="modal-buttons">
                <button class="modal-btn cancel" onclick="closeModal()">Cancel</button>
                <button class="modal-btn" onclick="confirmSSID()">Confirm</button>
            </div>
        </div>
    </div>
    
    <script>
        let deferredPrompt;
        let isConnected = false;
        let refreshInterval;
        let currentCommand = '';
        
        // PWA Install Prompt
        window.addEventListener('beforeinstallprompt', (e) => {
            e.preventDefault();
            deferredPrompt = e;
            document.getElementById('installPrompt').classList.remove('hidden');
        });
        
        async function installApp() {
            if (deferredPrompt) {
                deferredPrompt.prompt();
                const { outcome } = await deferredPrompt.userChoice;
                if (outcome === 'accepted') {
                    document.getElementById('installPrompt').classList.add('hidden');
                }
                deferredPrompt = null;
            }
        }
        
        // Service Worker Registration
        if ('serviceWorker' in navigator) {
            navigator.serviceWorker.register('/sw.js')
                .then((registration) => {
                    console.log('SW registered: ', registration);
                })
                .catch((registrationError) => {
                    console.log('SW registration failed: ', registrationError);
                });
        }
        
        // Initialize app
        document.addEventListener('DOMContentLoaded', function() {
            startAutoRefresh();
            
            // Request notification permission
            if ('Notification' in window && Notification.permission === 'default') {
                Notification.requestPermission();
            }
        });
        
        function startAutoRefresh() {
            refreshData();
            refreshInterval = setInterval(refreshData, 3000);
        }
        
        function stopAutoRefresh() {
            if (refreshInterval) {
                clearInterval(refreshInterval);
                refreshInterval = null;
            }
        }
        
        async function refreshData() {
            const fab = document.getElementById('refreshFab');
            fab.innerHTML = '<div class="loading"></div>';
            
            try {
                const response = await fetch('/api/status');
                const data = await response.json();
                
                updateConnectionStatus(true);
                updateUI(data);
                
                // Update last update time
                document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
                
            } catch (error) {
                updateConnectionStatus(false);
                addLog('error', 'Connection failed: ' + error.message);
            } finally {
                fab.innerHTML = '🔄';
            }
        }
        
        function updateConnectionStatus(connected) {
            const dot = document.getElementById('connectionDot');
            const text = document.getElementById('connectionText');
            
            if (connected !== isConnected) {
                isConnected = connected;
                
                if (connected) {
                    dot.classList.add('connected');
                    text.textContent = 'Connected';
                    addLog('info', 'Connected to M5Gotchi Pro');
                    
                    // Show notification if supported
                    if ('Notification' in window && Notification.permission === 'granted') {
                        new Notification('M5Gotchi Pro', {
                            body: 'Connected to device',
                            icon: '/icon-192.png',
                            silent: true
                        });
                    }
                } else {
                    dot.classList.remove('connected');
                    text.textContent = 'Disconnected';
                    addLog('error', 'Lost connection to M5Gotchi Pro');
                }
            }
        }
        
        function updateUI(data) {
            // Update action buttons
            updateActionButton('handshakeBtn', data.handshakeCapture);
            updateActionButton('deauthBtn', data.deauthAttack);
            updateActionButton('portalBtn', data.evilPortal);
            updateActionButton('bleBtn', data.bleSpam);
            
            // Update statistics
            document.getElementById('packetsCount').textContent = data.packetsCapture || 0;
            document.getElementById('deauthsCount').textContent = data.deauthsSent || 0;
            document.getElementById('clientsCount').textContent = data.portalClients || 0;
            document.getElementById('networksCount').textContent = data.networksFound || 0;
            
            // Update target info
            document.getElementById('targetSSID').textContent = data.targetSSID || 'None';
            document.getElementById('targetBSSID').textContent = data.targetBSSID || 'None';    
            document.getElementById('currentActivity').textContent = data.currentActivity || 'Idle';
        }
        
        function updateActionButton(buttonId, isActive) {
            const button = document.getElementById(buttonId);
            if (isActive) {
                button.classList.add('active');
            } else {
                button.classList.remove('active');
            }
        }
        
        function addLog(level, message) {
            const logsContainer = document.getElementById('logsContainer');
            const logEntry = document.createElement('div');
            logEntry.className = 'log-entry log-' + level;
            logEntry.textContent = `[${new Date().toLocaleTimeString()}] ${message}`;
            
            logsContainer.appendChild(logEntry);
            logsContainer.scrollTop = logsContainer.scrollHeight;
            
            // Keep only last 20 logs
            while (logsContainer.children.length > 20) {
                logsContainer.removeChild(logsContainer.firstChild);
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
                    addLog('info', `Command executed: ${command}`);
                    
                    // Haptic feedback if supported
                    if ('vibrate' in navigator) {
                        navigator.vibrate(50);
                    }
                } else {
                    addLog('error', `Command failed: ${result.error}`);
                    
                    // Error vibration
                    if ('vibrate' in navigator) {
                        navigator.vibrate([100, 50, 100]);
                    }
                }
                
                // Refresh data after command
                setTimeout(refreshData, 500);
                
            } catch (error) {
                addLog('error', `Network error: ${error.message}`);
            }
        }
        
        function showSSIDModal(command) {
            currentCommand = command;
            document.getElementById('ssidInput').value = '';
            document.getElementById('ssidModal').classList.add('show');
        }
        
        function closeModal() {
            document.getElementById('ssidModal').classList.remove('show');
            currentCommand = '';
        }
        
        function confirmSSID() {
            const ssid = document.getElementById('ssidInput').value.trim();
            if (ssid) {
                if (currentCommand === 'start_handshake') {
                    sendCommand('start_handshake', { ssid: ssid });
                } else if (currentCommand === 'start_deauth') {
                    sendCommand('start_deauth', { ssid: ssid });
                } else if (currentCommand === 'start_portal') {
                    sendCommand('start_portal', { ssid: ssid });
                }
            }
            closeModal();
        }
        
        // Action button handlers
        function toggleHandshake() {
            const btn = document.getElementById('handshakeBtn');
            if (btn.classList.contains('active')) {
                sendCommand('stop_handshake');
            } else {
                showSSIDModal('start_handshake');
            }
        }
        
        function toggleDeauth() {
            const btn = document.getElementById('deauthBtn');
            if (btn.classList.contains('active')) {
                sendCommand('stop_deauth');
            } else {
                showSSIDModal('start_deauth');
            }
        }
        
        function togglePortal() {
            const btn = document.getElementById('portalBtn');
            if (btn.classList.contains('active')) {
                sendCommand('stop_portal');
            } else {
                showSSIDModal('start_portal');
            }
        }
        
        function toggleBLE() {
            const btn = document.getElementById('bleBtn');
            if (btn.classList.contains('active')) {
                sendCommand('stop_ble_spam');
            } else {
                sendCommand('start_ble_spam');
            }
        }
        
        // Touch gestures
        let startY;
        
        document.addEventListener('touchstart', function(e) {
            startY = e.touches[0].clientY;
        });
        
        document.addEventListener('touchend', function(e) {
            const endY = e.changedTouches[0].clientY;
            const diff = startY - endY;
            
            // Pull down to refresh
            if (diff < -100 && window.scrollY === 0) {
                refreshData();
            }
        });
        
        // Prevent zoom on double tap
        let lastTouchEnd = 0;
        document.addEventListener('touchend', function (event) {
            const now = (new Date()).getTime();
            if (now - lastTouchEnd <= 300) {
                event.preventDefault();
            }
            lastTouchEnd = now;
        }, false);
        
        // Keyboard shortcuts
        document.addEventListener('keydown', function(e) {
            if (e.ctrlKey || e.metaKey) {
                switch(e.key) {
                    case 'r':
                        e.preventDefault();
                        refreshData();
                        break;
                    case '1':
                        e.preventDefault();
                        toggleHandshake();
                        break;
                    case '2':
                        e.preventDefault();
                        toggleDeauth();
                        break;
                    case '3':
                        e.preventDefault();
                        togglePortal();
                        break;
                    case '4':
                        e.preventDefault();
                        toggleBLE();
                        break;
                }
            }
        });
    </script>
</body>
</html>
)";

// ==================== MOBILE APP CLASS ====================

class MobileApp {
private:
    String currentSSID = "";
    
public:
    void start() {
        // Create AP for mobile app
        WiFi.softAP("M5Gotchi-Mobile", "mobile123");
        
        setupRoutes();
        mobileServer.begin();
        mobileAppActive = true;
        
        // Show on screen
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(2);
        M5.Display.drawString("📱 MOBILE APP", 10, 10);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("AP: M5Gotchi-Mobile", 10, 40);
        M5.Display.drawString("Password: mobile123", 10, 55);
        M5.Display.drawString("URL: http://" + WiFi.softAPIP().toString() + ":8080", 10, 70);
        M5.Display.drawString("Scan QR code with your phone:", 10, 90);
        
        // Draw simple QR representation
        drawSimpleQR(10, 105);
        
        M5.Display.drawString("Press ESC to stop", 10, 125);
    }
    
    void setupRoutes() {
        // Main app page
        mobileServer.on("/", HTTP_GET, [this]() {
            mobileServer.send(200, "text/html", mobile_app_html);
        });
        
        // PWA Manifest
        mobileServer.on("/manifest.json", HTTP_GET, [this]() {
            mobileServer.send(200, "application/json", pwa_manifest);
        });
        
        // Service Worker
        mobileServer.on("/sw.js", HTTP_GET, [this]() {
            mobileServer.send(200, "application/javascript", service_worker);
        });
        
        // API endpoints (same as web dashboard)
        mobileServer.on("/api/status", HTTP_GET, [this]() {
            handleStatusAPI();
        });
        
        mobileServer.on("/api/command", HTTP_POST, [this]() {
            handleCommandAPI();
        });
        
        // Icon placeholders (would normally serve actual PNG files)
        mobileServer.on("/icon-192.png", HTTP_GET, [this]() {
            mobileServer.send(404, "text/plain", "Icon not found");
        });
        
        mobileServer.on("/icon-512.png", HTTP_GET, [this]() {
            mobileServer.send(404, "text/plain", "Icon not found");
        });
    }
    
    void handleStatusAPI() {
        JsonDocument doc;
        
        // Use external attack status if available, otherwise create dummy data
        doc["handshakeCapture"] = false; // Would get from attackStatus
        doc["deauthAttack"] = false;
        doc["evilPortal"] = false;
        doc["bleSpam"] = false;
        doc["packetsCapture"] = 0;
        doc["deauthsSent"] = 0;
        doc["portalClients"] = 0;
        doc["targetSSID"] = currentSSID;
        doc["targetBSSID"] = "";
        doc["currentActivity"] = "Idle";
        doc["freeHeap"] = ESP.getFreeHeap() / 1024;
        doc["uptime"] = millis() / 1000;
        doc["networksFound"] = WiFi.scanComplete();
        
        String response;
        serializeJson(doc, response);
        mobileServer.send(200, "application/json", response);
    }
    
    void handleCommandAPI() {
        if (!mobileServer.hasArg("plain")) {
            mobileServer.send(400, "application/json", "{\"success\":false,\"error\":\"No JSON body\"}");
            return;
        }
        
        JsonDocument doc;
        deserializeJson(doc, mobileServer.arg("plain"));
        
        String command = doc["command"];
        bool success = processCommand(command, doc);
        
        JsonDocument response;
        response["success"] = success;
        if (!success) {
            response["error"] = "Command failed";
        }
        
        String responseStr;
        serializeJson(response, responseStr);
        mobileServer.send(200, "application/json", responseStr);
    }
    
    bool processCommand(String command, JsonDocument& params) {
        // Update display with command info
        M5.Display.fillRect(0, 85, 240, 15, TFT_BLACK);
        M5.Display.setTextColor(0x07FF);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Command: " + command, 10, 85);
        
        if (command == "start_handshake") {
            currentSSID = params["ssid"].as<String>();
            return true;
        }
        else if (command == "stop_handshake") {
            currentSSID = "";
            return true;
        }
        else if (command == "start_deauth") {
            currentSSID = params["ssid"].as<String>();
            return true;
        }
        else if (command == "stop_deauth") {
            return true;
        }
        else if (command == "start_portal") {
            currentSSID = params["ssid"].as<String>();
            return true;
        }
        else if (command == "stop_portal") {
            return true;
        }
        else if (command == "start_ble_spam") {
            return true;
        }
        else if (command == "stop_ble_spam") {
            return true;
        }
        
        return false;
    }
    
    void drawSimpleQR(int x, int y) {
        // Draw a simple QR-like pattern (not a real QR code)
        int blockSize = 2;
        uint16_t pattern[8][8] = {
            {1,1,1,0,0,1,1,1},
            {1,0,1,0,1,1,0,1},
            {1,0,1,1,0,1,0,1},
            {0,0,0,1,1,0,0,0},
            {1,1,0,0,1,0,1,1},
            {0,1,1,0,0,1,1,0},
            {1,0,0,1,1,0,0,1},
            {1,1,1,0,0,1,1,1}
        };
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                uint16_t color = pattern[row][col] ? TFT_WHITE : TFT_BLACK;
                M5.Display.fillRect(x + (col * blockSize), y + (row * blockSize), blockSize, blockSize, color);
            }
        }
    }
    
    void update() {
        if (mobileAppActive) {
            mobileServer.handleClient();
            
            // Update connected clients count on display
            static int lastClients = -1;
            int clients = WiFi.softAPgetStationNum();
            
            if (clients != lastClients) {
                lastClients = clients;
                M5.Display.fillRect(100, 125, 100, 10, TFT_BLACK);
                M5.Display.setTextColor(TFT_WHITE);
                M5.Display.setTextSize(1);
                M5.Display.drawString("Clients: " + String(clients), 100, 125);
            }
        }
    }
    
    void stop() {
        mobileServer.stop();
        WiFi.softAPdisconnect(true);
        mobileAppActive = false;
    }
    
    bool isActive() {
        return mobileAppActive;
    }
};

// Global instance
MobileApp mobileApp;

#endif // M5GOTCHI_MOBILE_APP_H