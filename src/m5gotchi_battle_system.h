/*
 * ⚔️ M5GOTCHI BATTLE SYSTEM v1.0
 * Sistema de batalha multiplayer BLE!
 * 
 * Features:
 * - Batalhas PvP via BLE
 * - Sistema baseado em stats
 * - 6+ habilidades desbloqueáveis
 * - AI para treino
 * - Sistema de energia
 */

#ifndef M5GOTCHI_BATTLE_SYSTEM_H
#define M5GOTCHI_BATTLE_SYSTEM_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5Cardputer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEClient.h>

// ==================== BATTLE STRUCTS ====================

struct BattleStats {
    int hp;
    int maxHp;
    int attack;
    int defense;
    int speed;
    int energy;
    int maxEnergy;
    int level;
};

enum BattleSkill {
    SKILL_DEAUTH_BLAST,    // High damage, costs energy
    SKILL_HANDSHAKE_HEAL,  // Restore HP
    SKILL_BEACON_SPAM,     // Multi-hit attack  
    SKILL_EVIL_PORTAL,     // Defense buff + damage
    SKILL_PMKID_STRIKE,    // Critical hit chance
    SKILL_PACKET_STORM     // Ultimate attack
};

enum BattleState {
    BATTLE_MENU,
    BATTLE_CONNECTING,
    BATTLE_FIGHT,
    BATTLE_VICTORY,
    BATTLE_DEFEAT,
    BATTLE_DISCONNECTED
};

struct BattleSkillData {
    String name;
    String description;
    int damage;
    int energyCost;
    int levelRequired;
    bool unlocked;
};

class BattleSystem {
private:
    BattleStats myStats;
    BattleStats opponentStats;
    BattleState currentState;
    int selectedSkill;
    bool isHost;
    bool isConnected;
    String opponentName;
    
    BLEServer* pServer;
    BLEClient* pClient;
    BLECharacteristic* pBattleChar;
    
    BattleSkillData skills[6] = {
        {"Deauth Blast", "Powerful WiFi attack", 25, 15, 1, true},
        {"Handshake Heal", "Restore HP from captured data", 0, 10, 3, false},
        {"Beacon Spam", "Overwhelm with fake APs", 15, 8, 2, false},
        {"Evil Portal", "Phishing attack + defense", 20, 12, 5, false},
        {"PMKID Strike", "Critical hash attack", 30, 18, 7, false},
        {"Packet Storm", "Ultimate network assault", 45, 25, 10, false}
    };
    
public:
    BattleSystem() {
        currentState = BATTLE_MENU;
        selectedSkill = 0;
        isHost = false;
        isConnected = false;
        opponentName = "Unknown";
        
        // Initialize stats based on cat data
        initBattleStats();
    }
    
    void initBattleStats() {
        // Base stats + bonuses from cat level/achievements
        myStats.level = 5; // Get from cat data
        myStats.maxHp = 100 + (myStats.level * 10);
        myStats.hp = myStats.maxHp;
        myStats.attack = 20 + (myStats.level * 2);
        myStats.defense = 15 + (myStats.level * 1);
        myStats.speed = 10 + (myStats.level * 1);
        myStats.maxEnergy = 50 + (myStats.level * 5);
        myStats.energy = myStats.maxEnergy;
        
        // Unlock skills based on level
        for (int i = 0; i < 6; i++) {
            if (myStats.level >= skills[i].levelRequired) {
                skills[i].unlocked = true;
            }
        }
    }
    
