/*
 * 🍣 M5GOTCHI SUSHI SQL INJECTION GAME v1.0
 * Mini-game educativo para aprender SQL injection de forma kawaii!
 * 
 * Como jogar:
 * - Escolha seus hashis (chopsticks) = técnica de bypass
 * - Adicione wasabi = payload de injection
 * - "Coma" o sushi = execute a query
 * - Objetivo: Acessar dados secretos do "restaurante"
 * 
 * Educational & Fun! 🎌
 */

#ifndef M5GOTCHI_SUSHI_SQL_GAME_H
#define M5GOTCHI_SUSHI_SQL_GAME_H

#include <M5Unified.h>
#include <vector>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_neko_sounds.h"

// ==================== GAME ENUMS ====================
enum SushiType {
    SALMON_ROLL = 0,     // 🍣 Basic SELECT query
    TUNA_ROLL,           // 🍤 WHERE clause
    EEL_ROLL,            // 🍱 JOIN operation  
    DRAGON_ROLL,         // 🐉 Complex query
    RAINBOW_ROLL,        // 🌈 UNION injection
    CALIFORNIA_ROLL,     // 🥑 ORDER BY injection
    SPICY_ROLL          // 🌶️ Blind SQL injection
};

enum ChopsticksType {
    BAMBOO_STICKS = 0,   // Basic injection
    SILVER_STICKS,       // URL encoding bypass
    GOLDEN_STICKS,       // Hex encoding bypass
    DIAMOND_STICKS,      // Unicode bypass
    NINJA_STICKS,        // Time-based blind
    CYBER_STICKS,        // Boolean-based blind
    KAWAII_STICKS       // Union-based injection
};

enum WasabiLevel {
    NO_WASABI = 0,       // No payload
    MILD_WASABI,         // ' OR 1=1--
    MEDIUM_WASABI,       // UNION SELECT
    HOT_WASABI,          // Stacked queries
    NUCLEAR_WASABI       // Advanced payloads
};

enum GameState {
    GAME_MENU = 0,
    CHOPSTICKS_SELECT,
    WASABI_SELECT,
    SUSHI_SELECT,
    EATING_ANIMATION,
    RESULT_SCREEN,
    HIGH_SCORES
};

// ==================== GAME STRUCTURES ====================
struct SushiItem {
    String name;
    String emoji;
    String table_name;
    String vulnerability;
    int difficulty;
    bool unlocked;
};

struct Chopsticks {
    String name;
    String emoji;
    String bypass_method;
    String description;
    int effectiveness;
    bool unlocked;
};

struct WasabiPayload {
    String name;
    String emoji;
    String payload;
    String description;
    int power_level;
    bool unlocked;
};

struct GameScore {
    String player_name;
    int total_score;
    int sushi_eaten;
    int tables_pwned;
    String favorite_combo;
};

// ==================== SUSHI SQL GAME CLASS ====================
class M5GotchiSushiSQLGame {
private:
    GameState currentState;
    int selectedItem;
    int currentScore;
    int sushiEaten;
    int tablesAccessed;
    
    // Game items
    SushiItem sushiMenu[7];
    Chopsticks chopstickTypes[7];
    WasabiPayload wasabiLevels[5];
    
    // Current selection
    int selectedSushi;
    int selectedChopsticks;
    int selectedWasabi;
    
    // Game results
    bool injectionSuccessful;
    String extractedData;
    int pointsEarned;
    
    // High scores
    std::vector<GameScore> highScores;
    
    // Animation
    int animationFrame;
    unsigned long lastAnimUpdate;
    
    // Sound system reference
    M5GotchiNekoSounds* nekoSounds;

public:
    M5GotchiSushiSQLGame() {
        currentState = GAME_MENU;
        selectedItem = 0;
        currentScore = 0;
        sushiEaten = 0;
        tablesAccessed = 0;
        
        selectedSushi = 0;
        selectedChopsticks = 0;  
        selectedWasabi = 0;
        
        injectionSuccessful = false;
        pointsEarned = 0;
        
        animationFrame = 0;
        lastAnimUpdate = 0;
        nekoSounds = nullptr;
        
        initGameItems();
        loadHighScores();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🍣 Initializing Sushi SQL Injection Game...");
        
        initGameItems();
        currentState = GAME_MENU;
        
        Serial.println("✅ Sushi SQL Game Ready! Itadakimasu! 🍴");
    }
    
    void setSoundSystem(M5GotchiNekoSounds* sounds) {
        nekoSounds = sounds;
    }
    
