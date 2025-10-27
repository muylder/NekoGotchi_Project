/*
 * 🐱 M5GOTCHI NEKO VIRTUAL PET v1.0
 * Tamagotchi-style virtual companion que reage aos seus hacks kawaii!
 * 
 * Features:
 * - Virtual neko pet com stats reais (hunger, happiness, energy, hack_skill)
 * - Reações dinâmicas aos seus hacks e atividades
 * - Sistema de evolução baseado no seu progresso como hacker
 * - Mini-games próprios do pet para interação
 * - Personality system que afeta comportamento
 * - Needs system - pet precisa de cuidados
 * - Mood system - humor baseado em tratamento e sucessos
 * - Growth stages - bebê → adulto → elite hacker neko
 * - Customização (nome, cor, acessórios)
 * - Pet reage a música, sons e vibrações
 * - Sistema de memory - pet lembra suas ações
 * - Sleep cycle - pet dorme e sonha com seus hacks
 * 
 * O pet é seu companion de hacking que fica mais forte quanto mais você hackear!
 * Cuide bem dele e ele te ajudará com bônus especiais! 💕
 */

#ifndef M5GOTCHI_NEKO_VIRTUAL_PET_H
#define M5GOTCHI_NEKO_VIRTUAL_PET_H

#include <M5Unified.h>
#include <vector>
#include <map>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_neko_sounds.h"

// ==================== PET ENUMS ====================
enum PetStage {
    EGG_STAGE = 0,           // 🥚 Pet ainda não nasceu
    KITTEN_STAGE,            // 🐱 Bebê neko hacker
    TEEN_STAGE,              // 😸 Adolescente aprendendo
    ADULT_STAGE,             // 😻 Hacker adulto competente
    ELITE_STAGE,             // 👑 Elite master hacker
    LEGENDARY_STAGE          // ✨ Lendário neko senpai
};

enum PetMood {
    VERY_SAD = 0,            // 😿 Muito triste
    SAD,                     // 😾 Triste  
    NEUTRAL,                 // 😐 Neutro
    HAPPY,                   // 😸 Feliz
    VERY_HAPPY,              // 😻 Muito feliz
    ECSTATIC,                // 🤩 Extasiado
    SLEEPY,                  // 😴 Com sono
    EXCITED,                 // 🤗 Empolgado
    ANGRY,                   // 😡 Bravo
    SICK                     // 🤒 Doente
};

enum PetPersonality {
    CURIOUS_EXPLORER = 0,    // Adora descobrir coisas novas
    LAZY_SLEEPER,           // Ama dormir e relaxar
    HYPERACTIVE_HACKER,     // Energia infinita para hackear
    SHY_OBSERVER,           // Tímido mas observador
    SOCIAL_BUTTERFLY,       // Adora interagir e fazer amigos
    PERFECTIONIST,          // Quer fazer tudo perfeito
    REBEL_SPIRIT           // Espírito rebelde e independente
};

enum PetActivity {
    SLEEPING = 0,
    EATING,
    PLAYING,
    HACKING_WITH_YOU,
    EXPLORING,
    SOCIALIZING,
    LEARNING,
    DREAMING,
    EXERCISING,
    BEING_PETTED
};

enum PetScreen {
    PET_MAIN = 0,
    PET_STATS,
    PET_CARE,
    PET_PLAY,
    PET_CUSTOMIZE,
    PET_MEMORY,
    PET_EVOLUTION,
    PET_SETTINGS
};

// ==================== PET STRUCTURES ====================
struct PetStats {
    // Core needs (0-100)
    int hunger;              // Precisa comer
    int happiness;           // Precisa de atenção e diversão
    int energy;              // Precisa dormir
    int health;              // Precisa de cuidados
    int cleanliness;         // Precisa de limpeza
    
    // Growth stats (0-1000)
    int hack_skill;          // Habilidade de hacking
    int social_skill;        // Habilidade social
    int intelligence;        // Inteligência
    int creativity;          // Criatividade
    int loyalty;             // Lealdade ao dono
    
    // Life stats
    int age_days;            // Idade em dias
    int experience_points;   // XP total
    int level;               // Nível atual
    int total_hacks_witnessed; // Hacks que presenciou
    int times_petted;        // Vezes que foi acariciado
    int games_played;        // Mini-games jogados
};

struct PetAppearance {
    String name;
    String main_emoji;       // Emoji principal do pet
    uint16_t primary_color;  // Cor primária
    uint16_t secondary_color; // Cor secundária
    String accessory;        // Acessório atual
    std::vector<String> unlocked_accessories;
    String current_expression; // Expressão facial atual
    bool has_special_animation; // Animação especial desbloqueada
};

struct PetMemory {
    String event_description;
    unsigned long timestamp;
    String your_action;
    String pet_reaction;
    int importance_level;    // 1-5, affects how long pet remembers
};

struct PetFood {
    String name;
    String emoji;
    int hunger_restore;
    int happiness_bonus;
    int energy_change;
    int cost_points;
    bool unlocked;
    String unlock_condition;
};

struct PetToy {
    String name;
    String emoji;
    int happiness_boost;
    int energy_cost;
    String play_description;
    bool unlocked;
    String unlock_condition;
};

// ==================== NEKO VIRTUAL PET CLASS ====================
class M5GotchiNekoVirtualPet {
private:
    // Pet state
    PetStats stats;
    PetAppearance appearance;
    PetStage currentStage;
    PetMood currentMood;
    PetPersonality personality;
    PetActivity currentActivity;
    
    // UI state
    PetScreen currentScreen;
    int selectedItem;
    int animationFrame;
    unsigned long lastAnimUpdate;
    
