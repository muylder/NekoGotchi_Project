# Plano de Implementação: Funcionalidades Avançadas

## Resumo Executivo
Este documento descreve funcionalidades avançadas do ESP32Marauder que podem ser integradas ao M5Gotchi Pro para torná-lo uma ferramenta de pentest WiFi/BLE mais completa.

---

## 1. Packet Monitor Avançado

### 1.1 Channel Analyzer (Analisador de Canal)
**Função:** Visualizar densidade de pacotes por canal em gráfico em tempo real

**Código Base:** `WiFiScan.cpp` linhas 2899-2966

```cpp
// Funcionalidade
- Gráfico X-Y com densidade por canal
- Scale buttons (1x, 2x, 4x)
- Color-coded por tipo de frame (MGMT/DATA/CTRL)
- Estatísticas em tempo real
```

**Interface Visual:**
```
┌───────────────────────────┐
│ CHANNEL ANALYZER          │
├───────────────────────────┤
│ Ch 1  ║                   │
│ Ch 6  ║████████           │
│ Ch 11 ║████████████       │
│ Ch 13 ║███                │
├───────────────────────────┤
│ MGMT: 65% DATA: 30%       │
│ CTRL: 5%  Scale: 2x       │
└───────────────────────────┘
```

**Implementação:**
```cpp
void channelAnalyzerLoop(uint32_t currentTime) {
    // Channel hop
    changeChannel();
    
    // Count packets per type
    uint32_t mgmt = 0, data = 0, ctrl = 0;
    
    // Draw bar graph
    for (int ch = 1; ch <= 13; ch++) {
        int bar_height = _analyzer_values[ch] * scale;
        drawBar(ch, bar_height, color);
    }
    
    // Update stats
    displayStats(mgmt, data, ctrl);
}
```

### 1.2 Packet Rate Monitor
**Função:** Medir pacotes/segundo por tipo

```
┌───────────────────────────┐
│ PACKET RATE MONITOR       │
├───────────────────────────┤
│ Beacon:   125 pkt/s       │
│ Probe:     45 pkt/s       │
│ Data:     320 pkt/s       │
│ Deauth:     2 pkt/s       │
│ EAPOL:      0 pkt/s       │
├───────────────────────────┤
│ Total:    492 pkt/s       │
│ Channel: 6  RSSI: -65     │
└───────────────────────────┘
```

### 1.3 Signal Strength Monitor
**Função:** Medir RSSI de AP específico em tempo real

**Uso:** Encontrar melhor posição para ataque

```cpp
void signalStrengthMonitor(uint8_t target_bssid[6]) {
    // Captura apenas beacons do target
    if (memcmp(packet_bssid, target_bssid, 6) == 0) {
        int rssi = packet->rx_ctrl.rssi;
        
        // Visual meter
        drawRSSIMeter(rssi);
        
        // Audio feedback (beeps mais rápidos = signal forte)
        if (rssi > -60) {
            beep(100, 2000);  // High pitch
        }
    }
}
```

---

## 2. Detecção de Dispositivos Especiais

### 2.1 Pwnagotchi Detection
**Função:** Detectar e interagir com Pwnagotchis

**Código Base:** `WiFiScan.cpp` linhas 4771-4819

```cpp
// Pwnagotchi envia JSON em beacon payload
struct PwnagotchiBeacon {
    String name;
    int pwnd_tot;        // Total de handshakes
    String version;
    bool deauth;
    int uptime;
};

// Parse beacon
String processPwnagotchiBeacon(const uint8_t* frame, int length) {
    // Procura por MAC 0xDEADBEEFDEAD
    if (memcmp(&frame[10], target_mac, 6) == 0) {
        // Extrai JSON do payload
        DynamicJsonDocument json(2048);
        deserializeJson(json, payload);
        
        String name = json["name"];
        int pwnd = json["pwnd_tot"];
        
        return "Pwnagotchi: " + name + " | Pwnd: " + String(pwnd);
    }
}
```

**Display:**
```
┌───────────────────────────┐
│ PWNAGOTCHI DETECTED       │
├───────────────────────────┤
│ Name: unit7               │
│ Pwnd: 1337                │
│ Version: 1.5.5            │
│ Deauth: true              │
│ Uptime: 4h 23m            │
├───────────────────────────┤
│ [Befriend] [Monitor]      │
└───────────────────────────┘
```

### 2.2 WiFi Pineapple Detection
**Código Base:** `WiFiScan.h` linhas 338-377

**Técnica:** Detecta APs com múltiplos SSIDs no mesmo BSSID

```cpp
struct PineScanTracker {
    uint8_t mac[6];
    bool suspicious_oui;      // OUI suspeito (Hak5)
    bool tag_and_susp_cap;    // Tags e capabilities suspeitos
    uint8_t channel;
    int8_t rssi;
    bool reported;
};

// Suspicious OUIs
const char suspicious_ouis[][7] = {
    "00:13:37",  // Hak5
    "00:C0:CA",  // Alfa
    // ...
};

// Detecta multi-SSID
void detectPineapple() {
    // Se BSSID anunciar 3+ SSIDs diferentes = Pineapple
    if (ssid_count_per_bssid[mac] >= 3) {
        displayAlert("WiFi Pineapple Detected!");
    }
}
```

