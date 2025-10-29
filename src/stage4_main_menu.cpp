#include <M5Cardputer.h>
#include <M5Unified.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>

#if __has_include(<NimBLEDevice.h>)
#include "bluetooth_attacks.h"
#define HAS_BLE_ATTACKS 1
#else
#define HAS_BLE_ATTACKS 0
enum BLEAttackType {
    BLE_ATTACK_SOUR_APPLE,
    BLE_ATTACK_SAMSUNG,
    BLE_ATTACK_GOOGLE,
    BLE_ATTACK_MICROSOFT,
    BLE_ATTACK_ALL
};

class BluetoothAttacks {
public:
    bool begin() { return false; }
    void end() {}
    void startSourApple() {}
    void startSamsungSpam() {}
    void startGoogleSpam() {}
    void startMicrosoftSpam() {}
    void startSpamAll() {}
    void stop() {}
    bool isRunning() const { return false; }
    uint32_t getPacketsSent() const { return 0; }
    BLEAttackType getCurrentAttack() const { return BLE_ATTACK_SOUR_APPLE; }
    void executeSourApple() {}
    void executeSamsungSpam() {}
    void executeGoogleSpam() {}
    void executeMicrosoftSpam() {}
    void executeRotateAll() {}
};
#endif

#include "m5gotchi_achievement_manager.h"
#include "m5gotchi_tutorial_system.h"
#include "m5gotchi_neko_virtual_pet.h"
#include "m5gotchi_neko_sounds.h"
#include "stage5_dashboard.h"

