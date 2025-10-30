/*
 * 🔒 M5GOTCHI - SECURITY CLEANUP TOOL
 * 
 * Este firmware deleta TODOS os arquivos sensíveis do SD card
 * Use APENAS se você viu dados sensíveis (senhas WiFi) nos arquivos
 * 
 * ATENÇÃO: DEPOIS DE USAR, MUDE SUA SENHA WIFI!
 */

#include <M5Unified.h>
#include <M5Cardputer.h>
#include <SD.h>
#include <FS.h>

#define SD_SCK 40
#define SD_MISO 39
#define SD_MOSI 14
#define SD_CS 12

bool sdAvailable = false;
int filesDeleted = 0;
int filesNotFound = 0;

// Lista de arquivos potencialmente sensíveis
const char* sensitiveFiles[] = {
    "/config.json",
    "/credentials.txt",
    "/session.json",
    "/stats.json",
    "/logs/system.log",
    "/logs/wifi.log",
    "/logs/attacks.log",
    "/ai_memory.json",
    "/wardrive.csv",
    "/vuln_db.json",
    "/handshakes/",
    "/portals/",
    "/captures/"
};

const int numFiles = sizeof(sensitiveFiles) / sizeof(sensitiveFiles[0]);

void deleteDirectory(const char* path) {
    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        return;
    }
    
    File file = dir.openNextFile();
    while (file) {
        String filePath = String(path) + "/" + String(file.name());
        
        if (file.isDirectory()) {
            deleteDirectory(filePath.c_str());
            SD.rmdir(filePath.c_str());
        } else {
            SD.remove(filePath.c_str());
            filesDeleted++;
        }
        
        file = dir.openNextFile();
    }
    
    SD.rmdir(path);
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(1);
    M5.Display.setBrightness(128);
    M5.Display.fillScreen(TFT_BLACK);
    
    Serial.begin(115200);
    delay(1000);
    
    // Título
    M5.Display.setTextColor(TFT_RED);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(30, 10);
    M5.Display.print("SECURITY");
    M5.Display.setCursor(40, 30);
    M5.Display.print("CLEANUP");
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_YELLOW);
    M5.Display.setCursor(10, 55);
    M5.Display.print("This will DELETE all");
    M5.Display.setCursor(10, 68);
    M5.Display.print("sensitive files!");
    
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setCursor(10, 90);
    M5.Display.print("Press ENTER to start");
    M5.Display.setCursor(10, 103);
    M5.Display.print("Press ESC to cancel");
    
    Serial.println("🔒 SECURITY CLEANUP TOOL");
    Serial.println("⚠️  Waiting for confirmation...");
    
    // Aguardar confirmação
    bool confirmed = false;
    bool cancelled = false;
    
    while (!confirmed && !cancelled) {
        M5.update();
        
        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
                
                for (auto key : status.word) {
                    if (key == 0x0D || key == '\n' || key == '\r') {
                        confirmed = true;
                    } else if (key == '`' || key == 0x1B) {
                        cancelled = true;
                    }
                }
            }
        }
        
        delay(10);
    }
    
    if (cancelled) {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(TFT_GREEN);
        M5.Display.setTextSize(2);
        M5.Display.setCursor(40, 60);
        M5.Display.print("CANCELLED");
        Serial.println("✅ Cleanup cancelled");
        delay(2000);
        ESP.restart();
        return;
    }
    
    // Iniciar limpeza
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_YELLOW);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(30, 20);
    M5.Display.print("CLEANING...");
    
    Serial.println("🧹 Starting cleanup...");
    
    // Inicializar SD
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SPI, 25000000)) {
        Serial.println("❌ SD Card not found!");
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.setCursor(10, 60);
        M5.Display.print("SD CARD ERROR!");
        delay(3000);
        ESP.restart();
        return;
    }
    
    sdAvailable = true;
    Serial.println("✅ SD Card found");
    
    // Deletar arquivos
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE);
    int y = 45;
    
    for (int i = 0; i < numFiles; i++) {
        String filename = String(sensitiveFiles[i]);
        
        Serial.printf("Checking: %s\n", filename.c_str());
        
        if (filename.endsWith("/")) {
            // É um diretório
            if (SD.exists(filename.c_str())) {
                M5.Display.setCursor(10, y);
                M5.Display.printf("Del dir: %s", filename.c_str());
                y += 10;
                
                deleteDirectory(filename.c_str());
                Serial.printf("✅ Deleted directory: %s\n", filename.c_str());
            } else {
                filesNotFound++;
                Serial.printf("⚪ Not found: %s\n", filename.c_str());
            }
        } else {
            // É um arquivo
            if (SD.exists(filename.c_str())) {
                M5.Display.setCursor(10, y);
                M5.Display.printf("Del: %s", filename.c_str());
                y += 10;
                
                SD.remove(filename.c_str());
                filesDeleted++;
                Serial.printf("✅ Deleted: %s\n", filename.c_str());
            } else {
                filesNotFound++;
                Serial.printf("⚪ Not found: %s\n", filename.c_str());
            }
        }
        
        if (y > 115) {
            delay(1000);
            M5.Display.fillRect(0, 45, 240, 80, TFT_BLACK);
            y = 45;
        }
    }
    
    // Resultado final
    delay(500);
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(30, 20);
    M5.Display.print("COMPLETE!");
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setCursor(10, 50);
    M5.Display.printf("Deleted: %d files", filesDeleted);
    M5.Display.setCursor(10, 65);
    M5.Display.printf("Not found: %d", filesNotFound);
    
    M5.Display.setTextColor(TFT_RED);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 90);
    M5.Display.print("IMPORTANT:");
    M5.Display.setTextColor(TFT_YELLOW);
    M5.Display.setCursor(10, 103);
    M5.Display.print("Change your WiFi");
    M5.Display.setCursor(10, 116);
    M5.Display.print("password NOW!");
    
    Serial.println("\n🔒 =====================");
    Serial.println("🔒 CLEANUP COMPLETE!");
    Serial.println("🔒 =====================");
    Serial.printf("✅ Deleted: %d files\n", filesDeleted);
    Serial.printf("⚪ Not found: %d files\n", filesNotFound);
    Serial.println("\n⚠️  CRITICAL: CHANGE YOUR WIFI PASSWORD NOW!");
    Serial.println("⚠️  Access your router settings and change it!");
}

void loop() {
    M5.update();
    delay(1000);
    
    // Piscar aviso
    static bool blink = false;
    blink = !blink;
    
    if (blink) {
        M5.Display.fillRect(0, 90, 240, 35, TFT_RED);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setCursor(10, 100);
        M5.Display.print("CHANGE WIFI PASSWORD!");
    } else {
        M5.Display.fillRect(0, 90, 240, 35, TFT_BLACK);
        M5.Display.setTextColor(TFT_YELLOW);
        M5.Display.setCursor(10, 103);
        M5.Display.print("Change your WiFi");
        M5.Display.setCursor(10, 116);
        M5.Display.print("password NOW!");
    }
}
