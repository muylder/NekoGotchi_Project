/*
 * 🐾 M5GOTCHI PAW SCANNER BYPASS GAME v1.0
 * Mini-game educativo para aprender bypass de biometria com patas kawaii!
 * 
 * Como jogar:
 * - Observe o padrão de patas do "sistema de segurança"
 * - Use diferentes técnicas de bypass (replay, spoofing, etc.)
 * - Toque nas patas na sequência certa ou use bypass
 * - Objetivo: Acessar sistemas protegidos por biometria
 * 
 * Educational & Adorable! 🐱
 */

#ifndef M5GOTCHI_PAW_SCANNER_GAME_H
#define M5GOTCHI_PAW_SCANNER_GAME_H

#include <M5Unified.h>
#include <vector>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_neko_sounds.h"

// ==================== GAME ENUMS ====================
enum PawType {
    CAT_PAW = 0,        // 🐾 Basic biometric
    DOG_PAW,            // 🐕 Medium security
    TIGER_PAW,          // 🐅 High security
    DRAGON_PAW,         // 🐉 Military grade
    ROBOT_PAW,          // 🤖 AI-enhanced
    RAINBOW_PAW,        // 🌈 Quantum biometric
    KAWAII_PAW         // 💖 Ultimate security
};

enum BypassMethod {
    DIRECT_TOUCH = 0,   // Touch pattern normally
    REPLAY_ATTACK,      // Record and replay
    SILICONE_MOLD,      // Physical bypass
    THERMAL_IMAGING,    // Heat signature
    FREQUENCY_JAMMING,  // RF interference
    SOCIAL_ENGINEERING, // Trick the system
    QUANTUM_TUNNELING,  // Advanced physics exploit
    KAWAII_OVERRIDE    // Power of cuteness
};

enum ScannerSecurity {
    BASIC_SCANNER = 0,  // Simple pattern matching
    PRESSURE_SCANNER,   // Pressure + pattern
    THERMAL_SCANNER,    // Heat + pattern  
    MULTI_FACTOR,       // Multiple biometrics
    AI_SCANNER,         // Machine learning
    QUANTUM_SCANNER,    // Quantum encryption
    KAWAII_SCANNER     // Cuteness detection
};

enum GamePhase {
    MENU_PHASE = 0,
    TARGET_SELECTION,
    METHOD_SELECTION,
    PATTERN_OBSERVATION,
    BYPASS_EXECUTION,
    SCANNING_ANIMATION,
    RESULT_PHASE,
    STATISTICS_PHASE
};

// ==================== GAME STRUCTURES ====================
struct PawPattern {
    int sequence[8];    // Up to 8 paw positions
    int length;
    int timing[8];      // Timing between presses
    PawType pawType;
    String description;
    int difficulty;
};

struct ScannerTarget {
    String name;
    String emoji;
    ScannerSecurity security_level;
    PawPattern required_pattern;
    String protected_data;
    int reward_points;
    bool unlocked;
};

struct BypassTechnique {
    String name;
    String emoji;
    String description;
    int effectiveness;
    int complexity;
    bool unlocked;
    String scientific_basis;
};

struct GameStats {
    int total_scans;
    int successful_bypasses;
    int failed_attempts;
    int highest_security_beaten;
    String favorite_technique;
    int total_score;
};

// ==================== PAW POSITIONS ====================
struct PawPosition {
    int x, y;
    PawType type;
    bool is_pressed;
    unsigned long press_time;
    String emoji;
};

// ==================== PAW SCANNER GAME CLASS ====================
class M5GotchiPawScannerGame {
private:
    GamePhase currentPhase;
    int selectedItem;
    
    // Game targets
    ScannerTarget scanners[7];
    BypassTechnique techniques[8];
    
    // Current game state
    int selectedScanner;
    int selectedTechnique;
    PawPattern currentPattern;
    std::vector<int> playerInput;
    std::vector<unsigned long> playerTiming;
    
    // Visual elements
    PawPosition pawPositions[9]; // 3x3 grid
    int animationFrame;
    unsigned long lastAnimUpdate;
    bool showingPattern;
    int patternStep;
    
    // Game results
    bool bypassSuccessful;
    String accessedData;
    int pointsEarned;
    GameStats stats;
    
    // Sound system
    M5GotchiNekoSounds* nekoSounds;
    