namespace stage4 {
constexpr uint16_t kBgColor = TFT_BLACK;
constexpr uint16_t kCardBg = 0x2108;       // Dark panel background
constexpr uint16_t kCardText = 0xC618;     // Soft white/grey
constexpr uint16_t kMenuBg = 0x294A;
constexpr uint16_t kMenuHighlight = TFT_DARKGREEN;
constexpr uint16_t kMutedColor = TFT_DARKGREY;
constexpr uint32_t kHeartbeatIntervalMs = 1400;
constexpr uint32_t kNekoFrameIntervalMs = 160;
constexpr uint32_t kMessageTimeoutMs = 4500;
constexpr uint32_t kStatusRefreshMs = 2200;
constexpr uint32_t kAnalyzerRefreshMs = 12000;
constexpr uint32_t kBluetoothTickMs = 60;
constexpr uint32_t kPetRedrawMs = 1500;
constexpr int kSdCsPin = 12;
constexpr int kSdSckPin = 40;
constexpr int kSdMisoPin = 39;
constexpr int kSdMosiPin = 14;

constexpr int kHeaderHeight = 28;
constexpr int kStatusX = 12;
constexpr int kStatusY = 36;
constexpr int kStatusW = 120;
constexpr int kStatusH = 52;
constexpr int kNekoX = 148;
constexpr int kNekoY = 36;
constexpr int kNekoW = 80;
constexpr int kNekoH = 58;
constexpr int kModeCardX = 12;
constexpr int kModeCardY = 90;
constexpr int kModeCardW = 216;
constexpr int kModeCardH = 38;
constexpr int kFooterY = 126;
constexpr int kNekoLines = 5;
constexpr int kNekoLineHeight = 8;
constexpr size_t kFileWindow = 3;
constexpr int kMenuVisibleRows = 6;
constexpr char kDefaultMessage[] = "ESC abre menu  |  ENTER executa";
constexpr int kPanelPadding = 8;
constexpr int kPanelTitleOffset = 18;

struct FrameSet {
    const char *lines[kNekoLines];
};

constexpr FrameSet kHandshakeFrames[] = {
    {"  /\\_/\\  ",
     " ( •_• ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " ~SCAN~ "},
    {"  /\\_/\\  ",
     " ( •_• ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " ~HASH~ "},
    {"  /\\_/\\  ",
     " ( •_• ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " ~SAVE~ "}
};

constexpr FrameSet kCloneFrames[] = {
    {"  /\\_/\\  ",
     " ( °o° ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  CLONE  "},
    {"  /\\_/\\  ",
     " ( o°o ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " DEAUTH  "}
};

constexpr FrameSet kPortalFrames[] = {
    {"  /\\_/\\  ",
     " ( ^_^ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " PORTAL  "},
    {"  /\\_/\\  ",
     " ( ^_^ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  PHISH  "}
};

constexpr FrameSet kAnalyzerFrames[] = {
    {"  /\\_/\\  ",
     " ( @.@ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " ANALYZE "},
    {"  /\\_/\\  ",
     " ( o@o ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " SPECTR  "}
};

constexpr FrameSet kFileFrames[] = {
    {"  /\\_/\\  ",
     " ( •‿• ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  FILES  "},
    {"  /\\_/\\  ",
     " ( •‿• ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  BROWSE "}
};

constexpr FrameSet kSettingsFrames[] = {
    {"  /\\_/\\  ",
     " ( o.o ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " SETUP  "},
    {"  /\\_/\\  ",
     " ( o.o ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " TWEAKS "}
};

constexpr FrameSet kDashboardFrames[] = {
    {"  /\_/\  ",
     " ( ^ω^ ) ",
     "  /|_|\  ",
     "  /   \  ",
     " DASH  "},
    {"  /\_/\  ",
     " ( >w< ) ",
     "  /|_|\  ",
     "  /   \  ",
     " STAGE5"}
};

constexpr FrameSet kBluetoothFrames[] = {
    {"  /\\_/\\  ",
     " ( ^.^ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  BLE!! "},
    {"  /\\_/\\  ",
     " ( ^_^ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  SPAM  "}
};

constexpr FrameSet kAchievementFrames[] = {
    {"  /\\_/\\  ",
     " ( *.* ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  XP++  "},
    {"  /\\_/\\  ",
     " ( ^o^ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  WIN!  "}
};

constexpr FrameSet kTutorialFrames[] = {
    {"  /\\_/\\  ",
     " ( ?_? ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  HELP  "},
    {"  /\\_/\\  ",
     " ( ^_^ ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     " GUIDE  "}
};

constexpr FrameSet kPetFrames[] = {
    {"  /\\_/\\  ",
     " ( =.= ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  NEKO  "},
    {"  /\\_/\\  ",
     " ( >.< ) ",
     "  /|_|\\  ",
     "  /   \\  ",
     "  PETME "}
};

enum class ModeType {
    kHandshake = 0,
    kClone,
    kEvilPortal,
    kAnalyzer,
    kFileManager,
    kSettings,
    kBluetooth,
    kAchievements,
    kDashboard,
    kTutorial,
    kVirtualPet
};

struct ModeInfo {
    const char *label;
    const char *summary;
    const char *hint;
    const FrameSet *frames;
    int frameCount;
    uint16_t accent;
    ModeType type;
    bool fullScreen;
};

constexpr ModeInfo kModes[] = {
    {"Handshake Capture",
     "Captura simulada com estagio 5 pronto",
     "SPACE reinicia simulação; concluindo libera conquista.",
     kHandshakeFrames,
     static_cast<int>(sizeof(kHandshakeFrames) / sizeof(FrameSet)),
     TFT_GREEN,
     ModeType::kHandshake,
     false},
    {"Clone + Deauth",
     "Clone AP e envia deauth temporario",
     "SPACE inicia/para flood deauth.",
     kCloneFrames,
     static_cast<int>(sizeof(kCloneFrames) / sizeof(FrameSet)),
     TFT_YELLOW,
     ModeType::kClone,
     false},
    {"Evil Portal",
     "Hotspot cativo com templates do SD",
     "SPACE liga portal simulado para testes.",
     kPortalFrames,
     static_cast<int>(sizeof(kPortalFrames) / sizeof(FrameSet)),
     TFT_CYAN,
     ModeType::kEvilPortal,
     false},
    {"Channel Analyzer",
     "Resumo de canais mais limpos",
     "Scanner automatico a cada 12 segundos.",
     kAnalyzerFrames,
     static_cast<int>(sizeof(kAnalyzerFrames) / sizeof(FrameSet)),
     TFT_BLUE,
     ModeType::kAnalyzer,
     false},
    {"File Manager",
     "Explore microSD (setas [] e O/B)",
     "[ ] navega, O abre, B sobe diretório.",
     kFileFrames,
     static_cast<int>(sizeof(kFileFrames) / sizeof(FrameSet)),
     TFT_ORANGE,
     ModeType::kFileManager,
     false},
    {"System Settings",
     "Idiomas, temas e atalhos",
     "Use futuro Stage 5 para ajustes avançados.",
     kSettingsFrames,
     static_cast<int>(sizeof(kSettingsFrames) / sizeof(FrameSet)),
     TFT_PURPLE,
     ModeType::kSettings,
     false},
    {"Bluetooth Spam",
     "Ataques SourApple / FastPair",
     "SPACE toggle, [ ] troca ataque.",
    kBluetoothFrames,
    static_cast<int>(sizeof(kBluetoothFrames) / sizeof(FrameSet)),
    0x4EDD,
     ModeType::kBluetooth,
     false},
    {"Achievements",
     "Progresso de conquistas hacker",
     "Eventos registram automaticamente conquistas.",
    kAchievementFrames,
    static_cast<int>(sizeof(kAchievementFrames) / sizeof(FrameSet)),
    0xFCA0,
     ModeType::kAchievements,
     false},
    {"Dashboard",
     "Painel central com missoes e pet",
     "W/S missoes, A/D macros, SPACE executa.",
     kDashboardFrames,
     static_cast<int>(sizeof(kDashboardFrames) / sizeof(FrameSet)),
     TFT_MAGENTA,
     ModeType::kDashboard,
     true},
    {"Tutorial Wizard",
     "Onboarding interativo",
     "ENTER abre wizard, N avança, B volta.",
     kTutorialFrames,
     static_cast<int>(sizeof(kTutorialFrames) / sizeof(FrameSet)),
     TFT_CYAN,
     ModeType::kTutorial,
     true},
    {"Neko Companion",
     "Cuide do pet hacker em tempo real",
     "F alimenta, P faz carinho, ENTER tela cheia.",
    kPetFrames,
    static_cast<int>(sizeof(kPetFrames) / sizeof(FrameSet)),
    0xFD5F,
     ModeType::kVirtualPet,
     true}
};

constexpr int kModeCount = static_cast<int>(sizeof(kModes) / sizeof(kModes[0]));

struct FileEntry {
    String name;
    bool isDir;
    uint32_t size;
};

struct KeyFlags {
    bool up = false;
    bool down = false;
    bool esc = false;
    bool space = false;
    bool feed = false;
    bool pet = false;
    bool cycleLeft = false;
    bool cycleRight = false;
    bool open = false;
    bool back = false;
    bool next = false;
    bool prev = false;
    bool skip = false;
    bool help = false;
    bool enter = false;
};

constexpr uint8_t kHidRightArrow = 0x4F;
constexpr uint8_t kHidLeftArrow  = 0x50;
constexpr uint8_t kHidDownArrow  = 0x51;
constexpr uint8_t kHidUpArrow    = 0x52;

int gSelectedIndex = 0;
int gActiveMode = 0;
bool gMenuVisible = false;
int gMenuScroll = 0;

bool gWifiOk = false;
int gNetworksFound = 0;
bool gSdOk = false;
uint64_t gSdSizeMb = 0;
uint64_t gSdUsedMb = 0;

uint32_t gModeStartedAt = 0;
uint8_t gNekoFrame = 0;
uint32_t gLastNekoTick = 0;
uint32_t gLastStatusTick = 0;
uint32_t gLastHeartbeat = 0;
bool gMessageActive = false;
uint32_t gMessageStart = 0;
char gMessageBuffer[120] = {0};

uint8_t gHandshakeProgress = 0;
bool gHandshakeCompleted = false;
uint32_t gHandshakeTick = 0;

const FrameSet *gActiveFrames = kHandshakeFrames;
int gActiveFrameCount = static_cast<int>(sizeof(kHandshakeFrames) / sizeof(FrameSet));

// Feature controllers
BluetoothAttacks gBleAttacks;
bool gBleReady = false;
bool gBleActive = false;
BLEAttackType gBleType = BLE_ATTACK_SOUR_APPLE;
uint32_t gBlePackets = 0;
uint32_t gBleLastTick = 0;

AchievementManager gAchievementManager;
bool gAchievementsReady = false;

TutorialSystem gTutorial;
bool gTutorialReady = false;

M5GotchiNekoSounds gNekoSounds;
M5GotchiNekoVirtualPet gNekoPet;
bool gNekoReady = false;
uint32_t gLastPetRedraw = 0;

std::vector<FileEntry> gFileEntries;
String gCurrentPath = "/";
int gFileSelection = 0;
int gFileScroll = 0;

bool gDeauthActive = false;
uint32_t gDeauthPackets = 0;
uint32_t gDeauthTick = 0;

bool gPortalActive = false;
uint32_t gPortalClients = 0;
uint32_t gPortalTick = 0;

int gChannelUsage[14] = {0};
int gBestChannel = 1;
uint32_t gLastChannelScan = 0;

const ModeInfo &activeMode() {
    return kModes[gActiveMode];
}

bool modeUsesFullScreen() {
    return activeMode().fullScreen;
}

void pumpAchievementNotifications();
void renderActiveMode(bool initial = true);
void drawFullScreenMode(bool initial);
void drawPanel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t accent, const char *title = nullptr);
String ellipsize(const String &text, uint8_t maxChars);
void drawHeader();
void drawStatusCard();
void drawNekoPanel();
void drawNekoFrame(bool forceClear = false);
void drawModeCard();
void drawFooter();
void drawLayout();
void drawMenuOverlay();
void hideMenuOverlay();
void showMessage(const char *text);
void advanceSelection(int delta);
void ensureMenuScroll();
void handleEnter();
void handleEsc();
void initPeripherals();
void applyMode(int index);
void updateHandshakeSimulation(uint32_t now);
void updateChannelAnalyzer(uint32_t now);
void updateDeauthSimulation(uint32_t now);
void updatePortalSimulation(uint32_t now);
void tickBluetooth(uint32_t now);
void refreshFileListing(const String &path);
void moveFileSelection(int delta);
void openSelectedFile();
void goFileParent();
String formatSize(uint32_t bytes);
String parentPath(const String &path);
void processModeShortcuts(const KeyFlags &flags);
void toggleDeauth();
void togglePortal();
void toggleBluetooth();
void changeBluetoothAttack(int delta);
void triggerAchievementEvent(AchievementEvent ev, uint16_t value = 1);
const char *bleAttackName(BLEAttackType type);
int wifiStrengthPercent();

void drawPanel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t accent, const char *title) {
    M5.Display.fillRoundRect(x, y, w, h, 6, kCardBg);
    M5.Display.drawRoundRect(x, y, w, h, 6, accent);
    if (title) {
        M5.Display.setTextColor(accent, kCardBg);
        M5.Display.setCursor(x + kPanelPadding, y + 6);
        M5.Display.print(title);
        int lineX = x + kPanelPadding;
        int lineW = w - (kPanelPadding * 2);
        if (lineW > 0) {
            M5.Display.drawFastHLine(lineX, y + 16, lineW, accent);
        }
        M5.Display.setTextColor(kCardText, kCardBg);
    }
}

String ellipsize(const String &text, uint8_t maxChars) {
    if (maxChars == 0) {
        return "";
    }
    if (text.length() <= maxChars) {
        return text;
    }
    if (maxChars < 3) {
        return text.substring(0, maxChars);
    }
    return text.substring(0, maxChars - 3) + "...";
}

void drawHeader() {
    const uint16_t accent = activeMode().accent;
    const String petName = gNekoReady ? gNekoPet.getPetName() : String("Neko");
    M5.Display.fillRect(0, 0, 240, kHeaderHeight, kBgColor);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(12, 5);
    M5.Display.setTextColor(accent, kBgColor);
    M5.Display.print(ellipsize(petName, 12));
    M5.Display.setTextSize(1);
    M5.Display.setCursor(12, 20);
    M5.Display.setTextColor(kMutedColor, kBgColor);
    M5.Display.print(activeMode().label);
}

void drawStatusCard() {
    if (modeUsesFullScreen()) {
        return;
    }
    const uint16_t accent = activeMode().accent;
    drawPanel(kStatusX, kStatusY, kStatusW, kStatusH, accent, "Status");

    const int contentX = kStatusX + kPanelPadding;
    int lineY = kStatusY + kPanelTitleOffset;

    char wifiLine[48];
    if (gWifiOk) {
        std::snprintf(wifiLine, sizeof(wifiLine), "WiFi: OK (%d)", gNetworksFound);
    } else {
        std::snprintf(wifiLine, sizeof(wifiLine), "WiFi: OFF");
    }
    M5.Display.setCursor(contentX, lineY);
    M5.Display.setTextColor(kCardText, kCardBg);
    M5.Display.print(wifiLine);

    char sdLine[48];
    if (gSdOk) {
        std::snprintf(sdLine, sizeof(sdLine), "SD: OK (%llu MB)",
                      static_cast<unsigned long long>(gSdSizeMb));
    } else {
        std::snprintf(sdLine, sizeof(sdLine), "SD: OFF");
    }
    lineY += 12;
    M5.Display.setCursor(contentX, lineY);
    M5.Display.print(sdLine);

    lineY += 12;
    uint32_t uptimeSeconds = (millis() - gModeStartedAt) / 1000;
    uint32_t minutes = uptimeSeconds / 60;
    uint32_t seconds = uptimeSeconds % 60;
    char timeLine[48];
    std::snprintf(timeLine, sizeof(timeLine), "Uptime: %lu:%02lu",
                  static_cast<unsigned long>(minutes),
                  static_cast<unsigned long>(seconds));
    M5.Display.setCursor(contentX, lineY);
    M5.Display.setTextColor(kMutedColor, kCardBg);
    M5.Display.print(timeLine);
    M5.Display.setTextColor(kCardText, kCardBg);

    stage5::syncWifiStrength(wifiStrengthPercent());
    if (gNekoReady) {
        stage5::syncPetStats(gNekoPet.getEnergy(),
                             gNekoPet.getHappiness(),
                             static_cast<int>(gNekoPet.getCurrentMood()));
    }
}

void drawNekoFrame(bool forceClear) {
    if (modeUsesFullScreen()) {
        return;
    }
    (void)forceClear;
    const int innerX = kNekoX + kPanelPadding;
    const int innerY = kNekoY + kPanelTitleOffset;
    const int innerW = kNekoW - (kPanelPadding * 2);
    const int innerH = kNekoH - kPanelTitleOffset - kPanelPadding;
    M5.Display.fillRect(innerX, innerY, innerW, innerH, kCardBg);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kCardText, kCardBg);

    if (gActiveFrameCount <= 0 || innerH <= 0) {
        return;
    }

    const FrameSet &frame = gActiveFrames[gNekoFrame % gActiveFrameCount];
    const int maxLines = std::min(kNekoLines, innerH / kNekoLineHeight);
    for (int line = 0; line < maxLines; ++line) {
        M5.Display.setCursor(innerX + 2, innerY + (line * kNekoLineHeight));
        M5.Display.print(frame.lines[line]);
    }
}

void drawNekoPanel() {
    if (modeUsesFullScreen()) {
        return;
    }
    const uint16_t accent = activeMode().accent;
    drawPanel(kNekoX, kNekoY, kNekoW, kNekoH, accent, "Companion");
    drawNekoFrame(true);
}

void drawBluetoothStatus();
void drawAchievementsStatus();
void drawFileManagerStatus();
void drawAnalyzerStatus();
void drawPortalStatus();
void drawCloneStatus();
void drawPetStatusCard();
void drawTutorialStatusCard();

void drawModeCard() {
    if (modeUsesFullScreen()) {
        return;
    }
    const ModeInfo &mode = activeMode();
    drawPanel(kModeCardX, kModeCardY, kModeCardW, kModeCardH, mode.accent, mode.label);
    M5.Display.setTextSize(1);
    const int contentX = kModeCardX + kPanelPadding;
    int lineY = kModeCardY + kPanelTitleOffset;
    M5.Display.setTextColor(kCardText, kCardBg);

    switch (mode.type) {
        case ModeType::kHandshake: {
            M5.Display.setCursor(contentX, lineY);
            M5.Display.printf("Progresso: %3u%%", gHandshakeProgress);
            lineY += 12;
            uint32_t elapsed = (millis() - gModeStartedAt) / 1000;
            uint32_t minutes = elapsed / 60;
            uint32_t seconds = elapsed % 60;
            M5.Display.setTextColor(kMutedColor, kCardBg);
            M5.Display.setCursor(contentX, lineY);
            M5.Display.printf("Tempo: %lu:%02lu", static_cast<unsigned long>(minutes),
                              static_cast<unsigned long>(seconds));
            if (gHandshakeCompleted) {
                lineY += 12;
                M5.Display.setTextColor(kCardText, kCardBg);
                M5.Display.setCursor(contentX, lineY);
                M5.Display.print("Status: concluido");
            }
            M5.Display.setTextColor(kCardText, kCardBg);
            break;
        }
        case ModeType::kClone:
            drawCloneStatus();
            break;
        case ModeType::kEvilPortal:
            drawPortalStatus();
            break;
        case ModeType::kAnalyzer:
            drawAnalyzerStatus();
            break;
        case ModeType::kFileManager:
            drawFileManagerStatus();
            break;
        case ModeType::kSettings: {
            M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
            M5.Display.print("Temas: Dark/Neon/Kawaii");
            M5.Display.setCursor(kModeCardX + 10, kModeCardY + 32);
            M5.Display.print("Use Stage 5 para ajustes completos.");
            break;
        }
        case ModeType::kBluetooth:
            drawBluetoothStatus();
            break;
        case ModeType::kAchievements:
            drawAchievementsStatus();
            break;
        case ModeType::kTutorial:
            drawTutorialStatusCard();
            break;
        case ModeType::kVirtualPet:
            drawPetStatusCard();
            break;
    }
}

void drawFooter() {
    if (modeUsesFullScreen()) {
        return;
    }
    M5.Display.fillRect(0, kFooterY, 240, 135 - kFooterY, kBgColor);
    M5.Display.setCursor(12, kFooterY + 4);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kMutedColor, kBgColor);
    if (gMessageActive) {
        M5.Display.print(gMessageBuffer);
    } else {
        M5.Display.print(kDefaultMessage);
    }
}

void drawLayout() {
    if (modeUsesFullScreen()) {
        drawFullScreenMode(true);
        return;
    }
    M5.Display.fillScreen(kBgColor);
    drawHeader();
    drawStatusCard();
    drawNekoPanel();
    drawModeCard();
    drawFooter();
}

void showMessage(const char *text) {
    if (!text) {
        return;
    }
    std::strncpy(gMessageBuffer, text, sizeof(gMessageBuffer) - 1);
    gMessageBuffer[sizeof(gMessageBuffer) - 1] = '\0';
    gMessageActive = true;
    gMessageStart = millis();
    if (!modeUsesFullScreen()) {
        drawFooter();
    }
}

void ensureMenuScroll() {
    const int maxStart = std::max(0, kModeCount - kMenuVisibleRows);
    if (gSelectedIndex < gMenuScroll) {
        gMenuScroll = gSelectedIndex;
    } else if (gSelectedIndex >= gMenuScroll + kMenuVisibleRows) {
        gMenuScroll = gSelectedIndex - kMenuVisibleRows + 1;
    }
    gMenuScroll = std::max(0, std::min(gMenuScroll, maxStart));
}

void drawMenuOverlay() {
    gMenuVisible = true;
    if (activeMode().type == ModeType::kDashboard) {
        stage5::setMenuOverlay(true);
    }
    if (!modeUsesFullScreen()) {
        drawLayout();
    }
    ensureMenuScroll();
    const int panelX = 20;
    const int panelY = 32;
    const int panelW = 200;
    const int rowHeight = 12;
    const int panelH = (kMenuVisibleRows * rowHeight) + (kPanelPadding * 2) + 6;
    const uint16_t accent = activeMode().accent;
    const String petName = gNekoReady ? gNekoPet.getPetName() : String("Neko");
    drawPanel(panelX, panelY, panelW, panelH, accent, petName.c_str());
    M5.Display.setTextSize(1);

    for (int i = 0; i < kMenuVisibleRows; ++i) {
        const int modeIndex = gMenuScroll + i;
        if (modeIndex >= kModeCount) {
            break;
        }
        const int itemY = panelY + kPanelTitleOffset + (i * rowHeight);
        const bool selected = modeIndex == gSelectedIndex;
        if (selected) {
            M5.Display.fillRoundRect(panelX + kPanelPadding,
                                     itemY - 2,
                                     panelW - (kPanelPadding * 2),
                                     rowHeight,
                                     3,
                                     kMenuHighlight);
            M5.Display.setTextColor(TFT_WHITE, kMenuHighlight);
        } else {
            M5.Display.setTextColor(kCardText, kCardBg);
        }
        M5.Display.setCursor(panelX + kPanelPadding + 4, itemY + 7);
        M5.Display.print(kModes[modeIndex].label);
    }

    M5.Display.setTextColor(kMutedColor, kCardBg);
    if (gMenuScroll > 0) {
        M5.Display.setCursor(panelX + panelW - 16, panelY + kPanelPadding + 2);
        M5.Display.print("^");
    }
    if (gMenuScroll + kMenuVisibleRows < kModeCount) {
        M5.Display.setCursor(panelX + panelW - 16,
                             panelY + panelH - kPanelPadding - 8);
        M5.Display.print("v");
    }
}

void hideMenuOverlay() {
    gMenuVisible = false;
    if (activeMode().type == ModeType::kDashboard) {
        stage5::setMenuOverlay(false);
    }
    renderActiveMode(true);
}

void advanceSelection(int delta) {
    if (!gMenuVisible) {
        return;
    }
    gSelectedIndex = (gSelectedIndex + delta + kModeCount) % kModeCount;
    drawMenuOverlay();
    showMessage(kModes[gSelectedIndex].hint);
}

void handleEnter() {
    if (gMenuVisible) {
        gMenuVisible = false;
        applyMode(gSelectedIndex);
        showMessage(kModes[gActiveMode].hint);
    } else if (modeUsesFullScreen()) {
        drawFullScreenMode(true);
    } else {
        showMessage("Pressione ESC para abrir o menu principal.");
    }
}

void handleEsc() {
    if (gMenuVisible) {
        hideMenuOverlay();
        showMessage("Menu fechado.");
    } else {
        gSelectedIndex = gActiveMode;
        drawMenuOverlay();
        showMessage("Use ;/. ou W/S para navegar.");
    }
}

void initPeripherals() {
    Serial.println(F("[A] Varredura WiFi"));
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(200);
    int networks = WiFi.scanNetworks(false, true);
    gWifiOk = networks >= 0;
    gNetworksFound = gWifiOk ? networks : 0;
    if (gWifiOk) {
        Serial.printf("WiFi: %d redes encontradas\n", gNetworksFound);
    } else {
        Serial.println(F("WiFi scan falhou"));
    }
    WiFi.scanDelete();

    Serial.println(F("[B] Inicializando SD"));
    SPI.begin(kSdSckPin, kSdMisoPin, kSdMosiPin, kSdCsPin);
    gSdOk = SD.begin(kSdCsPin, SPI, 25000000);
    if (gSdOk) {
        gSdSizeMb = SD.cardSize() / (1024ULL * 1024ULL);
#if defined(ESP32)
        gSdUsedMb = SD.usedBytes() / (1024ULL * 1024ULL);
#endif
        Serial.printf("SD OK: %llu MB\n",
                      static_cast<unsigned long long>(gSdSizeMb));
        refreshFileListing(gCurrentPath);
    } else {
        gSdSizeMb = 0;
        gSdUsedMb = 0;
        Serial.println(F("SD nao montado"));
    }

    gBleReady = gBleAttacks.begin();
    gAchievementsReady = gAchievementManager.init();
    gTutorialReady = gTutorial.init();
    gTutorial.setContext(CONTEXT_MAIN_MENU);
    gNekoSounds.init();
    gNekoPet.setSoundSystem(&gNekoSounds);
    gNekoPet.init();
    gNekoReady = true;

    stage5::init();
    if (gNekoReady) {
        stage5::syncPetStats(gNekoPet.getEnergy(),
                             gNekoPet.getHappiness(),
                             static_cast<int>(gNekoPet.getCurrentMood()));
        stage5::setMissionStatus(0, "Monitorando handshakes");
        stage5::setMissionStatus(1, "Portal aguardando");
        stage5::setMissionStatus(2, "Deauth/BLE standby");
    }
    stage5::syncWifiStrength(wifiStrengthPercent());
}

void renderActiveMode(bool initial) {
    if (modeUsesFullScreen()) {
        drawFullScreenMode(initial);
    } else {
        if (initial) {
            drawLayout();
        } else {
            drawModeCard();
            drawFooter();
        }
    }
}

void applyMode(int index) {
    if (index < 0 || index >= kModeCount) {
        return;
    }
    ModeType previousType = activeMode().type;
    gActiveMode = index;
    gSelectedIndex = index;
    gModeStartedAt = millis();
    gActiveFrames = kModes[index].frames;
    gActiveFrameCount = kModes[index].frameCount;
    gNekoFrame = 0;

    if (activeMode().type == ModeType::kHandshake) {
        gHandshakeProgress = 0;
        gHandshakeCompleted = false;
        gHandshakeTick = millis();
        stage5::setMissionProgress(0, 0);
        stage5::setMissionStatus(0, "Monitorando handshakes");
    }

    if (activeMode().type == ModeType::kFileManager && gSdOk) {
        refreshFileListing(gCurrentPath);
    }

    if (activeMode().type == ModeType::kTutorial && gTutorialReady && !gTutorial.isTutorialActive()) {
        gTutorial.startWizard();
    }

    if (previousType == ModeType::kDashboard && activeMode().type != ModeType::kDashboard) {
        stage5::exit();
    }

    renderActiveMode(true);
}

void triggerAchievementEvent(AchievementEvent ev, uint16_t value) {
    if (!gAchievementsReady) {
        return;
    }
    gAchievementManager.onEvent(ev, value);
    pumpAchievementNotifications();
}

void updateHandshakeSimulation(uint32_t now) {
    if (activeMode().type != ModeType::kHandshake) {
        return;
    }
    if (now - gHandshakeTick < 180) {
        return;
    }
    gHandshakeTick = now;
    if (gHandshakeProgress < 100) {
        gHandshakeProgress = static_cast<uint8_t>(gHandshakeProgress + 2);
        if (gHandshakeProgress > 100) {
            gHandshakeProgress = 100;
        }
        stage5::setMissionProgress(0, gHandshakeProgress);
        if (gHandshakeProgress < 100) {
            stage5::setMissionStatus(0, String("Capturando handshakes: ") + gHandshakeProgress + "%");
        }
        if (!modeUsesFullScreen() && !gMenuVisible) {
            drawModeCard();
        }
        if (gHandshakeProgress == 100 && !gHandshakeCompleted) {
            gHandshakeCompleted = true;
            showMessage("Simulacao completa! Stage 5 ativa captura real.");
            triggerAchievementEvent(EVENT_WIFI_HANDSHAKE);
            if (gNekoReady) {
                gNekoPet.notifyHackSuccess("Handshake");
            }
            stage5::setMissionStatus(0, "Handshake concluido");
            stage5::logEvent("Handshake concluido", TFT_GREEN);
        }
    }
}

void updateDeauthSimulation(uint32_t now) {
    if (!gDeauthActive) {
        return;
    }
    if (now - gDeauthTick < 120) {
        return;
    }
    gDeauthTick = now;
    gDeauthPackets += 6;
    if (!modeUsesFullScreen() && activeMode().type == ModeType::kClone) {
        drawModeCard();
    }
    if ((gDeauthPackets % 120) == 0) {
        triggerAchievementEvent(EVENT_WIFI_DEAUTH, 1);
    }
}

void updatePortalSimulation(uint32_t now) {
    if (!gPortalActive) {
        return;
    }
    if (now - gPortalTick < 800) {
        return;
    }
    gPortalTick = now;
    gPortalClients = (gPortalClients + 1) % 8;
    if (!modeUsesFullScreen() && activeMode().type == ModeType::kEvilPortal) {
        drawModeCard();
    }
    stage5::setMissionStatus(1, String("Clientes fake: ") + gPortalClients);
    const uint8_t progress = static_cast<uint8_t>(std::min<int>(100, static_cast<int>(gPortalClients) * 12));
    stage5::setMissionProgress(1, progress);
}

void updateChannelAnalyzer(uint32_t now) {
    if (activeMode().type != ModeType::kAnalyzer) {
        return;
    }
    if (now - gLastChannelScan < kAnalyzerRefreshMs) {
        return;
    }
    gLastChannelScan = now;
    int networks = WiFi.scanNetworks(false, true);
    if (networks < 0) {
        showMessage("Scanner de canais falhou.");
        return;
    }
    memset(gChannelUsage, 0, sizeof(gChannelUsage));
    for (int i = 0; i < networks; ++i) {
        int channel = WiFi.channel(i);
        if (channel >= 1 && channel <= 13) {
            gChannelUsage[channel]++;
        }
    }
    gBestChannel = 1;
    int bestLoad = gChannelUsage[1];
    for (int ch = 2; ch <= 13; ++ch) {
        if (gChannelUsage[ch] < bestLoad) {
            bestLoad = gChannelUsage[ch];
            gBestChannel = ch;
        }
    }
    if (!modeUsesFullScreen()) {
        drawModeCard();
    }
    WiFi.scanDelete();
}

void tickBluetooth(uint32_t now) {
    if (!gBleReady || !gBleActive) {
        return;
    }
    if (now - gBleLastTick < kBluetoothTickMs) {
        return;
    }
    gBleLastTick = now;
    switch (gBleType) {
        case BLE_ATTACK_SOUR_APPLE:
            gBleAttacks.executeSourApple();
            break;
        case BLE_ATTACK_SAMSUNG:
            gBleAttacks.executeSamsungSpam();
            break;
        case BLE_ATTACK_GOOGLE:
            gBleAttacks.executeGoogleSpam();
            break;
        case BLE_ATTACK_MICROSOFT:
            gBleAttacks.executeMicrosoftSpam();
            break;
        case BLE_ATTACK_ALL:
            gBleAttacks.executeRotateAll();
            break;
    }
    gBlePackets++;
    if (gBleActive) {
        stage5::setMissionStatus(2, String("BLE ativo: ") + bleAttackName(gBleType) + " (" + gBlePackets + ")");
        const uint8_t progress = static_cast<uint8_t>(std::min<uint32_t>(100u, gBlePackets / 4));
        stage5::setMissionProgress(2, progress);
    }
    if (!modeUsesFullScreen() && activeMode().type == ModeType::kBluetooth) {
        drawModeCard();
    }
    if ((gBlePackets % 80) == 0) {
        triggerAchievementEvent(EVENT_BLE_SPAM, 1);
    }
}

void drawFullScreenMode(bool initial) {
    const ModeType type = activeMode().type;
    if (!modeUsesFullScreen()) {
        return;
    }
    if (type == ModeType::kDashboard) {
        String petName = gNekoReady ? gNekoPet.getPetName() : String("Kiisu");
        int petEnergy = gNekoReady ? gNekoPet.getEnergy() : 82;
        int petHappiness = gNekoReady ? gNekoPet.getHappiness() : 88;
        int moodIndex = gNekoReady ? static_cast<int>(gNekoPet.getCurrentMood()) : 3;
    int wifiStrength = wifiStrengthPercent();
        stage5::syncPetStats(petEnergy, petHappiness, moodIndex);
        stage5::syncWifiStrength(wifiStrength);
        if (initial) {
            stage5::enter(petName, petEnergy, petHappiness, moodIndex, wifiStrength);
        } else {
            stage5::forceRender();
        }
        return;
    }
    if (type == ModeType::kTutorial) {
        if (!gTutorialReady) {
            M5.Display.fillScreen(kBgColor);
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.setCursor(20, 60);
            M5.Display.print("Tutorial indisponivel.");
            return;
        }
        if (initial) {
            gTutorial.drawCurrentStep();
        }
    } else if (type == ModeType::kVirtualPet) {
        if (!gNekoReady) {
            M5.Display.fillScreen(kBgColor);
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.setCursor(20, 60);
            M5.Display.print("Pet nao inicializado.");
            return;
        }
        if (initial || millis() - gLastPetRedraw > kPetRedrawMs) {
            M5.Display.fillScreen(TFT_BLACK);
            gNekoPet.drawPetScreen();
            gLastPetRedraw = millis();
        }
    }
}

void drawCloneStatus() {
    const int contentX = kModeCardX + kPanelPadding;
    int lineY = kModeCardY + kPanelTitleOffset;
    M5.Display.setCursor(contentX, lineY);
    if (gDeauthActive) {
        M5.Display.print("Flood ativo");
        lineY += 12;
        M5.Display.setCursor(contentX, lineY);
        M5.Display.printf("Pacotes: %lu", static_cast<unsigned long>(gDeauthPackets));
        lineY += 12;
        M5.Display.setTextColor(kMutedColor, kCardBg);
        M5.Display.setCursor(contentX, lineY);
        M5.Display.print("SPACE desliga flood");
    } else {
        M5.Display.print("Pronto para iniciar");
        lineY += 12;
        M5.Display.setTextColor(kMutedColor, kCardBg);
        M5.Display.setCursor(contentX, lineY);
        M5.Display.print("SPACE liga flood");
    }
    M5.Display.setTextColor(kCardText, kCardBg);
}

void drawPortalStatus() {
    const int contentX = kModeCardX + kPanelPadding;
    int lineY = kModeCardY + kPanelTitleOffset;
    M5.Display.setCursor(contentX, lineY);
    if (gPortalActive) {
        M5.Display.print("Portal simulando autent.");
        lineY += 12;
        M5.Display.setCursor(contentX, lineY);
        M5.Display.printf("Clientes fake: %lu", static_cast<unsigned long>(gPortalClients));
        lineY += 12;
        M5.Display.setTextColor(kMutedColor, kCardBg);
        M5.Display.setCursor(contentX, lineY);
        M5.Display.print("SPACE encerra");
    } else {
        M5.Display.print("Portal parado.");
        lineY += 12;
        M5.Display.setTextColor(kMutedColor, kCardBg);
        M5.Display.setCursor(contentX, lineY);
        M5.Display.print("SPACE inicia listeners");
    }
    M5.Display.setTextColor(kCardText, kCardBg);
}

void drawAnalyzerStatus() {
    const int contentX = kModeCardX + kPanelPadding;
    int lineY = kModeCardY + kPanelTitleOffset;
    M5.Display.setCursor(contentX, lineY);
    M5.Display.printf("Canal ideal: %d", gBestChannel);
    lineY += 12;
    int load = gChannelUsage[gBestChannel];
    M5.Display.setCursor(contentX, lineY);
    M5.Display.printf("Carregamento: %d redes", load);
    lineY += 12;
    M5.Display.setTextColor(kMutedColor, kCardBg);
    M5.Display.setCursor(contentX, lineY);
    M5.Display.print("Auto-refresh a cada 12s");
    M5.Display.setTextColor(kCardText, kCardBg);
}

void drawFileManagerStatus() {
    const int contentX = kModeCardX + kPanelPadding;
    int lineY = kModeCardY + kPanelTitleOffset;
    M5.Display.setCursor(contentX, lineY);
    if (!gSdOk) {
        M5.Display.print("SD nao detectado.");
        return;
    }
    M5.Display.print(ellipsize(gCurrentPath, 30));
    size_t shown = 0;
    lineY += 12;
    for (size_t i = gFileScroll; i < gFileEntries.size() && shown < kFileWindow; ++i, ++shown) {
        const FileEntry &entry = gFileEntries[i];
        bool selected = static_cast<int>(i) == gFileSelection;
        M5.Display.setCursor(contentX, lineY + static_cast<int>(shown) * 11);
        M5.Display.setTextColor(selected ? activeMode().accent : kCardText, kCardBg);
        M5.Display.print(selected ? "> " : "  ");
        String label = entry.name;
        if (entry.isDir && entry.name != "..") {
            label += "/";
        } else if (!entry.isDir) {
            label += "  ";
            label += formatSize(entry.size);
        }
        M5.Display.print(ellipsize(label, 26));
    }
    M5.Display.setTextColor(kCardText, kCardBg);
}

void drawBluetoothStatus() {
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
    if (!gBleReady) {
        M5.Display.print("BLE stack indisponivel.");
        return;
    }
    M5.Display.printf("Ataque: %s", bleAttackName(gBleType));
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 32);
    M5.Display.printf("Estado: %s", gBleActive ? "Ativo" : "Standby");
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 42);
    M5.Display.printf("Pkts: %lu", static_cast<unsigned long>(gBlePackets));
}

void drawAchievementsStatus() {
    if (!gAchievementsReady) {
        M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
        M5.Display.print("Sist. conquistas off.");
        return;
    }
    const PlayerStats &stats = gAchievementManager.getStats();
    uint8_t unlocked = gAchievementManager.countUnlocked();
    uint8_t total = gAchievementManager.getCount();
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
    M5.Display.printf("Desbloqueadas: %u/%u", unlocked, total);
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 32);
    M5.Display.printf("Pontuacao: %u", stats.totalPoints);
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 42);
    M5.Display.printf("Scans: %u  BLE: %u", stats.networksScanned, stats.bleAttacks);
}

void drawTutorialStatusCard() {
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
    if (!gTutorialReady) {
        M5.Display.print("Tutorial indisponivel.");
        return;
    }
    M5.Display.print(gTutorial.isFirstRun() ? "Primeira execucao detectada" : "Ajuda rapida pronta");
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 32);
    M5.Display.print("ENTER abre wizard, N avanca.");
}

void drawPetStatusCard() {
    if (!gNekoReady) {
        M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
        M5.Display.print("Pet kawaii nao pronto.");
        return;
    }
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 22);
    M5.Display.printf("Humor: %d  Felicidade: %d", gNekoPet.getCurrentMood(), gNekoPet.getHappiness());
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 32);
    M5.Display.printf("Skill hack: %d", gNekoPet.getHackSkill());
    M5.Display.setCursor(kModeCardX + 10, kModeCardY + 42);
    M5.Display.print("F: snack  P: carinho");
}

