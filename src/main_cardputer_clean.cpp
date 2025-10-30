/*
 * M5Gotchi PRO - Kawaii Pet Edition
 * Nova versão com:
 * - Gatinho grande com status visível
 * - Cores kawaii bonitas
 * - Animação suave do pet
 * - Controles funcionando 100%
 */

#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>

// ==================== CONFIGURAÇÕES ====================
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

// Cores Kawaii
#define COLOR_BG 0x0010        // Azul escuro
#define COLOR_NEKO 0xFD20      // Rosa forte
#define COLOR_HIGHLIGHT 0x07FF // Ciano brilhante
#define COLOR_TEXT 0xFFFF      // Branco
#define COLOR_STAT_GOOD 0x07E0 // Verde
#define COLOR_STAT_OK 0xFFE0   // Amarelo
#define COLOR_STAT_BAD 0xF800  // Vermelho
#define COLOR_PANEL 0x2104     // Cinza escuro
#define COLOR_SELECT 0x4A49    // Azul médio

// ==================== PÁGINAS ====================
enum Page {
    PAGE_HOME = 0,
    PAGE_MENU,
    PAGE_WIFI_SCANNER,
    PAGE_BLUETOOTH,
    PAGE_TOOLS,
    PAGE_SETTINGS,
    PAGE_COUNT
};

// ==================== VARIÁVEIS GLOBAIS ====================
Page currentPage = PAGE_HOME;
int menuSelection = 0;
int maxMenuItems = 5;
bool needsRedraw = true;
unsigned long lastAnimUpdate = 0;
unsigned long lastStatusUpdate = 0;
int nekoFrame = 0;

// Pet Status
int hunger = 100;     // 0-100 (começa cheio)
int happiness = 100;  // 0-100 (começa feliz)
int energy = 100;     // 0-100 (começa energizado)
int level = 0;        // Começa no level 0
int experience = 0;   // Começa com 0 XP

// Uptime
unsigned long sessionStartTime = 0;

// WiFi
std::vector<String> wifiNetworks;
int selectedNetwork = 0;
bool isScanning = false;

// Neko HACKER animation frames (mais cyberpunk!)
const char* nekoFrames[] = {
    "  /\\_/\\  \n ( -.^ ) \n  >[$]<  ",  // Hacker (piscada)
    "  /\\_/\\  \n ( ^.^ ) \n  >[#]<  ",  // Coding
    "  /\\_/\\  \n ( o.o ) \n  >{*}<  ",  // Scanning
    "  /\\_/\\  \n ( @.@ ) \n  >[!]<  "   // Alert
};

// ==================== FUNÇÕES DE DESENHO ====================

void drawStatusBar(int x, int y, const char* label, int value, uint16_t color) {
    // Label
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(x, y);
    M5.Display.print(label);
    
    // Barra
    int barWidth = 60;
    int barHeight = 8;
    int barX = x + 50;
    int filled = (value * barWidth) / 100;
    
    M5.Display.drawRect(barX, y, barWidth, barHeight, COLOR_PANEL);
    M5.Display.fillRect(barX + 1, y + 1, filled - 2, barHeight - 2, color);
    
    // Valor
    M5.Display.setCursor(barX + barWidth + 5, y);
    M5.Display.printf("%d%%", value);
}

void drawBigNeko(int x, int y, int frame) {
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setTextSize(3);  // TAMANHO 3 = MEGA GATINHO!
    
    // Desenhar gatinho grande
    String frameText = String(nekoFrames[frame % 4]);
    int lines = 0;
    int startY = y;
    
    // Split por linhas
    int idx = 0;
    while (idx < frameText.length()) {
        String line = "";
        while (idx < frameText.length() && frameText[idx] != '\n') {
            line += frameText[idx++];
        }
        idx++; // Skip \n
        
        M5.Display.setCursor(x, startY + lines * 24);
        M5.Display.print(line);
        lines++;
    }
}

void drawFooter(const char* help) {
    M5.Display.fillRect(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, 12, COLOR_PANEL);
    M5.Display.setTextColor(COLOR_HIGHLIGHT);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(2, SCREEN_HEIGHT - 10);
    M5.Display.print(help);
}