    void initGameItems() {
        // Initialize sushi menu
        sushiMenu[SALMON_ROLL] = {
            "Salmon Roll", "🍣", "users", "Basic SELECT", 1, true
        };
        sushiMenu[TUNA_ROLL] = {
            "Tuna Roll", "🍤", "passwords", "WHERE bypass", 2, true
        };
        sushiMenu[EEL_ROLL] = {
            "Eel Roll", "🍱", "admin_logs", "JOIN injection", 3, false
        };
        sushiMenu[DRAGON_ROLL] = {
            "Dragon Roll", "🐉", "secret_data", "Complex query", 4, false
        };
        sushiMenu[RAINBOW_ROLL] = {
            "Rainbow Roll", "🌈", "all_tables", "UNION attack", 5, false
        };
        sushiMenu[CALIFORNIA_ROLL] = {
            "California Roll", "🥑", "system_info", "ORDER BY", 3, false
        };
        sushiMenu[SPICY_ROLL] = {
            "Spicy Roll", "🌶️", "hidden_secrets", "Blind injection", 5, false
        };
        
        // Initialize chopsticks
        chopstickTypes[BAMBOO_STICKS] = {
            "Bamboo Sticks", "🎋", "Direct injection", "Basic bypass", 1, true
        };
        chopstickTypes[SILVER_STICKS] = {
            "Silver Sticks", "⚪", "URL encoding", "%27 OR %31=%31", 2, true
        };
        chopstickTypes[GOLDEN_STICKS] = {
            "Golden Sticks", "🟡", "Hex encoding", "0x27204F5220313D31", 3, false
        };
        chopstickTypes[DIAMOND_STICKS] = {
            "Diamond Sticks", "💎", "Unicode bypass", "\\u0027 OR \\u0031=\\u0031", 4, false
        };
        chopstickTypes[NINJA_STICKS] = {
            "Ninja Sticks", "🥷", "Time-based blind", "IF(condition, SLEEP(5), 0)", 4, false
        };
        chopstickTypes[CYBER_STICKS] = {
            "Cyber Sticks", "🤖", "Boolean blind", "AND 1=(SELECT COUNT(*) FROM users)", 3, false
        };
        chopstickTypes[KAWAII_STICKS] = {
            "Kawaii Sticks", "💕", "Union-based", "UNION SELECT kawaii_data", 5, false
        };
        
        // Initialize wasabi payloads
        wasabiLevels[NO_WASABI] = {
            "No Wasabi", "⚪", "", "Plain query", 0, true
        };
        wasabiLevels[MILD_WASABI] = {
            "Mild Wasabi", "🟢", "' OR 1=1--", "Basic OR bypass", 1, true
        };
        wasabiLevels[MEDIUM_WASABI] = {
            "Medium Wasabi", "🟡", "' UNION SELECT * FROM", "Union injection", 3, true
        };
        wasabiLevels[HOT_WASABI] = {
            "Hot Wasabi", "🟠", "'; DROP TABLE users;--", "Destructive payload", 4, false
        };
        wasabiLevels[NUCLEAR_WASABI] = {
            "Nuclear Wasabi", "🔴", "'; EXEC xp_cmdshell('calc');--", "RCE payload", 5, false
        };
    }
    