    // Pet behavior
    std::vector<PetMemory> memories;
    std::vector<PetFood> availableFoods;
    std::vector<PetToy> availableToys;
    
    // Time tracking
    unsigned long lastUpdate;
    unsigned long lastFeed;
    unsigned long lastPlay;
    unsigned long lastSleep;
    unsigned long birthTime;
    
    // Reactions and responses
    std::map<String, String> personalityResponses;
    std::vector<String> currentThoughts;
    String lastReaction;
    unsigned long reactionTime;
    
    // Special states
    bool isEvolutionReady;
    bool isAsleep;
    bool isSick;
    bool needsAttention;
    unsigned long attentionTimer;
    
    // Sound system
    M5GotchiNekoSounds* nekoSounds;
    
    // Stage emojis and info
    String stageEmojis[6] = {"🥚", "🐱", "😸", "😻", "👑", "✨"};
    String stageNames[6] = {"Egg", "Kitten", "Teen", "Adult", "Elite", "Legend"};
    
    // Mood emojis
    String moodEmojis[10] = {"😿", "😾", "😐", "😸", "😻", "🤩", "😴", "🤗", "😡", "🤒"};

public:
    M5GotchiNekoVirtualPet() {
        // Initialize with default pet
        currentStage = KITTEN_STAGE;
        currentMood = HAPPY;
        personality = CURIOUS_EXPLORER;
        currentActivity = EXPLORING;
        currentScreen = PET_MAIN;
        selectedItem = 0;
        animationFrame = 0;
        lastAnimUpdate = 0;
        
        isEvolutionReady = false;
        isAsleep = false;
        isSick = false;
        needsAttention = false;
        attentionTimer = 0;
        
        lastUpdate = millis();
        lastFeed = millis();
        lastPlay = millis();
        lastSleep = millis();
        birthTime = millis();
        reactionTime = 0;
        
        nekoSounds = nullptr;
        
        initializePet();
        initializeFoodsAndToys();
        initializePersonalityResponses();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🐱 Initializing Neko Virtual Pet...");
        
        initializePet();
        loadPetData();
        
        Serial.printf("✅ Virtual Pet Ready! Meet %s the %s! 🎉\n", 
                     appearance.name.c_str(), stageNames[currentStage]);
    }
    
    void setSoundSystem(M5GotchiNekoSounds* sounds) {
        nekoSounds = sounds;
    }
    
    void initializePet() {
        // Initialize stats
        stats.hunger = 80;
        stats.happiness = 75;
        stats.energy = 85;
        stats.health = 100;
        stats.cleanliness = 90;
        
        stats.hack_skill = 10;
        stats.social_skill = 15;
        stats.intelligence = 20;
        stats.creativity = 25;
        stats.loyalty = 50;
        
        stats.age_days = 0;
        stats.experience_points = 0;
        stats.level = 1;
        stats.total_hacks_witnessed = 0;
        stats.times_petted = 0;
        stats.games_played = 0;
        
        // Initialize appearance
        appearance.name = "Neko";
        appearance.main_emoji = "🐱";
        appearance.primary_color = ORANGE;
        appearance.secondary_color = WHITE;
        appearance.accessory = "";
        appearance.current_expression = "😸";
        appearance.has_special_animation = false;
        
        // Default accessories
        appearance.unlocked_accessories.push_back("🎩"); // Hat
        appearance.unlocked_accessories.push_back("👓"); // Glasses
        
        currentThoughts.push_back("Nya~ I'm ready to hack with you!");
        currentThoughts.push_back("This device looks interesting...");
        currentThoughts.push_back("I wonder what we'll discover today!");
    }
    
    void initializeFoodsAndToys() {
        // Initialize foods
        availableFoods.clear();
        
        availableFoods.push_back({
            "Fish Snack", "🐟", 20, 5, -5, 10, true, ""
        });
        availableFoods.push_back({
            "Milk Bowl", "🥛", 15, 10, 0, 15, true, ""
        });
        availableFoods.push_back({
            "Tuna Sushi", "🍣", 35, 15, -3, 25, false, "Complete 5 SQL injection games"
        });
        availableFoods.push_back({
            "Cyber Pizza", "🍕", 40, 20, 5, 30, false, "Reach Adult stage"
        });
        availableFoods.push_back({
            "Energy Drink", "⚡", 0, 5, 50, 20, false, "Complete 10 successful hacks"
        });
        
        // Initialize toys
        availableToys.clear();
        
        availableToys.push_back({
            "Ball of Yarn", "🧶", 15, 10, "Chases the yarn playfully", true, ""
        });
        availableToys.push_back({
            "Laser Pointer", "🔴", 25, 15, "Goes crazy chasing the red dot!", true, ""
        });
        availableToys.push_back({
            "Hacker Keyboard", "⌨️", 30, 20, "Practices hacking skills", false, "Reach Teen stage"
        });
        availableToys.push_back({
            "WiFi Antenna", "📡", 35, 25, "Sniffs for networks like a pro", false, "Complete 25 network scans"
        });
    }
    
