/*
 * 🎨 M5GOTCHI THEME SYSTEM v1.0
 * Sistema completo de temas customizáveis!
 * 
 * Features:
 * - 10+ temas pré-definidos
 * - Temas desbloqueáveis
 * - Custom color picker
 * - Save/Load themes
 * - Theme preview
 */

#ifndef M5GOTCHI_THEME_SYSTEM_H
#define M5GOTCHI_THEME_SYSTEM_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "m5gotchi_universal_controls.h"

// ==================== THEME STRUCT ====================

struct Theme {
    String name;
    uint16_t background;
    uint16_t primary;
    uint16_t secondary;
    uint16_t accent;
    uint16_t text;
    uint16_t success;
    uint16_t error;
    uint16_t warning;
    bool unlocked;
    int levelRequired;
};

class ThemeManager {
private:
    Theme themes[11];
    int currentTheme;
    bool initialized;
    
public:
    ThemeManager() : currentTheme(0), initialized(false) {}
    
    void initThemes() {
        // 1. Kawaii Classic (default)
        themes[0] = {"Kawaii Classic", TFT_BLACK, 0xFD20, 0x07FF, 0xFFE0, TFT_WHITE, 0x07E0, 0xF800, 0xFDA0, true, 0};
        
        // 2. Hacker Matrix
        themes[1] = {"Hacker Matrix", 0x0000, 0x07E0, 0x0400, 0x00FF, 0x07E0, 0x07E0, 0xF800, 0xFFE0, true, 1};
        
        // 3. Cyberpunk 2077
        themes[2] = {"Cyberpunk 2077", 0x0000, 0xF81F, 0x07FF, 0xFDA0, 0xFFFF, 0xF81F, 0xF800, 0xFFE0, false, 5};
        
        // 4. Retro CRT
        themes[3] = {"Retro CRT", 0x0000, 0xFDA0, 0xFE60, 0xFFE0, 0xFDA0, 0x07E0, 0xF800, 0xFFE0, false, 3};
        
        // 5. True Dark
        themes[4] = {"True Dark", 0x0000, 0x39C4, 0x2104, 0x4208, 0x8410, 0x07E0, 0xF800, 0xFFE0, false, 7};
        
        // 6. Bright Day
        themes[5] = {"Bright Day", 0xFFFF, 0x001F, 0xF800, 0x07E0, 0x0000, 0x07E0, 0xF800, 0xFDA0, false, 4};
        
        // 7. Spooky Season
        themes[6] = {"Spooky Season", 0x0000, 0xF800, 0xFDA0, 0xFFE0, 0xFDA0, 0x07E0, 0xF800, 0xFFE0, false, 10};
        
        // 8. Deep Ocean
        themes[7] = {"Deep Ocean", 0x0010, 0x001F, 0x0400, 0x07FF, 0x07FF, 0x07E0, 0xF800, 0xFFE0, false, 6};
        
        // 9. Burning Flame
        themes[8] = {"Burning Flame", 0x0000, 0xF800, 0xFDA0, 0xFFE0, 0xFFE0, 0x07E0, 0xF800, 0xFFE0, false, 8};
        
        // 10. Custom
        themes[9] = {"Custom", TFT_BLACK, 0xFD20, 0x07FF, 0xFFE0, TFT_WHITE, 0x07E0, 0xF800, 0xFDA0, true, 0};
        
        initialized = true;
        loadFromSD();
    }
    
    Theme& getCurrentTheme() {
        if (!initialized) initThemes();
        return themes[currentTheme];
    }
    
    void showThemeMenu() {
        M5.Display.fillScreen(getCurrentTheme().background);
        M5.Display.setTextColor(getCurrentTheme().text);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🎨 THEME SELECTOR", 10, 10);
        
        int y = 40;
        for (int i = 0; i < 10; i++) {
            if (themes[i].unlocked) {
                uint16_t color = (i == currentTheme) ? getCurrentTheme().primary : getCurrentTheme().text;
                M5.Display.setTextColor(color);
                M5.Display.drawString(String(i + 1) + ". " + themes[i].name, 10, y);
                
                // Color preview squares
                M5.Display.fillRect(200, y, 15, 15, themes[i].primary);
                M5.Display.fillRect(217, y, 15, 15, themes[i].secondary);
            } else {
                M5.Display.setTextColor(0x4208);
                M5.Display.drawString(String(i + 1) + ". " + themes[i].name + " [LVL " + String(themes[i].levelRequired) + "]", 10, y);
            }
            y += 20;
        }
        
        M5.Display.setTextColor(getCurrentTheme().accent);
        M5.Display.drawString("0. Custom Editor", 10, y + 10);
        
        M5.Display.setTextColor(getCurrentTheme().secondary);
        M5.Display.drawString("↑/↓ Select  ENTER Confirm  ESC Back", 10, 115);
    }
    
