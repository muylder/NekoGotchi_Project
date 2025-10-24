/*
 * M5Gotchi Optimizado - WiFi Handshake Capture
 * Baseado no ESP32 WiFi Hash Monster
 * 
 * OTIMIZAÇÕES IMPLEMENTADAS:
 * 1. Tempo por canal aumentado para 3 segundos
 * 2. Foco em canais prioritários (1, 6, 11) com rotação inteligente
 * 3. Captura de PMKID + EAPOL completo
 * 
 * USO APENAS EM REDES PRÓPRIAS OU COM AUTORIZAÇÃO!
 */

#include <M5Stack.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SD.h>
#include "esp_wifi_types.h"

// ==================== CONFIGURAÇÕES ====================
#define SNAP_LEN 2324  // Buffer aumentado para captura completa
#define CHANNEL_HOP_INTERVAL_MS 3000  // 3 segundos por canal
#define MAX_PCAP_SIZE 10000000  // 10MB por arquivo
#define ENABLE_SD_CARD true  // Habilitar gravação em SD

// Canais prioritários (onde está 80% do tráfego WiFi)
const uint8_t PRIORITY_CHANNELS[] = {1, 6, 11};
const uint8_t PRIORITY_CHANNEL_COUNT = 3;

// Todos os canais (para varredura completa se desejar)
const uint8_t ALL_CHANNELS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const uint8_t ALL_CHANNEL_COUNT = 13;

// Modo de operação
bool usePriorityChannelsOnly = true;  // true = canais 1,6,11 | false = todos os canais

// ==================== VARIÁVEIS GLOBAIS ====================
uint8_t currentChannel = 1;
uint8_t channelIndex = 0;
unsigned long lastChannelChange = 0;
unsigned long packetCount = 0;
unsigned long eapolCount = 0;
unsigned long pmkidCount = 0;
unsigned long beaconCount = 0;
unsigned long probeCount = 0;

File pcapFile;
bool sdCardEnabled = false;
int fileCounter = 1;
unsigned long currentFileSize = 0;

// ==================== ESTRUTURAS DE DADOS ====================
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

// ==================== PCAP HEADER ====================
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

// ==================== FUNÇÕES AUXILIARES ====================

void drawHeader() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(60, 10);
    M5.Lcd.println("M5GOTCHI OPTIMIZED");
    M5.Lcd.drawLine(0, 35, 320, 35, GREEN);
}

void updateDisplay() {
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BLACK);
    
    // Canal atual
    M5.Lcd.setCursor(10, 50);
    M5.Lcd.printf("Canal: %02d       ", currentChannel);
    
    // Modo
    M5.Lcd.setCursor(10, 75);
    if (usePriorityChannelsOnly) {
        M5.Lcd.printf("Modo: PRIORITARIO");
    } else {
        M5.Lcd.printf("Modo: TODOS      ");
    }
    
    // Estatísticas
    M5.Lcd.setTextColor(CYAN, BLACK);
    M5.Lcd.setCursor(10, 105);
    M5.Lcd.printf("Pacotes: %lu    ", packetCount);
    
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(10, 130);
    M5.Lcd.printf("EAPOL: %lu      ", eapolCount);
    
    M5.Lcd.setTextColor(MAGENTA, BLACK);
    M5.Lcd.setCursor(10, 155);
    M5.Lcd.printf("PMKID: %lu      ", pmkidCount);
    
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(10, 180);
    M5.Lcd.printf("Beacons: %lu    ", beaconCount);
    
    M5.Lcd.setCursor(10, 205);
    M5.Lcd.printf("Probes: %lu     ", probeCount);
    
    // Status SD Card
    M5.Lcd.drawLine(0, 215, 320, 215, GREEN);
    M5.Lcd.setCursor(10, 225);
    if (sdCardEnabled) {
        M5.Lcd.setTextColor(GREEN, BLACK);
        M5.Lcd.printf("SD: ATIVO [%d.pcap]", fileCounter);
    } else {
        M5.Lcd.setTextColor(RED, BLACK);
        M5.Lcd.println("SD: DESABILITADO   ");
    }
}

void initPcapFile() {
    if (!sdCardEnabled) return;
    
    char filename[32];
    sprintf(filename, "/%d.pcap", fileCounter);
    
    pcapFile = SD.open(filename, FILE_WRITE);
    if (!pcapFile) {
        Serial.println("Erro ao criar arquivo PCAP!");
        return;
    }
    
    // Escrever header PCAP
    pcap_hdr_t pcap_header;
    pcap_header.magic_number = 0xa1b2c3d4;
    pcap_header.version_major = 2;
    pcap_header.version_minor = 4;
    pcap_header.thiszone = 0;
    pcap_header.sigfigs = 0;
    pcap_header.snaplen = SNAP_LEN;
    pcap_header.network = 105; // IEEE 802.11
    
    pcapFile.write((uint8_t*)&pcap_header, sizeof(pcap_hdr_t));
    pcapFile.flush();
    
    currentFileSize = sizeof(pcap_hdr_t);
    Serial.printf("Arquivo PCAP criado: %s\n", filename);
}

void writePcapPacket(const uint8_t* buf, uint32_t len) {
    if (!sdCardEnabled || !pcapFile) return;
    
    // Verificar tamanho do arquivo
    if (currentFileSize >= MAX_PCAP_SIZE) {
        pcapFile.close();
        fileCounter++;
        currentFileSize = 0;
        initPcapFile();
    }
    
    // Escrever header do pacote
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
    // EAPOL tem EtherType 0x888E
    // Verificar se é um pacote de dados (type 2)
    if (len < 36) return false;
    
    uint8_t frameType = buf[0];
    uint8_t frameSubType = (frameType & 0xF0) >> 4;
    
    // Type 2 = Data
    if ((frameType & 0x0C) != 0x08) return false;
    
    // Procurar por EAPOL signature (0x888E)
    for (int i = 0; i < len - 1; i++) {
        if (buf[i] == 0x88 && buf[i+1] == 0x8E) {
            return true;
        }
    }
    
    return false;
}