    void initializePersonalityResponses() {
        personalityResponses.clear();
        
        // Responses for different personalities
        personalityResponses["curious_hack_success"] = "Ooh! How did you do that? Teach me! 🤓";
        personalityResponses["lazy_hack_success"] = "Nice work... *yawn* Can we nap now? 😴";
        personalityResponses["hyperactive_hack_success"] = "YES! AGAIN! DO IT AGAIN! LET'S HACK EVERYTHING! 🤩";
        personalityResponses["shy_hack_success"] = "W-wow... you're really good at this... 😊";
        personalityResponses["social_hack_success"] = "Amazing! We should tell everyone about this! 📢";
        personalityResponses["perfectionist_hack_success"] = "Good, but I think we can do better next time 🤔";
        personalityResponses["rebel_hack_success"] = "Take that, system! We showed them who's boss! 😈";
        
        personalityResponses["curious_hack_fail"] = "Hmm, what went wrong? Let's analyze this! 🔍";
        personalityResponses["lazy_hack_fail"] = "Eh, happens to everyone... maybe try later? 😑";
        personalityResponses["hyperactive_hack_fail"] = "NO WORRIES! LET'S TRY AGAIN RIGHT NOW! 💪";
        personalityResponses["shy_hack_fail"] = "It's ok... everyone makes mistakes... 😔";
        personalityResponses["social_hack_fail"] = "Don't worry, we'll get it next time together! 🤝";
        personalityResponses["perfectionist_hack_fail"] = "This is unacceptable. We need more practice! 😤";
        personalityResponses["rebel_hack_fail"] = "Pfft, stupid security got lucky this time! 😾";
    }
    
    // ==================== PET UPDATES ====================
    void update() {
        unsigned long currentTime = millis();
        
        // Update pet needs over time
        updateNeeds(currentTime);
        
        // Update mood based on stats
        updateMood();
        
        // Update activity
        updateActivity(currentTime);
        
        // Check for evolution
        checkEvolution();
        
        // Update animations
        updateAnimations();
        
        // Age the pet
        updateAge(currentTime);
        
        // Process memories (decay over time)
        updateMemories(currentTime);
        
        lastUpdate = currentTime;
    }
    
    void updateNeeds(unsigned long currentTime) {
        unsigned long timeDiff = currentTime - lastUpdate;
        int minutes = timeDiff / 60000;
        
        if (minutes > 0) {
            // Decrease needs over time
            stats.hunger = max(0, stats.hunger - minutes * 2);
            stats.happiness = max(0, stats.happiness - minutes * 1);
            stats.energy = max(0, stats.energy - minutes * 3);
            stats.cleanliness = max(0, stats.cleanliness - minutes * 1);
            
            // Health decreases if other needs are low
            if (stats.hunger < 20 || stats.happiness < 20 || stats.energy < 10) {
                stats.health = max(0, stats.health - minutes * 2);
            }
            
            // Check if pet needs attention
            if (stats.hunger < 30 || stats.happiness < 25 || stats.energy < 15) {
                needsAttention = true;
                attentionTimer = currentTime;
            }
        }
    }
    
    void updateMood() {
        // Calculate mood based on stats
        int avgNeeds = (stats.hunger + stats.happiness + stats.energy + stats.health) / 4;
        
        if (stats.health < 30) {
            currentMood = SICK;
        } else if (stats.energy < 20) {
            currentMood = SLEEPY;
        } else if (avgNeeds < 20) {
            currentMood = VERY_SAD;
        } else if (avgNeeds < 40) {
            currentMood = SAD;
        } else if (avgNeeds < 60) {
            currentMood = NEUTRAL;
        } else if (avgNeeds < 80) {
            currentMood = HAPPY;
        } else if (avgNeeds < 95) {
            currentMood = VERY_HAPPY;
        } else {
            currentMood = ECSTATIC;
        }
        
        // Update expression based on mood
        appearance.current_expression = moodEmojis[currentMood];
    }
    
    void updateActivity(unsigned long currentTime) {
        // Change activity based on stats and personality
        if (stats.energy < 20) {
            currentActivity = SLEEPING;
            isAsleep = true;
        } else if (stats.hunger < 30) {
            currentActivity = EATING;
            isAsleep = false;
        } else if (currentTime - lastPlay > 1800000) { // 30 minutes
            currentActivity = PLAYING;
            isAsleep = false;
        } else {
            // Random activity based on personality
            switch (personality) {
                case CURIOUS_EXPLORER:
                    currentActivity = (random(0, 100) < 60) ? EXPLORING : LEARNING;
                    break;
                case LAZY_SLEEPER:
                    currentActivity = (random(0, 100) < 40) ? SLEEPING : BEING_PETTED;
                    break;
                case HYPERACTIVE_HACKER:
                    currentActivity = (random(0, 100) < 70) ? HACKING_WITH_YOU : EXERCISING;
                    break;
                case SHY_OBSERVER:
                    currentActivity = (random(0, 100) < 50) ? LEARNING : EXPLORING;
                    break;
                case SOCIAL_BUTTERFLY:
                    currentActivity = SOCIALIZING;
                    break;
                case PERFECTIONIST:
                    currentActivity = (random(0, 100) < 80) ? LEARNING : HACKING_WITH_YOU;
                    break;
                case REBEL_SPIRIT:
                    currentActivity = (random(0, 100) < 60) ? HACKING_WITH_YOU : PLAYING;
                    break;
            }
            isAsleep = false;
        }
    }
    
    void updateAge(unsigned long currentTime) {
        // Age pet (1 real hour = 1 pet day)
        int newAgeDays = (currentTime - birthTime) / 3600000;
        if (newAgeDays > stats.age_days) {
            stats.age_days = newAgeDays;
            
            // Birthday effects
            stats.happiness = min(100, stats.happiness + 20);
            addMemory("It's my birthday! 🎂", "celebrated", "purrs happily", 5);
            
            if (nekoSounds) {
                nekoSounds->playContextSound("birthday");
            }
        }
    }
    
    void updateMemories(unsigned long currentTime) {
        // Remove old memories based on importance
        for (int i = memories.size() - 1; i >= 0; i--) {
            unsigned long memoryAge = currentTime - memories[i].timestamp;
            int maxAgeHours = memories[i].importance_level * 24; // Days to hours
            
            if (memoryAge > maxAgeHours * 3600000) {
                memories.erase(memories.begin() + i);
            }
        }
    }
    
