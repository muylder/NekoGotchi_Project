#include <M5Cardputer.h>
#include <M5Unified.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "stage5_dashboard.h"

namespace stage5 {
constexpr uint16_t kColorBg = 0x0841;          // Deep midnight blue
constexpr uint16_t kColorBgAlt = 0x1063;       // Slightly lighter band
constexpr uint16_t kColorHeader = 0x2108;      // Charcoal header
constexpr uint16_t kColorFooter = 0x18C3;      // Footer strip
constexpr uint16_t kColorPanel = 0x31A6;       // Soft slate card
constexpr uint16_t kColorPanelAlt = 0x39C7;    // Lighter slate accent
constexpr uint16_t kColorTextPrimary = 0xFFFF; // Pure white
constexpr uint16_t kColorTextMuted = 0xBDF7;   // Misty cyan
constexpr uint16_t kColorAccentMissions = 0xFDA0; // Warm amber
constexpr uint16_t kColorAccentMacros = 0x67FF;   // Aqua mint
constexpr uint16_t kColorAccentPet = 0xF81F;      // Sakura pink
constexpr uint16_t kColorAccentSignals = 0x7E9E;  // Lime punch
constexpr int16_t kPadding = 8;
constexpr int16_t kScreenW = 240;
constexpr int16_t kScreenH = 135;
constexpr int16_t kHeaderHeight = 26;
constexpr int16_t kFooterHeight = 20;
constexpr int16_t kLogLines = 4;

struct Rect {
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;
    constexpr Rect() = default;
    constexpr Rect(int32_t px, int32_t py, int32_t pw, int32_t ph)
        : x(static_cast<int16_t>(px)),
          y(static_cast<int16_t>(py)),
          w(static_cast<int16_t>(pw)),
          h(static_cast<int16_t>(ph)) {}
};

struct MissionCard {
    const char *title;
    String status;
    uint8_t progress;
    uint16_t accent;
};

struct MacroButton {
    const char *label;
    String result;
    uint16_t accent;
    bool active;
};

struct ConsoleLine {
    String text;
    uint16_t color;
};

constexpr size_t kMissionCount = 3;
constexpr size_t kMacroCount = 3;

MissionCard gMissions[kMissionCount] = {
    {"Captura AP", "Monitorando handshakes", 0, kColorAccentMissions},
    {"Portal Cativo", "Templates prontos no SD", 0, 0xFFE0},
    {"Ataques RF", "Deauth/BLE standby", 0, 0x4EDD},
};

MacroButton gMacros[kMacroCount] = {
    {"Analisar", "Logs atualizados", 0x5FFF, false},
    {"Jam AP", "Deauth executado", 0xFBE0, false},
    {"Portal ON", "Portal standby", 0x6D19, false},
};

ConsoleLine gConsole[kLogLines] = {
    {"[12:00] Neko feliz (mood +2)", 0x5FFF},
    {"[12:02] Canal 11 ruido moderado", 0xE73C},
    {"[12:05] Portal phish rodando", 0xFFE0},
    {"[12:07] BLE spam limitado", 0xFBE0},
};

uint8_t gDialPhase = 0;
uint32_t gLastDialTick = 0;
constexpr uint32_t kDialTickMs = 180;

int gSelectedMission = 0;
int gSelectedMacro = 0;

int gPetEnergy = 88;
int gPetHappiness = 90;
int gPetMoodIndex = 4;
int gWifiStrength = 82;
String gPetName = "Kiisu";
bool gIsActive = false;

constexpr size_t kPageCount = 3;
constexpr const char *kPageLabels[kPageCount] = {
    "Neko",
    "Operacoes",
    "Sinais"
};

constexpr const char *kPageTitles[kPageCount] = {
    "NekoGotchi",
    "Painel de Operacoes",
    "Radios & Logs"
};

int gActivePage = 0;
bool gMenuOverlayActive = false;

constexpr const char *kMoodLabels[] = {
    "Exausto",
    "Sonolento",
    "Neutro",
    "Feliz",
    "Radiante",
    "Extasiado"
};

constexpr size_t kMoodLabelCount = sizeof(kMoodLabels) / sizeof(kMoodLabels[0]);

bool gNeedsRedraw = true;

template <typename T>
T clampValue(T value, T minValue, T maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

void printTruncated(const char *text, size_t maxChars) {
    if (!text) {
        return;
    }
    const size_t len = std::strlen(text);
    const size_t limit = std::min(len, maxChars);
    for (size_t i = 0; i < limit; ++i) {
        M5.Display.write(static_cast<uint8_t>(text[i]));
    }
    if (len > maxChars) {
        M5.Display.print("...");
    }
}

void printTruncated(const String &text, size_t maxChars) {
    printTruncated(text.c_str(), maxChars);
}

void requestRedraw() {
    gNeedsRedraw = true;
}

void changePage(int delta) {
    const int pageCount = static_cast<int>(kPageCount);
    gActivePage = (gActivePage + delta + pageCount) % pageCount;
    requestRedraw();
}

const char *currentMoodLabel() {
    int idx = clampValue(gPetMoodIndex, 0, static_cast<int>(kMoodLabelCount) - 1);
    return kMoodLabels[idx];
}

void appendConsoleLine(const String &text, uint16_t color) {
    for (size_t i = 0; i + 1 < kLogLines; ++i) {
        gConsole[i] = gConsole[i + 1];
    }
    gConsole[kLogLines - 1] = {text, color};
    requestRedraw();
}

void adjustMissionProgressInternal(size_t index, int delta) {
    if (index >= kMissionCount) {
        return;
    }
    MissionCard &mission = gMissions[index];
    int value = clampValue(static_cast<int>(mission.progress) + delta, 0, 100);
    mission.progress = static_cast<uint8_t>(value);
    requestRedraw();
}

void setMissionStatusInternal(size_t index, const String &status) {
    if (index >= kMissionCount) {
        return;
    }
    gMissions[index].status = status;
    requestRedraw();
}

void adjustPetEnergy(int delta) {
    gPetEnergy = clampValue(gPetEnergy + delta, 0, 100);
    requestRedraw();
}

void adjustPetHappiness(int delta) {
    gPetHappiness = clampValue(gPetHappiness + delta, 0, 100);
    requestRedraw();
}

void adjustPetMood(int delta) {
    gPetMoodIndex = clampValue(gPetMoodIndex + delta, 0, static_cast<int>(kMoodLabelCount) - 1);
    requestRedraw();
}

void adjustWifiStrength(int delta) {
    gWifiStrength = clampValue(gWifiStrength + delta, 0, 100);
    requestRedraw();
}

void cycleMissionSelection(int delta) {
    const int count = static_cast<int>(kMissionCount);
    gSelectedMission = (gSelectedMission + delta + count) % count;
    requestRedraw();
}

void cycleMacroSelection(int delta) {
    const int count = static_cast<int>(kMacroCount);
    gSelectedMacro = (gSelectedMacro + delta + count) % count;
    requestRedraw();
}

String buildTimestamped(const String &message) {
    uint32_t seconds = millis() / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t rem = seconds % 60;
    char buffer[12];
    std::snprintf(buffer, sizeof(buffer), "[%02lu:%02lu] ", static_cast<unsigned long>(minutes), static_cast<unsigned long>(rem));
    return String(buffer) + message;
}

void executeMacro(size_t index) {
    if (index >= kMacroCount) {
        return;
    }
    MacroButton &macro = gMacros[index];
    macro.active = !macro.active;
    switch (index) {
        case 0: {  // Analisar
            if (macro.active) {
                macro.result = F("Scanner em execucao");
                appendConsoleLine(buildTimestamped("Varredura WiFi iniciada"), macro.accent);
                setMissionStatusInternal(0, "Capturando handshakes...");
                adjustMissionProgressInternal(0, 7);
                adjustPetMood(+1);
            } else {
                macro.result = F("Scanner pausado");
                appendConsoleLine(buildTimestamped("Scanner pausado"), macro.accent);
                setMissionStatusInternal(0, "Aguardando nova varredura");
            }
            break;
        }
        case 1: {  // Jam AP
            if (macro.active) {
                macro.result = F("Deauth ativo");
                appendConsoleLine(buildTimestamped("Ataque deauth liberado"), macro.accent);
                adjustWifiStrength(-12);
                setMissionStatusInternal(2, "Interferencia aplicada");
                adjustMissionProgressInternal(2, 12);
                adjustPetMood(+1);
            } else {
                macro.result = F("Deauth parado");
                appendConsoleLine(buildTimestamped("Deauth encerrado"), macro.accent);
                adjustWifiStrength(+8);
            }
            break;
        }
        case 2: {  // Portal ON
            if (macro.active) {
                macro.result = F("Portal online");
                appendConsoleLine(buildTimestamped("Portal cativo em execucao"), macro.accent);
                setMissionStatusInternal(1, "Servindo template wifi_cafe.html");
                adjustMissionProgressInternal(1, 10);
                adjustPetHappiness(+4);
            } else {
                macro.result = F("Portal standby");
                appendConsoleLine(buildTimestamped("Portal desligado"), macro.accent);
                setMissionStatusInternal(1, "Portal aguardando" );
                adjustPetHappiness(-2);
            }
            break;
        }
        default:
            break;
    }
    requestRedraw();
}

void feedPet() {
    appendConsoleLine(buildTimestamped("Kiisu devorou um petisco"), kColorAccentPet);
    adjustPetEnergy(+6);
    adjustPetHappiness(+3);
    adjustPetMood(+1);
}

void petPet() {
    appendConsoleLine(buildTimestamped("Kiisu recebeu carinho"), kColorAccentPet);
    adjustPetHappiness(+4);
    adjustPetMood(+1);
}

void drawRoundedPanel(const Rect &rect, uint16_t fillColor, uint16_t borderColor, uint8_t radius = 8) {
    M5.Display.fillRoundRect(rect.x, rect.y, rect.w, rect.h, radius, fillColor);
    M5.Display.drawRoundRect(rect.x, rect.y, rect.w, rect.h, radius, borderColor);
}

void drawHeader() {
    M5.Display.fillRect(0, 0, kScreenW, kHeaderHeight, kColorHeader);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(kColorTextPrimary, kColorHeader);
    M5.Display.setCursor(kPadding, 6);
    M5.Display.print(kPageTitles[gActivePage]);

    M5.Display.setTextSize(1);
    M5.Display.setCursor(kScreenW - 90, 6);
    M5.Display.setTextColor(kColorTextMuted, kColorHeader);
    M5.Display.printf("Pg %d/%d", gActivePage + 1, static_cast<int>(kPageCount));

    const int16_t wifiX = kScreenW - 64;
    const int bars = clampValue(gWifiStrength / 25, 0, 4);
    for (int i = 0; i < 4; ++i) {
        int16_t barH = 4 + i * 3;
        int16_t barY = kHeaderHeight - barH - 4;
        uint16_t barColor = (i < bars) ? kColorAccentSignals : kColorTextMuted;
        M5.Display.fillRect(wifiX + i * 6, barY, 4, barH, barColor);
    }
    M5.Display.setCursor(wifiX + 26, 10);
    M5.Display.setTextColor(kColorTextPrimary, kColorHeader);
    M5.Display.printf("%d%%", gWifiStrength);

    if (gActivePage == 0) {
        M5.Display.setCursor(kScreenW - 112, 14);
        M5.Display.setTextColor(kColorAccentPet, kColorHeader);
        M5.Display.printf("E:%d H:%d", gPetEnergy, gPetHappiness);
    }
}

void drawFooter() {
    M5.Display.fillRect(0, kScreenH - kFooterHeight, kScreenW, kFooterHeight, kColorFooter);
    M5.Display.setTextColor(kColorTextPrimary, kColorFooter);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(kPadding, kScreenH - kFooterHeight + 6);
    switch (gActivePage) {
        case 0:
            M5.Display.print("↑↓ missoes | ←→ macros | SPACE executa");
            break;
        case 1:
            M5.Display.print("W/S missoes | A/D macros | SPACE executa");
            break;
        default:
            M5.Display.print("ENTER troca aba | N avanca | B volta");
            break;
    }
}

void drawBackground() {
    M5.Display.fillScreen(kColorBg);
    for (int16_t y = kHeaderHeight; y < kScreenH - kFooterHeight; y += 18) {
        M5.Display.fillRect(0, y, kScreenW, 9, kColorBgAlt);
    }
}

void drawMissionColumn(uint8_t phase) {
    const int16_t contentTop = kHeaderHeight + 4;
    const int16_t cardHeight = 20;
    const int16_t columnWidth = 136;
    int16_t y = contentTop;

    for (size_t i = 0; i < kMissionCount; ++i) {
        MissionCard &mission = gMissions[i];
        Rect card{ kPadding, y, columnWidth, cardHeight };
        const bool selected = static_cast<int>(i) == gSelectedMission;
        const uint16_t border = selected && (phase % 16 < 8) ? mission.accent : kColorPanelAlt;
        drawRoundedPanel(card, kColorPanel, border, 6);

        M5.Display.setTextSize(1);
        M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
        M5.Display.setCursor(card.x + 6, card.y + 4);
        printTruncated(mission.title, 12);

    M5.Display.setCursor(card.x + card.w - 32, card.y + 4);
    M5.Display.printf("%3u%%", mission.progress);

    M5.Display.setTextColor(kColorTextMuted, kColorPanel);
    M5.Display.setCursor(card.x + 6, card.y + 14);
    printTruncated(mission.status, 20);
        int16_t barX = card.x + 6;
        int16_t barY = card.y + card.h - 4;
        int16_t barW = card.w - 12;
        M5.Display.fillRect(barX, barY, barW, 2, kColorBgAlt);
        int16_t filled = (barW * mission.progress) / 100;
        if (filled > 0) {
            M5.Display.fillRect(barX, barY, filled, 2, mission.accent);
        }

        y += cardHeight + 4;
    }

    Rect focus{ kPadding, y, columnWidth, 20 };
    drawRoundedPanel(focus, kColorPanel, kColorPanelAlt, 6);
    M5.Display.setTextColor(kColorTextMuted, kColorPanel);
    M5.Display.setCursor(focus.x + 6, focus.y + 6);
    M5.Display.print("Foco: ");
    M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
    printTruncated(gMissions[gSelectedMission].title, 14);
}

void drawMacroStack(uint8_t phase) {
    const int16_t contentTop = kHeaderHeight + 4;
    const int16_t contentBottom = kScreenH - kFooterHeight - 4;
    const int16_t columnX = 144;
    const int16_t columnWidth = kScreenW - columnX - kPadding;
    const int16_t slotHeight = 22;
    int16_t y = contentTop;

    for (size_t i = 0; i < kMacroCount; ++i) {
        bool selected = static_cast<int>(i) == gSelectedMacro;
        bool active = gMacros[i].active;
        Rect slot{ columnX, y, columnWidth, slotHeight };
        uint16_t border = active ? gMacros[i].accent : kColorPanelAlt;
        if (selected && (phase % 14 < 7)) {
            border = kColorAccentMacros;
        }
        drawRoundedPanel(slot, kColorPanel, border, 6);

        M5.Display.setTextSize(1);
        M5.Display.setCursor(slot.x + 6, slot.y + 6);
        M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
        M5.Display.print(gMacros[i].label);

        M5.Display.setCursor(slot.x + slot.w - 6 - 36, slot.y + 6);
        M5.Display.setTextColor(active ? gMacros[i].accent : kColorTextMuted, kColorPanel);
        M5.Display.print(active ? "ATIVO" : "PRONTO");

        M5.Display.setCursor(slot.x + 6, slot.y + slot.h - 6);
        M5.Display.setTextColor(kColorTextMuted, kColorPanel);
        printTruncated(gMacros[i].result, 18);

        y += slotHeight + 4;
    }

    const int16_t remaining = contentBottom - y;
    if (remaining > 24) {
        Rect logPanel{ columnX, y, columnWidth, remaining };
        drawRoundedPanel(logPanel, kColorPanel, kColorPanelAlt, 6);
        M5.Display.setTextColor(kColorTextMuted, kColorPanel);
        M5.Display.setCursor(logPanel.x + 6, logPanel.y + 6);
        M5.Display.print("Eventos recentes");

        int16_t logY = logPanel.y + 18;
        for (int i = static_cast<int>(kLogLines) - 1; i >= 0; --i) {
            const auto &line = gConsole[i];
            if (logY >= logPanel.y + logPanel.h - 6) {
                break;
            }
            M5.Display.setCursor(logPanel.x + 6, logY);
            M5.Display.setTextColor(line.color, kColorPanel);
            printTruncated(line.text, 18);
            logY += 11;
        }
    }
}

void drawOperationsPage(uint8_t phase) {
    drawMissionColumn(phase);
    drawMacroStack(phase);
}

void drawNekoPage() {
    const int16_t contentTop = kHeaderHeight + 4;
    const int16_t contentBottom = kScreenH - kFooterHeight - 4;
    const int16_t fullWidth = kScreenW - kPadding * 2;

    Rect hero{ kPadding, contentTop, fullWidth, 56 };
    drawRoundedPanel(hero, kColorPanel, kColorAccentPet, 8);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(kColorAccentPet, kColorPanel);
    M5.Display.setCursor(hero.x + 8, hero.y + 8);
    M5.Display.print(gPetName);

    M5.Display.setTextSize(1);
    M5.Display.setCursor(hero.x + 8, hero.y + 32);
    M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
    M5.Display.printf("Energia %d%%  Felicidade %d%%", gPetEnergy, gPetHappiness);
    M5.Display.setCursor(hero.x + 8, hero.y + 44);
    M5.Display.setTextColor(kColorTextMuted, kColorPanel);
    M5.Display.printf("Humor: %s", currentMoodLabel());

    M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
    M5.Display.setCursor(hero.x + hero.w - 72, hero.y + 28);
    M5.Display.print("(=^.^=)");

    Rect routine{ kPadding, hero.y + hero.h + 4, fullWidth, 30 };
    drawRoundedPanel(routine, kColorPanel, kColorPanelAlt, 6);
    M5.Display.setTextColor(kColorTextMuted, kColorPanel);
    M5.Display.setCursor(routine.x + 6, routine.y + 8);
    M5.Display.print("F: petisco   P: carinho   SPACE: macro ativa");

    Rect diary{ kPadding, routine.y + routine.h + 4, fullWidth, contentBottom - (routine.y + routine.h + 4) };
    drawRoundedPanel(diary, kColorPanel, kColorPanelAlt, 6);
    M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
    M5.Display.setCursor(diary.x + 6, diary.y + 6);
    M5.Display.print("Diario do Neko");

    int16_t logY = diary.y + 18;
    for (int i = static_cast<int>(kLogLines) - 1; i >= 0; --i) {
        const auto &line = gConsole[i];
        if (logY >= diary.y + diary.h - 6) {
            break;
        }
        M5.Display.setCursor(diary.x + 6, logY);
        M5.Display.setTextColor(line.color, kColorPanel);
        printTruncated(line.text, 32);
        logY += 12;
    }
}

void drawSignalsPage() {
    const int16_t contentTop = kHeaderHeight + 4;
    const int16_t contentBottom = kScreenH - kFooterHeight - 4;
    const int16_t columnWidth = (kScreenW - kPadding * 3) / 2;

    Rect signal{ kPadding, contentTop, columnWidth, 48 };
    drawRoundedPanel(signal, kColorPanel, kColorAccentSignals, 8);
    M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
    M5.Display.setCursor(signal.x + 6, signal.y + 6);
    M5.Display.print("Radar de Sinais");

    int16_t gaugeY = signal.y + 24;
    M5.Display.fillRect(signal.x + 6, gaugeY, signal.w - 12, 6, kColorBgAlt);
    int16_t filled = ((signal.w - 12) * gWifiStrength) / 100;
    M5.Display.fillRect(signal.x + 6, gaugeY, filled, 6, kColorAccentSignals);
    M5.Display.setCursor(signal.x + 6, gaugeY + 10);
    M5.Display.setTextColor(kColorTextMuted, kColorPanel);
    M5.Display.printf("WiFi forte: %d%%", gWifiStrength);

    Rect macroSummary{ signal.x, signal.y + signal.h + 4, columnWidth, contentBottom - (signal.y + signal.h + 4) };
    drawRoundedPanel(macroSummary, kColorPanel, kColorPanelAlt, 6);
    M5.Display.setCursor(macroSummary.x + 6, macroSummary.y + 6);
    M5.Display.setTextColor(kColorTextMuted, kColorPanel);
    M5.Display.print("Macros");
    int16_t macroY = macroSummary.y + 18;
    for (size_t i = 0; i < kMacroCount && macroY < macroSummary.y + macroSummary.h - 6; ++i) {
        M5.Display.setCursor(macroSummary.x + 6, macroY);
        M5.Display.setTextColor(gMacros[i].active ? gMacros[i].accent : kColorTextPrimary, kColorPanel);
        M5.Display.print(gMacros[i].label);
        M5.Display.setCursor(macroSummary.x + macroSummary.w - 36, macroY);
        M5.Display.setTextColor(kColorTextMuted, kColorPanel);
        M5.Display.print(gMacros[i].active ? "ON" : "OFF");
        macroY += 12;
        M5.Display.setCursor(macroSummary.x + 6, macroY);
        M5.Display.setTextColor(kColorTextMuted, kColorPanel);
        printTruncated(gMacros[i].result, 20);
        macroY += 12;
    }

    Rect timeline{ signal.x + signal.w + kPadding, contentTop, columnWidth, contentBottom - contentTop }; 
    drawRoundedPanel(timeline, kColorPanel, kColorPanelAlt, 6);
    M5.Display.setCursor(timeline.x + 6, timeline.y + 6);
    M5.Display.setTextColor(kColorTextPrimary, kColorPanel);
    M5.Display.print("Linha do tempo");

    int16_t lineY = timeline.y + 18;
    for (int i = static_cast<int>(kLogLines) - 1; i >= 0; --i) {
        const auto &line = gConsole[i];
        if (lineY >= timeline.y + timeline.h - 6) {
            break;
        }
        M5.Display.setCursor(timeline.x + 6, lineY);
        M5.Display.setTextColor(line.color, kColorPanel);
        printTruncated(line.text, 24);
        lineY += 12;
    }
}

void drawDashboard(uint8_t phase) {
    drawBackground();
    drawHeader();
    switch (gActivePage) {
        case 0:
            drawNekoPage();
            break;
        case 1:
            drawOperationsPage(phase);
            break;
        case 2:
            drawSignalsPage();
            break;
        default:
            drawNekoPage();
            break;
    }
    drawFooter();
    gNeedsRedraw = false;
}

void render() {
    drawDashboard(gDialPhase);
}

void init() {
    gDialPhase = 0;
    gLastDialTick = millis();
    gNeedsRedraw = true;
    gIsActive = false;
    gActivePage = 0;
    gMenuOverlayActive = false;
}

void enter(const String &petName, int petEnergy, int petHappiness, int moodIndex, int wifiStrength) {
    gIsActive = true;
    gActivePage = 0;
    if (petName.length() > 0) {
        gPetName = petName;
    }
    gPetEnergy = clampValue(petEnergy, 0, 100);
    gPetHappiness = clampValue(petHappiness, 0, 100);
    gPetMoodIndex = clampValue(moodIndex, 0, static_cast<int>(kMoodLabelCount) - 1);
    gWifiStrength = clampValue(wifiStrength, 0, 100);
    requestRedraw();
    render();
}

void exit() {
    gIsActive = false;
    gMenuOverlayActive = false;
}

void handleInput(const Input &input) {
    if (!gIsActive || gMenuOverlayActive) {
        return;
    }

    if (input.enter) {
        changePage(+1);
        return;
    }

    if (gActivePage != 1) {
        if (input.nextPage) {
            changePage(+1);
            return;
        }
        if (input.prevPage) {
            changePage(-1);
            return;
        }
    }

    switch (gActivePage) {
        case 0: {  // Neko page
            if (input.missionUp) {
                feedPet();
            }
            if (input.missionDown) {
                petPet();
            }
            break;
        }
        case 1: {  // Operations page
            if (input.missionUp) {
                cycleMissionSelection(-1);
            }
            if (input.missionDown) {
                cycleMissionSelection(+1);
            }
            if (input.macroLeft) {
                cycleMacroSelection(-1);
            }
            if (input.macroRight) {
                cycleMacroSelection(+1);
            }
            if (input.toggleMacro) {
                executeMacro(gSelectedMacro);
            }
            break;
        }
        case 2: {
            // Currently no additional per-input behaviour for signals page
            break;
        }
        default:
            break;
    }

    if (input.feedPet) {
        feedPet();
    }
    if (input.petPet) {
        petPet();
    }
}

void tick(uint32_t now) {
    if (!gIsActive) {
        return;
    }
    if (!gMenuOverlayActive && now - gLastDialTick >= kDialTickMs) {
        gLastDialTick = now;
        gDialPhase = (gDialPhase + 1) % 60;
        requestRedraw();
    }
    if (gNeedsRedraw) {
        render();
    }
}

void forceRender() {
    render();
}

void syncPetStats(int energy, int happiness, int moodIndex) {
    gPetEnergy = clampValue(energy, 0, 100);
    gPetHappiness = clampValue(happiness, 0, 100);
    gPetMoodIndex = clampValue(moodIndex, 0, static_cast<int>(kMoodLabelCount) - 1);
    requestRedraw();
}

void syncWifiStrength(int strength) {
    gWifiStrength = clampValue(strength, 0, 100);
    requestRedraw();
}

void setMissionStatus(size_t index, const String &status) {
    setMissionStatusInternal(index, status);
}

void setMissionProgress(size_t index, uint8_t progress) {
    if (index >= kMissionCount) {
        return;
    }
    const uint8_t safeProgress = static_cast<uint8_t>(clampValue<int>(progress, 0, 100));
    gMissions[index].progress = safeProgress;
    requestRedraw();
}

void adjustMissionProgress(size_t index, int delta) {
    adjustMissionProgressInternal(index, delta);
}

void logEvent(const String &line, uint16_t color) {
    appendConsoleLine(buildTimestamped(line), color);
}

void setMacroActive(size_t index, bool active) {
    if (index >= kMacroCount) {
        return;
    }
    gMacros[index].active = active;
    requestRedraw();
}

void setMacroResult(size_t index, const String &text) {
    if (index >= kMacroCount) {
        return;
    }
    gMacros[index].result = text;
    requestRedraw();
}

void setMenuOverlay(bool active) {
    if (gMenuOverlayActive == active) {
        return;
    }
    gMenuOverlayActive = active;
    if (!active) {
        requestRedraw();
    }
}

} // namespace stage5
