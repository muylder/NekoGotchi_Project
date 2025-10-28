#include <M5Cardputer.h>
#include <M5Unified.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

// Define ENABLE_SD_CHECK to enable SD card checking (disabled by default)
// #define ENABLE_SD_CHECK

namespace {
constexpr uint16_t kBgColor = TFT_BLACK;
constexpr uint16_t kFgColor = TFT_GREEN;
constexpr uint32_t kHeartbeatIntervalMs = 1000;

// Official M5Stack Cardputer SD card pins
constexpr int kSdCsPin = 12;
constexpr int kSdSckPin = 40;
constexpr int kSdMisoPin = 39;
constexpr int kSdMosiPin = 14;
}

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println(F("========================"));
    Serial.println(F("Stage 3: SD Check"));
    Serial.println(F("========================"));

    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.println(F("[1/8] M5.begin OK"));

    M5Cardputer.begin();
    Serial.println(F("[2/8] M5Cardputer.begin OK"));

    M5.Display.setRotation(1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kFgColor, kBgColor);
    M5.Display.fillScreen(kBgColor);
    Serial.println(F("[3/8] Display init OK"));

    M5.Display.setCursor(20, 24);
    M5.Display.setTextSize(2);
    M5.Display.println(F("M5Gotchi PRO"));

    M5.Display.setCursor(20, 48);
    M5.Display.setTextSize(1);
#ifdef ENABLE_SD_CHECK
    M5.Display.println(F("Stage 3: SD + WiFi"));
#else
    M5.Display.println(F("Stage 3: WiFi Test"));
#endif
    Serial.println(F("[4/8] Banner drawn"));

    // Pre-fill status slots so the user immediately sees what is being tested.
    M5.Display.setTextColor(TFT_YELLOW, kBgColor);
    M5.Display.setCursor(20, 72);
    M5.Display.println(F("WiFi: checking..."));
#ifdef ENABLE_SD_CHECK
    M5.Display.setCursor(20, 96);
    M5.Display.println(F("SD: checking..."));
#endif
    M5.Display.setTextColor(kFgColor, kBgColor);

    Serial.println(F("[5/8] WiFi scan"));
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(200);
    const int networksFound = WiFi.scanNetworks(false, true);
    WiFi.scanDelete();
    const bool wifiOk = networksFound >= 0;
    Serial.printf("WiFi networks found: %d\n", networksFound);

    M5.Display.fillRect(20, 72, 200, 10, kBgColor);
    M5.Display.setCursor(20, 72);
    if (wifiOk) {
        M5.Display.setTextColor(TFT_GREEN, kBgColor);
        M5.Display.printf("WiFi: OK (%d)", networksFound);
    } else {
        M5.Display.setTextColor(TFT_RED, kBgColor);
        M5.Display.println(F("WiFi: FAIL"));
    }

    bool sdReady = false;  // Default: SD check disabled

#ifdef ENABLE_SD_CHECK
    Serial.println(F("[6/8] Preparing SD bus"));
    SPI.begin(kSdSckPin, kSdMisoPin, kSdMosiPin, kSdCsPin);
    delay(100);

    Serial.println(F("[7/8] Mounting SD"));
    Serial.printf("CS=%d, SCK=%d, MISO=%d, MOSI=%d\n",
                  kSdCsPin, kSdSckPin, kSdMisoPin, kSdMosiPin);

    // Simple direct approach - no timeout (user must enable this manually)
    sdReady = SD.begin(kSdCsPin, SPI, 4000000);

    if (!sdReady) {
        Serial.println(F("Trying legacy pins..."));
        SD.end();
        SPI.end();
        delay(200);

        constexpr int legacyCS = 34, legacySCK = 36, legacyMISO = 35, legacyMOSI = 37;
        SPI.begin(legacySCK, legacyMISO, legacyMOSI, legacyCS);
        delay(100);
        sdReady = SD.begin(legacyCS, SPI, 4000000);
    }

    M5.Display.fillRect(20, 96, 200, 10, kBgColor);
    M5.Display.setCursor(20, 96);
    if (sdReady) {
        const uint8_t type = SD.cardType();
        const uint64_t sizeMb = SD.cardSize() / (1024ULL * 1024ULL);
        Serial.printf("SD mount OK (type %u, %llu MB)\n", type, sizeMb);
        M5.Display.setTextColor(TFT_GREEN, kBgColor);
        M5.Display.printf("SD: OK (%llu MB)", sizeMb);
    } else {
        Serial.println(F("SD mount failed"));
        M5.Display.setTextColor(TFT_RED, kBgColor);
        M5.Display.println(F("SD: FAIL"));
    }
#else
    Serial.println(F("[6/8] SD check DISABLED (no hang risk)"));
    Serial.println(F("[7/8] To enable: uncomment #define ENABLE_SD_CHECK"));
    Serial.println(F("      WARNING: May hang if SD hardware not present!"));
#endif

    const bool overallOk = wifiOk;
    M5.Display.fillRect(20, 120, 200, 10, kBgColor);
    M5.Display.setCursor(20, 120);
    M5.Display.setTextColor(overallOk ? TFT_GREEN : TFT_RED, kBgColor);
    M5.Display.println(overallOk ? F("System check OK") : F("System check FAILED"));

    M5.Display.setTextColor(kFgColor, kBgColor);

    Serial.println(F("[8/8] Stage complete"));
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