String formatSize(uint32_t bytes) {
    if (bytes < 1024) {
        return String(bytes) + " B";
    }
    if (bytes < 1024 * 1024) {
        return String(bytes / 1024) + " KB";
    }
    return String(bytes / (1024 * 1024)) + " MB";
}

String parentPath(const String &path) {
    if (path == "/") {
        return "/";
    }
    int slash = path.lastIndexOf('/');
    if (slash <= 0) {
        return "/";
    }
    return path.substring(0, slash);
}

void refreshFileListing(const String &path) {
    gFileEntries.clear();
    if (!gSdOk) {
        return;
    }
    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        showMessage("Diretorio invalido");
        return;
    }
    if (path != "/") {
        gFileEntries.push_back({"..", true, 0});
    }
    for (File entry = dir.openNextFile(); entry; entry = dir.openNextFile()) {
        FileEntry item;
        item.name = String(entry.name());
        item.isDir = entry.isDirectory();
        item.size = entry.size();
        gFileEntries.push_back(item);
        entry.close();
    }
    dir.close();
    gCurrentPath = path;
    gFileSelection = 0;
    gFileScroll = 0;
}

void moveFileSelection(int delta) {
    if (gFileEntries.empty()) {
        return;
    }
    gFileSelection = (gFileSelection + delta + static_cast<int>(gFileEntries.size())) % static_cast<int>(gFileEntries.size());
    if (gFileSelection < gFileScroll) {
        gFileScroll = gFileSelection;
    } else if (gFileSelection >= gFileScroll + static_cast<int>(kFileWindow)) {
        gFileScroll = gFileSelection - static_cast<int>(kFileWindow) + 1;
    }
    drawModeCard();
}