    // Colors for different paw types
    uint16_t pawColors[7] = {
        ORANGE,    // Cat paw
        BROWN,     // Dog paw
        YELLOW,    // Tiger paw
        RED,       // Dragon paw
        CYAN,      // Robot paw
        MAGENTA,   // Rainbow paw
        PINK       // Kawaii paw
    };

public:
    M5GotchiPawScannerGame() {
        currentPhase = MENU_PHASE;
        selectedItem = 0;
        selectedScanner = 0;
        selectedTechnique = 0;
        showingPattern = false;
        patternStep = 0;
        bypassSuccessful = false;
        pointsEarned = 0;
        animationFrame = 0;
        lastAnimUpdate = 0;
        nekoSounds = nullptr;
        
        initGameData();
        initPawPositions();
        loadStats();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🐾 Initializing Paw Scanner Bypass Game...");
        
        initGameData();
        initPawPositions();
        currentPhase = MENU_PHASE;
        
        Serial.println("✅ Paw Scanner Game Ready! *paw print* 🐾");
    }
    
    void setSoundSystem(M5GotchiNekoSounds* sounds) {
        nekoSounds = sounds;
    }
    
    void initGameData() {
        // Initialize scanner targets
        scanners[0] = {
            "Pet Door", "🚪", BASIC_SCANNER, 
            {{0, 4, 8}, 3, {500, 500, 500}, CAT_PAW, "Simple 3-paw unlock", 1},
            "Pet treats inventory", 100, true
        };
        
        scanners[1] = {
            "Laptop Lock", "💻", PRESSURE_SCANNER,
            {{1, 3, 5, 7}, 4, {400, 300, 400, 300}, DOG_PAW, "Cross pattern", 2},
            "Personal files access", 250, true
        };
        
        scanners[2] = {
            "Safe Vault", "🏦", THERMAL_SCANNER,
            {{0, 1, 2, 5, 8}, 5, {300, 200, 300, 200, 400}, TIGER_PAW, "L-shape pattern", 3},
            "Banking credentials", 500, false
        };
        
        scanners[3] = {
            "Military Base", "🏛️", MULTI_FACTOR,
            {{2, 0, 6, 8, 4, 1}, 6, {250, 250, 250, 250, 250, 250}, DRAGON_PAW, "Complex zigzag", 4},
            "Classified documents", 1000, false
        };
        
        scanners[4] = {
            "AI Lab", "🤖", AI_SCANNER,
            {{4, 0, 2, 6, 8, 3, 5}, 7, {200, 150, 200, 150, 200, 150, 300}, ROBOT_PAW, "Star pattern", 5},
            "AI research data", 1500, false
        };
        
        scanners[5] = {
            "Quantum Vault", "⚛️", QUANTUM_SCANNER,
            {{1, 7, 3, 5, 0, 8, 2, 6}, 8, {100, 100, 100, 100, 100, 100, 100, 100}, RAINBOW_PAW, "Quantum entanglement", 6},
            "Quantum algorithms", 2500, false
        };
        
        scanners[6] = {
            "Kawaii Fortress", "🏰", KAWAII_SCANNER,
            {{4, 1, 3, 5, 7}, 5, {500, 300, 200, 300, 500}, KAWAII_PAW, "Heart pattern", 5},
            "Ultimate cuteness secrets", 3000, false
        };
        
        // Initialize bypass techniques
        techniques[DIRECT_TOUCH] = {
            "Direct Touch", "👆", "Touch the pattern normally", 1, 1, true,
            "Basic biometric authentication"
        };
        
        techniques[REPLAY_ATTACK] = {
            "Replay Attack", "🔄", "Record and replay the pattern", 3, 2, true,
            "Captures and replays biometric data"
        };
        
        techniques[SILICONE_MOLD] = {
            "Silicone Mold", "🧤", "Physical fingerprint bypass", 4, 3, false,
            "3D printed or molded fake biometrics"
        };
        
        techniques[THERMAL_IMAGING] = {
            "Thermal Imaging", "🌡️", "Detect heat signatures", 5, 4, false,
            "Uses residual heat to reveal patterns"
        };
        
        techniques[FREQUENCY_JAMMING] = {
            "RF Jamming", "📡", "Interfere with scanner signals", 3, 5, false,
            "Electromagnetic frequency interference"
        };
        
        techniques[SOCIAL_ENGINEERING] = {
            "Social Engineering", "🗣️", "Trick system admin", 6, 2, false,
            "Psychological manipulation techniques"
        };
        
        techniques[QUANTUM_TUNNELING] = {
            "Quantum Tunneling", "⚛️", "Quantum physics exploit", 8, 6, false,
            "Exploits quantum mechanical properties"
        };
        
        techniques[KAWAII_OVERRIDE] = {
            "Kawaii Override", "💖", "Power of absolute cuteness", 10, 1, false,
            "Overwhelming kawaii energy bypass"
        };
    }
    
