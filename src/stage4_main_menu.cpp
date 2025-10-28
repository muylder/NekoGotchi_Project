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
constexpr uint16_t kMenuPanelBg = TFT_DARKGREY;
constexpr uint32_t kHeartbeatIntervalMs = 1000;
constexpr uint32_t kNekoFrameIntervalMs = 160;
constexpr uint32_t kMessageTimeoutMs = 3000;
constexpr uint32_t kStatusRefreshMs = 1000;
constexpr int kSdCsPin = 12;
constexpr int kSdSckPin = 40;
constexpr int kSdMisoPin = 39;
constexpr int kSdMosiPin = 14;
constexpr int kMenuPanelX = 12;
constexpr int kMenuPanelY = 40;
constexpr int kMenuPanelPadding = 3;
constexpr int kMenuLineHeight = 11;
constexpr int kMenuPanelWidth = 140;
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
bool gMenuVisible = false;
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
constexpr char kDefaultHint[] = "ESC abre menu  |  ; /. navega  |  ENTER executa";

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

void clearMenuPanel() {
    const int panelHeight = (kMenuPanelPadding * 2) + (kMenuCount * kMenuLineHeight);
    M5.Display.fillRect(kMenuPanelX - 2, kMenuPanelY - 2, kMenuPanelWidth + 4, panelHeight + 4, kBgColor);
}

void drawMenuPanel() {
    const int panelHeight = (kMenuPanelPadding * 2) + (kMenuCount * kMenuLineHeight);
    M5.Display.fillRoundRect(kMenuPanelX, kMenuPanelY, kMenuPanelWidth, panelHeight, 6, kMenuPanelBg);
    M5.Display.drawRoundRect(kMenuPanelX, kMenuPanelY, kMenuPanelWidth, panelHeight, 6, kAccentColor);

    for (int i = 0; i < kMenuCount; ++i) {
        const int itemY = kMenuPanelY + kMenuPanelPadding + (i * kMenuLineHeight);
        M5.Display.setTextSize(1);
        if (i == gSelectedIndex) {
            M5.Display.fillRoundRect(kMenuPanelX + 2, itemY - 1, kMenuPanelWidth - 4, kMenuLineHeight, 4, kHighlightBg);
            M5.Display.setTextColor(kHighlightFg, kHighlightBg);
            M5.Display.setCursor(kMenuPanelX + 6, itemY + 1);
            M5.Display.print("> ");
            M5.Display.print(kMenuItems[i].label);
        } else {
            M5.Display.setTextColor(kFgColor, kMenuPanelBg);
            M5.Display.setCursor(kMenuPanelX + 6, itemY + 1);
            M5.Display.print("  ");
            M5.Display.print(kMenuItems[i].label);
        }
    }
}

void renderMessage() {
    M5.Display.fillRect(12, 116, 216, 16, kBgColor);
    M5.Display.setCursor(16, 118);
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
    if (!gMenuVisible) {
        return;
    }
    gSelectedIndex = (gSelectedIndex + delta + kMenuCount) % kMenuCount;
    drawMenuPanel();
    showTemporaryMessage(kMenuItems[gSelectedIndex].description);
}

void handleEnter() {
    if (!gMenuVisible) {
        showTemporaryMessage("Abra o menu com ESC");
        return;
    }
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "TODO: %s", kMenuItems[gSelectedIndex].label);
    Serial.println(buffer);
    showTemporaryMessage(buffer);
}

void handleEsc() {
    if (gMenuVisible) {
        clearMenuPanel();
        gMenuVisible = false;
        drawNekoFrame();
        gMessageActive = false;
        refreshVitals();
        refreshStatus();
        renderMessage();
        showTemporaryMessage("Menu fechado");
    } else {
        gMenuVisible = true;
        drawMenuPanel();
        showTemporaryMessage(kMenuItems[gSelectedIndex].description);
    }
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
    drawNekoFrame();
    renderMessage();
    refreshVitals();
    refreshStatus();

    initPeripherals();
    refreshStatus();
    showTemporaryMessage("ESC abre menu principal");

    gLastHeartbeat = millis();
    gLastNekoTick = millis();
    gLastStatusTick = millis();
}

void loop() {
    M5.update();

    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        bool upRequested = false;
        bool downRequested = false;
        bool escRequested = false;

        for (auto key : status.word) {
            if (key == ';' || key == 'w' || key == 'W') {
                upRequested = true;
            } else if (key == '.' || key == 's' || key == 'S') {
                downRequested = true;
            } else if (key == '`' || key == 0x1B) {
                escRequested = true;
            }
        }

        if (escRequested) {
            handleEsc();
        }
        if (upRequested) {
            advanceSelection(-1);
        }
        if (downRequested) {
            advanceSelection(1);
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
        if (!gMenuVisible) {
            refreshVitals();
            refreshStatus();
        }
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