void openSelectedFile() {
    if (!gSdOk || gFileEntries.empty()) {
        return;
    }
    FileEntry entry = gFileEntries[gFileSelection];
    if (entry.isDir) {
        if (entry.name == "..") {
            goFileParent();
            return;
        }
        String next = gCurrentPath == "/" ? "" : gCurrentPath;
        next += "/";
        next += entry.name;
        refreshFileListing(next);
        drawModeCard();
        return;
    }
    char buffer[96];
    std::snprintf(buffer, sizeof(buffer), "Arquivo: %s", entry.name.c_str());
    showMessage(buffer);
}

void goFileParent() {
    if (gCurrentPath == "/") {
        return;
    }
    String parent = parentPath(gCurrentPath);
    if (parent.isEmpty()) {
        parent = "/";
    }
    refreshFileListing(parent);
    drawModeCard();
}

void toggleDeauth() {
    gDeauthActive = !gDeauthActive;
    if (gDeauthActive) {
        gDeauthPackets = 0;
        gDeauthTick = millis();
        showMessage("Deauth simulada ativada.");
        stage5::setMissionStatus(2, "Deauth simulada ativa");
        stage5::adjustMissionProgress(2, 6);
        stage5::logEvent("Deauth simulada ativada", 0xFBE0);
        stage5::setMacroResult(1, "Deauth ativo");
    } else {
        showMessage("Flood pausado.");
        const char *bleLabel = bleAttackName(gBleType);
        if (gBleActive) {
            stage5::setMissionStatus(2, String("BLE ativo: ") + bleLabel);
        } else {
            stage5::setMissionStatus(2, "Deauth/BLE standby");
            stage5::setMissionProgress(2, 0);
        }
        stage5::setMacroResult(1, "Deauth standby");
    }
    stage5::setMacroActive(1, gDeauthActive);
    drawModeCard();
}