    void initPawPositions() {
        // Setup 3x3 grid of paw positions
        for (int i = 0; i < 9; i++) {
            int row = i / 3;
            int col = i % 3;
            
            pawPositions[i].x = 50 + col * 60;
            pawPositions[i].y = 40 + row * 30;
            pawPositions[i].type = CAT_PAW;
            pawPositions[i].is_pressed = false;
            pawPositions[i].press_time = 0;
            pawPositions[i].emoji = "🐾";
        }
    }
    
    // ==================== GAME FLOW ====================
    void start() {
        currentPhase = MENU_PHASE;
        selectedItem = 0;
        drawGameScreen();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("curious");
        }
    }
    
    void stop() {
        saveStats();
        if (nekoSounds) {
            nekoSounds->playContextSound("happy");
        }
    }
    
    void update() {
        updateAnimations();
        updatePawStates();
        
        // Auto-advance pattern observation
        if (currentPhase == PATTERN_OBSERVATION && showingPattern) {
            static unsigned long patternStartTime = 0;
            if (patternStartTime == 0) patternStartTime = millis();
            
            PawPattern& pattern = scanners[selectedScanner].required_pattern;
            unsigned long elapsed = millis() - patternStartTime;
            
            // Show each step of the pattern
            if (patternStep < pattern.length) {
                if (elapsed > patternStep * 800) { // 800ms per step
                    // Highlight current paw
                    int pawIndex = pattern.sequence[patternStep];
                    pawPositions[pawIndex].is_pressed = true;
                    pawPositions[pawIndex].press_time = millis();
                    patternStep++;
                    
                    if (nekoSounds) {
                        nekoSounds->playMeow();
                    }
                }
            } else if (elapsed > pattern.length * 800 + 2000) {
                // Pattern shown completely, start bypass phase
                showingPattern = false;
                patternStep = 0;
                patternStartTime = 0;
                currentPhase = BYPASS_EXECUTION;
                clearPawStates();
                drawGameScreen();
            }
        }
        
        // Auto-advance scanning animation
        if (currentPhase == SCANNING_ANIMATION) {
            static unsigned long scanStartTime = 0;
            if (scanStartTime == 0) scanStartTime = millis();
            
            if (millis() - scanStartTime > 3000) {
                executeBypass();
                currentPhase = RESULT_PHASE;
                scanStartTime = 0;
                drawGameScreen();
            }
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawGameScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentPhase) {
            case MENU_PHASE:
                drawMainMenu();
                break;
            case TARGET_SELECTION:
                drawTargetSelection();
                break;
            case METHOD_SELECTION:
                drawMethodSelection();
                break;
            case PATTERN_OBSERVATION:
                drawPatternObservation();
                break;
            case BYPASS_EXECUTION:
                drawBypassExecution();
                break;
            case SCANNING_ANIMATION:
                drawScanningAnimation();
                break;
            case RESULT_PHASE:
                drawResults();
                break;
            case STATISTICS_PHASE:
                drawStatistics();
                break;
        }
    }
    
    void drawMainMenu() {
        // Header
        M5.Display.setTextColor(CYAN);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(40, 10);
        M5.Display.print("🐾 PAW SCANNER");
        
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(60, 25);
        M5.Display.print("Bypass Game!");
        
        // Menu options
        String menuItems[] = {
            "🎯 Start Mission",
            "📊 Statistics",
            "📚 Learn Techniques", 
            "⚙️ Settings",
            "🚪 Exit"
        };
        
        for (int i = 0; i < 5; i++) {
            uint16_t color = (i == selectedItem) ? GREEN : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 50 + i * 12);
            M5.Display.print(menuItems[i]);
        }
        
        // Current stats
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(10, 115);
        M5.Display.printf("Score: %d | Success: %d/%d", 
                         stats.total_score, stats.successful_bypasses, stats.total_scans);
    }
    
    void drawTargetSelection() {
        // Header
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(50, 10);
        M5.Display.print("🎯 Select Target");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(30, 25);
        M5.Display.print("Choose scanner to bypass:");
        
        // Target list (show first 4)
        for (int i = 0; i < 4; i++) {
            ScannerTarget& target = scanners[i];
            
            uint16_t color = (i == selectedItem) ? GREEN : 
                           (target.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 15);
            M5.Display.print(target.emoji + " " + target.name);
            
            // Security level indicator
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(15, 45 + i * 15 + 8);
            String securityBars = "";
            for (int s = 0; s < target.security_level + 1; s++) {
                securityBars += "🔒";
            }
            M5.Display.print("Security: " + securityBars);
        }
        
        // Reward preview
        if (selectedItem < 7) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 110);
            M5.Display.printf("Reward: %d pts", scanners[selectedItem].reward_points);
            M5.Display.setCursor(10, 120);
            M5.Display.print("Data: " + scanners[selectedItem].protected_data);
        }
    }
    
    void drawMethodSelection() {
        // Header
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(40, 10);
        M5.Display.print("🔧 Choose Method");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(30, 25);
        M5.Display.print("Select bypass technique:");
        
        // Show available techniques (first 4)
        for (int i = 0; i < 4; i++) {
            BypassTechnique& tech = techniques[i];
            
            uint16_t color = (i == selectedItem) ? GREEN : 
                           (tech.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 12);
            M5.Display.print(tech.emoji + " " + tech.name);
            
            // Effectiveness bar
            M5.Display.setCursor(150, 45 + i * 12);
            String effectBar = "";
            for (int e = 0; e < tech.effectiveness && e < 5; e++) {
                effectBar += "⭐";
            }
            M5.Display.print(effectBar);
        }
        
        // Selected technique description
        if (selectedItem < 8 && techniques[selectedItem].unlocked) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 100);
            M5.Display.print(techniques[selectedItem].description);
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 110);
            M5.Display.print("Science: " + techniques[selectedItem].scientific_basis);
        }
    }
    
    void drawPatternObservation() {
        // Header
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(50, 5);
        M5.Display.print("👀 Pattern Learning");
        
        // Target info
        ScannerTarget& target = scanners[selectedScanner];
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 18);
        M5.Display.print("Target: " + target.name + " " + target.emoji);
        
        // Draw paw grid
        drawPawGrid();
        
        // Instructions
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 110);
        if (showingPattern) {
            M5.Display.print("Observing pattern... " + String(patternStep + 1) + "/" + String(target.required_pattern.length));
        } else {
            M5.Display.print("Press ⏎ to observe pattern");
        }
    }
    
    void drawBypassExecution() {
        // Header
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🔓 Execute Bypass");
        
        // Current technique
        BypassTechnique& tech = techniques[selectedTechnique];
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 18);
        M5.Display.print("Method: " + tech.name + " " + tech.emoji);
        
        // Draw paw grid
        drawPawGrid();
        
        // Player input progress
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 110);
        PawPattern& pattern = scanners[selectedScanner].required_pattern;
        M5.Display.printf("Input: %d/%d paws", playerInput.size(), pattern.length);
        
        // Current input sequence
        M5.Display.setCursor(10, 120);
        String inputStr = "Sequence: ";
        for (int i = 0; i < playerInput.size() && i < 5; i++) {
            inputStr += String(playerInput[i]) + " ";
        }
        M5.Display.print(inputStr);
    }
    
    void drawScanningAnimation() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(60, 30);
        M5.Display.print("🔍 SCANNING...");
        
        // Animated scanner beam
        int beamY = 50 + sin(animationFrame * 0.2) * 20;
        M5.Display.drawLine(0, beamY, 240, beamY, CYAN);
        
        // Technique being used
        BypassTechnique& tech = techniques[selectedTechnique];
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(40, 70);
        M5.Display.print("Using: " + tech.name);
        
        // Progress indicator
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(50, 90);
        String dots = "";
        for (int i = 0; i < (animationFrame / 15) % 4; i++) {
            dots += ".";
        }
        M5.Display.print("Analyzing" + dots);
        
        // Fake technical readouts
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 110);
        M5.Display.printf("Confidence: %d%%", 50 + (animationFrame % 50));
        M5.Display.setCursor(130, 110);
        M5.Display.printf("Signal: %d dB", -20 + (animationFrame % 15));
    }
    
    void drawResults() {
        if (bypassSuccessful) {
            // Success screen
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(70, 20);
            M5.Display.print("🎉 ACCESS GRANTED!");
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(40, 40);
            M5.Display.print("Bypass successful!");
            
            // Show accessed data
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 60);
            M5.Display.print("Accessed: " + accessedData);
            
            // Points earned
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(10, 80);
            M5.Display.printf("Points earned: +%d", pointsEarned);
            
            // Technique used
            M5.Display.setTextColor(MAGENTA);
            M5.Display.setCursor(10, 95);
            M5.Display.print("Method: " + techniques[selectedTechnique].name);
            
        } else {
            // Failure screen
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(70, 30);
            M5.Display.print("🚫 ACCESS DENIED!");
            
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(40, 50);
            M5.Display.print("Bypass failed!");
            
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(20, 70);
            M5.Display.print("Try different technique");
            
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(30, 85);
            M5.Display.print("or improve timing");
        }
        
        // Continue prompt
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(40, 115);
        M5.Display.print("Press ⏎ to continue");
    }
    
    void drawStatistics() {
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(70, 10);
        M5.Display.print("📊 STATISTICS");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.printf("Total Scans: %d", stats.total_scans);
        
        M5.Display.setCursor(10, 45);
        M5.Display.printf("Successful: %d", stats.successful_bypasses);
        
        M5.Display.setCursor(10, 60);
        M5.Display.printf("Failed: %d", stats.failed_attempts);
        
        float successRate = stats.total_scans > 0 ? 
            (float)stats.successful_bypasses / stats.total_scans * 100 : 0;
        M5.Display.setCursor(10, 75);
        M5.Display.printf("Success Rate: %.1f%%", successRate);
        
        M5.Display.setCursor(10, 90);
        M5.Display.printf("Total Score: %d", stats.total_score);
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 105);
        M5.Display.print("Favorite: " + stats.favorite_technique);
    }
    
    void drawPawGrid() {
        // Draw 3x3 grid of paw positions
        for (int i = 0; i < 9; i++) {
            PawPosition& paw = pawPositions[i];
            
            uint16_t color = WHITE;
            if (paw.is_pressed && millis() - paw.press_time < 500) {
                color = pawColors[paw.type];
            }
            
            // Draw paw circle
            M5.Display.fillCircle(paw.x, paw.y, 8, color);
            M5.Display.drawCircle(paw.x, paw.y, 8, DARKGREY);
            
            // Draw paw number
            M5.Display.setTextColor(BLACK);
            M5.Display.setCursor(paw.x - 3, paw.y - 4);
            M5.Display.print(String(i));
        }
    }
    
    // ==================== GAME LOGIC ====================
    void executeBypass() {
        BypassTechnique& tech = techniques[selectedTechnique];
        ScannerTarget& target = scanners[selectedScanner];
        PawPattern& pattern = target.required_pattern;
        
        // Calculate success probability based on multiple factors
        int baseProbability = tech.effectiveness * 10; // 0-100%
        
        // Pattern accuracy bonus/penalty
        int patternAccuracy = calculatePatternAccuracy();
        baseProbability += patternAccuracy;
        
        // Security level penalty
        baseProbability -= target.security_level * 15;
        
        // Special technique bonuses
        if (selectedTechnique == KAWAII_OVERRIDE) {
            baseProbability += 50; // Kawaii power!
        }
        if (selectedTechnique == QUANTUM_TUNNELING && target.security_level >= QUANTUM_SCANNER) {
            baseProbability += 30; // Quantum vs quantum
        }
        
        // Clamp probability
        int finalProbability = constrain(baseProbability, 5, 95);
        
        bypassSuccessful = (random(0, 100) < finalProbability);
        
        if (bypassSuccessful) {
            accessedData = target.protected_data;
            pointsEarned = target.reward_points + (tech.effectiveness * 50);
            stats.total_score += pointsEarned;
            stats.successful_bypasses++;
            
            unlockNewContent();
            
            if (nekoSounds) {
                nekoSounds->playContextSound("hack_success");
            }
            
        } else {
            accessedData = "";
            pointsEarned = 0;
            stats.failed_attempts++;
            
            if (nekoSounds) {
                nekoSounds->playContextSound("hack_failed");
            }
        }
        
        stats.total_scans++;
        
        // Update favorite technique
        if (bypassSuccessful) {
            stats.favorite_technique = tech.name;
        }
    }
    
    int calculatePatternAccuracy() {
        PawPattern& pattern = scanners[selectedScanner].required_pattern;
        
        if (selectedTechnique == DIRECT_TOUCH) {
            // Direct touch requires exact pattern match
            if (playerInput.size() != pattern.length) return -30;
            
            int matches = 0;
            for (int i = 0; i < pattern.length; i++) {
                if (i < playerInput.size() && playerInput[i] == pattern.sequence[i]) {
                    matches++;
                }
            }
            
            return (matches * 100 / pattern.length) - 50; // -50 to +50
        } else {
            // Other techniques are less dependent on exact pattern
            return random(-10, 20);
        }
    }
    
    void unlockNewContent() {
        // Unlock new scanners based on score
        if (stats.total_score >= 500) {
            scanners[2].unlocked = true; // Safe Vault
            techniques[SILICONE_MOLD].unlocked = true;
        }
        
        if (stats.total_score >= 1000) {
            scanners[3].unlocked = true; // Military Base
            techniques[THERMAL_IMAGING].unlocked = true;
        }
        
        if (stats.successful_bypasses >= 10) {
            techniques[FREQUENCY_JAMMING].unlocked = true;
            scanners[4].unlocked = true; // AI Lab
        }
        
        if (stats.total_score >= 3000) {
            techniques[SOCIAL_ENGINEERING].unlocked = true;
            scanners[5].unlocked = true; // Quantum Vault
        }
        
        if (stats.successful_bypasses >= 25) {
            techniques[QUANTUM_TUNNELING].unlocked = true;
            techniques[KAWAII_OVERRIDE].unlocked = true;
            scanners[6].unlocked = true; // Kawaii Fortress
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
            case ACTION_BACK:
                goBack();
                break;
        }
        
        drawGameScreen();
    }
    
    void navigateUp() {
        if (currentPhase == BYPASS_EXECUTION) {
            // Navigate paw grid
            if (selectedItem >= 3) selectedItem -= 3;
        } else {
            if (selectedItem > 0) selectedItem--;
        }
    }
    
    void navigateDown() {
        if (currentPhase == BYPASS_EXECUTION) {
            // Navigate paw grid
            if (selectedItem < 6) selectedItem += 3;
        } else {
            int maxItems = getMaxItemsForCurrentPhase();
            selectedItem = (selectedItem + 1) % maxItems;
        }
    }
    
    void navigateLeft() {
        if (currentPhase == BYPASS_EXECUTION && selectedItem % 3 > 0) {
            selectedItem--;
        }
    }
    
    void navigateRight() {
        if (currentPhase == BYPASS_EXECUTION && selectedItem % 3 < 2) {
            selectedItem++;
        }
    }
    
    void selectCurrentItem() {
        switch (currentPhase) {
            case MENU_PHASE:
                handleMenuSelection();
                break;
            case TARGET_SELECTION:
                selectedScanner = selectedItem;
                currentPhase = METHOD_SELECTION;
                selectedItem = 0;
                break;
            case METHOD_SELECTION:
                selectedTechnique = selectedItem;
                currentPhase = PATTERN_OBSERVATION;
                selectedItem = 0;
                break;
            case PATTERN_OBSERVATION:
                if (!showingPattern) {
                    showingPattern = true;
                    patternStep = 0;
                }
                break;
            case BYPASS_EXECUTION:
                // Add paw to input sequence
                playerInput.push_back(selectedItem);
                playerTiming.push_back(millis());
                pawPositions[selectedItem].is_pressed = true;
                pawPositions[selectedItem].press_time = millis();
                
                if (nekoSounds) {
                    nekoSounds->playPurr();
                }
                
                // Check if pattern complete
                PawPattern& pattern = scanners[selectedScanner].required_pattern;
                if (playerInput.size() >= pattern.length) {
                    currentPhase = SCANNING_ANIMATION;
                    animationFrame = 0;
                }
                break;
            case RESULT_PHASE:
                currentPhase = MENU_PHASE;
                selectedItem = 0;
                playerInput.clear();
                playerTiming.clear();
                clearPawStates();
                break;
            case STATISTICS_PHASE:
                currentPhase = MENU_PHASE;
                selectedItem = 0;
                break;
        }
    }
    
    void goBack() {
        switch (currentPhase) {
            case TARGET_SELECTION:
            case STATISTICS_PHASE:
                currentPhase = MENU_PHASE;
                break;
            case METHOD_SELECTION:
                currentPhase = TARGET_SELECTION;
                break;
            case PATTERN_OBSERVATION:
                currentPhase = METHOD_SELECTION;
                showingPattern = false;
                patternStep = 0;
                break;
            case BYPASS_EXECUTION:
                if (playerInput.size() > 0) {
                    // Remove last input
                    playerInput.pop_back();
                    if (playerTiming.size() > 0) playerTiming.pop_back();
                } else {
                    currentPhase = PATTERN_OBSERVATION;
                }
                break;
        }
        selectedItem = 0;
    }
    
    void handleMenuSelection() {
        switch (selectedItem) {
            case 0: // Start Mission
                currentPhase = TARGET_SELECTION;
                selectedItem = 0;
                break;
            case 1: // Statistics
                currentPhase = STATISTICS_PHASE;
                selectedItem = 0;
                break;
            case 2: // Learn Techniques
                showTechniqueTutorial();
                break;
            case 3: // Settings
                showSettings();
                break;
            case 4: // Exit
                // Handled in main application
                break;
        }
    }
    
    int getMaxItemsForCurrentPhase() {
        switch (currentPhase) {
            case MENU_PHASE: return 5;
            case TARGET_SELECTION: return 7;
            case METHOD_SELECTION: return 8;
            case BYPASS_EXECUTION: return 9;
            default: return 1;
        }
    }
    
    // ==================== ANIMATIONS & UPDATES ====================
    void updateAnimations() {
        if (millis() - lastAnimUpdate > 100) {
            animationFrame++;
            if (animationFrame > 1000) animationFrame = 0;
            lastAnimUpdate = millis();
        }
    }
    
    void updatePawStates() {
        // Reset pressed state after delay
        for (int i = 0; i < 9; i++) {
            if (pawPositions[i].is_pressed && 
                millis() - pawPositions[i].press_time > 500) {
                pawPositions[i].is_pressed = false;
            }
        }
    }
    
    void clearPawStates() {
        for (int i = 0; i < 9; i++) {
            pawPositions[i].is_pressed = false;
            pawPositions[i].press_time = 0;
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    void showTechniqueTutorial() {
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(50, 10);
        M5.Display.print("🔧 BYPASS TECHNIQUES");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.print("Real-world biometric bypasses:");
        
        M5.Display.setCursor(10, 50);
        M5.Display.print("🧤 Silicone molds");
        M5.Display.setCursor(10, 65);
        M5.Display.print("🌡️ Thermal imaging");
        M5.Display.setCursor(10, 80);
        M5.Display.print("📡 RF interference");
        M5.Display.setCursor(10, 95);
        M5.Display.print("🗣️ Social engineering");
        
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(20, 115);
        M5.Display.print("Educational purposes only!");
        
        delay(5000);
    }
    
    void showSettings() {
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(80, 60);
        M5.Display.print("Settings");
        M5.Display.setCursor(60, 80);
        M5.Display.print("Coming Soon!");
        delay(2000);
    }
    
    void saveStats() {
        // Placeholder - would save to SD card
        Serial.println("💾 Saving paw scanner stats...");
    }
    
    void loadStats() {
        // Initialize stats
        stats.total_scans = 0;
        stats.successful_bypasses = 0;
        stats.failed_attempts = 0;
        stats.highest_security_beaten = 0;
        stats.favorite_technique = "Direct Touch";
        stats.total_score = 0;
        
        Serial.println("📂 Loading paw scanner stats...");
    }
    
    // ==================== GETTERS ====================
    GamePhase getCurrentPhase() { return currentPhase; }
    int getTotalScore() { return stats.total_score; }
    int getSuccessfulBypasses() { return stats.successful_bypasses; }
    bool isGameActive() { return currentPhase != MENU_PHASE; }
};

#endif // M5GOTCHI_PAW_SCANNER_GAME_H