    void checkEvolution() {
        PetStage nextStage = currentStage;
        
        switch (currentStage) {
            case EGG_STAGE:
                if (stats.age_days >= 1) {
                    nextStage = KITTEN_STAGE;
                }
                break;
            case KITTEN_STAGE:
                if (stats.age_days >= 3 && stats.hack_skill >= 50) {
                    nextStage = TEEN_STAGE;
                }
                break;
            case TEEN_STAGE:
                if (stats.age_days >= 7 && stats.hack_skill >= 150 && stats.intelligence >= 100) {
                    nextStage = ADULT_STAGE;
                }
                break;
            case ADULT_STAGE:
                if (stats.age_days >= 14 && stats.hack_skill >= 300 && stats.total_hacks_witnessed >= 50) {
                    nextStage = ELITE_STAGE;
                }
                break;
            case ELITE_STAGE:
                if (stats.age_days >= 30 && stats.hack_skill >= 500 && stats.loyalty >= 200) {
                    nextStage = LEGENDARY_STAGE;
                }
                break;
        }
        
        if (nextStage != currentStage) {
            isEvolutionReady = true;
        }
    }
    
    void evolve() {
        if (!isEvolutionReady) return;
        
        currentStage = (PetStage)(currentStage + 1);
        appearance.main_emoji = stageEmojis[currentStage];
        
        // Evolution bonuses
        stats.experience_points += 100;
        stats.level++;
        stats.happiness = 100;
        stats.energy = 100;
        
        // Unlock new accessories
        if (currentStage == TEEN_STAGE) {
            appearance.unlocked_accessories.push_back("🎓"); // Graduation cap
        } else if (currentStage == ADULT_STAGE) {
            appearance.unlocked_accessories.push_back("💼"); // Briefcase
        } else if (currentStage == ELITE_STAGE) {
            appearance.unlocked_accessories.push_back("👑"); // Crown
        } else if (currentStage == LEGENDARY_STAGE) {
            appearance.unlocked_accessories.push_back("✨"); // Sparkles
            appearance.has_special_animation = true;
        }
        
        addMemory("I evolved to " + stageNames[currentStage] + "! 🎉", 
                 "evolved", "dances with joy", 5);
        
        isEvolutionReady = false;
        
        if (nekoSounds) {
            nekoSounds->playContextSound("evolution");
        }
        
        Serial.printf("🎉 Pet evolved to %s stage!\n", stageNames[currentStage]);
    }
    
    // ==================== PET INTERACTIONS ====================
    void feedPet(int foodIndex) {
        if (foodIndex >= availableFoods.size()) return;
        
        PetFood& food = availableFoods[foodIndex];
        if (!food.unlocked) return;
        
        // Apply food effects
        stats.hunger = min(100, stats.hunger + food.hunger_restore);
        stats.happiness = min(100, stats.happiness + food.happiness_bonus);
        stats.energy = constrain(stats.energy + food.energy_change, 0, 100);
        
        // Pet reaction based on personality and food
        String reaction = getPersonalityResponse("food_" + food.name);
        lastReaction = reaction;
        reactionTime = millis();
        
        addMemory("You fed me " + food.name + " " + food.emoji, 
                 "fed", reaction, 3);
        
        lastFeed = millis();
        needsAttention = false;
        
        if (nekoSounds) {
            nekoSounds->playContextSound("eating");
        }
        
        Serial.printf("🍽️ Fed pet %s, hunger: %d\n", food.name.c_str(), stats.hunger);
    }
    
    void playWithPet(int toyIndex) {
        if (toyIndex >= availableToys.size()) return;
        
        PetToy& toy = availableToys[toyIndex];
        if (!toy.unlocked) return;
        
        // Apply toy effects
        stats.happiness = min(100, stats.happiness + toy.happiness_boost);
        stats.energy = max(0, stats.energy - toy.energy_cost);
        stats.games_played++;
        
        // Special skill bonuses for specific toys
        if (toy.name == "Hacker Keyboard") {
            stats.hack_skill = min(1000, stats.hack_skill + 5);
        } else if (toy.name == "WiFi Antenna") {
            stats.intelligence = min(1000, stats.intelligence + 3);
        }
        
        lastReaction = toy.play_description;
        reactionTime = millis();
        
        addMemory("We played with " + toy.name + " " + toy.emoji, 
                 "played", toy.play_description, 4);
        
        lastPlay = millis();
        currentActivity = PLAYING;
        
        if (nekoSounds) {
            nekoSounds->playContextSound("playing");
        }
        
        Serial.printf("🎮 Played with %s, happiness: %d\n", toy.name.c_str(), stats.happiness);
    }
    
    void petThePet() {
        stats.happiness = min(100, stats.happiness + 10);
        stats.loyalty = min(1000, stats.loyalty + 2);
        stats.times_petted++;
        
        String reaction = getPersonalityResponse("petting");
        lastReaction = reaction;
        reactionTime = millis();
        
        addMemory("You petted me gently 🐾", "petted", reaction, 2);
        
        currentActivity = BEING_PETTED;
        
        if (nekoSounds) {
            nekoSounds->playSound(SOUND_PURR_SUCCESS);
        }
        
        Serial.printf("🐾 Pet petted, happiness: %d, loyalty: %d\n", 
                     stats.happiness, stats.loyalty);
    }
    