void togglePortal() {
    gPortalActive = !gPortalActive;
    if (gPortalActive) {
        gPortalClients = 0;
        gPortalTick = millis();
        showMessage("Portal simulado iniciado.");
        triggerAchievementEvent(EVENT_PORTAL_CREATE, 1);
        stage5::setMissionStatus(1, "Portal servindo template");
        stage5::adjustMissionProgress(1, 8);
        stage5::logEvent("Portal cativo iniciado", 0xFFE0);
        stage5::setMacroResult(2, "Portal ativo");
    } else {
        showMessage("Portal desligado.");
        stage5::setMissionStatus(1, "Portal aguardando");
        stage5::setMissionProgress(1, 0);
        stage5::setMacroResult(2, "Portal standby");
    }
    stage5::setMacroActive(2, gPortalActive);
    drawModeCard();
}

const char *bleAttackName(BLEAttackType type) {
    switch (type) {
        case BLE_ATTACK_SOUR_APPLE: return "SourApple";
        case BLE_ATTACK_SAMSUNG: return "Samsung";
        case BLE_ATTACK_GOOGLE: return "FastPair";
        case BLE_ATTACK_MICROSOFT: return "SwiftPair";
        case BLE_ATTACK_ALL: return "Mix";
    }
    return "?";
}