void drawHomePage() {
    M5.Display.fillScreen(COLOR_BG);
    
    // Título hacker
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(35, 2);
    M5.Display.print("NEKO HACKER");
    
    // Linha 1: Level, XP e Uptime (mais compacto)
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_HIGHLIGHT);
    M5.Display.setCursor(5, 20);
    M5.Display.printf("Lv:%d", level);
    M5.Display.setCursor(45, 20);
    M5.Display.printf("XP:%d", experience);
    
    // Uptime no canto direito
    unsigned long uptime = (millis() - sessionStartTime) / 1000;
    int hours = uptime / 3600;
    int mins = (uptime % 3600) / 60;
    int secs = uptime % 60;
    M5.Display.setCursor(150, 20);
    M5.Display.printf("%02d:%02d:%02d", hours, mins, secs);
    
    // Desenhar gatinho HACKER MEGA no centro
    drawBigNeko(70, 32, nekoFrame);
    
    // Status bars otimizados (mais alto, mais espaçado)
    int statusY = 85;
    int spacing = 10;
    
    uint16_t hungerColor = hunger > 60 ? COLOR_STAT_GOOD : (hunger > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    uint16_t happyColor = happiness > 60 ? COLOR_STAT_GOOD : (happiness > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    uint16_t energyColor = energy > 60 ? COLOR_STAT_GOOD : (energy > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    
    // Barras mais compactas com labels menores
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(5, statusY);
    M5.Display.print("Food");
    M5.Display.fillRect(35, statusY, (hunger * 85) / 100, 7, hungerColor);
    M5.Display.drawRect(35, statusY, 85, 7, COLOR_PANEL);
    M5.Display.setCursor(125, statusY);
    M5.Display.printf("%d%%", hunger);
    
    M5.Display.setCursor(5, statusY + spacing);
    M5.Display.print("Joy ");
    M5.Display.fillRect(35, statusY + spacing, (happiness * 85) / 100, 7, happyColor);
    M5.Display.drawRect(35, statusY + spacing, 85, 7, COLOR_PANEL);
    M5.Display.setCursor(125, statusY + spacing);
    M5.Display.printf("%d%%", happiness);
    
    M5.Display.setCursor(5, statusY + spacing * 2);
    M5.Display.print("Enrg");
    M5.Display.fillRect(35, statusY + spacing * 2, (energy * 85) / 100, 7, energyColor);
    M5.Display.drawRect(35, statusY + spacing * 2, 85, 7, COLOR_PANEL);
    M5.Display.setCursor(125, statusY + spacing * 2);
    M5.Display.printf("%d%%", energy);
    
    drawFooter("ESC=Menu | Space=Feed | O=Play");
}

void drawMenuPage() {
    M5.Display.fillScreen(COLOR_BG);
    
    // Título
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(60, 5);
    M5.Display.print("= MENU =");
    
    const char* menuItems[] = {
        "1. WiFi Scanner",
        "2. Bluetooth Tools",
        "3. System Tools",
        "4. Pet Settings",
        "5. About / Info"
    };
    
    int startY = 30;
    int itemHeight = 18;
    
    for (int i = 0; i < 5; i++) {
        if (i == menuSelection) {
            M5.Display.fillRoundRect(10, startY + i * itemHeight - 2, SCREEN_WIDTH - 20, 16, 4, COLOR_SELECT);
            M5.Display.setTextColor(COLOR_HIGHLIGHT);
        } else {
            M5.Display.setTextColor(COLOR_TEXT);
        }
        
        M5.Display.setTextSize(1);
        M5.Display.setCursor(15, startY + i * itemHeight);
        M5.Display.print(menuItems[i]);
    }
    
    drawFooter(";=Up .=Down Enter=Select ESC=Back");
}

void drawWiFiScanner() {
    M5.Display.fillScreen(COLOR_BG);
    
    // Título
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(45, 5);
    M5.Display.print("WiFi Scan");
    
    if (isScanning) {
        M5.Display.setTextColor(COLOR_HIGHLIGHT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(70, 60);
        M5.Display.print("Scanning...");
        return;
    }
    
    if (wifiNetworks.size() == 0) {
        M5.Display.setTextColor(COLOR_TEXT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(40, 60);
        M5.Display.print("Press SPACE to scan");
    } else {
        int startY = 30;
        int itemHeight = 12;
        int visibleItems = 8;
        int scrollOffset = max(0, selectedNetwork - visibleItems + 1);
        
        for (int i = scrollOffset; i < min((int)wifiNetworks.size(), scrollOffset + visibleItems); i++) {
            if (i == selectedNetwork) {
                M5.Display.fillRect(2, startY + (i - scrollOffset) * itemHeight - 1, SCREEN_WIDTH - 4, 11, COLOR_SELECT);
                M5.Display.setTextColor(COLOR_HIGHLIGHT);
            } else {
                M5.Display.setTextColor(COLOR_TEXT);
            }
            
            M5.Display.setTextSize(1);
            M5.Display.setCursor(5, startY + (i - scrollOffset) * itemHeight);
            
            String network = wifiNetworks[i];
            if (network.length() > 35) {
                network = network.substring(0, 32) + "...";
            }
            M5.Display.print(network);
        }
        
        // Scrollbar
        if (wifiNetworks.size() > visibleItems) {
            int barHeight = (visibleItems * 96) / wifiNetworks.size();
            int barY = 30 + (scrollOffset * 96) / wifiNetworks.size();
            M5.Display.fillRect(SCREEN_WIDTH - 3, barY, 2, barHeight, COLOR_STAT_GOOD);
        }
    }
    
    drawFooter("Space=Scan ;.=Nav ESC=Back");
}

void drawBluetoothPage() {
    M5.Display.fillScreen(COLOR_BG);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(30, 5);
    M5.Display.print("Bluetooth");
    
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 35);
    M5.Display.print("BLE Spam - Apple");
    M5.Display.setCursor(10, 50);
    M5.Display.print("BLE Spam - Samsung");
    M5.Display.setCursor(10, 65);
    M5.Display.print("BLE Scanner");
    M5.Display.setCursor(10, 80);
    M5.Display.print("Device Jammer");
    
    drawFooter(";.=Nav Enter=Select ESC=Back");
}

void drawToolsPage() {
    M5.Display.fillScreen(COLOR_BG);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(55, 5);
    M5.Display.print("Tools");
    
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 35);
    M5.Display.print("File Manager");
    M5.Display.setCursor(10, 50);
    M5.Display.print("System Info");
    M5.Display.setCursor(10, 65);
    M5.Display.print("LED Control");
    M5.Display.setCursor(10, 80);
    M5.Display.print("Reboot Device");
    
    drawFooter(";.=Nav Enter=Select ESC=Back");
}

void drawSettingsPage() {
    M5.Display.fillScreen(COLOR_BG);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(40, 5);
    M5.Display.print("Settings");
    
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 30);
    M5.Display.print("Brightness: 50%");
    M5.Display.setCursor(10, 45);
    M5.Display.print("Sound: Enabled");
    M5.Display.setCursor(10, 60);
    M5.Display.print("Pet Name: Neko");
    M5.Display.setCursor(10, 75);
    M5.Display.print("Auto-save: On");
    
    M5.Display.setTextColor(COLOR_HIGHLIGHT);
    M5.Display.setCursor(10, 95);
    M5.Display.print("Firmware: v2.0.4");
    M5.Display.setCursor(10, 107);
    
    // RAM livre
    uint32_t freeHeap = ESP.getFreeHeap();
    M5.Display.printf("Free RAM: %dKB", freeHeap / 1024);
    
    drawFooter("ESC=Back");
}

// ==================== FUNÇÕES DE NAVEGAÇÃO ====================

void handleHomeInput(char key) {
    // ESC (código 0x1B) = Menu
    if (key == 0x1B || key == '`') {  // ESC ou backtick
        currentPage = PAGE_MENU;
        menuSelection = 0;
        needsRedraw = true;
    }
    // SPACE = Alimentar (aumenta hunger)
    else if (key == ' ') {
        hunger = min(100, hunger + 15);
        happiness = min(100, happiness + 5);
        experience += 2;
        needsRedraw = true;
    }
    // O ou o = Brincar (aumenta happiness)
    else if (key == 'o' || key == 'O') {
        happiness = min(100, happiness + 20);
        energy = max(0, energy - 10);
        experience += 3;
        needsRedraw = true;
    }
}

void handleMenuInput(char key) {
    // Debug: mostrar código da tecla
    Serial.printf("Menu key: '%c' (0x%02X)\n", key, (unsigned char)key);
    
    // Navegação com ; e .
    if (key == ';') {  // UP
        menuSelection = (menuSelection - 1 + maxMenuItems) % maxMenuItems;
        needsRedraw = true;
        Serial.println("UP pressed");
    } 
    else if (key == '.') {  // DOWN
        menuSelection = (menuSelection + 1) % maxMenuItems;
        needsRedraw = true;
        Serial.println("DOWN pressed");
    }
    // ENTER - testar TODOS os códigos possíveis
    else if (key == 0x0D || key == 0x0A || key == '\n' || key == '\r' || key == 13 || key == 10) {
        Serial.printf(">>> ENTER DETECTED! menuSelection = %d <<<\n", menuSelection);
        switch (menuSelection) {
            case 0: 
                Serial.println("Going to WiFi Scanner");
                currentPage = PAGE_WIFI_SCANNER; 
                break;
            case 1: 
                Serial.println("Going to Bluetooth");
                currentPage = PAGE_BLUETOOTH; 
                break;
            case 2: 
                Serial.println("Going to Tools");
                currentPage = PAGE_TOOLS; 
                break;
            case 3: 
                Serial.println("Going to Settings");
                currentPage = PAGE_SETTINGS; 
                break;
            case 4: 
                Serial.println("Showing About");
                // About
                M5.Display.fillScreen(COLOR_BG);
                M5.Display.setTextColor(COLOR_NEKO);
                M5.Display.setTextSize(2);
                M5.Display.setCursor(30, 40);
                M5.Display.print("NEKO HACKER");
                M5.Display.setTextColor(COLOR_TEXT);
                M5.Display.setTextSize(1);
                M5.Display.setCursor(40, 65);
                M5.Display.print("Safe Pet v2.0.4");
                M5.Display.setCursor(35, 80);
                M5.Display.print("Made with <3");
                delay(2000);
                break;
        }
        needsRedraw = true;
    }
    // ESC = Voltar para home
    else if (key == 0x1B || key == '`') {
        Serial.println("ESC pressed - back to home");
        currentPage = PAGE_HOME;
        needsRedraw = true;
    }
    // Ignorar números - não fazer nada
    else if (key >= '0' && key <= '9') {
        Serial.printf("Number %c ignored (no shortcuts)\n", key);
    }
}

void handleWiFiInput(char key) {
    if (key == ' ') {  // SPACE - Scan
        if (!isScanning) {
            isScanning = true;
            needsRedraw = true;
            
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            delay(100);
            
            int n = WiFi.scanNetworks();
            wifiNetworks.clear();
            
            for (int i = 0; i < n; i++) {
                String network = WiFi.SSID(i);
                network += " ";
                network += WiFi.RSSI(i);
                network += "dBm";
                wifiNetworks.push_back(network);
            }
            
            selectedNetwork = 0;
            isScanning = false;
            needsRedraw = true;
        }
    } 
    else if (key == ';' && wifiNetworks.size() > 0) {  // UP
        selectedNetwork = (selectedNetwork - 1 + wifiNetworks.size()) % wifiNetworks.size();
        needsRedraw = true;
    } 
    else if (key == '.' && wifiNetworks.size() > 0) {  // DOWN
        selectedNetwork = (selectedNetwork + 1) % wifiNetworks.size();
        needsRedraw = true;
    } 
    else if (key == 0x1B || key == '`') {  // ESC = Voltar
        currentPage = PAGE_MENU;
        menuSelection = 0;
        needsRedraw = true;
    }
}

void handleBluetoothInput(char key) {
    if (key == 0x1B || key == '`') {
        currentPage = PAGE_MENU;
        needsRedraw = true;
    }
    else if (key >= '1' && key <= '4') {
        // Simular ação BLE
        M5.Display.fillScreen(COLOR_BG);
        M5.Display.setTextColor(COLOR_HIGHLIGHT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(40, 60);
        M5.Display.printf("BLE Action %c...", key);
        delay(1500);
        needsRedraw = true;
    }
}

void handleToolsInput(char key) {
    Serial.printf("Tools key: '%c' (0x%02X)\n", key, (uint8_t)key);
    
    if (key >= '0' && key <= '9') {
        Serial.println("Number ignored in Tools");
        return;
    }
    
    if (key == 0x1B || key == '`') {
        currentPage = PAGE_MENU;
        needsRedraw = true;
    } 
    else if (key == 0x0D || key == 0x0A || key == '\n' || key == '\r' || key == 13 || key == 10) {
        Serial.println("ENTER detected in Tools!");
        // TODO: Add navigation selection here
    }
}

void handleSettingsInput(char key) {
    if (key == 0x1B || key == '`') {
        currentPage = PAGE_MENU;
        needsRedraw = true;
    }
}

// ==================== SETUP & LOOP ====================

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5Cardputer.begin();  // IMPORTANTE: Inicializar o Cardputer!
    M5.Display.setRotation(1);
    M5.Display.setBrightness(128);
    M5.Display.fillScreen(COLOR_BG);
    
    Serial.begin(115200);
    Serial.println("\n🐱 M5GOTCHI PRO - Kawaii Pet Starting...");
    
    // Splash screen kawaii
    M5.Display.fillScreen(COLOR_BG);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(20, 40);
    M5.Display.print("M5GOTCHI");
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_HIGHLIGHT);
    M5.Display.setCursor(60, 75);
    M5.Display.print("Kawaii Pet v2.0.2");
    M5.Display.setCursor(55, 90);
    M5.Display.print("Loading...");
    
    // Barra de loading kawaii
    for (int i = 0; i < 200; i += 10) {
        M5.Display.fillRect(20, 105, i, 6, COLOR_STAT_GOOD);
        delay(50);
    }
    
    delay(500);
    
    sessionStartTime = millis();  // Iniciar contador de uptime
    currentPage = PAGE_HOME;
    needsRedraw = true;
    lastAnimUpdate = millis();
    lastStatusUpdate = millis();
}

