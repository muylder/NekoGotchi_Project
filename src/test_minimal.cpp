#include <M5Unified.h>
#include <M5Cardputer.h>

void setup() {
    // Try Serial 3 times
    for(int i = 0; i < 3; i++) {
        Serial.begin(115200);
        delay(500);
    }
    
    Serial.println("\n\n=== MINIMAL TEST START ===");
    Serial.flush();
    
    // Step 1: Config
    auto cfg = M5.config();
    Serial.println("Step 1: config OK");
    Serial.flush();
    
    // Step 2: M5.begin
    M5.begin(cfg);
    Serial.println("Step 2: M5.begin OK");
    Serial.flush();
    
    // Step 3: Display
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(20, 40);
    M5.Display.println("TEST");
    M5.Display.println("OK!");
    Serial.println("Step 3: Display OK");
    Serial.flush();
    
    // Step 4: Cardputer
    M5Cardputer.begin();
    Serial.println("Step 4: Cardputer OK");
    Serial.flush();
    
    Serial.println("=== ALL STEPS PASSED ===\n");
}

void loop() {
    M5.update();
    static unsigned long last = 0;
    if (millis() - last > 1000) {
        Serial.println("Loop: " + String(millis()));
        last = millis();
    }
    delay(10);
}
