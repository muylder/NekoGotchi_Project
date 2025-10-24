/*
 * 🐱‍💻 M5GOTCHI NEKO TERMINAL v1.0
 * Sistema de terminal kawaii com personalidades de gato!
 * 
 * Features:
 * - Prompts temáticos com personalidades
 * - Respostas kawaii contextuais
 * - Sistema de humor do neko
 * - Comandos especiais de gato
 * - ASCII art cats
 */

#ifndef M5GOTCHI_NEKO_TERMINAL_H
#define M5GOTCHI_NEKO_TERMINAL_H

#include <M5Unified.h>
#include <vector>
#include <ArduinoJson.h>
#include "m5gotchi_universal_controls.h"

// ==================== NEKO PERSONALITIES ====================
enum NekoPersonality {
    KUROMI_CHAN = 0,    // 🖤 Dark hacker queen
    NYA_NULL,           // 🔴 Red team specialist  
    MIKU_HACK,          // 💙 Blue team defender
    SAKURA_SHELL,       // 🌸 Social engineer
    KAWAII_ROOT,        // 👑 Privilege escalation queen
    NEKO_ZERO,          // ⚫ Zero-day hunter
    CIPHER_CAT          // 🔐 Cryptography expert
};

enum NekoMood {
    HAPPY = 0,      // 😸 Successful hacks
    SLEEPY,         // 😴 Idle/low activity
    EXCITED,        // 🤩 New target found
    ANGRY,          // 😾 Failed attacks
    CURIOUS,        // 🙀 Scanning mode
    SMUG,           // 😏 Easy target pwned
    PLAYFUL         // 😹 Game mode
};

enum NekoCommandType {
    CMD_SCAN,
    CMD_ATTACK,
    CMD_STEALTH,
    CMD_SOCIAL,
    CMD_CRYPTO,
    CMD_STATUS,
    CMD_HELP,
    CMD_PURR,
    CMD_UNKNOWN
};

// ==================== NEKO TERMINAL CLASS ====================
class M5GotchiNekoTerminal {
private:
    NekoPersonality currentPersonality;
    NekoMood currentMood;
    String currentPrompt;
    int happinessLevel;
    int energyLevel;
    unsigned long lastActivity;
    std::vector<String> commandHistory;
    std::vector<String> responseHistory;
    int selectedCommand;
    bool terminalActive;
    
    // Personality data
    struct NekoProfile {
        String name;
        String prompt;
        String emoji;
        String specialty;
        uint16_t primaryColor;
        uint16_t accentColor;
    };
    
    NekoProfile personalities[7];
    
    // Response templates
    String happyResponses[10];
    String errorResponses[10];
    String successResponses[10];
    String greetings[7];
    
    // ASCII Art
    String catAscii[5];

public:
    M5GotchiNekoTerminal() {
        currentPersonality = KUROMI_CHAN;
        currentMood = HAPPY;
        happinessLevel = 50;
        energyLevel = 100;
        lastActivity = 0;
        selectedCommand = 0;
        terminalActive = false;
        
        initPersonalities();
        initResponses();
        initAsciiArt();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🐱‍💻 Initializing Neko Terminal...");
        
        updatePrompt();
        currentMood = HAPPY;
        
        Serial.println("✅ Neko Terminal Ready!");
        Serial.println(getGreeting());
    }
    
    void start() {
        terminalActive = true;
        drawTerminalInterface();
        showGreeting();
    }
    
    void stop() {
        terminalActive = false;
        showGoodbye();
    }
    