int wifiStrengthPercent() {
    if (!gWifiOk) {
        return 0;
    }
    return std::min(100, gNetworksFound * 12);
}

void toggleBluetooth() {
    if (!gBleReady) {
        showMessage("BLE nao inicializado.");
        return;
    }
    if (gBleActive) {
        gBleAttacks.stop();
        gBleActive = false;
        showMessage("BLE spam parado.");
        if (gDeauthActive) {
            stage5::setMissionStatus(2, "Deauth simulada ativa");
        } else {
            stage5::setMissionStatus(2, "Deauth/BLE standby");
        }
        stage5::logEvent("BLE spam parado", 0x4EDD);
    } else {
        switch (gBleType) {
            case BLE_ATTACK_SOUR_APPLE: gBleAttacks.startSourApple(); break;
            case BLE_ATTACK_SAMSUNG: gBleAttacks.startSamsungSpam(); break;
            case BLE_ATTACK_GOOGLE: gBleAttacks.startGoogleSpam(); break;
            case BLE_ATTACK_MICROSOFT: gBleAttacks.startMicrosoftSpam(); break;
            case BLE_ATTACK_ALL: gBleAttacks.startSpamAll(); break;
        }
        gBleActive = true;
        gBlePackets = 0;
        showMessage("BLE spam ativo.");
        triggerAchievementEvent(EVENT_BLE_SPAM, 1);
        stage5::setMissionStatus(2, String("BLE ativo: ") + bleAttackName(gBleType));
        stage5::adjustMissionProgress(2, 8);
        stage5::logEvent(String("BLE spam ativo ( ") + bleAttackName(gBleType) + " )", 0x4EDD);
    }
    drawModeCard();
}