    // ==================== GAME FLOW ====================
    void start() {
        currentState = GAME_MENU;
        selectedItem = 0;
        drawGameScreen();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("excited");
        }
    }
    
    void stop() {
        saveHighScores();
        if (nekoSounds) {
            nekoSounds->playContextSound("happy");
        }
    }
    
    void update() {
        updateAnimations();
        
        // Auto-advance eating animation
        if (currentState == EATING_ANIMATION) {
            static unsigned long eatStartTime = 0;
            if (eatStartTime == 0) eatStartTime = millis();
            
            if (millis() - eatStartTime > 3000) {
                executeInjection();
                currentState = RESULT_SCREEN;
                eatStartTime = 0;
                drawGameScreen();
            }
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawGameScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentState) {
            case GAME_MENU:
                drawMainMenu();
                break;
            case CHOPSTICKS_SELECT:
                drawChopsticksSelection();
                break;
            case WASABI_SELECT:
                drawWasabiSelection();
                break;
            case SUSHI_SELECT:
                drawSushiSelection();
                break;
            case EATING_ANIMATION:
                drawEatingAnimation();
                break;
            case RESULT_SCREEN:
                drawResults();
                break;
            case HIGH_SCORES:
                drawHighScores();
                break;
        }
    }
    
    void drawMainMenu() {
        // Header
        M5.Display.setTextColor(RED);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(60, 10);
        M5.Display.print("🍣 SUSHI SQL");
        
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(50, 25);
        M5.Display.print("Injection Game!");
        
        // Menu options
        String menuItems[] = {
            "🍴 Start New Game",
            "🏆 High Scores", 
            "📚 Tutorial",
            "⚙️ Settings",
            "🚪 Exit"
        };
        
        for (int i = 0; i < 5; i++) {
            uint16_t color = (i == selectedItem) ? GREEN : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 50 + i * 12);
            M5.Display.print(menuItems[i]);
        }
        
        // Stats
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 115);
        M5.Display.printf("Score: %d | Sushi: %d | Tables: %d", 
                         currentScore, sushiEaten, tablesAccessed);
    }
    
    void drawChopsticksSelection() {
        // Header
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(50, 10);
        M5.Display.print("🥢 Choose Chopsticks");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(30, 25);
        M5.Display.print("Select your bypass method:");
        
        // Chopsticks list
        for (int i = 0; i < 4; i++) { // Show first 4 that fit on screen
            Chopsticks& sticks = chopstickTypes[i];
            
            if (!sticks.unlocked) continue;
            
            uint16_t color = (i == selectedItem) ? GREEN : 
                           (sticks.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 15);
            M5.Display.print(sticks.emoji + " " + sticks.name);
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(15, 45 + i * 15 + 8);
            M5.Display.print("Power: " + String(sticks.effectiveness) + "/5");
        }
        
        // Selected description
        if (selectedItem < 7 && chopstickTypes[selectedItem].unlocked) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 110);
            M5.Display.print(chopstickTypes[selectedItem].description);
        }
    }
    
    void drawWasabiSelection() {
        // Header  
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(70, 10);
        M5.Display.print("🟢 Add Wasabi");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(40, 25);
        M5.Display.print("Choose payload strength:");
        
        // Wasabi levels
        for (int i = 0; i < 5; i++) {
            WasabiPayload& wasabi = wasabiLevels[i];
            
            uint16_t color = (i == selectedItem) ? GREEN : 
                           (wasabi.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 12);
            M5.Display.print(wasabi.emoji + " " + wasabi.name);
            
            // Power indicator
            String powerBar = "";
            for (int p = 0; p < 5; p++) {
                powerBar += (p < wasabi.power_level) ? "🔥" : "⚪";
            }
            M5.Display.setCursor(150, 45 + i * 12);
            M5.Display.print(powerBar);
        }
        
        // Warning for dangerous payloads
        if (selectedItem >= HOT_WASABI) {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(20, 110);
            M5.Display.print("⚠️ DANGEROUS PAYLOAD!");
            M5.Display.setCursor(30, 120);
            M5.Display.print("Educational only!");
        }
    }
    
    void drawSushiSelection() {
        // Header
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(60, 10);
        M5.Display.print("🍣 Pick Sushi");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(45, 25);
        M5.Display.print("Choose your target:");
        
        // Sushi menu
        for (int i = 0; i < 4; i++) { // Show first 4
            SushiItem& sushi = sushiMenu[i];
            
            uint16_t color = (i == selectedItem) ? GREEN : 
                           (sushi.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 15);
            M5.Display.print(sushi.emoji + " " + sushi.name);
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(15, 45 + i * 15 + 8);
            M5.Display.print("Table: " + sushi.table_name + 
                           " | Difficulty: " + String(sushi.difficulty) + "/5");
        }
        
        // Current selection summary
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(10, 110);
        M5.Display.print("Combo: " + chopstickTypes[selectedChopsticks].emoji + 
                        " + " + wasabiLevels[selectedWasabi].emoji + 
                        " + " + sushiMenu[selectedItem].emoji);
    }
    
    void drawEatingAnimation() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(60, 30);
        M5.Display.print("🍴 EATING...");
        
        // Animated chopsticks
        M5.Display.setCursor(80, 50);
        M5.Display.print(chopstickTypes[selectedChopsticks].emoji);
        
        // Animated sushi
        int bounceY = 70 + sin(animationFrame * 0.3) * 5;
        M5.Display.setCursor(100, bounceY);
        M5.Display.print(sushiMenu[selectedSushi].emoji);
        
        // Executing text
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(40, 90);
        M5.Display.print("Executing injection...");
        
        // Progress dots
        String dots = "";
        for (int i = 0; i < (animationFrame / 10) % 4; i++) {
            dots += ".";
        }
        M5.Display.setCursor(180, 90);
        M5.Display.print(dots);
        
        // Fake SQL query
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(10, 110);
        M5.Display.print("SELECT * FROM " + sushiMenu[selectedSushi].table_name);
        M5.Display.setCursor(10, 120);
        M5.Display.print("WHERE id=" + wasabiLevels[selectedWasabi].payload);
    }
    
    void drawResults() {
        if (injectionSuccessful) {
            // Success screen
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(70, 20);
            M5.Display.print("🎉 SUCCESS!");
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(40, 40);
            M5.Display.print("Injection successful!");
            
            // Extracted data preview
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 60);
            M5.Display.print("Data extracted:");
            M5.Display.setCursor(10, 75);
            M5.Display.print(extractedData);
            
            // Points earned
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 95);
            M5.Display.printf("Points earned: +%d", pointsEarned);
            
        } else {
            // Failure screen
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(70, 30);
            M5.Display.print("💥 FAILED!");
            
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(30, 50);
            M5.Display.print("Injection blocked!");
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(20, 70);
            M5.Display.print("Try different technique");
        }
        
        // Continue prompt
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(40, 115);
        M5.Display.print("Press ⏎ to continue");
    }
    
    void drawHighScores() {
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(70, 10);
        M5.Display.print("🏆 HIGH SCORES");
        
        for (int i = 0; i < min(5, (int)highScores.size()); i++) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 30 + i * 15);
            M5.Display.printf("%d. %s", i + 1, highScores[i].player_name.c_str());
            
            M5.Display.setCursor(100, 30 + i * 15);
            M5.Display.printf("%d pts", highScores[i].total_score);
            
            M5.Display.setTextColor(CYAN); 
            M5.Display.setCursor(15, 30 + i * 15 + 8);
            M5.Display.printf("Sushi: %d | Tables: %d", 
                            highScores[i].sushi_eaten, highScores[i].tables_pwned);
        }
    }
    
    // ==================== GAME LOGIC ====================
    void executeInjection() {
        Chopsticks& sticks = chopstickTypes[selectedChopsticks];
        WasabiPayload& wasabi = wasabiLevels[selectedWasabi];
        SushiItem& sushi = sushiMenu[selectedSushi];
        
        // Calculate success probability
        int totalPower = sticks.effectiveness + wasabi.power_level;
        int requiredPower = sushi.difficulty * 2;
        
        int successChance = (totalPower * 100) / max(requiredPower, 1);
        successChance = constrain(successChance, 10, 95); // 10-95% range
        
        injectionSuccessful = (random(0, 100) < successChance);
        
        if (injectionSuccessful) {
            // Generate fake extracted data
            extractedData = generateFakeData(sushi.table_name);
            pointsEarned = sushi.difficulty * 100 + wasabi.power_level * 50;
            currentScore += pointsEarned;
            sushiEaten++;
            
            if (!sushi.table_name.equals("users")) {
                tablesAccessed++;
            }
            
            // Unlock new items based on progress
            unlockNewItems();
            
            if (nekoSounds) {
                nekoSounds->playContextSound("hack_success");
            }
            
        } else {
            pointsEarned = 0;
            extractedData = "";
            
            if (nekoSounds) {
                nekoSounds->playContextSound("hack_failed");
            }
        }
    }
    
    String generateFakeData(String tableName) {
        if (tableName == "users") {
            return "admin:password123, user:12345";
        } else if (tableName == "passwords") {
            return "hash:5e884898da28, salt:kawaii";
        } else if (tableName == "secret_data") {
            return "flag{sushi_sql_master}";
        } else if (tableName == "all_tables") {
            return "users,passwords,admin_logs,secrets";
        } else {
            return "sensitive_info: [CLASSIFIED]";
        }
    }
    
    void unlockNewItems() {
        // Unlock items based on score and progress
        if (currentScore >= 500) {
            sushiMenu[EEL_ROLL].unlocked = true;
            chopstickTypes[GOLDEN_STICKS].unlocked = true;
        }
        
        if (currentScore >= 1000) {
            sushiMenu[DRAGON_ROLL].unlocked = true;
            wasabiLevels[HOT_WASABI].unlocked = true;
        }
        
        if (sushiEaten >= 10) {
            chopstickTypes[NINJA_STICKS].unlocked = true;
            sushiMenu[RAINBOW_ROLL].unlocked = true;
        }
        
        if (tablesAccessed >= 5) {
            chopstickTypes[KAWAII_STICKS].unlocked = true;
            wasabiLevels[NUCLEAR_WASABI].unlocked = true;
        }
    }
    
    // ==================== CONTROLS ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                navigateUp();
                break;
            case ACTION_DOWN:
                navigateDown();
                break;
            case ACTION_LEFT:
                navigateLeft();
                break;
            case ACTION_RIGHT:
                navigateRight();
                break;
            case ACTION_SELECT:
                selectCurrentItem();
                break;
        }
        
        drawGameScreen();
    }
    
    void navigateUp() {
        if (selectedItem > 0) {
            selectedItem--;
        }
    }
    
    void navigateDown() {
        int maxItems = getMaxItemsForCurrentState();
        selectedItem = (selectedItem + 1) % maxItems;
    }
    
    void navigateLeft() {
        // Context-dependent navigation
        if (currentState == GAME_MENU) {
            // Maybe show help or previous page
        }
    }
    
    void navigateRight() {
        // Context-dependent navigation
        if (currentState == GAME_MENU) {
            // Maybe show next page or quick start
        }
    }
    
    void selectCurrentItem() {
        switch (currentState) {
            case GAME_MENU:
                handleMenuSelection();
                break;
            case CHOPSTICKS_SELECT:
                selectedChopsticks = selectedItem;
                currentState = WASABI_SELECT;
                selectedItem = 0;
                break;
            case WASABI_SELECT:
                selectedWasabi = selectedItem;
                currentState = SUSHI_SELECT;
                selectedItem = 0;
                break;
            case SUSHI_SELECT:
                selectedSushi = selectedItem;
                currentState = EATING_ANIMATION;
                animationFrame = 0;
                break;
            case RESULT_SCREEN:
                currentState = GAME_MENU;
                selectedItem = 0;
                break;
            case HIGH_SCORES:
                currentState = GAME_MENU;
                selectedItem = 0;
                break;
        }
    }
    
    void handleMenuSelection() {
        switch (selectedItem) {
            case 0: // Start New Game
                currentState = CHOPSTICKS_SELECT;
                selectedItem = 0;
                break;
            case 1: // High Scores
                currentState = HIGH_SCORES;
                selectedItem = 0;
                break;
            case 2: // Tutorial
                showTutorial();
                break;
            case 3: // Settings
                showSettings();
                break;
            case 4: // Exit
                // Handle in main application
                break;
        }
    }
    
    int getMaxItemsForCurrentState() {
        switch (currentState) {
            case GAME_MENU: return 5;
            case CHOPSTICKS_SELECT: return 7;
            case WASABI_SELECT: return 5;
            case SUSHI_SELECT: return 7;
            default: return 1;
        }
    }
    
    // ==================== ANIMATIONS ====================
    void updateAnimations() {
        if (millis() - lastAnimUpdate > 100) {
            animationFrame++;
            if (animationFrame > 1000) animationFrame = 0;
            lastAnimUpdate = millis();
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    void showTutorial() {
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(60, 10);
        M5.Display.print("🍣 TUTORIAL");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.print("1. Choose chopsticks (bypass method)");
        M5.Display.setCursor(10, 45);
        M5.Display.print("2. Add wasabi (injection payload)");
        M5.Display.setCursor(10, 60);
        M5.Display.print("3. Pick sushi (target table)");
        M5.Display.setCursor(10, 75);
        M5.Display.print("4. Eat and see if injection works!");
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 95);
        M5.Display.print("Higher power = better success rate");
        
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(40, 115);
        M5.Display.print("Press any key to continue");
        
        delay(5000);
    }
    
    void showSettings() {
        // Placeholder for settings screen
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(80, 60);
        M5.Display.print("Settings");
        M5.Display.setCursor(60, 80);
        M5.Display.print("Coming Soon!");
        delay(2000);
    }
    
    void saveHighScores() {
        // Placeholder - would save to SD card
        Serial.println("💾 Saving high scores...");
    }
    
    void loadHighScores() {
        // Placeholder - would load from SD card
        Serial.println("📂 Loading high scores...");
        
        // Add some demo scores
        highScores.push_back({"NekoMaster", 2500, 25, 8, "💎+🔴+🐉"});
        highScores.push_back({"SushiHacker", 1800, 18, 5, "🥷+🟡+🌈"});
        highScores.push_back({"KawaiiSQL", 1200, 12, 3, "💕+🟢+🍱"});
    }
    
    // ==================== GETTERS ====================
    GameState getCurrentState() { return currentState; }
    int getCurrentScore() { return currentScore; }
    int getSushiEaten() { return sushiEaten; }
    int getTablesAccessed() { return tablesAccessed; }
    bool isGameActive() { return currentState != GAME_MENU; }
};

#endif // M5GOTCHI_SUSHI_SQL_GAME_H