#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace stage5 {

struct Input {
    bool missionUp = false;
    bool missionDown = false;
    bool macroLeft = false;
    bool macroRight = false;
    bool toggleMacro = false;
    bool feedPet = false;
    bool petPet = false;
    bool enter = false;
    bool nextPage = false;
    bool prevPage = false;
};

void init();
void enter(const String &petName, int petEnergy, int petHappiness, int moodIndex, int wifiStrength);
void exit();
void handleInput(const Input &input);
void tick(uint32_t now);
void forceRender();
void syncPetStats(int energy, int happiness, int moodIndex);
void syncWifiStrength(int strength);
void setMissionStatus(size_t index, const String &status);
void setMissionProgress(size_t index, uint8_t progress);
void adjustMissionProgress(size_t index, int delta);
void logEvent(const String &line, uint16_t color);
void setMacroActive(size_t index, bool active);
void setMacroResult(size_t index, const String &text);
void setMenuOverlay(bool active);

}  // namespace stage5
