#include <M5Cardputer.h>
#include <M5Unified.h>
#include <WiFi.h>

namespace {
constexpr uint16_t kBgColor = TFT_BLACK;
constexpr uint16_t kFgColor = TFT_GREEN;
constexpr uint32_t kHeartbeatIntervalMs = 1000;
}

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println(F("========================"));
    Serial.println(F("Stage 2: WiFi Scan Test"));
    Serial.println(F("========================"));

    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.println(F("[1/6] M5.begin OK"));

    M5Cardputer.begin();
    Serial.println(F("[2/6] M5Cardputer.begin OK"));

    M5.Display.setRotation(1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kFgColor, kBgColor);
    M5.Display.fillScreen(kBgColor);
    Serial.println(F("[3/6] Display init OK"));

    M5.Display.setCursor(20, 30);
    M5.Display.setTextSize(2);
    M5.Display.println(F("M5Gotchi PRO"));

    M5.Display.setCursor(20, 56);
    M5.Display.setTextSize(1);
    M5.Display.println(F("Stage 2: WiFi Scan"));

    Serial.println(F("[4/6] Banner drawn"));

    Serial.println(F("[5/6] WiFi setup..."));
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(200);

    Serial.println(F("Scanning for networks..."));
    const int networksFound = WiFi.scanNetworks(false, true);
    Serial.printf("WiFi.scanNetworks -> %d\n", networksFound);

    M5.Display.setCursor(20, 80);
    if (networksFound >= 0) {
        M5.Display.printf("WiFi networks: %d\n", networksFound);
        Serial.println(F("[6/6] WiFi scan complete"));
    } else {
        M5.Display.println(F("WiFi scan failed"));
        Serial.println(F("[6/6] WiFi scan failed"));
    }

    WiFi.scanDelete();
}

void loop() {
    M5.update();

    static uint32_t lastLog = 0;
    const uint32_t now = millis();
    if (now - lastLog >= kHeartbeatIntervalMs) {
        Serial.println(F("Heartbeat: M5 core alive"));
        lastLog = now;
    }

    delay(10);
}
