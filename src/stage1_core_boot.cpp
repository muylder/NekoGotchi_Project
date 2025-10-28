#include <M5Cardputer.h>
#include <M5Unified.h>

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
    Serial.println(F("Stage 1: Core Boot Test"));
    Serial.println(F("========================"));

    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.println(F("[1/4] M5.begin OK"));

    M5Cardputer.begin();
    Serial.println(F("[2/4] M5Cardputer.begin OK"));

    M5.Display.setRotation(1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kFgColor, kBgColor);
    M5.Display.fillScreen(kBgColor);
    Serial.println(F("[3/4] Display init OK"));

    M5.Display.setCursor(20, 36);
    M5.Display.setTextSize(2);
    M5.Display.println(F("M5Gotchi PRO"));

    M5.Display.setCursor(20, 64);
    M5.Display.setTextSize(1);
    M5.Display.println(F("Stage 1: Core Boot"));

    M5.Display.setCursor(20, 80);
    M5.Display.println(F("Waiting..."));
    Serial.println(F("[4/4] Screen draw OK"));

    Serial.println(F("Setup complete. Loop heartbeat running."));
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