    void showBattleMenu() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xFD20); // Pink
        M5.Display.setTextSize(2);
        M5.Display.drawString("⚔️ BATTLE MODE", 10, 10);
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("1. Host Battle (BLE Server)", 10, 40);
        M5.Display.drawString("2. Join Battle (Scan & Connect)", 10, 55);
        M5.Display.drawString("3. Train vs AI", 10, 70);
        M5.Display.drawString("4. View Skills", 10, 85);
        M5.Display.drawString("5. Battle Stats", 10, 100);
        
        M5.Display.setTextColor(0x07FF); // Cyan
        M5.Display.drawString("ESC Back", 10, 120);
    }
    
    bool handleBattleMenu() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == "1") {
                startHostBattle();
                return true;
            } else if (key == "2") {
                startJoinBattle();
                return true;
            } else if (key == "3") {
                startAIBattle();
                return true;
            } else if (key == "4") {
                showSkills();
                return true;
            } else if (key == "5") {
                showBattleStats();
                return true;
            } else if (key == "`") { // ESC
                return false;
            }
        }
        return true;
    }
    
    void startHostBattle() {
        currentState = BATTLE_CONNECTING;
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xFFE0);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🔗 HOSTING BATTLE", 10, 30);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Waiting for opponent...", 10, 60);
        M5.Display.drawString("Others can connect to:", 10, 80);
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("M5Gotchi-Battle-" + String(random(1000, 9999)), 10, 95);
        
        // Initialize BLE server
        initBLEServer();
    }
    
    void startJoinBattle() {
        currentState = BATTLE_CONNECTING;
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xFFE0);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🔍 SCANNING...", 10, 30);
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Looking for battles...", 10, 60);
        
        // Scan and connect
        scanForBattles();
    }
    
    void startAIBattle() {
        // Setup AI opponent
        opponentStats = myStats;
        opponentStats.attack += random(-5, 10);
        opponentStats.defense += random(-3, 8);
        opponentStats.speed += random(-2, 5);
        opponentName = "AI Hacker";
        
        currentState = BATTLE_FIGHT;
        showBattleScreen();
    }
    
    void showBattleScreen() {
        M5.Display.fillScreen(TFT_BLACK);
        
        // Draw battle UI
        M5.Display.setTextColor(0xFD20);
        M5.Display.setTextSize(1);
        M5.Display.drawString("⚔️ BATTLE vs " + opponentName, 10, 5);
        
        // My stats (left side)
        M5.Display.setTextColor(0x07E0);
        M5.Display.drawString("YOU", 10, 20);
        drawHPBar(10, 30, 80, myStats.hp, myStats.maxHp, 0x07E0);
        drawEnergyBar(10, 40, 80, myStats.energy, myStats.maxEnergy, 0x07FF);
        
        // Opponent stats (right side)
        M5.Display.setTextColor(0xF800);
        M5.Display.drawString(opponentName, 160, 20);
        drawHPBar(160, 30, 80, opponentStats.hp, opponentStats.maxHp, 0xF800);
        
        // Skills menu
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString("SKILLS:", 10, 60);
        
        for (int i = 0; i < 4; i++) {
            if (skills[i].unlocked) {
                uint16_t color = (i == selectedSkill) ? 0xFFE0 : TFT_WHITE;
                M5.Display.setTextColor(color);
                M5.Display.drawString(String(i + 1) + "." + skills[i].name, 10, 75 + (i * 10));
            }
        }
        
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("↑/↓ Select  ENTER Attack", 10, 120);
    }
    
    void drawHPBar(int x, int y, int width, int current, int max, uint16_t color) {
        float ratio = (float)current / max;
        int fillWidth = width * ratio;
        
        M5.Display.drawRect(x, y, width, 8, TFT_WHITE);
        M5.Display.fillRect(x + 1, y + 1, fillWidth - 2, 6, color);
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.drawString(String(current) + "/" + String(max), x + width + 5, y);
    }
    
    void drawEnergyBar(int x, int y, int width, int current, int max, uint16_t color) {
        float ratio = (float)current / max;
        int fillWidth = width * ratio;
        
        M5.Display.drawRect(x, y, width, 6, TFT_WHITE);
        M5.Display.fillRect(x + 1, y + 1, fillWidth - 2, 4, color);
        
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("EN:" + String(current), x + width + 5, y);
    }
    
    bool handleBattleInput() {
        M5Cardputer.update();
        
        if (M5Cardputer.Keyboard.isPressed()) {
            auto keyState = M5Cardputer.Keyboard.keysState();
            String key = String(keyState.word.data());
            
            if (key == ";") { // UP
                selectedSkill = (selectedSkill - 1 + 4) % 4;
                return true;
            } else if (key == ".") { // DOWN
                selectedSkill = (selectedSkill + 1) % 4;
                return true;
            } else if (key == "\n") { // ENTER - Attack
                if (skills[selectedSkill].unlocked && myStats.energy >= skills[selectedSkill].energyCost) {
                    executeAttack(selectedSkill);
                }
                return true;
            } else if (key == "`") { // ESC
                currentState = BATTLE_MENU;
                return false;
            }
        }
        return true;
    }
    
    void executeAttack(int skillId) {
        BattleSkillData& skill = skills[skillId];
        
        // Show attack animation
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xFFE0);
        M5.Display.setTextSize(2);
        M5.Display.drawString("💥 " + skill.name + "!", 10, 50);
        delay(1000);
        
        // Calculate damage
        int damage = skill.damage + random(-5, 10);
        myStats.energy -= skill.energyCost;
        
        // Apply damage to opponent
        if (skillId == 1) { // Heal skill
            myStats.hp = min(myStats.maxHp, myStats.hp + damage);
        } else {
            opponentStats.hp = max(0, opponentStats.hp - damage);
        }
        
        // Check for victory/defeat
        if (opponentStats.hp <= 0) {
            currentState = BATTLE_VICTORY;
            showVictoryScreen();
            return;
        }
        
        if (myStats.hp <= 0) {
            currentState = BATTLE_DEFEAT;
            showDefeatScreen();
            return;
        }
        
        // AI turn (simple)
        if (!isConnected) {
            delay(1000);
            executeAITurn();
        }
        
        // Regenerate energy
        myStats.energy = min(myStats.maxEnergy, myStats.energy + 3);
        opponentStats.energy = min(opponentStats.maxEnergy, opponentStats.energy + 3);
        
        showBattleScreen();
    }
    
    void executeAITurn() {
        int aiSkill = random(0, 3);
        int damage = 15 + random(-3, 8);
        
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xF800);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🤖 AI ATTACKS!", 10, 50);
        delay(1000);
        
        myStats.hp = max(0, myStats.hp - damage);
    }
    
    void showVictoryScreen() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0x07E0);
        M5.Display.setTextSize(3);
        M5.Display.drawString("🏆 VICTORY!", 10, 40);
        
        M5.Display.setTextColor(0xFFE0);
        M5.Display.setTextSize(1);
        M5.Display.drawString("You defeated " + opponentName + "!", 10, 80);
        M5.Display.drawString("Rewards: +50 XP, +Battle Win", 10, 95);
        
        M5.Display.drawString("Press any key to continue", 10, 115);
        
        while (!M5Cardputer.Keyboard.isPressed()) {
            M5Cardputer.update();
            delay(50);
        }
        
        currentState = BATTLE_MENU;
    }
    
    void showDefeatScreen() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xF800);
        M5.Display.setTextSize(3);
        M5.Display.drawString("💀 DEFEAT", 10, 40);
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString(opponentName + " was too strong!", 10, 80);
        M5.Display.drawString("Train more and try again!", 10, 95);
        
        M5.Display.drawString("Press any key to continue", 10, 115);
        
        while (!M5Cardputer.Keyboard.isPressed()) {
            M5Cardputer.update();
            delay(50);
        }
        
        currentState = BATTLE_MENU;
    }
    
    void showSkills() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xFD20);
        M5.Display.setTextSize(2);
        M5.Display.drawString("🎯 BATTLE SKILLS", 10, 10);
        
        int y = 35;
        for (int i = 0; i < 6; i++) {
            uint16_t color = skills[i].unlocked ? 0x07E0 : 0x4208;
            M5.Display.setTextColor(color);
            M5.Display.setTextSize(1);
            
            String status = skills[i].unlocked ? "✓" : "[LVL " + String(skills[i].levelRequired) + "]";
            M5.Display.drawString(status + " " + skills[i].name, 10, y);
            M5.Display.drawString("    " + skills[i].description, 10, y + 10);
            
            y += 25;
        }
        
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Press any key to return", 10, y + 10);
        
        while (!M5Cardputer.Keyboard.isPressed()) {
            M5Cardputer.update();  
            delay(50);
        }
    }
    
    void showBattleStats() {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setTextColor(0xFD20);
        M5.Display.setTextSize(2);
        M5.Display.drawString("📊 BATTLE STATS", 10, 10);
        
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextSize(1);
        M5.Display.drawString("Level: " + String(myStats.level), 10, 35);
        M5.Display.drawString("HP: " + String(myStats.maxHp), 10, 50);
        M5.Display.drawString("ATK: " + String(myStats.attack), 10, 65);
        M5.Display.drawString("DEF: " + String(myStats.defense), 10, 80);
        M5.Display.drawString("SPD: " + String(myStats.speed), 10, 95);
        M5.Display.drawString("Energy: " + String(myStats.maxEnergy), 10, 110);
        
        M5.Display.setTextColor(0x07FF);
        M5.Display.drawString("Press any key to return", 130, 120);
        
        while (!M5Cardputer.Keyboard.isPressed()) {
            M5Cardputer.update();
            delay(50);
        }
    }
    
    void initBLEServer() {
        BLEDevice::init("M5Gotchi-Battle");
        pServer = BLEDevice::createServer();
        
        BLEService *pService = pServer->createService("12345678-1234-1234-1234-123456789abc");
        pBattleChar = pService->createCharacteristic(
            "87654321-4321-4321-4321-cba987654321",
            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
        );
        
        pService->start();
        pServer->getAdvertising()->start();
        isHost = true;
    }
    
    void scanForBattles() {
        // Simplified - would implement BLE scanning here
        M5.Display.drawString("No battles found nearby", 10, 110);
        delay(2000);
        currentState = BATTLE_MENU;
    }
    
    // ==================== CONTROLE UNIVERSAL ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                if (currentMenuSelection > 0) currentMenuSelection--;
                break;
            case ACTION_DOWN:
                currentMenuSelection = (currentMenuSelection + 1) % 4; // 4 opções no menu
                break;
            case ACTION_LEFT:
                // Navegar páginas se tiver
                break;
            case ACTION_RIGHT:
                // Navegar páginas se tiver  
                break;
            case ACTION_SELECT:
                handleBattleMenuSelection();
                break;
        }
    }
    
    void handleBattleMenuSelection() {
        switch (currentMenuSelection) {
            case 0: // Host Battle
                initBLEServer();
                currentState = BATTLE_FIGHT;
                break;
            case 1: // Join Battle
                scanForBattles();
                break;
            case 2: // View Stats
                showStats();
                break;
            case 3: // Exit - handled in main
                break;
        }
    }
    
    void run() {
        switch (currentState) {
            case BATTLE_MENU:
                showBattleMenu();
                if (!handleBattleMenu()) {
                    // Exit battle system
                    return;
                }
                break;
                
            case BATTLE_FIGHT:
                showBattleScreen();
                handleBattleInput();
                break;
                
            default:
                break;
        }
    }
};

// Global instance
BattleSystem battleSystem;

#endif // M5GOTCHI_BATTLE_SYSTEM_H