    void cleanPet() {
        stats.cleanliness = 100;
        stats.health = min(100, stats.health + 5);
        stats.happiness = min(100, stats.happiness + 5);
        
        lastReaction = "Ah~ Much better! Thank you! ✨";
        reactionTime = millis();
        
        addMemory("You cleaned me up 🛁", "cleaned", "feels fresh and happy", 2);
        
        if (nekoSounds) {
            nekoSounds->playContextSound("cleaning");
        }
        
        Serial.printf("🛁 Pet cleaned, cleanliness: %d\n", stats.cleanliness);
    }
    
    void letPetSleep() {
        if (currentActivity != SLEEPING) {
            currentActivity = SLEEPING;
            isAsleep = true;
            lastSleep = millis();
            
            lastReaction = "Zzz... dreaming of hacks... 😴";
            reactionTime = millis();
            
            if (nekoSounds) {
                nekoSounds->playContextSound("sleeping");
            }
        } else {
            // Wake up pet
            isAsleep = false;
            currentActivity = EXPLORING;
            stats.energy = min(100, stats.energy + 30);
            
            lastReaction = "Yawn~ Good morning! Ready to hack! 😸";
            reactionTime = millis();
            
            if (nekoSounds) {
                nekoSounds->playContextSound("waking_up");
            }
        }
    }
    
    // ==================== HACK REACTIONS ====================
    void witnessHackSuccess(String hackType) {
        stats.total_hacks_witnessed++;
        stats.hack_skill = min(1000, stats.hack_skill + 3);
        stats.experience_points += 10;
        stats.happiness = min(100, stats.happiness + 5);
        
        String reaction = getPersonalityResponse(getPersonalityKey() + "_hack_success");
        lastReaction = reaction;
        reactionTime = millis();
        
        addMemory("Witnessed your " + hackType + " success! 🎯", 
                 "witnessed_hack", reaction, 4);
        
        // Special reactions for different hack types
        if (hackType.indexOf("handshake") >= 0) {
            stats.intelligence = min(1000, stats.intelligence + 2);
        } else if (hackType.indexOf("deauth") >= 0) {
            stats.creativity = min(1000, stats.creativity + 2);
        }
        
        if (nekoSounds) {
            nekoSounds->playContextSound("hack_success_reaction");
        }
        
        Serial.printf("👁️ Pet witnessed %s success, skill: %d\n", 
                     hackType.c_str(), stats.hack_skill);
    }
    
    void witnessHackFailure(String hackType) {
        String reaction = getPersonalityResponse(getPersonalityKey() + "_hack_fail");
        lastReaction = reaction;
        reactionTime = millis();
        
        addMemory("Saw your " + hackType + " attempt", 
                 "witnessed_fail", reaction, 2);
        
        // Comfort you
        stats.loyalty = min(1000, stats.loyalty + 1);
        
        if (nekoSounds) {
            nekoSounds->playContextSound("comfort");
        }
    }
    
