#include <M5Cardputer.h>
#include <M5Unified.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <cstring>
#include <cstdio>

namespace {
constexpr uint16_t kBgColor = TFT_BLACK;
constexpr uint16_t kFgColor = TFT_GREEN;
constexpr uint16_t kAccentColor = TFT_CYAN;
constexpr uint16_t kMutedColor = TFT_DARKGREY;
constexpr uint16_t kHighlightBg = TFT_DARKGREEN;
constexpr uint16_t kHighlightFg = TFT_WHITE;
constexpr uint32_t kHeartbeatIntervalMs = 1000;
constexpr uint32_t kNekoFrameIntervalMs = 160;
constexpr uint32_t kMessageTimeoutMs = 3000;
constexpr uint32_t kStatusRefreshMs = 1000;
constexpr int kSdCsPin = 12;
constexpr int kSdSckPin = 40;
constexpr int kSdMisoPin = 39;
constexpr int kSdMosiPin = 14;
constexpr int kMenuX = 16;
constexpr int kMenuY = 48;
constexpr int kMenuLineHeight = 14;
constexpr int kMenuWidth = 120;
constexpr int kNekoX = 150;
constexpr int kNekoY = 44;
constexpr int kNekoLineHeight = 12;
constexpr int kNekoLines = 5;
struct MenuItem {
    const char *label;
    const char *description;
};

const MenuItem kMenuItems[] = {
    {"Handshake Capture", "Sniff WPA handshakes"},
    {"Clone + Deauth", "Rogue AP & disconnect"},
    {"Evil Portal", "Captive portal phishing"},
    {"Channel Analyzer", "Live RF spectrum"},
    {"File Manager", "Browse SD storage"},
    {"System Settings", "Themes & config"}
};

constexpr int kMenuCount = sizeof(kMenuItems) / sizeof(kMenuItems[0]);

const char *kNekoFrames[][kNekoLines] = {
    {
        "  /\\_/\\  ",
        " ( o.o ) ",
        "  > ^ <  ",
        "  /   \\  ",
        "         "
    },
    {
        "  /\\_/\\  ",
        " ( o.o ) ",
        "  > ^ <  ",
        "   / \\   ",
        "         "
    },
    {
        "  /\\_/\\  ",
        " ( o.o ) ",
        "  > ^ <  ",
        "  / \\    ",
        "         "
    },
    {
        "  /\\_/\\  ",
        " ( o.o ) ",
        "  > ^ <  ",
        "   /   \\ ",
        "         "
    }
};

constexpr int kNekoFrameCount = sizeof(kNekoFrames) / sizeof(kNekoFrames[0]);

int gSelectedIndex = 0;
bool gWifiOk = false;
int gNetworksFound = 0;
bool gSdOk = false;
uint8_t gSdType = CARD_NONE;
uint64_t gSdSizeMb = 0;
uint64_t gSdUsedMb = 0;
uint32_t gLastHeartbeat = 0;
uint32_t gLastNekoTick = 0;
uint32_t gLastStatusTick = 0;
int gNekoFrame = 0;
bool gMessageActive = false;
uint32_t gMessageStart = 0;
char gMessageBuffer[64] = {0};
constexpr char kDefaultHint[] = "; /. navega  |  ENTER seleciona  |  ESC limpa";

void drawHeader() {
    M5.Display.fillScreen(kBgColor);
    M5.Display.setTextColor(kAccentColor, kBgColor);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(12, 10);
    M5.Display.println(F("M5Gotchi PRO"));

    M5.Display.setTextSize(1);
    M5.Display.setCursor(16, 30);
    M5.Display.setTextColor(kMutedColor, kBgColor);
    M5.Display.println(F("Stage 4: Main Menu"));

    M5.Display.fillRect(12, 38, 216, 1, kAccentColor);
}

void drawNekoFrame() {
    M5.Display.fillRect(kNekoX - 8, kNekoY - 4, 96, kNekoLines * kNekoLineHeight, kBgColor);

    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kFgColor, kBgColor);

    for (int line = 0; line < kNekoLines; ++line) {
        M5.Display.setCursor(kNekoX, kNekoY + (line * kNekoLineHeight));
        M5.Display.print(kNekoFrames[gNekoFrame][line]);
    }
}

void drawMenu() {
    M5.Display.fillRect(kMenuX - 6, kMenuY - 4, kMenuWidth + 12, kMenuLineHeight * kMenuCount + 8, kBgColor);

    for (int i = 0; i < kMenuCount; ++i) {
        const int itemY = kMenuY + (i * kMenuLineHeight);
        if (i == gSelectedIndex) {
            M5.Display.fillRect(kMenuX - 6, itemY - 2, kMenuWidth + 12, kMenuLineHeight, kHighlightBg);
            M5.Display.setTextColor(kHighlightFg, kHighlightBg);
        } else {
            M5.Display.setTextColor(kFgColor, kBgColor);
        }
        M5.Display.setTextSize(1);
        M5.Display.setCursor(kMenuX, itemY);
        M5.Display.print(kMenuItems[i].label);
        if (i == gSelectedIndex) {
            M5.Display.setCursor(kMenuX, itemY + kMenuLineHeight - 6);
            M5.Display.setTextColor(kAccentColor, kHighlightBg);
            M5.Display.print(kMenuItems[i].description);
        }
    }
}