    void update() {
        if (!terminalActive) return;
        
        // Update mood based on activity
        updateMood();
        
        // Auto-update display
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 1000) {
            updateStatusBar();
            lastUpdate = millis();
        }
    }
    
    // ==================== PERSONALITY SYSTEM ====================
    void initPersonalities() {
        personalities[KUROMI_CHAN] = {
            "Kuromi-chan", "kuromi>", "🖤", "Elite Pentesting",
            M5.Display.color565(139, 0, 139), M5.Display.color565(255, 20, 147)
        };
        
        personalities[NYA_NULL] = {
            "Nya-Null", "null$", "🔴", "Red Team Ops",
            RED, M5.Display.color565(255, 69, 0)
        };
        
        personalities[MIKU_HACK] = {
            "Miku-Hack", "miku~", "💙", "Blue Team Defense", 
            BLUE, CYAN
        };
        
        personalities[SAKURA_SHELL] = {
            "Sakura-Shell", "sakura>", "🌸", "Social Engineering",
            M5.Display.color565(255, 182, 193), M5.Display.color565(255, 105, 180)
        };
        
        personalities[KAWAII_ROOT] = {
            "Kawaii-Root", "root#", "👑", "Privilege Escalation",
            M5.Display.color565(255, 215, 0), YELLOW
        };
        
        personalities[NEKO_ZERO] = {
            "Neko-Zero", "zero:", "⚫", "Zero-Day Hunter",
            M5.Display.color565(64, 64, 64), WHITE
        };
        
        personalities[CIPHER_CAT] = {
            "Cipher-Cat", "cipher$", "🔐", "Cryptography",
            M5.Display.color565(128, 0, 128), M5.Display.color565(186, 85, 211)
        };
    }
    
    void initResponses() {
        // Happy responses
        String happy[] = {
            "Nya~ Perfect execution! ✨",
            "Paw-some work! 🐾",
            "Purr-fect! Target acquired! 😸",
            "Meow-nificent! System compromised! 💕",
            "Kawaii desu ne~ Attack successful! 🌟",
            "Nya-ice! Another victory! 🎉",
            "Purring with satisfaction~ 😻",
            "Target pwned like a true neko! 🥷",
            "Elegant hack, worthy of a cat! 👑",
            "Stealth level: Silent cat paws! 🐾"
        };
        
        // Error responses  
        String errors[] = {
            "Nya-no! Something went wrong >_<",
            "Hiss... Target too strong! 😾",
            "Meow... Need more coffee! ☕",
            "Oopsie! Clumsy paws strike again! 🐾",
            "Nya~ This human is too smart! 😿",
            "Error 404: Cat not found! 🙀",
            "System says no... *sad meow* 😢",
            "Firewall too scratchy! Can't get through! 🔥",
            "Target has anti-cat defenses! 🛡️",
            "Need to sharpen my digital claws! ⚔️"
        };
        
        // Success responses
        String success[] = {
            "Mission accomplished! *purr* 😸",
            "Another network falls to feline superiority! 👑",
            "Stealth mission: Paw-sitively perfect! 🥷",
            "Data exfiltrated with cat-like grace! 📁",
            "Credentials harvested like fresh tuna! 🍣",
            "Backdoor installed... through the cat flap! 🚪",
            "Root access gained! I am the alpha cat! 👑",
            "Social engineering: Purr-suasion successful! 💕",
            "Vulnerability exploited with kawaii precision! 🎯",
            "System dominated! Bow before the neko overlord! 😈"
        };
        
        for (int i = 0; i < 10; i++) {
            happyResponses[i] = happy[i];
            errorResponses[i] = errors[i];
            successResponses[i] = success[i];
        }
        
        // Personality greetings
        greetings[KUROMI_CHAN] = "🖤 Kuromi-chan reporting for dark ops duty!";
        greetings[NYA_NULL] = "🔴 Nya-Null online. Ready to pwn everything!";
        greetings[MIKU_HACK] = "💙 Miku-Hack initialized. Protecting kawaii cyberspace!";
        greetings[SAKURA_SHELL] = "🌸 Sakura-Shell here~ Let's make friends... with their passwords!";
        greetings[KAWAII_ROOT] = "👑 Kawaii-Root activated! Time to rule the system!";
        greetings[NEKO_ZERO] = "⚫ Neko-Zero deployed. Hunting for zero-days...";
        greetings[CIPHER_CAT] = "🔐 Cipher-Cat ready! Let's break some codes!";
    }
    
    void initAsciiArt() {
        catAscii[0] = 
            "    ∧___∧\n"
            "   (  ._. )\n"
            "  o_(")(")";
            
        catAscii[1] = 
            "  ∧,,,∧\n"
            " ( ._. )\n"
            "o_(")(")";
            
        catAscii[2] = 
            "   /\\   /\\\n"
            "  (  . .)\n"
            "   )   (\n"
            "  (  v  )";
            
        catAscii[3] = 
            "  ∧＿∧\n"
            "（｡◕‿‿◕｡）\n"
            "　＼　　　／\n"
            "　　＼___／";
            
        catAscii[4] = 
            "    /\\_/\\\n"
            "   ( o.o )\n"
            "    > ^ <";
    }
    
    // ==================== PERSONALITY MANAGEMENT ====================
    void switchPersonality(NekoPersonality newPersonality) {
        currentPersonality = newPersonality;
        updatePrompt();
        happinessLevel = 50; // Reset mood
        energyLevel = 100;
        
        M5.Display.fillScreen(BLACK);
        showPersonalitySwitch();
        drawTerminalInterface();
    }
    
    void showPersonalitySwitch() {
        NekoProfile& profile = personalities[currentPersonality];
        
        M5.Display.setTextColor(profile.primaryColor);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(10, 20);
        M5.Display.print("Switching to:");
        
        M5.Display.setTextColor(profile.accentColor);
        M5.Display.setCursor(10, 35);
        M5.Display.print(profile.emoji + " " + profile.name);
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 50);
        M5.Display.print("Specialty: " + profile.specialty);
        
        // Show ASCII cat
        M5.Display.setCursor(10, 70);
        M5.Display.print(catAscii[currentPersonality % 5]);
        
        delay(2000);
    }
    
    void updatePrompt() {
        currentPrompt = personalities[currentPersonality].prompt + " ";
    }
    
    // ==================== MOOD SYSTEM ====================
    void updateMood() {
        unsigned long now = millis();
        
        // Mood changes based on activity and success
        if (now - lastActivity > 30000) { // 30 seconds idle
            currentMood = SLEEPY;
            energyLevel = max(20, energyLevel - 1);
        }
        
        // Happiness affects mood
        if (happinessLevel > 80) currentMood = EXCITED;
        else if (happinessLevel < 20) currentMood = ANGRY;
        else if (happinessLevel > 60) currentMood = HAPPY;
        
        // Random mood shifts (cats are unpredictable!)
        if (random(0, 1000) < 5) { // 0.5% chance per update
            currentMood = (NekoMood)random(0, 7);
        }
    }
    
    String getMoodEmoji() {
        switch (currentMood) {
            case HAPPY: return "😸";
            case SLEEPY: return "😴";  
            case EXCITED: return "🤩";
            case ANGRY: return "😾";
            case CURIOUS: return "🙀";
            case SMUG: return "😏";
            case PLAYFUL: return "😹";
            default: return "🐱";
        }
    }
    
    // ==================== COMMAND PROCESSING ====================
    NekoCommandType parseCommand(String cmd) {
        cmd.toLowerCase();
        
        if (cmd.indexOf("scan") >= 0 || cmd.indexOf("nmap") >= 0) return CMD_SCAN;
        if (cmd.indexOf("attack") >= 0 || cmd.indexOf("exploit") >= 0) return CMD_ATTACK;
        if (cmd.indexOf("stealth") >= 0 || cmd.indexOf("ninja") >= 0) return CMD_STEALTH;
        if (cmd.indexOf("social") >= 0 || cmd.indexOf("phish") >= 0) return CMD_SOCIAL;
        if (cmd.indexOf("crypto") >= 0 || cmd.indexOf("decrypt") >= 0) return CMD_CRYPTO;
        if (cmd.indexOf("status") >= 0 || cmd.indexOf("stats") >= 0) return CMD_STATUS;
        if (cmd.indexOf("help") >= 0 || cmd.indexOf("?") >= 0) return CMD_HELP;
        if (cmd.indexOf("purr") >= 0 || cmd.indexOf("nya") >= 0) return CMD_PURR;
        
        return CMD_UNKNOWN;
    }
    
    String executeCommand(String command) {
        lastActivity = millis();
        NekoCommandType cmdType = parseCommand(command);
        
        commandHistory.push_back(command);
        if (commandHistory.size() > 20) {
            commandHistory.erase(commandHistory.begin());
        }
        
        String response = processCommand(cmdType, command);
        responseHistory.push_back(response);
        
        return response;
    }
    
    String processCommand(NekoCommandType type, String cmd) {
        switch (type) {
            case CMD_SCAN:
                happinessLevel += 5;
                return "🔍 " + personalities[currentPersonality].name + " is scanning... " + 
                       "Nya~ Found " + String(random(1, 20)) + " targets!";
                       
            case CMD_ATTACK:
                if (random(0, 100) < 70) { // 70% success rate
                    happinessLevel += 10;
                    return getRandomResponse(successResponses);
                } else {
                    happinessLevel -= 5;
                    return getRandomResponse(errorResponses);
                }
                
            case CMD_STEALTH:
                happinessLevel += 3;
                return "🥷 Silent paws activated! Moving like shadow cat...";
                
            case CMD_SOCIAL:
                happinessLevel += 7;
                return "💕 " + personalities[currentPersonality].name + 
                       " uses cute charm! It's super effective!";
                       
            case CMD_CRYPTO:
                happinessLevel += 8;
                return "🔐 Decrypting with kawaii algorithms... Cipher broken!";
                
            case CMD_STATUS:
                return getStatusReport();
                
            case CMD_HELP:
                return getHelpText();
                
            case CMD_PURR:
                happinessLevel += 15;
                currentMood = HAPPY;
                return "😸 *PURR PURR PURR* Nya~ You're the best human! 💕";
                
            default:
                happinessLevel -= 2;
                return "🙀 " + personalities[currentPersonality].name + 
                       " doesn't understand... Try 'help' for commands!";
        }
    }
    
    String getRandomResponse(String responses[]) {
        return responses[random(0, 10)];
    }
    
    // ==================== INTERFACE FUNCTIONS ====================
    void drawTerminalInterface() {
        M5.Display.fillScreen(BLACK);
        
        // Header with personality
        NekoProfile& profile = personalities[currentPersonality];
        M5.Display.fillRect(0, 0, 240, 20, profile.primaryColor);
        M5.Display.setTextColor(WHITE);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(5, 10);
        M5.Display.print(profile.emoji + " " + profile.name + " Terminal");
        
        // Status bar
        M5.Display.fillRect(0, 20, 240, 15, profile.accentColor);
        M5.Display.setTextColor(BLACK);
        M5.Display.setCursor(5, 28);
        M5.Display.print("Mood: " + getMoodEmoji() + " | Happy: " + String(happinessLevel) + 
                         "% | Energy: " + String(energyLevel) + "%");
        
        // Command area
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(5, 45);
        M5.Display.print(currentPrompt);
        
        // Show recent commands
        showCommandHistory();
    }
    
    void showCommandHistory() {
        int y = 60;
        int historySize = min(5, (int)commandHistory.size());
        
        for (int i = historySize - 1; i >= 0; i--) {
            M5.Display.setTextColor(CYAN);
            M5.Display.setCursor(5, y);
            M5.Display.print("> " + commandHistory[commandHistory.size() - 1 - i]);
            y += 10;
            
            if (i < responseHistory.size()) {
                M5.Display.setTextColor(YELLOW);
                M5.Display.setCursor(5, y);
                String response = responseHistory[responseHistory.size() - 1 - i];
                if (response.length() > 35) response = response.substring(0, 32) + "...";
                M5.Display.print(response);
                y += 10;
            }
        }
    }
    
    void updateStatusBar() {
        NekoProfile& profile = personalities[currentPersonality];
        M5.Display.fillRect(0, 20, 240, 15, profile.accentColor);
        M5.Display.setTextColor(BLACK);
        M5.Display.setCursor(5, 28);
        M5.Display.print("Mood: " + getMoodEmoji() + " | Happy: " + String(happinessLevel) + 
                         "% | Energy: " + String(energyLevel) + "%");
    }
    
    void showGreeting() {
        M5.Display.fillScreen(BLACK);
        
        // Show ASCII cat
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(80, 20);
        M5.Display.print(catAscii[currentPersonality % 5]);
        
        // Greeting message
        M5.Display.setTextColor(personalities[currentPersonality].primaryColor);
        M5.Display.setCursor(10, 80);
        M5.Display.print(getGreeting());
        
        delay(3000);
        drawTerminalInterface();
    }
    
    void showGoodbye() {
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(personalities[currentPersonality].primaryColor);
        M5.Display.setCursor(60, 50);
        M5.Display.print("Nya~ Goodbye!");
        M5.Display.setCursor(40, 70);
        M5.Display.print("Stay kawaii! 💕");
        
        // Sleeping cat
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(80, 90);
        M5.Display.print("😴 zzz...");
        
        delay(2000);
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    String getGreeting() {
        return greetings[currentPersonality];
    }
    
    String getStatusReport() {
        return "📊 Neko Status Report:\n" +
               "Personality: " + personalities[currentPersonality].name + "\n" +
               "Mood: " + getMoodEmoji() + "\n" +
               "Happiness: " + String(happinessLevel) + "%\n" +
               "Energy: " + String(energyLevel) + "%\n" +
               "Commands run: " + String(commandHistory.size());
    }
    
    String getHelpText() {
        return "🐱‍💻 Neko Terminal Commands:\n" +
               "scan - Find targets\n" +
               "attack - Launch exploit\n" +
               "stealth - Ninja mode\n" +
               "social - Social engineering\n" +
               "crypto - Cryptography\n" +
               "status - Show stats\n" +
               "purr - Make me happy! 💕";
    }
    
    // ==================== CONTROL INTERFACE ====================
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                // Cycle through personalities
                switchPersonality((NekoPersonality)((currentPersonality + 1) % 7));
                break;
            case ACTION_DOWN:
                // Show status
                M5.Display.fillScreen(BLACK);
                M5.Display.setTextColor(WHITE);
                M5.Display.setCursor(10, 20);
                M5.Display.print(getStatusReport());
                delay(3000);
                drawTerminalInterface();
                break;
            case ACTION_LEFT:
                // Previous command simulation
                if (!commandHistory.empty()) {
                    String lastCmd = commandHistory.back();
                    String response = executeCommand(lastCmd);
                    drawTerminalInterface();
                }
                break;
            case ACTION_RIGHT:
                // Quick purr command
                String response = executeCommand("purr");
                drawTerminalInterface();
                break;
            case ACTION_SELECT:
                // Show help
                M5.Display.fillScreen(BLACK);
                M5.Display.setTextColor(GREEN);
                M5.Display.setCursor(10, 20);
                M5.Display.print(getHelpText());
                delay(5000);
                drawTerminalInterface();
                break;
        }
    }
    
    // ==================== PUBLIC GETTERS ====================
    NekoPersonality getCurrentPersonality() { return currentPersonality; }
    NekoMood getCurrentMood() { return currentMood; }
    int getHappinessLevel() { return happinessLevel; }
    String getCurrentPrompt() { return currentPrompt; }
    bool isActive() { return terminalActive; }
};

#endif // M5GOTCHI_NEKO_TERMINAL_H