### 2.3 Espressif Device Scanner
**Função:** Listar todos dispositivos ESP32/ESP8266

```cpp
// OUIs Espressif
const char espressif_ouis[][9] = {
    "24:0A:C4",
    "30:AE:A4",
    "7C:9E:BD",
    "A0:20:A6",
    // ...
};

void scanEspressif() {
    // Filtra apenas OUIs Espressif
    for (auto& ap : access_points) {
        if (isEspressifOUI(ap.bssid)) {
            displayESP(ap);
        }
    }
}
```

---

## 3. Ataques Avançados

### 3.1 Beacon Spam Variants

#### Rick Roll Beacon
```cpp
const char* rick_roll[8] = {
    "01 Never gonna give you up",
    "02 Never gonna let you down",
    "03 Never gonna run around",
    "04 and desert you",
    "05 Never gonna make you cry",
    "06 Never gonna say goodbye",
    "07 Never gonna tell a lie",
    "08 and hurt you"
};

void rickRollBeaconSpam() {
    for (int i = 0; i < 8; i++) {
        sendBeacon(rick_roll[i]);
        delay(100);
    }
}
```

#### Funny Beacon Spam
```cpp
const char* funny_beacons[12] = {
    "Abraham Linksys",
    "Benjamin FrankLAN",
    "Dora the Internet Explorer",
    "FBI Surveillance Van 4",
    "Get Off My LAN",
    "Loading...",
    "Martin Router King",
    "404 Wi-Fi Unavailable",
    "Test Wi-Fi Please Ignore",
    "This LAN is My LAN",
    "Titanic Syncing",
    "Winternet is Coming"
};
```

### 3.2 Targeted Attacks

#### Bad Message Attack
**Função:** Envia EAPOL frames malformados para causar disassociation

```cpp
void sendBadMsgAttack(uint8_t bssid[6], int channel) {
    // EAPOL frame com Key MIC inválido
    uint8_t eapol_frame[99] = {
        0x02, 0x03, 0x00, 0x5f,  // EAPOL header
        0x02,                     // Key descriptor
        // ... key info com MIC flag
        0x00, 0x00, 0x00, 0x00,  // Invalid MIC
    };
    
    sendFrame(bssid, eapol_frame, sizeof(eapol_frame));
}
```

#### Association Sleep Attack
**Função:** Força cliente a dormir indefinidamente

```cpp
void sendAssocSleepAttack(uint8_t bssid[6], uint8_t client[6]) {
    // Association frame com Listen Interval = 0xFFFF
    uint8_t assoc_frame[26] = {
        0x00, 0x00,              // Frame Control
        // ...
        0xFF, 0xFF,              // Listen Interval (máximo)
        // ...
    };
    
    sendFrame(bssid, assoc_frame, sizeof(assoc_frame));
}
```

### 3.3 Mimic Attack
**Função:** Clona AP alvo e atrai clientes

```cpp
void mimicAP(AccessPoint target) {
    // Copia SSID, canal, security
    WiFi.softAP(target.essid.c_str(), "", target.channel);
    
    // Spoofar BSSID
    esp_wifi_set_mac(WIFI_IF_AP, target.bssid);
    
    // Aumentar potência TX
    esp_wifi_set_max_tx_power(78);  // 19.5 dBm
    
    // Deauth original AP
    sendDeauth(target.bssid, broadcast_mac);
}
```

---

## 4. Network Analysis

### 4.1 ARP Scanner
**Função:** Descobrir hosts ativos na rede conectada

```cpp
void arpScan(IPAddress subnet, IPAddress gateway) {
    for (int i = 1; i < 255; i++) {
        IPAddress target = IPAddress(subnet[0], subnet[1], subnet[2], i);
        
        // Send ARP request
        if (singleARP(target)) {
            Serial.println("Host alive: " + target.toString());
            ip_list.add(target);
        }
    }
}
```

### 4.2 Port Scanner
**Função:** Scan portas comuns em hosts

```cpp
const uint16_t common_ports[] = {
    22,    // SSH
    23,    // Telnet
    25,    // SMTP
    53,    // DNS
    80,    // HTTP
    443,   // HTTPS
    3389,  // RDP
};

void portScan(IPAddress target) {
    for (auto port : common_ports) {
        if (checkHostPort(target, port, 100)) {
            Serial.println("Port " + String(port) + " OPEN");
        }
    }
}
```

### 4.3 Ping Scanner
```cpp
void pingScan(IPAddress subnet) {
    for (int i = 1; i < 255; i++) {
        IPAddress target(subnet[0], subnet[1], subnet[2], i);
        
        if (Ping.ping(target, 1)) {
            Serial.println("Host " + target.toString() + " reachable");
        }
    }
}
```