void renderMessage() {
    M5.Display.fillRect(12, 108, 216, 20, kBgColor);
    M5.Display.setCursor(16, 110);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kAccentColor, kBgColor);
    if (gMessageActive) {
        M5.Display.print(gMessageBuffer);
    } else {
        M5.Display.print(kDefaultHint);
    }
}

void showTemporaryMessage(const char *text) {
    strncpy(gMessageBuffer, text, sizeof(gMessageBuffer) - 1);
    gMessageBuffer[sizeof(gMessageBuffer) - 1] = '\0';
    gMessageActive = true;
    gMessageStart = millis();
    renderMessage();
}

void refreshStatus() {
    M5.Display.fillRect(12, 92, 216, 12, kBgColor);
    M5.Display.setCursor(16, 94);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kFgColor, kBgColor);

    char statusLine[128];
    snprintf(statusLine, sizeof(statusLine), "WiFi: %s (%d redes)  |  SD: %s (%llu MB)",
             gWifiOk ? "OK" : "FAIL",
             gNetworksFound,
             gSdOk ? "OK" : "FAIL",
             static_cast<unsigned long long>(gSdSizeMb));
    M5.Display.print(statusLine);
}

void refreshVitals() {
    M5.Display.fillRect(12, 82, 216, 10, kBgColor);
    M5.Display.setCursor(16, 82);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kMutedColor, kBgColor);
    const uint32_t uptimeSeconds = millis() / 1000;
    const uint32_t minutes = uptimeSeconds / 60;
    const uint32_t seconds = uptimeSeconds % 60;
    char vitals[64];
    snprintf(vitals, sizeof(vitals), "Uptime %lu:%02lu  |  Neko frame %d", static_cast<unsigned long>(minutes), static_cast<unsigned long>(seconds), gNekoFrame);
    M5.Display.print(vitals);
}

void advanceSelection(int delta) {
    gSelectedIndex = (gSelectedIndex + delta + kMenuCount) % kMenuCount;
    drawMenu();
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "> %s", kMenuItems[gSelectedIndex].label);
    showTemporaryMessage(buffer);
}

void handleEnter() {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "TODO: %s", kMenuItems[gSelectedIndex].label);
    Serial.println(buffer);
    showTemporaryMessage(buffer);
}

void handleEsc() {
    showTemporaryMessage("ESC: aguardando implementacao");
}

void initPeripherals() {
    Serial.println(F("[A] WiFi scan start"));
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(200);
    gNetworksFound = WiFi.scanNetworks(false, true);
    gWifiOk = gNetworksFound >= 0;
    if (!gWifiOk) {
        gNetworksFound = 0;
        Serial.println(F("WiFi scan failed"));
    } else {
        Serial.printf("WiFi networks found: %d\n", gNetworksFound);
    }
    WiFi.scanDelete();

    Serial.println(F("[B] SD init"));
    SPI.begin(kSdSckPin, kSdMisoPin, kSdMosiPin, kSdCsPin);
    gSdOk = SD.begin(kSdCsPin, SPI, 25000000);
    if (gSdOk) {
        gSdType = SD.cardType();
        gSdSizeMb = SD.cardSize() / (1024ULL * 1024ULL);
#if defined(ESP32)
        gSdUsedMb = SD.usedBytes() / (1024ULL * 1024ULL);
#endif
        Serial.printf("SD OK type=%u size=%lluMB used=%lluMB\n", gSdType, static_cast<unsigned long long>(gSdSizeMb), static_cast<unsigned long long>(gSdUsedMb));
    } else {
        gSdType = CARD_NONE;
        gSdSizeMb = 0;
        gSdUsedMb = 0;
        Serial.println(F("SD mount failed"));
    }
}
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println(F("========================"));
    Serial.println(F("Stage 4: Main Menu"));
    Serial.println(F("========================"));

    auto cfg = M5.config();
    M5.begin(cfg);
    M5Cardputer.begin();

    M5.Display.setRotation(1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kFgColor, kBgColor);

    drawHeader();
    drawMenu();
    drawNekoFrame();
    renderMessage();
    refreshVitals();
    refreshStatus();

    initPeripherals();
    refreshStatus();
    showTemporaryMessage("> Handshake Capture");

    gLastHeartbeat = millis();
    gLastNekoTick = millis();
    gLastStatusTick = millis();
}

void loop() {
    M5.update();

    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        for (auto key : status.word) {
            if (key == ';') {
                advanceSelection(-1);
            } else if (key == '.') {
                advanceSelection(1);
            } else if (key == 0x1B) {
                handleEsc();
            }
        }
        if (status.enter) {
            handleEnter();
        }
    }

    const uint32_t now = millis();

    if (now - gLastNekoTick >= kNekoFrameIntervalMs) {
        gNekoFrame = (gNekoFrame + 1) % kNekoFrameCount;
        drawNekoFrame();
        gLastNekoTick = now;
    }

    if (now - gLastStatusTick >= kStatusRefreshMs) {
        refreshVitals();
        refreshStatus();
        gLastStatusTick = now;
    }

    if (gMessageActive && (now - gMessageStart >= kMessageTimeoutMs)) {
        gMessageActive = false;
        renderMessage();
    }

    if (now - gLastHeartbeat >= kHeartbeatIntervalMs) {
        Serial.println(F("Heartbeat: menu alive"));
        gLastHeartbeat = now;
    }

    delay(10);
}