bool hasPMKID(const uint8_t* buf, uint32_t len) {
    // PMKID está presente na primeira mensagem do handshake (EAPOL Key)
    // Procurar pela tag PMKID (0xDD com OUI específico)
    if (!isEAPOL(buf, len)) return false;
    
    // Procurar por PMKID tag
    for (int i = 0; i < len - 20; i++) {
        if (buf[i] == 0xDD && buf[i+1] >= 0x14) {
            // Verificar OUI (00-0F-AC) e tipo (04 para PMKID)
            if (buf[i+2] == 0x00 && buf[i+3] == 0x0F && 
                buf[i+4] == 0xAC && buf[i+5] == 0x04) {
                return true;
            }
        }
    }
    
    return false;
}

// ==================== CALLBACK DE CAPTURA ====================
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
    
    uint32_t len = ppkt->rx_ctrl.sig_len;
    
    packetCount++;
    
    // Classificar tipo de frame
    uint8_t frameType = hdr->frame_ctrl & 0x0C;
    uint8_t frameSubType = (hdr->frame_ctrl & 0xF0) >> 4;
    
    // Beacon frame
    if (frameType == 0x00 && frameSubType == 0x08) {
        beaconCount++;
    }
    
    // Probe request
    if (frameType == 0x00 && frameSubType == 0x04) {
        probeCount++;
    }
    
    // Verificar EAPOL (4-way handshake)
    bool isEapolPacket = isEAPOL(ppkt->payload, len);
    if (isEapolPacket) {
        eapolCount++;
        writePcapPacket(ppkt->payload, len);
        
        // Verificar se tem PMKID
        if (hasPMKID(ppkt->payload, len)) {
            pmkidCount++;
            Serial.println("*** PMKID CAPTURADO! ***");
        }
        
        Serial.printf("EAPOL capturado! Total: %lu\n", eapolCount);
    }
}

// ==================== CONTROLE DE CANAIS ====================
void changeChannel() {
    unsigned long now = millis();
    
    if (now - lastChannelChange >= CHANNEL_HOP_INTERVAL_MS) {
        if (usePriorityChannelsOnly) {
            // Rotacionar entre canais prioritários
            channelIndex = (channelIndex + 1) % PRIORITY_CHANNEL_COUNT;
            currentChannel = PRIORITY_CHANNELS[channelIndex];
        } else {
            // Rotacionar entre todos os canais
            channelIndex = (channelIndex + 1) % ALL_CHANNEL_COUNT;
            currentChannel = ALL_CHANNELS[channelIndex];
        }
        
        esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        lastChannelChange = now;
        
        Serial.printf("Mudando para canal: %d\n", currentChannel);
    }
}

// ==================== SETUP ====================
void setup() {
    M5.begin();
    Serial.begin(115200);
    
    M5.Lcd.setBrightness(100);
    drawHeader();
    
    // Inicializar SD Card
    if (ENABLE_SD_CARD) {
        if (SD.begin()) {
            sdCardEnabled = true;
            initPcapFile();
            Serial.println("SD Card inicializado com sucesso!");
        } else {
            Serial.println("Falha ao inicializar SD Card!");
            M5.Lcd.setCursor(10, 100);
            M5.Lcd.setTextColor(RED);
            M5.Lcd.println("SD CARD FALHOU!");
            delay(2000);
        }
    }
    
    // Configurar WiFi em modo promíscuo
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    
    Serial.println("M5Gotchi Optimized iniciado!");
    Serial.printf("Modo: %s\n", usePriorityChannelsOnly ? "CANAIS PRIORITÁRIOS" : "TODOS OS CANAIS");
    Serial.printf("Tempo por canal: %d ms\n", CHANNEL_HOP_INTERVAL_MS);
    
    updateDisplay();
}

// ==================== LOOP ====================
void loop() {
    M5.update();
    
    // Botão A: Alternar modo de canais
    if (M5.BtnA.wasPressed()) {
        usePriorityChannelsOnly = !usePriorityChannelsOnly;
        channelIndex = 0;
        currentChannel = usePriorityChannelsOnly ? PRIORITY_CHANNELS[0] : ALL_CHANNELS[0];
        esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        Serial.printf("Modo alterado: %s\n", usePriorityChannelsOnly ? "PRIORITÁRIO" : "TODOS");
    }
    
    // Botão B: Habilitar/Desabilitar SD Card
    if (M5.BtnB.wasPressed()) {
        if (!sdCardEnabled && SD.begin()) {
            sdCardEnabled = true;
            initPcapFile();
            Serial.println("SD Card habilitado!");
        } else if (sdCardEnabled) {
            if (pcapFile) pcapFile.close();
            sdCardEnabled = false;
            Serial.println("SD Card desabilitado!");
        }
    }
    
    // Botão C: Reset contadores
    if (M5.BtnC.wasPressed()) {
        packetCount = 0;
        eapolCount = 0;
        pmkidCount = 0;
        beaconCount = 0;
        probeCount = 0;
        Serial.println("Contadores resetados!");
    }
    
    // Trocar de canal
    changeChannel();
    
    // Atualizar display a cada 500ms
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate >= 500) {
        updateDisplay();
        lastDisplayUpdate = millis();
    }
    
    delay(10);
}