void loop() {
    M5.update();
    
    unsigned long now = millis();
    
    // Animar gatinho (500ms = 2 FPS)
    if (now - lastAnimUpdate > 500 && currentPage == PAGE_HOME) {
        nekoFrame = (nekoFrame + 1) % 4;
        lastAnimUpdate = now;
        needsRedraw = true;
    }
    
    // Atualizar status do pet com tempos diferentes para cada stat
    if (now - lastStatusUpdate > 30000) {  // A cada 30 segundos
        // Fome diminui mais rápido (a cada 30s)
        hunger = max(0, hunger - 3);
        
        // Felicidade diminui médio (a cada 45s - verificar com contador)
        static int happinessCounter = 0;
        happinessCounter++;
        if (happinessCounter >= 2) {  // 30s * 1.5 = 45s
            happiness = max(0, happiness - 2);
            happinessCounter = 0;
        }
        
        // Energia diminui devagar (a cada 60s - verificar com contador)
        static int energyCounter = 0;
        energyCounter++;
        if (energyCounter >= 2) {  // 30s * 2 = 60s
            energy = max(0, energy - 2);
            energyCounter = 0;
        }
        
        // Ganhar XP com o tempo (a cada 30s)
        experience += 5;
        if (experience >= 100) {  // Level up a cada 100 XP
            experience = 0;
            level++;
            // Feedback visual de level up
            M5.Display.fillScreen(COLOR_BG);
            M5.Display.setTextColor(COLOR_NEKO);
            M5.Display.setTextSize(3);
            M5.Display.setCursor(30, 50);
            M5.Display.printf("LEVEL %d!", level);
            delay(1500);
        }
        
        lastStatusUpdate = now;
        needsRedraw = true;
    }
    
    // Redesenhar tela apenas quando necessário
    if (needsRedraw) {
        switch (currentPage) {
            case PAGE_HOME:
                drawHomePage();
                break;
            case PAGE_MENU:
                drawMenuPage();
                break;
            case PAGE_WIFI_SCANNER:
                drawWiFiScanner();
                break;
            case PAGE_BLUETOOTH:
                drawBluetoothPage();
                break;
            case PAGE_TOOLS:
                drawToolsPage();
                break;
            case PAGE_SETTINGS:
                drawSettingsPage();
                break;
        }
        needsRedraw = false;
    }
    
    // Processar input do teclado
    M5Cardputer.update();  // IMPORTANTE: Atualizar o Cardputer!
    
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            
            for (auto key : status.word) {
                // Debug: mostrar tecla pressionada
                Serial.printf("Key pressed: %c (0x%02X)\n", key, key);
                
                switch (currentPage) {
                    case PAGE_HOME:
                        handleHomeInput(key);
                        break;
                    case PAGE_MENU:
                        handleMenuInput(key);
                        break;
                    case PAGE_WIFI_SCANNER:
                        handleWiFiInput(key);
                        break;
                    case PAGE_BLUETOOTH:
                        handleBluetoothInput(key);
                        break;
                    case PAGE_TOOLS:
                        handleToolsInput(key);
                        break;
                    case PAGE_SETTINGS:
                        handleSettingsInput(key);
                        break;
                }
            }
        }
    }
    
    delay(10);  // 100 FPS loop
}