void changeBluetoothAttack(int delta) {
    int type = static_cast<int>(gBleType);
    type = (type + delta + 5) % 5;
    gBleType = static_cast<BLEAttackType>(type);
    if (gBleActive) {
        toggleBluetooth();
        toggleBluetooth();
    } else {
        stage5::setMissionStatus(2, String("BLE pronto: ") + bleAttackName(gBleType));
    }
    drawModeCard();
}

void pumpAchievementNotifications() {
    if (!gAchievementsReady || !gAchievementManager.hasNewUnlocks()) {
        return;
    }
    auto unlocked = gAchievementManager.getNewUnlocks();
    if (unlocked.empty()) {
        return;
    }
    uint8_t id = unlocked.front();
    const Achievement *ach = gAchievementManager.getAchievement(id);
    if (!ach) {
        return;
    }
    char buffer[120];
    std::snprintf(buffer, sizeof(buffer), "Conquista desbloqueada: %s", ach->name_P);
    showMessage(buffer);
}

void processModeShortcuts(const KeyFlags &flags) {
    const ModeType type = activeMode().type;
    if (type == ModeType::kDashboard) {
        stage5::Input input;
        input.missionUp = flags.up;
        input.missionDown = flags.down;
        input.macroLeft = flags.cycleLeft;
        input.macroRight = flags.cycleRight;
        input.toggleMacro = flags.space;
        input.feedPet = flags.feed;
        input.petPet = flags.pet;
        input.enter = flags.enter;
        input.nextPage = flags.next;
        input.prevPage = flags.prev;
        stage5::handleInput(input);
        return;
    }
    if (flags.space) {
        switch (type) {
            case ModeType::kClone: toggleDeauth(); break;
            case ModeType::kEvilPortal: togglePortal(); break;
            case ModeType::kBluetooth: toggleBluetooth(); break;
            case ModeType::kHandshake:
                gHandshakeProgress = 0;
                gHandshakeCompleted = false;
                showMessage("Simulacao reiniciada.");
                break;
            default: break;
        }
    }

    if (flags.cycleLeft) {
        if (type == ModeType::kBluetooth) {
            changeBluetoothAttack(-1);
        } else if (type == ModeType::kFileManager) {
            moveFileSelection(-1);
        }
    }
    if (flags.cycleRight) {
        if (type == ModeType::kBluetooth) {
            changeBluetoothAttack(1);
        } else if (type == ModeType::kFileManager) {
            moveFileSelection(1);
        }
    }

    if (flags.open && type == ModeType::kFileManager) {
        openSelectedFile();
    }
    if (flags.back && type == ModeType::kFileManager) {
        goFileParent();
    }

    if (type == ModeType::kVirtualPet && gNekoReady) {
        if (flags.feed) {
            gNekoPet.quickFeed();
            showMessage("Neko alimentado (snack)");
            stage5::syncPetStats(gNekoPet.getEnergy(),
                                  gNekoPet.getHappiness(),
                                  static_cast<int>(gNekoPet.getCurrentMood()));
        }
        if (flags.pet) {
            gNekoPet.quickPet();
            showMessage("Neko feliz (carinho)");
            stage5::syncPetStats(gNekoPet.getEnergy(),
                                  gNekoPet.getHappiness(),
                                  static_cast<int>(gNekoPet.getCurrentMood()));
        }
    }

    if (type == ModeType::kTutorial && gTutorialReady) {
        if (flags.next) {
            gTutorial.nextStep();
        }
        if (flags.prev) {
            gTutorial.previousStep();
        }
        if (flags.skip) {
            gTutorial.skipTutorial();
            showMessage("Tutorial pulado.");
        }
        if (flags.help) {
            gTutorial.showContextualHelp();
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
    M5Cardputer.begin(cfg);

    M5.Display.setRotation(1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(kCardText, kBgColor);

    initPeripherals();
    applyMode(0);
    showMessage("ESC abre menu | ENTER troca abas | setas navegam.");

    gLastNekoTick = millis();
    gLastStatusTick = millis();
    gLastHeartbeat = millis();
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        KeyFlags flags;
        const bool dashboardActive = activeMode().type == ModeType::kDashboard;
        for (auto key : status.word) {
            switch (key) {
                case ';':
                case 'w':
                case 'W':
                    if (!dashboardActive) {
                        flags.up = true;
                    }
                    break;
                case '.':
                case 's':
                case 'S':
                    if (!dashboardActive) {
                        flags.down = true;
                    }
                    break;
                case '`':
                case 0x1B:
                    flags.esc = true;
                    break;
                case ' ': flags.space = true; break;
                case 'f':
                case 'F':
                    flags.feed = true;
                    break;
                case 'p':
                case 'P':
                    flags.pet = true;
                    break;
                case '[':
                case '{':
                case 'a':
                case 'A':
                    if (!dashboardActive) {
                        flags.cycleLeft = true;
                    }
                    break;
                case ']':
                case '}':
                case 'd':
                case 'D':
                    if (!dashboardActive) {
                        flags.cycleRight = true;
                    }
                    break;
                case 'o':
                case 'O':
                    flags.open = true;
                    break;
                case 'b':
                case 'B':
                    if (!dashboardActive) {
                        flags.back = true;
                        flags.prev = true;
                    }
                    break;
                case 'n':
                case 'N':
                    if (!dashboardActive) {
                        flags.next = true;
                    }
                    break;
                case 'h':
                case 'H':
                case '?':
                    flags.help = true;
                    break;
                case 'x':
                case 'X':
                    flags.skip = true;
                    break;
                default:
                    break;
            }
        }

        if (status.tab && !dashboardActive) {
            if (status.shift) {
                flags.prev = true;
            } else {
                flags.next = true;
            }
        }

        for (auto hid : status.hid_keys) {
            switch (hid) {
                case 0x4F:  // HID right arrow
                    flags.cycleRight = true;
                    flags.next = true;
                    break;
                case 0x50:  // HID left arrow
                    flags.cycleLeft = true;
                    flags.prev = true;
                    break;
                case 0x52:  // HID up arrow
                    flags.up = true;
                    break;
                case 0x51:  // HID down arrow
                    flags.down = true;
                    break;
                default:
                    break;
            }
        }

        flags.enter = status.enter;

        if (flags.esc) {
            handleEsc();
        }
        if (flags.up) {
            advanceSelection(-1);
        }
        if (flags.down) {
            advanceSelection(1);
        }
        if (status.enter && !(activeMode().type == ModeType::kDashboard && !gMenuVisible)) {
            handleEnter();
        }
        processModeShortcuts(flags);
    }

    const uint32_t now = millis();

    if (!modeUsesFullScreen() && !gMenuVisible && now - gLastNekoTick >= kNekoFrameIntervalMs && gActiveFrameCount > 0) {
        gNekoFrame = static_cast<uint8_t>((gNekoFrame + 1) % gActiveFrameCount);
        drawNekoFrame();
        gLastNekoTick = now;
    }

    if (!modeUsesFullScreen() && !gMenuVisible && now - gLastStatusTick >= kStatusRefreshMs) {
        drawStatusCard();
        drawModeCard();
        gLastStatusTick = now;
    }

    updateHandshakeSimulation(now);
    updateDeauthSimulation(now);
    updatePortalSimulation(now);
    updateChannelAnalyzer(now);
    tickBluetooth(now);

    if (activeMode().type == ModeType::kDashboard) {
        stage5::tick(now);
    }

    if (gNekoReady) {
        gNekoPet.update();
    }
    if (gTutorialReady) {
        gTutorial.update();
    }
    if (gAchievementsReady) {
        gAchievementManager.update();
    }

    pumpAchievementNotifications();

    if (gMessageActive && (now - gMessageStart >= kMessageTimeoutMs)) {
        gMessageActive = false;
        if (!modeUsesFullScreen()) {
            drawFooter();
        }
    }

    if (now - gLastHeartbeat >= kHeartbeatIntervalMs) {
        Serial.println(F("Heartbeat: menu alive"));
        gLastHeartbeat = now;
    }

    delay(6);
}

}  // namespace stage4

void setup() {
    stage4::setup();
}

void loop() {
    stage4::loop();
}