    void reactToNewNetwork(String networkName) {
        stats.intelligence = min(1000, stats.intelligence + 1);
        
        lastReaction = "Ooh! New network: " + networkName + " 📡";
        reactionTime = millis();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("discovery");
        }
    }
    
    void reactToMusic() {
        stats.happiness = min(100, stats.happiness + 3);
        currentActivity = (random(0, 100) < 50) ? PLAYING : EXERCISING;
        
        lastReaction = "I love this music! *dances* 🎵";
        reactionTime = millis();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("dancing");
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void start() {
        currentScreen = PET_MAIN;
        selectedItem = 0;
        drawPetScreen();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("pet_greeting");
        }
    }
    
    void drawPetScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentScreen) {
            case PET_MAIN:
                drawMainPetView();
                break;
            case PET_STATS:
                drawStatsView();
                break;
            case PET_CARE:
                drawCareView();
                break;
            case PET_PLAY:
                drawPlayView();
                break;
            case PET_CUSTOMIZE:
                drawCustomizeView();
                break;
            case PET_MEMORY:
                drawMemoryView();
                break;
            case PET_EVOLUTION:
                drawEvolutionView();
                break;
            case PET_SETTINGS:
                drawSettingsView();
                break;
        }
    }
    
    void drawMainPetView() {
        // Pet display area
        M5.Display.fillRect(60, 30, 120, 80, DARKGREY);
        M5.Display.drawRect(60, 30, 120, 80, WHITE);
        
        // Pet sprite (large)
        M5.Display.setTextSize(3);
        M5.Display.setTextColor(appearance.primary_color);
        M5.Display.setCursor(110, 50);
        M5.Display.print(appearance.main_emoji);
        
        // Pet expression/mood
        M5.Display.setTextSize(2);
        M5.Display.setCursor(130, 75);
        M5.Display.print(appearance.current_expression);
        
        // Special animation for legendary pets
        if (appearance.has_special_animation && currentStage == LEGENDARY_STAGE) {
            // Draw sparkles around pet
            for (int i = 0; i < 3; i++) {
                int sparkleX = 80 + random(-20, 140);
                int sparkleY = 40 + random(-10, 70);
                M5.Display.setTextColor(YELLOW);
                M5.Display.setCursor(sparkleX, sparkleY);
                M5.Display.print("✨");
            }
        }
        
        // Pet name and stage
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(90, 15);
        M5.Display.print(appearance.name + " the " + stageNames[currentStage]);
        
        // Current activity
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(85, 115);
        M5.Display.print(getActivityString());
        
        // Recent reaction/thought
        if (millis() - reactionTime < 5000 && !lastReaction.isEmpty()) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 125);
            String displayReaction = lastReaction;
            if (displayReaction.length() > 35) {
                displayReaction = displayReaction.substring(0, 35) + "...";
            }
            M5.Display.print(displayReaction);
        }
        
        // Quick stats bar
        drawQuickStats();
        
        // Attention indicator
        if (needsAttention) {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(10, 10);
            M5.Display.print("❗ Needs attention!");
        }
        
        // Evolution ready indicator
        if (isEvolutionReady) {
            M5.Display.setTextColor(MAGENTA);
            M5.Display.setCursor(150, 10);
            M5.Display.print("🎉 Can evolve!");
        }
    }
    
    void drawQuickStats() {
        // Quick stats at bottom
        int barY = 130;
        int barWidth = 35;
        
        // Hunger bar
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(5, barY - 8);
        M5.Display.print("🍽");
        drawStatBar(5, barY, barWidth, stats.hunger, ORANGE);
        
        // Happiness bar
        M5.Display.setTextColor(PINK);
        M5.Display.setCursor(50, barY - 8);
        M5.Display.print("😊");
        drawStatBar(50, barY, barWidth, stats.happiness, PINK);
        
        // Energy bar
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(95, barY - 8);
        M5.Display.print("⚡");
        drawStatBar(95, barY, barWidth, stats.energy, YELLOW);
        
        // Health bar
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(140, barY - 8);
        M5.Display.print("❤️");
        drawStatBar(140, barY, barWidth, stats.health, GREEN);
        
        // Level
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(190, barY - 8);
        M5.Display.printf("Lv.%d", stats.level);
    }
    
    void drawStatBar(int x, int y, int width, int value, uint16_t color) {
        // Background
        M5.Display.drawRect(x, y, width, 4, DARKGREY);
        
        // Fill based on value
        int fillWidth = (value * width) / 100;
        M5.Display.fillRect(x + 1, y + 1, fillWidth - 1, 2, color);
    }
    
    void drawStatsView() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(80, 5);
        M5.Display.print("📊 Pet Stats");
        
        // Core needs
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 20);
        M5.Display.print("=== CORE NEEDS ===");
        
        M5.Display.setCursor(10, 35);
        M5.Display.printf("Hunger:     %d/100", stats.hunger);
        M5.Display.setCursor(10, 45);
        M5.Display.printf("Happiness:  %d/100", stats.happiness);
        M5.Display.setCursor(10, 55);
        M5.Display.printf("Energy:     %d/100", stats.energy);
        M5.Display.setCursor(10, 65);
        M5.Display.printf("Health:     %d/100", stats.health);
        M5.Display.setCursor(10, 75);
        M5.Display.printf("Clean:      %d/100", stats.cleanliness);
        
        // Growth stats
        M5.Display.setCursor(10, 90);
        M5.Display.print("=== SKILLS ===");
        M5.Display.setCursor(10, 105);
        M5.Display.printf("Hack Skill: %d/1000", stats.hack_skill);
        M5.Display.setCursor(10, 115);
        M5.Display.printf("Intelligence: %d/1000", stats.intelligence);
        M5.Display.setCursor(10, 125);
        M5.Display.printf("Loyalty:    %d/1000", stats.loyalty);
    }
    
    void drawCareView() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(70, 5);
        M5.Display.print("🍽️ Pet Care");
        
        // Food options
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.print("=== FOOD ===");
        
        for (int i = 0; i < min(3, (int)availableFoods.size()); i++) {
            PetFood& food = availableFoods[i];
            uint16_t color = (i == selectedItem) ? YELLOW : 
                           (food.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 40 + i * 12);
            M5.Display.print(food.emoji + " " + food.name);
            
            if (food.unlocked) {
                M5.Display.setCursor(120, 40 + i * 12);
                M5.Display.printf("H+%d E%+d", food.hunger_restore, food.energy_change);
            } else {
                M5.Display.setCursor(120, 40 + i * 12);
                M5.Display.print("LOCKED");
            }
        }
        
        // Care actions
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 85);
        M5.Display.print("=== CARE ===");
        
        String careActions[] = {"🛁 Clean Pet", "😴 Sleep Time", "🐾 Pet Gently"};
        for (int i = 0; i < 3; i++) {
            uint16_t color = (i + 3 == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 100 + i * 10);
            M5.Display.print(careActions[i]);
        }
    }
    
    void drawPlayView() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(80, 5);
        M5.Display.print("🎮 Play Time");
        
        // Available toys
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.print("Choose a toy:");
        
        for (int i = 0; i < min(4, (int)availableToys.size()); i++) {
            PetToy& toy = availableToys[i];
            uint16_t color = (i == selectedItem) ? YELLOW : 
                           (toy.unlocked ? WHITE : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 15);
            M5.Display.print(toy.emoji + " " + toy.name);
            
            if (toy.unlocked) {
                M5.Display.setTextColor(CYAN);
                M5.Display.setCursor(15, 45 + i * 15 + 8);
                M5.Display.printf("Fun +%d, Energy -%d", toy.happiness_boost, toy.energy_cost);
            } else {
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(15, 45 + i * 15 + 8);
                M5.Display.print(toy.unlock_condition);
            }
        }
    }
    
    void drawCustomizeView() {
        M5.Display.setTextColor(PINK);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🎨 Customize Pet");
        
        // Pet preview
        M5.Display.setTextSize(2);
        M5.Display.setTextColor(appearance.primary_color);
        M5.Display.setCursor(100, 30);
        M5.Display.print(appearance.main_emoji);
        
        // Current accessory
        if (!appearance.accessory.isEmpty()) {
            M5.Display.setCursor(120, 25);
            M5.Display.print(appearance.accessory);
        }
        
        // Customization options
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 60);
        M5.Display.print("Name: " + appearance.name);
        
        M5.Display.setCursor(10, 75);
        M5.Display.print("Accessories:");
        
        for (int i = 0; i < min(3, (int)appearance.unlocked_accessories.size()); i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 90 + i * 10);
            M5.Display.print(appearance.unlocked_accessories[i] + " Accessory " + String(i + 1));
        }
    }
    
    void drawMemoryView() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(70, 5);
        M5.Display.print("🧠 Pet Memories");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 20);
        M5.Display.printf("Total memories: %d", memories.size());
        
        // Show recent memories
        int displayCount = min(5, (int)memories.size());
        for (int i = 0; i < displayCount; i++) {
            int memIdx = memories.size() - 1 - i; // Show newest first
            PetMemory& memory = memories[memIdx];
            
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 35 + i * 18);
            
            String shortDesc = memory.event_description;
            if (shortDesc.length() > 25) {
                shortDesc = shortDesc.substring(0, 25) + "...";
            }
            M5.Display.print(shortDesc);
            
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(10, 35 + i * 18 + 8);
            M5.Display.print(formatMemoryTime(memory.timestamp));
        }
    }
    
    void drawEvolutionView() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🎉 Evolution Tree");
        
        // Current stage
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(10, 25);
        M5.Display.print("Current: " + stageNames[currentStage] + " " + stageEmojis[currentStage]);
        
        // Next stage requirements
        if (currentStage < LEGENDARY_STAGE) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 45);
            M5.Display.print("Next: " + stageNames[currentStage + 1] + " " + stageEmojis[currentStage + 1]);
            
            M5.Display.setCursor(10, 60);
            M5.Display.print("Requirements:");
            
            // Show requirements based on current stage
            switch (currentStage) {
                case KITTEN_STAGE:
                    M5.Display.setCursor(15, 75);
                    M5.Display.printf("Age: %d/3 days", stats.age_days);
                    M5.Display.setCursor(15, 85);
                    M5.Display.printf("Hack Skill: %d/50", stats.hack_skill);
                    break;
                case TEEN_STAGE:
                    M5.Display.setCursor(15, 75);
                    M5.Display.printf("Age: %d/7 days", stats.age_days);
                    M5.Display.setCursor(15, 85);
                    M5.Display.printf("Hack Skill: %d/150", stats.hack_skill);
                    M5.Display.setCursor(15, 95);
                    M5.Display.printf("Intelligence: %d/100", stats.intelligence);
                    break;
                // Add more cases...
            }
            
            if (isEvolutionReady) {
                M5.Display.setTextColor(GREEN);
                M5.Display.setCursor(60, 110);
                M5.Display.print("⏎ EVOLVE NOW!");
            }
        } else {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(40, 50);
            M5.Display.print("🌟 MAX EVOLUTION! 🌟");
            M5.Display.setCursor(20, 70);
            M5.Display.print("Your pet is legendary!");
        }
    }
    
    void drawSettingsView() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(70, 5);
        M5.Display.print("⚙️ Pet Settings");
        
        String settingsItems[] = {
            "📝 Rename Pet",
            "🎭 Change Personality",
            "🔊 Sound Effects: " + String(nekoSounds ? "ON" : "OFF"),
            "📊 Reset Stats",
            "🗑️ Clear Memories",
            "← Back"
        };
        
        for (int i = 0; i < 6; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 30 + i * 15);
            M5.Display.print(settingsItems[i]);
        }
        
        // Current personality
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(10, 115);
        M5.Display.print("Personality: " + getPersonalityName());
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
        
        drawPetScreen();
    }
    
    void navigateUp() {
        if (selectedItem > 0) selectedItem--;
    }
    
    void navigateDown() {
        int maxItems = getMaxItemsForCurrentScreen();
        if (selectedItem < maxItems - 1) selectedItem++;
    }
    
    void navigateLeft() {
        // Navigate between screens
        if (currentScreen > PET_MAIN) {
            currentScreen = (PetScreen)(currentScreen - 1);
            selectedItem = 0;
        }
    }
    
    void navigateRight() {
        // Navigate between screens
        if (currentScreen < PET_SETTINGS) {
            currentScreen = (PetScreen)(currentScreen + 1);
            selectedItem = 0;
        }
    }
    
    void selectCurrentItem() {
        switch (currentScreen) {
            case PET_MAIN:
                // Quick actions on main screen
                if (needsAttention) {
                    if (stats.hunger < 30) {
                        currentScreen = PET_CARE;
                    } else if (stats.happiness < 25) {
                        currentScreen = PET_PLAY;
                    } else {
                        petThePet();
                    }
                } else if (isEvolutionReady) {
                    evolve();
                } else {
                    petThePet();
                }
                break;
                
            case PET_CARE:
                if (selectedItem < availableFoods.size()) {
                    feedPet(selectedItem);
                } else if (selectedItem == 3) {
                    cleanPet();
                } else if (selectedItem == 4) {
                    letPetSleep();
                } else if (selectedItem == 5) {
                    petThePet();
                }
                break;
                
            case PET_PLAY:
                if (selectedItem < availableToys.size()) {
                    playWithPet(selectedItem);
                }
                break;
                
            case PET_CUSTOMIZE:
                if (selectedItem < appearance.unlocked_accessories.size()) {
                    appearance.accessory = appearance.unlocked_accessories[selectedItem];
                }
                break;
                
            case PET_EVOLUTION:
                if (isEvolutionReady) {
                    evolve();
                }
                break;
                
            case PET_SETTINGS:
                handleSettingsSelection();
                break;
        }
    }
    
    void goBack() {
        currentScreen = PET_MAIN;
        selectedItem = 0;
    }
    
    void handleSettingsSelection() {
        switch (selectedItem) {
            case 0: { // Rename Pet
                // Simple name cycling for demo
                String names[] = {"Neko", "Cyber", "Hacker", "Kawaii", "Shadow", "Pixel"};
                for (int i = 0; i < 6; i++) {
                    if (appearance.name == names[i]) {
                        appearance.name = names[(i + 1) % 6];
                        break;
                    }
                }
                break;
            }
            case 1: { // Change Personality
                personality = (PetPersonality)((personality + 1) % 7);
                break;
            }
            case 3: { // Reset Stats (dangerous!)
                if (stats.loyalty > 500) { // Only if very loyal
                    initializePet();
                }
                break;
            }
            case 4: { // Clear Memories
                memories.clear();
                break;
            }
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    String getActivityString() {
        switch (currentActivity) {
            case SLEEPING: return "😴 Sleeping peacefully";
            case EATING: return "🍽️ Munching food";
            case PLAYING: return "🎮 Playing happily";
            case HACKING_WITH_YOU: return "💻 Hacking together";
            case EXPLORING: return "🔍 Exploring around";
            case SOCIALIZING: return "👥 Being social";
            case LEARNING: return "📚 Learning new things";
            case DREAMING: return "💭 Dreaming of hacks";
            case EXERCISING: return "🏃 Getting exercise";
            case BEING_PETTED: return "🐾 Enjoying pets";
            default: return "🤔 Thinking";
        }
    }
    
    String getPersonalityKey() {
        switch (personality) {
            case CURIOUS_EXPLORER: return "curious";
            case LAZY_SLEEPER: return "lazy";
            case HYPERACTIVE_HACKER: return "hyperactive";
            case SHY_OBSERVER: return "shy";
            case SOCIAL_BUTTERFLY: return "social";
            case PERFECTIONIST: return "perfectionist";
            case REBEL_SPIRIT: return "rebel";
            default: return "curious";
        }
    }
    
    String getPersonalityName() {
        switch (personality) {
            case CURIOUS_EXPLORER: return "Curious Explorer";
            case LAZY_SLEEPER: return "Lazy Sleeper";
            case HYPERACTIVE_HACKER: return "Hyperactive Hacker";
            case SHY_OBSERVER: return "Shy Observer";
            case SOCIAL_BUTTERFLY: return "Social Butterfly";
            case PERFECTIONIST: return "Perfectionist";
            case REBEL_SPIRIT: return "Rebel Spirit";
            default: return "Unknown";
        }
    }
    
    String getPersonalityResponse(String key) {
        if (personalityResponses.find(key) != personalityResponses.end()) {
            return personalityResponses[key];
        }
        return "Nya~ 😸"; // Default response
    }
    
    void addMemory(String event, String action, String reaction, int importance) {
        PetMemory memory;
        memory.event_description = event;
        memory.timestamp = millis();
        memory.your_action = action;
        memory.pet_reaction = reaction;
        memory.importance_level = importance;
        
        memories.push_back(memory);
        
        // Keep only last 50 memories to save memory
        if (memories.size() > 50) {
            memories.erase(memories.begin());
        }
    }
    
    String formatMemoryTime(unsigned long timestamp) {
        unsigned long elapsed = millis() - timestamp;
        int minutes = elapsed / 60000;
        
        if (minutes < 1) return "Just now";
        if (minutes < 60) return String(minutes) + "m ago";
        int hours = minutes / 60;
        if (hours < 24) return String(hours) + "h ago";
        return String(hours / 24) + "d ago";
    }
    
    int getMaxItemsForCurrentScreen() {
        switch (currentScreen) {
            case PET_CARE: return 6; // 3 foods + 3 care actions
            case PET_PLAY: return availableToys.size();
            case PET_CUSTOMIZE: return appearance.unlocked_accessories.size();
            case PET_MEMORY: return min(5, (int)memories.size());
            case PET_SETTINGS: return 6;
            default: return 1;
        }
    }
    
    void updateAnimations() {
        if (millis() - lastAnimUpdate > 200) {
            animationFrame++;
            if (animationFrame > 1000) animationFrame = 0;
            lastAnimUpdate = millis();
        }
    }
    
    void savePetData() {
        Serial.println("💾 Saving pet data...");
        // Would save to SD card in real implementation
    }
    
    void loadPetData() {
        Serial.println("📂 Loading pet data...");
        // Would load from SD card in real implementation
    }
    
    // ==================== API FUNCTIONS ====================
    
    // Called by other systems to trigger reactions
    void notifyHackSuccess(String hackType) { witnessHackSuccess(hackType); }
    void notifyHackFailure(String hackType) { witnessHackFailure(hackType); }
    void notifyNewNetwork(String networkName) { reactToNewNetwork(networkName); }
    void notifyMusicPlaying() { reactToMusic(); }
    
    // Getters
    PetStage getCurrentStage() { return currentStage; }
    PetMood getCurrentMood() { return currentMood; }
    String getPetName() { return appearance.name; }
    int getHappiness() { return stats.happiness; }
    int getHackSkill() { return stats.hack_skill; }
    int getLoyalty() { return stats.loyalty; }
    int getAge() { return stats.age_days; }
    bool needsAttentionNow() { return needsAttention; }
    bool canEvolve() { return isEvolutionReady; }
    bool isAsleepNow() { return isAsleep; }
    
    // Quick care functions for main app
    void quickFeed() { if (!availableFoods.empty()) feedPet(0); }
    void quickPlay() { if (!availableToys.empty()) playWithPet(0); }
    void quickPet() { petThePet(); }
};

#endif // M5GOTCHI_NEKO_VIRTUAL_PET_H