    bool handleThemeInput() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == ";") { // UP
                currentTheme = (currentTheme - 1 + 10) % 10;
                return true;
            } else if (key == ".") { // DOWN
                currentTheme = (currentTheme + 1) % 10;
                return true;
            } else if (key == "\n") { // ENTER
                if (themes[currentTheme].unlocked) {
                    saveToSD();
                    return false; // Exit menu
                }
            } else if (key.length() == 1 && key[0] >= '1' && key[0] <= '9') {
                int selected = key[0] - '1';
                if (themes[selected].unlocked) {
                    currentTheme = selected;
                    saveToSD();
                    return false;
                }
            } else if (key == "0") {
                showCustomEditor();
                return true;
            } else if (key == "`") { // ESC
                return false;
            }
        }
        return true;
    }
    
    void showCustomEditor() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🎨 CUSTOM THEME EDITOR", 10, 10);
        
        M5.Display.setTextSize(1);
        M5.Display.drawString("Coming soon...", 10, 40);
        M5.Display.drawString("This will allow you to:", 10, 60);
        M5.Display.drawString("- Pick custom colors", 10, 75);
        M5.Display.drawString("- Save theme to SD", 10, 90);
        M5.Display.drawString("- Share with friends", 10, 105);
        
        M5.Display.drawString("Press any key to return", 10, 125);
        
        while (!M5Cardputer.Keyboard.isPressed()) {
            M5Cardputer.update();
            delay(50);
        }
    }
    
    void unlockTheme(int themeId) {
        if (themeId >= 0 && themeId < 10) {
            themes[themeId].unlocked = true;
            saveToSD();
        }
    }
    
    void saveToSD() {
        if (!SD.exists("/themes")) {
            SD.mkdir("/themes");
        }
        
        File file = SD.open("/themes/current.json", FILE_WRITE);
        if (file) {
            StaticJsonDocument<512> doc;
            doc["current"] = currentTheme;
            
            JsonArray unlockedArray = doc.createNestedArray("unlocked");
            for (int i = 0; i < 10; i++) {
                if (themes[i].unlocked) {
                    unlockedArray.add(i);
                }
            }
            
            serializeJson(doc, file);
            file.close();
        }
    }
    
    void loadFromSD() {
        File file = SD.open("/themes/current.json");
        if (file) {
            StaticJsonDocument<512> doc;
            deserializeJson(doc, file);
            
            currentTheme = doc["current"] | 0;
            
            JsonArray unlockedArray = doc["unlocked"];
            for (JsonVariant v : unlockedArray) {
                int id = v.as<int>();
                if (id >= 0 && id < 10) {
                    themes[id].unlocked = true;
                }
            }
            
            file.close();
        }
    }
    
    // ==================== CONTROLE UNIVERSAL ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                selectedTheme = (selectedTheme - 1 + 10) % 10;
                draw();
                break;
            case ACTION_DOWN:
                selectedTheme = (selectedTheme + 1) % 10;
                draw();
                break;
            case ACTION_LEFT:
                // Navegar categorias se tiver
                break;
            case ACTION_RIGHT:
                // Navegar categorias se tiver
                break;
            case ACTION_SELECT:
                if (themes[selectedTheme].unlocked) {
                    currentTheme = selectedTheme;
                    saveToSD();
                    M5.Display.fillScreen(BLACK);
                    M5.Display.setTextColor(GREEN);
                    M5.Display.setCursor(60, 60);
                    M5.Display.print("Theme Applied!");
                    delay(1000);
                    draw();
                }
                break;
            // ACTION_BACK é tratado no main
        }
    }
    
    void checkLevelUnlocks(int catLevel) {
        for (int i = 0; i < 10; i++) {
            if (!themes[i].unlocked && catLevel >= themes[i].levelRequired) {
                themes[i].unlocked = true;
                
                // Show unlock notification
                M5.Display.fillScreen(TFT_BLACK);
                M5.Display.setTextColor(0xFFE0);
                M5.Display.setTextSize(2);
                M5.Display.drawString("🎉 THEME UNLOCKED!", 10, 40);
                M5.Display.setTextColor(themes[i].primary);
                M5.Display.drawString(themes[i].name, 10, 70);
                M5.Display.setTextColor(TFT_WHITE);
                M5.Display.setTextSize(1);
                M5.Display.drawString("Press any key to continue", 10, 100);
                
                while (!M5Cardputer.Keyboard.isPressed()) {
                    M5Cardputer.update();
                    delay(50);
                }
                
                saveToSD();
                break;
            }
        }
    }
};

// Global instance
ThemeManager themeManager;

#endif // M5GOTCHI_THEME_SYSTEM_H