---

## 5. Evil Portal Enhancements

### 5.1 Multiple Templates
**Templates pré-configurados:**
```
/sd/portals/
├── portal_facebook.html
├── portal_google.html
├── portal_netflix.html
├── portal_wifi.html
└── portal_custom.html
```

### 5.2 Captive Portal Endpoints
**Auto-redirect em múltiplos sistemas:**

```cpp
const char* captive_endpoints[] = {
    "/hotspot-detect.html",         // Apple
    "/library/test/success.html",   // Apple iOS
    "/success.txt",                 // Apple iOS 8+
    "/generate_204",                // Android
    "/gen_204",                     // Android
    "/ncsi.txt",                    // Windows
    "/connecttest.txt",             // Windows
    "/redirect",                    // Windows
};

// Auto-serve portal em todos endpoints
for (auto endpoint : captive_endpoints) {
    server.on(endpoint, HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
    });
}
```

### 5.3 Deauth Integration
**Opcional:** Deauth APs próximos para forçar conexão

```cpp
void runEvilPortalWithDeauth() {
    // Iniciar portal
    startEvilPortal();
    
    // Loop deauth
    while (portal_active) {
        for (auto& ap : nearby_aps) {
            sendDeauth(ap.bssid, broadcast_mac);
        }
        delay(5000);
    }
}
```

---

## 6. Data Export & Analysis

### 6.1 PCAP Export
**Já implementado, mas pode melhorar:**

```cpp
// Adicionar metadados
struct pcap_header {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t  thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
};

// Adicionar timestamps precisos
struct pcap_packet_header {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
};
```

### 6.2 JSON Export
**Para análise programática:**

```json
{
  "scan_date": "2024-01-01T12:00:00Z",
  "device": "M5Gotchi Pro",
  "version": "2.0.0",
  "access_points": [
    {
      "ssid": "MyNetwork",
      "bssid": "AA:BB:CC:DD:EE:FF",
      "channel": 6,
      "rssi": -65,
      "security": "WPA2_PSK",
      "wps": false,
      "manufacturer": "TP-Link",
      "first_seen": "2024-01-01T12:00:00Z",
      "packets": 127
    }
  ],
  "stations": [
    {
      "mac": "11:22:33:44:55:66",
      "ap_bssid": "AA:BB:CC:DD:EE:FF",
      "rssi": -70,
      "packets": 45
    }
  ]
}
```

### 6.3 Statistics Report
```cpp
void generateReport() {
    Serial.println("=== SCAN REPORT ===");
    Serial.println("Total APs: " + String(ap_count));
    Serial.println("Open: " + String(open_count));
    Serial.println("WEP: " + String(wep_count));
    Serial.println("WPA: " + String(wpa_count));
    Serial.println("WPA2: " + String(wpa2_count));
    Serial.println("WPA3: " + String(wpa3_count));
    Serial.println("\n=== MANUFACTURERS ===");
    for (auto& oui : oui_count) {
        Serial.println(oui.name + ": " + String(oui.count));
    }
}
```

---

## 7. Priorização de Implementação

### Fase 1 (Alta Prioridade - 1 semana)
1. ✅ Bluetooth Attacks (Sour Apple, AirTag)
2. ✅ GPS Wardriving básico
3. Channel Analyzer
4. Pwnagotchi Detection

### Fase 2 (Média Prioridade - 2 semanas)
1. WiFi Pineapple Detection
2. Rick Roll / Funny Beacons
3. Signal Strength Monitor
4. JSON Export

### Fase 3 (Baixa Prioridade - 1 mês)
1. Mimic Attack
2. Bad Message Attack
3. ARP/Port Scanner
4. Advanced Evil Portal

---

## 8. Recursos Necessários

### Hardware
- GPS Module (NEO-6M) - $10
- Maior bateria (para wardriving) - $15

### Bibliotecas Adicionais
```ini
lib_deps = 
    h2zero/NimBLE-Arduino@^1.4.1       # Bluetooth
    stevemarple/MicroNMEA@^2.0.6       # GPS
    bblanchon/ArduinoJson@^6.21.3      # JSON export
    mathieucarbou/ESPAsyncWebServer    # Evil Portal
```

### Armazenamento
- SD Card 16GB+ (para logs extensos)

---

## 9. Referências

### ESP32Marauder
- Repo: https://github.com/justcallmekoko/ESP32Marauder
- Wiki: https://github.com/justcallmekoko/ESP32Marauder/wiki
- Discord: https://discord.gg/marauder

### Protocolos
- IEEE 802.11: https://standards.ieee.org/standard/802_11-2020.html
- Bluetooth LE: https://www.bluetooth.com/specifications/specs/
- WPA2/WPA3: https://www.wi-fi.org/discover-wi-fi/security

### Ferramentas Similares
- Flipper Zero
- WiFi Pineapple
- Airgeddon
- Wifite2
