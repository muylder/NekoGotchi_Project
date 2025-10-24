/*
 * 🏆 M5GOTCHI NEKO ACHIEVEMENT SYSTEM v1.0
 * Sistema de conquistas kawaii para gamificar toda experiência M5Gotchi!
 * 
 * Features:
 * - 50+ achievements únicos temáticos de hacker neko
 * - Sistema de badges com raridade (Bronze, Silver, Gold, Platinum, Kawaii)
 * - Progress tracking para todas as atividades
 * - Notificações cute de unlock
 * - Reward system com unlocks especiais
 * - Statistics completas e leaderboards locais
 * 
 * Achievement Categories:
 * 🔓 Hacking Mastery - WiFi attacks, bypasses, exploits
 * 🎮 Gaming Glory - Mini-games, high scores, challenges  
 * 🐾 Kawaii Collector - Cute unlocks, personalities, themes
 * 🎵 Sound Master - Audio achievements, melodies, reactions
 * 📊 Data Detective - Analysis, discoveries, intelligence
 * 🏰 Social Butterfly - Interactions, sharing, community
 * 
 * Ganbatte, neko hacker! 💖
 */

#ifndef M5GOTCHI_NEKO_ACHIEVEMENTS_H
#define M5GOTCHI_NEKO_ACHIEVEMENTS_H

#include <M5Unified.h>
#include <vector>
#include <map>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_neko_sounds.h"

// ==================== ACHIEVEMENT ENUMS ====================
enum AchievementCategory {
    HACKING_MASTERY = 0,
    GAMING_GLORY,
    KAWAII_COLLECTOR,
    SOUND_MASTER,
    DATA_DETECTIVE,
    SOCIAL_BUTTERFLY,
    SECRET_ACHIEVEMENTS
};

enum AchievementRarity {
    BRONZE = 0,      // Common achievements
    SILVER,          // Uncommon achievements  
    GOLD,            // Rare achievements
    PLATINUM,        // Epic achievements
    KAWAII,          // Legendary kawaii achievements
    ULTRA_RARE      // Ultra rare secret achievements
};

enum AchievementType {
    COUNTER = 0,     // Count-based (do X times)
    MILESTONE,       // Reach specific value
    SEQUENCE,        // Do things in specific order
    CHALLENGE,       // Special conditions
    HIDDEN,          // Secret requirements
    TIME_LIMITED    // Must be done within timeframe
};

// ==================== ACHIEVEMENT STRUCTURES ====================
struct Achievement {
    int id;
    String name;
    String description;
    String emoji;
    AchievementCategory category;
    AchievementRarity rarity;
    AchievementType type;
    
    // Progress tracking
    int current_progress;
    int required_progress;
    bool unlocked;
    unsigned long unlock_time;
    
    // Rewards
    int points_reward;
    String unlock_message;
    String special_unlock;  // Special content unlocked
    
    // Display
    uint16_t color;
    String category_name;
    bool is_new;           // For notification system
};

struct PlayerStats {
    // Hacking stats
    int networks_scanned;
    int handshakes_captured;
    int deauth_attacks;
    int evil_portals_created;
    int bluetooth_attacks;
    int successful_hacks;
    
    // Gaming stats
    int games_played;
    int high_scores_beaten;
    int mini_games_completed;
    int sushi_eaten;
    int paws_scanned;
    
    // Social stats
    int sounds_played;
    int personalities_unlocked;
    int themes_unlocked;
    int friends_added;
    
    // General stats
    int total_uptime_minutes;
    int button_presses;
    int screens_viewed;
    int easter_eggs_found;
    
    // Achievement stats
    int total_achievements;
    int bronze_achievements;
    int silver_achievements;
    int gold_achievements;
    int platinum_achievements;
    int kawaii_achievements;
    int total_points;
};

// ==================== NEKO ACHIEVEMENT SYSTEM CLASS ====================
class M5GotchiNekoAchievements {
private:
    std::vector<Achievement> achievements;
    PlayerStats stats;
    
    // Display state
    int selectedCategory;
    int selectedAchievement;
    int currentPage;
    bool showingDetails;
    
    // Notification system
    std::vector<int> pendingNotifications;
    unsigned long lastNotificationTime;
    bool showingNotification;
    int currentNotificationId;
    
    // Animation
    int animationFrame;
    unsigned long lastAnimUpdate;
    
    // Sound system
    M5GotchiNekoSounds* nekoSounds;
    
    // Colors for rarities
    uint16_t rarityColors[6] = {
        0x8410,    // Bronze - brownish
        0xC618,    // Silver - light gray
        0xFFE0,    // Gold - yellow
        0xF81F,    // Platinum - magenta
        0xF81F,    // Kawaii - pink
        0x07FF     // Ultra Rare - cyan
    };

public:
    M5GotchiNekoAchievements() {
        selectedCategory = 0;
        selectedAchievement = 0;
        currentPage = 0;
        showingDetails = false;
        showingNotification = false;
        currentNotificationId = -1;
        lastNotificationTime = 0;
        animationFrame = 0;
        lastAnimUpdate = 0;
        nekoSounds = nullptr;
        
        initializeStats();
        initializeAchievements();
        loadProgress();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🏆 Initializing Neko Achievement System...");
        
        initializeAchievements();
        loadProgress();
        checkInitialAchievements();
        
        Serial.println("✅ Achievement System Ready! Time to earn badges! 🎖️");
    }
    
    void setSoundSystem(M5GotchiNekoSounds* sounds) {
        nekoSounds = sounds;
    }
    
    void initializeStats() {
        // Reset all stats
        stats = {0}; // Zero-initialize all fields
        
        Serial.println("📊 Player stats initialized");
    }
    
    void initializeAchievements() {
        achievements.clear();
        
        // ==================== HACKING MASTERY ACHIEVEMENTS ====================
        
        // Bronze Hacking
        addAchievement(1, "First Scan", "Scan your first WiFi network", "📡", 
                      HACKING_MASTERY, BRONZE, COUNTER, 1, 50, 
                      "Welcome to the neko hacker world!", "");
        
        addAchievement(2, "Network Explorer", "Scan 50 WiFi networks", "🗺️", 
                      HACKING_MASTERY, BRONZE, COUNTER, 50, 100, 
                      "You're getting the hang of this!", "");
        
        addAchievement(3, "First Handshake", "Capture your first handshake", "🤝", 
                      HACKING_MASTERY, BRONZE, COUNTER, 1, 150, 
                      "Pawsome! Your first handshake captured!", "");
        
        // Silver Hacking  
        addAchievement(4, "Handshake Collector", "Capture 10 handshakes", "📦", 
                      HACKING_MASTERY, SILVER, COUNTER, 10, 300, 
                      "You're becoming a real neko hacker!", "advanced_scanner");
        
        addAchievement(5, "Deauth Master", "Perform 25 deauth attacks", "💥", 
                      HACKING_MASTERY, SILVER, COUNTER, 25, 400, 
                      "Network disruption master achieved!", "stealth_mode");
        
        addAchievement(6, "Portal Creator", "Deploy 5 evil portals", "🕳️", 
                      HACKING_MASTERY, SILVER, COUNTER, 5, 500, 
                      "Social engineering skills unlocked!", "custom_portals");
        
        // Gold Hacking
        addAchievement(7, "Bluetooth Ninja", "Complete 15 BLE attacks", "🥷", 
                      HACKING_MASTERY, GOLD, COUNTER, 15, 750, 
                      "Wireless ninja skills mastered!", "ble_advanced");
        
        addAchievement(8, "Hack Streak", "Complete 10 successful hacks in a row", "🔥", 
                      HACKING_MASTERY, GOLD, SEQUENCE, 10, 1000, 
                      "Unstoppable hacking streak!", "lucky_charm");
        
        // Platinum Hacking
        addAchievement(9, "Elite Hacker", "Achieve 100 successful hacks", "👑", 
                      HACKING_MASTERY, PLATINUM, MILESTONE, 100, 2000, 
                      "Welcome to the elite neko hackers!", "elite_badge");
        
        // ==================== GAMING GLORY ACHIEVEMENTS ====================
        
        // Bronze Gaming
        addAchievement(10, "First Game", "Play your first mini-game", "🎮", 
                       GAMING_GLORY, BRONZE, COUNTER, 1, 50, 
                       "Game time! Let's have some fun!", "");
        
        addAchievement(11, "Sushi Apprentice", "Eat 5 sushi in SQL game", "🍣", 
                       GAMING_GLORY, BRONZE, COUNTER, 5, 100, 
                       "Delicious SQL injection skills!", "");
        
        addAchievement(12, "Paw Toucher", "Touch 10 paws in scanner game", "🐾", 
                       GAMING_GLORY, BRONZE, COUNTER, 10, 100, 
                       "Such soft digital paws!", "");
        
        // Silver Gaming
        addAchievement(13, "Sushi Master", "Eat 50 sushi with perfect combos", "🍱", 
                       GAMING_GLORY, SILVER, COUNTER, 50, 400, 
                       "SQL injection master achieved!", "sushi_chef_hat");
        
        addAchievement(14, "Paw Scanner Pro", "Successfully bypass 20 scanners", "🔓", 
                       GAMING_GLORY, SILVER, COUNTER, 20, 500, 
                       "Biometric bypass expert!", "bypass_tools");
        
        addAchievement(15, "High Score Hunter", "Beat 10 personal high scores", "📈", 
                       GAMING_GLORY, SILVER, COUNTER, 10, 300, 
                       "Always improving! Great job!", "score_multiplier");
        
        // Gold Gaming
        addAchievement(16, "Perfect Combo", "Execute 5 perfect game combos", "⭐", 
                       GAMING_GLORY, GOLD, CHALLENGE, 5, 750, 
                       "Perfection achieved! Sugoi!", "perfect_aura");
        
        addAchievement(17, "Game Marathon", "Play for 2 straight hours", "⏰", 
                       GAMING_GLORY, GOLD, TIME_LIMITED, 120, 800, 
                       "Dedication level: Neko master!", "endurance_badge");
        
        // Platinum Gaming
        addAchievement(18, "Gaming Legend", "Unlock all mini-game achievements", "🏆", 
                       GAMING_GLORY, PLATINUM, MILESTONE, 100, 2500, 
                       "Legendary gaming neko achieved!", "legend_crown");
        
        // ==================== KAWAII COLLECTOR ACHIEVEMENTS ====================
        
        // Bronze Kawaii
        addAchievement(19, "First Purr", "Play your first cat sound", "😻", 
                       KAWAII_COLLECTOR, BRONZE, COUNTER, 1, 50, 
                       "Nyaa~! So kawaii!", "");
        
        addAchievement(20, "Sound Explorer", "Play 20 different sounds", "🎵", 
                       KAWAII_COLLECTOR, BRONZE, COUNTER, 20, 100, 
                       "Such a musical neko!", "");
        
        addAchievement(21, "Personality Try", "Try 3 different personalities", "🎭", 
                       KAWAII_COLLECTOR, BRONZE, COUNTER, 3, 150, 
                       "Exploring your inner neko!", "");
        
        // Silver Kawaii  
        addAchievement(22, "Sound Library", "Unlock 50% of all sounds", "📚", 
                       KAWAII_COLLECTOR, SILVER, MILESTONE, 50, 400, 
                       "Impressive sound collection!", "sound_studio");
        
        addAchievement(23, "Neko Psychiatrist", "Try all 7 personalities", "🧠", 
                       KAWAII_COLLECTOR, SILVER, COUNTER, 7, 500, 
                       "Understanding all neko minds!", "personality_expert");
        
        addAchievement(24, "Theme Master", "Unlock 5 visual themes", "🎨", 
                       KAWAII_COLLECTOR, SILVER, COUNTER, 5, 300, 
                       "Aesthetic mastery achieved!", "theme_editor");
        
        // Gold Kawaii
        addAchievement(25, "Kawaii Completionist", "Unlock 80% of all kawaii content", "💖", 
                       KAWAII_COLLECTOR, GOLD, MILESTONE, 80, 1000, 
                       "Maximum kawaii level reached!", "kawaii_aura");
        
        addAchievement(26, "Melody Composer", "Create 10 custom melodies", "🎼", 
                       KAWAII_COLLECTOR, GOLD, COUNTER, 10, 750, 
                       "Musical genius neko!", "composer_tools");
        
        // Platinum Kawaii
        addAchievement(27, "Ultimate Kawaii", "Achieve maximum cuteness in all areas", "🌟", 
                       KAWAII_COLLECTOR, PLATINUM, CHALLENGE, 1, 3000, 
                       "Ultimate kawaii neko hacker!", "ultimate_kawaii");
        
        // ==================== SOUND MASTER ACHIEVEMENTS ====================
        
        // Bronze Sound
        addAchievement(28, "Volume Control", "Adjust volume 10 times", "🔊", 
                       SOUND_MASTER, BRONZE, COUNTER, 10, 50, 
                       "Learning sound control!", "");
        
        addAchievement(29, "Purr Lover", "Play purr sounds 25 times", "😸", 
                       SOUND_MASTER, BRONZE, COUNTER, 25, 100, 
                       "Purr-fect choice! So relaxing!", "");
        
        // Silver Sound
        addAchievement(30, "Sound Effects Pro", "Use context sounds in 50 hacks", "🎭", 
                       SOUND_MASTER, SILVER, COUNTER, 50, 400, 
                       "Sound enhances everything!", "sound_fx_pack");
        
        addAchievement(31, "Melody Master", "Play 100 complete melodies", "🎶", 
                       SOUND_MASTER, SILVER, COUNTER, 100, 500, 
                       "Such musical talent!", "melody_library");
        
        // Gold Sound
        addAchievement(32, "Sound Architect", "Create perfect sound combo", "🏗️", 
                       SOUND_MASTER, GOLD, CHALLENGE, 1, 750, 
                       "Sound design master!", "sound_architect");
        
        // ==================== DATA DETECTIVE ACHIEVEMENTS ====================
        
        // Bronze Data
        addAchievement(33, "First Discovery", "Find your first hidden network", "🔍", 
                       DATA_DETECTIVE, BRONZE, COUNTER, 1, 50, 
                       "Great detective work!", "");
        
        addAchievement(34, "Signal Analyst", "Analyze 100 signal strengths", "📊", 
                       DATA_DETECTIVE, BRONZE, COUNTER, 100, 100, 
                       "Signal analysis expert!", "");
        
        // Silver Data
        addAchievement(35, "Channel Hunter", "Find networks on all WiFi channels", "📡", 
                       DATA_DETECTIVE, SILVER, CHALLENGE, 14, 400, 
                       "Channel surfing master!", "channel_analyzer");
        
        addAchievement(36, "Encryption Expert", "Identify 50 different security types", "🔐", 
                       DATA_DETECTIVE, SILVER, COUNTER, 50, 500, 
                       "Encryption knowledge mastered!", "crypto_tools");
        
        // Gold Data  
        addAchievement(37, "Data Archaeologist", "Uncover 25 hidden system details", "🏺", 
                       DATA_DETECTIVE, GOLD, COUNTER, 25, 800, 
                       "Ancient data secrets revealed!", "archaeologist_tools");
        
        // ==================== SOCIAL BUTTERFLY ACHIEVEMENTS ====================
        
        // Bronze Social
        addAchievement(38, "First Friend", "Add your first device friend", "👥", 
                       SOCIAL_BUTTERFLY, BRONZE, COUNTER, 1, 100, 
                       "Friendship is magic!", "");
        
        addAchievement(39, "Sharing is Caring", "Share 5 discoveries", "💝", 
                       SOCIAL_BUTTERFLY, BRONZE, COUNTER, 5, 150, 
                       "Spreading knowledge!", "");
        
        // Silver Social  
        addAchievement(40, "Community Helper", "Help 10 other users", "🤝", 
                       SOCIAL_BUTTERFLY, SILVER, COUNTER, 10, 400, 
                       "Such a helpful neko!", "helper_badge");
        
        // ==================== SECRET ACHIEVEMENTS ====================
        
        // Hidden achievements (requirements not shown)
        addAchievement(41, "Easter Egg Hunter", "Find 10 hidden easter eggs", "🥚", 
                       SECRET_ACHIEVEMENTS, KAWAII, HIDDEN, 10, 1000, 
                       "Secret hunter extraordinaire!", "easter_detector");
        
        addAchievement(42, "Konami Neko", "Enter secret konami code", "⬆️", 
                       SECRET_ACHIEVEMENTS, ULTRA_RARE, HIDDEN, 1, 2000, 
                       "Old school gaming neko!", "retro_powers");
        
        addAchievement(43, "Time Traveler", "Use device at exactly midnight", "🕛", 
                       SECRET_ACHIEVEMENTS, PLATINUM, HIDDEN, 1, 1500, 
                       "Midnight hacker discovered!", "time_powers");
        
        addAchievement(44, "Lucky Number", "Achieve exactly 777 total points", "🍀", 
                       SECRET_ACHIEVEMENTS, GOLD, HIDDEN, 777, 777, 
                       "Lucky neko strikes again!", "luck_boost");
        
        // Ultra rare secret
        addAchievement(45, "Neko Senpai", "Help another user unlock their first achievement", "🎓", 
                       SECRET_ACHIEVEMENTS, ULTRA_RARE, HIDDEN, 1, 5000, 
                       "Teaching brings the greatest joy!", "senpai_status");
        
        Serial.printf("📋 Loaded %d achievements across %d categories\n", 
                     achievements.size(), 7);
    }
    
    void addAchievement(int id, String name, String description, String emoji,
                       AchievementCategory category, AchievementRarity rarity, 
                       AchievementType type, int required, int points,
                       String message, String unlock) {
        Achievement ach;
        ach.id = id;
        ach.name = name;
        ach.description = description;
        ach.emoji = emoji;
        ach.category = category;
        ach.rarity = rarity;
        ach.type = type;
        ach.current_progress = 0;
        ach.required_progress = required;
        ach.unlocked = false;
        ach.unlock_time = 0;
        ach.points_reward = points;
        ach.unlock_message = message;
        ach.special_unlock = unlock;
        ach.color = rarityColors[rarity];
        ach.is_new = false;
        
        // Set category name
        String categories[] = {"Hacking", "Gaming", "Kawaii", "Sound", "Detective", "Social", "Secret"};
        ach.category_name = categories[category];
        
        achievements.push_back(ach);
    }
    
    // ==================== PROGRESS TRACKING ====================
    void updateProgress(String event, int value = 1) {
        // Update relevant stats based on event
        if (event == "network_scan") {
            stats.networks_scanned += value;
            checkAchievement(1);  // First Scan
            checkAchievement(2);  // Network Explorer
            
        } else if (event == "handshake_capture") {
            stats.handshakes_captured += value;
            checkAchievement(3);  // First Handshake
            checkAchievement(4);  // Handshake Collector
            
        } else if (event == "deauth_attack") {
            stats.deauth_attacks += value;
            checkAchievement(5);  // Deauth Master
            
        } else if (event == "evil_portal") {
            stats.evil_portals_created += value;
            checkAchievement(6);  // Portal Creator
            
        } else if (event == "bluetooth_attack") {
            stats.bluetooth_attacks += value;
            checkAchievement(7);  // Bluetooth Ninja
            
        } else if (event == "successful_hack") {
            stats.successful_hacks += value;
            checkAchievement(8);  // Hack Streak (special logic needed)
            checkAchievement(9);  // Elite Hacker
            
        } else if (event == "game_played") {
            stats.games_played += value;
            checkAchievement(10); // First Game
            
        } else if (event == "sushi_eaten") {
            stats.sushi_eaten += value;
            checkAchievement(11); // Sushi Apprentice
            checkAchievement(13); // Sushi Master
            
        } else if (event == "paw_touched") {
            stats.paws_scanned += value;
            checkAchievement(12); // Paw Toucher
            checkAchievement(14); // Paw Scanner Pro
            
        } else if (event == "sound_played") {
            stats.sounds_played += value;
            checkAchievement(19); // First Purr
            checkAchievement(20); // Sound Explorer
            checkAchievement(29); // Purr Lover (if purr sound)
            checkAchievement(30); // Sound Effects Pro
            
        } else if (event == "personality_changed") {
            stats.personalities_unlocked += value;
            checkAchievement(21); // Personality Try
            checkAchievement(23); // Neko Psychiatrist
            
        } else if (event == "button_press") {
            stats.button_presses += value;
            checkAchievement(28); // Volume Control (if volume button)
            
        } else if (event == "hidden_network") {
            checkAchievement(33); // First Discovery
            
        } else if (event == "signal_analysis") {
            checkAchievement(34); // Signal Analyst
            
        } else if (event == "easter_egg") {
            stats.easter_eggs_found += value;
            checkAchievement(41); // Easter Egg Hunter
        }
        
        // Check time-based achievements
        checkTimeBasedAchievements();
        
        // Process any new unlocks
        processNotifications();
    }
    
    void checkAchievement(int achievementId) {
        for (Achievement& ach : achievements) {
            if (ach.id == achievementId && !ach.unlocked) {
                bool shouldUnlock = false;
                
                switch (ach.id) {
                    case 1: // First Scan
                        shouldUnlock = (stats.networks_scanned >= 1);
                        ach.current_progress = stats.networks_scanned;
                        break;
                    case 2: // Network Explorer  
                        shouldUnlock = (stats.networks_scanned >= 50);
                        ach.current_progress = stats.networks_scanned;
                        break;
                    case 3: // First Handshake
                        shouldUnlock = (stats.handshakes_captured >= 1);
                        ach.current_progress = stats.handshakes_captured;
                        break;
                    case 4: // Handshake Collector
                        shouldUnlock = (stats.handshakes_captured >= 10);
                        ach.current_progress = stats.handshakes_captured;
                        break;
                    case 5: // Deauth Master
                        shouldUnlock = (stats.deauth_attacks >= 25);
                        ach.current_progress = stats.deauth_attacks;
                        break;
                    case 6: // Portal Creator
                        shouldUnlock = (stats.evil_portals_created >= 5);
                        ach.current_progress = stats.evil_portals_created;
                        break;
                    case 7: // Bluetooth Ninja
                        shouldUnlock = (stats.bluetooth_attacks >= 15);
                        ach.current_progress = stats.bluetooth_attacks;
                        break;
                    case 9: // Elite Hacker
                        shouldUnlock = (stats.successful_hacks >= 100);
                        ach.current_progress = stats.successful_hacks;
                        break;
                    case 10: // First Game
                        shouldUnlock = (stats.games_played >= 1);
                        ach.current_progress = stats.games_played;
                        break;
                    case 11: // Sushi Apprentice
                        shouldUnlock = (stats.sushi_eaten >= 5);
                        ach.current_progress = stats.sushi_eaten;
                        break;
                    case 12: // Paw Toucher
                        shouldUnlock = (stats.paws_scanned >= 10);
                        ach.current_progress = stats.paws_scanned;
                        break;
                    case 13: // Sushi Master
                        shouldUnlock = (stats.sushi_eaten >= 50);
                        ach.current_progress = stats.sushi_eaten;
                        break;
                    case 19: // First Purr
                        shouldUnlock = (stats.sounds_played >= 1);
                        ach.current_progress = stats.sounds_played;
                        break;
                    case 20: // Sound Explorer
                        shouldUnlock = (stats.sounds_played >= 20);
                        ach.current_progress = stats.sounds_played;
                        break;
                    case 21: // Personality Try
                        shouldUnlock = (stats.personalities_unlocked >= 3);
                        ach.current_progress = stats.personalities_unlocked;
                        break;
                    case 23: // Neko Psychiatrist
                        shouldUnlock = (stats.personalities_unlocked >= 7);
                        ach.current_progress = stats.personalities_unlocked;
                        break;
                    case 29: // Purr Lover - needs special tracking
                        // Would need to track purr sounds specifically
                        break;
                    case 33: // First Discovery
                        shouldUnlock = true; // Triggered directly by event
                        ach.current_progress = 1;
                        break;
                    case 41: // Easter Egg Hunter
                        shouldUnlock = (stats.easter_eggs_found >= 10);
                        ach.current_progress = stats.easter_eggs_found;
                        break;
                    // Add more cases as needed
                }
                
                if (shouldUnlock) {
                    unlockAchievement(ach);
                }
                break;
            }
        }
    }
    
    void unlockAchievement(Achievement& ach) {
        if (ach.unlocked) return;
        
        ach.unlocked = true;
        ach.unlock_time = millis();
        ach.is_new = true;
        
        // Update stats
        stats.total_achievements++;
        stats.total_points += ach.points_reward;
        
        switch (ach.rarity) {
            case BRONZE: stats.bronze_achievements++; break;
            case SILVER: stats.silver_achievements++; break;
            case GOLD: stats.gold_achievements++; break;
            case PLATINUM: stats.platinum_achievements++; break;
            case KAWAII: stats.kawaii_achievements++; break;
        }
        
        // Add to notification queue
        pendingNotifications.push_back(ach.id);
        
        // Play unlock sound
        if (nekoSounds) {
            if (ach.rarity >= PLATINUM) {
                nekoSounds->playContextSound("legendary_unlock");
            } else if (ach.rarity >= GOLD) {
                nekoSounds->playContextSound("rare_unlock");
            } else {
                nekoSounds->playContextSound("achievement_unlock");
            }
        }
        
        Serial.printf("🏆 Achievement Unlocked: %s (%s)\n", 
                     ach.name.c_str(), ach.emoji.c_str());
    }
    
    void checkTimeBasedAchievements() {
        // Check midnight achievement
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
                checkAchievement(43); // Time Traveler
            }
        }
        
        // Check uptime achievements
        stats.total_uptime_minutes = millis() / 60000;
        if (stats.total_uptime_minutes >= 120) {
            checkAchievement(17); // Game Marathon
        }
    }
    
    void checkInitialAchievements() {
        // Check if any achievements should already be unlocked
        // based on current stats (for when system is added to existing save)
        for (Achievement& ach : achievements) {
            if (!ach.unlocked) {
                checkAchievement(ach.id);
            }
        }
    }
    
    // ==================== NOTIFICATION SYSTEM ====================
    void processNotifications() {
        if (!pendingNotifications.empty() && 
            !showingNotification && 
            millis() - lastNotificationTime > 3000) {
            
            showNotification(pendingNotifications[0]);
            pendingNotifications.erase(pendingNotifications.begin());
        }
    }
    
    void showNotification(int achievementId) {
        for (Achievement& ach : achievements) {
            if (ach.id == achievementId) {
                showingNotification = true;
                currentNotificationId = achievementId;
                lastNotificationTime = millis();
                break;
            }
        }
    }
    
    void updateNotifications() {
        if (showingNotification && 
            millis() - lastNotificationTime > 4000) {
            showingNotification = false;
            currentNotificationId = -1;
        }
    }
    
    void drawNotification() {
        if (!showingNotification || currentNotificationId == -1) return;
        
        Achievement* ach = nullptr;
        for (Achievement& a : achievements) {
            if (a.id == currentNotificationId) {
                ach = &a;
                break;
            }
        }
        
        if (!ach) return;
        
        // Draw notification popup
        int popupY = 20;
        int popupHeight = 60;
        
        // Background with border
        M5.Display.fillRoundRect(10, popupY, 220, popupHeight, 8, BLACK);
        M5.Display.drawRoundRect(10, popupY, 220, popupHeight, 8, ach->color);
        
        // Achievement unlocked text
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(20, popupY + 8);
        M5.Display.print("🏆 ACHIEVEMENT UNLOCKED!");
        
        // Achievement details
        M5.Display.setTextColor(ach->color);
        M5.Display.setCursor(20, popupY + 25);
        M5.Display.print(ach->emoji + " " + ach->name);
        
        // Points
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(20, popupY + 40);
        M5.Display.printf("+%d points", ach->points_reward);
        
        // Rarity indicator
        String rarityNames[] = {"Bronze", "Silver", "Gold", "Platinum", "Kawaii", "Ultra"};
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(20, popupY + 52);
        M5.Display.print(rarityNames[ach->rarity] + " Rarity");
    }
    
    // ==================== DISPLAY SYSTEM ====================
    void start() {
        selectedCategory = 0;
        selectedAchievement = 0;
        currentPage = 0;
        showingDetails = false;
        drawAchievementScreen();
    }
    
    void update() {
        updateAnimations();
        updateNotifications();
        processNotifications();
    }
    
    void drawAchievementScreen() {
        M5.Display.fillScreen(BLACK);
        
        if (showingDetails) {
            drawAchievementDetails();
        } else {
            drawAchievementList();
        }
        
        // Always draw notifications on top
        drawNotification();
    }
    
    void drawAchievementList() {
        // Header
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🏆 ACHIEVEMENTS");
        
        // Category tabs
        String categoryNames[] = {"Hack", "Game", "Cute", "Sound", "Data", "Social", "???"};
        for (int i = 0; i < 7; i++) {
            uint16_t color = (i == selectedCategory) ? GREEN : DARKGREY;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10 + i * 30, 20);
            M5.Display.print(categoryNames[i]);
        }
        
        // Achievement list for selected category
        std::vector<Achievement*> categoryAchievements;
        for (Achievement& ach : achievements) {
            if (ach.category == selectedCategory) {
                categoryAchievements.push_back(&ach);
            }
        }
        
        // Show achievements (4 per page)
        int startIndex = currentPage * 4;
        for (int i = 0; i < 4 && startIndex + i < categoryAchievements.size(); i++) {
            Achievement* ach = categoryAchievements[startIndex + i];
            int yPos = 40 + i * 20;
            
            // Selection highlight
            if (i == selectedAchievement) {
                M5.Display.fillRect(5, yPos - 2, 230, 16, DARKGREY);
            }
            
            // Achievement status icon
            if (ach->unlocked) {
                M5.Display.setTextColor(ach->color);
                M5.Display.setCursor(10, yPos);
                M5.Display.print("✓");
            } else {
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(10, yPos);
                M5.Display.print("☐");
            }
            
            // Achievement emoji and name
            uint16_t textColor = ach->unlocked ? WHITE : DARKGREY;
            M5.Display.setTextColor(textColor);
            M5.Display.setCursor(25, yPos);
            
            if (ach->type == HIDDEN && !ach->unlocked) {
                M5.Display.print("??? Secret Achievement");
            } else {
                M5.Display.print(ach->emoji + " " + ach->name);
            }
            
            // Progress bar for non-hidden achievements
            if (ach->type != HIDDEN && !ach->unlocked) {
                int barWidth = 60;
                int progress = (ach->current_progress * barWidth) / ach->required_progress;
                progress = constrain(progress, 0, barWidth);
                
                M5.Display.drawRect(170, yPos + 2, barWidth, 6, DARKGREY);
                M5.Display.fillRect(170, yPos + 2, progress, 6, ach->color);
                
                M5.Display.setTextColor(CYAN);
                M5.Display.setCursor(170, yPos + 10);
                M5.Display.printf("%d/%d", ach->current_progress, ach->required_progress);
            }
            
            // Points value
            M5.Display.setTextColor(YELLOW); 
            M5.Display.setCursor(200, yPos);
            M5.Display.printf("%dp", ach->points_reward);
        }
        
        // Page indicator
        if (categoryAchievements.size() > 4) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(100, 120);
            M5.Display.printf("Page %d/%d", currentPage + 1, 
                             (categoryAchievements.size() + 3) / 4);
        }
        
        // Stats footer
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 130);
        M5.Display.printf("Total: %d/%d | Points: %d", 
                         stats.total_achievements, achievements.size(), stats.total_points);
    }
    
    void drawAchievementDetails() {
        Achievement* selectedAch = getSelectedAchievement();
        if (!selectedAch) return;
        
        // Header
        M5.Display.setTextColor(selectedAch->color);
        M5.Display.setCursor(10, 10);
        M5.Display.print(selectedAch->emoji + " " + selectedAch->name);
        
        // Rarity
        String rarityNames[] = {"Bronze", "Silver", "Gold", "Platinum", "Kawaii", "Ultra Rare"};
        M5.Display.setTextColor(selectedAch->color);
        M5.Display.setCursor(10, 25);
        M5.Display.print(rarityNames[selectedAch->rarity] + " Achievement");
        
        // Category
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 40);
        M5.Display.print("Category: " + selectedAch->category_name);
        
        // Description
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 55);
        M5.Display.print("Description:");
        M5.Display.setCursor(10, 68);
        M5.Display.print(selectedAch->description);
        
        // Progress (if not hidden and not unlocked)
        if (selectedAch->type != HIDDEN && !selectedAch->unlocked) {
            M5.Display.setTextColor(YELLOW);
            M5.Display.setCursor(10, 85);
            M5.Display.printf("Progress: %d / %d", 
                             selectedAch->current_progress, selectedAch->required_progress);
            
            // Visual progress bar
            int barWidth = 200;
            int progress = (selectedAch->current_progress * barWidth) / selectedAch->required_progress;
            progress = constrain(progress, 0, barWidth);
            
            M5.Display.drawRect(10, 95, barWidth, 8, DARKGREY);
            M5.Display.fillRect(10, 95, progress, 8, selectedAch->color);
        }
        
        // Unlock status
        if (selectedAch->unlocked) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(10, 110);
            M5.Display.print("✓ UNLOCKED!");
            
            // Unlock time (if available)
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(10, 122);
            M5.Display.print("Earned: " + formatUnlockTime(selectedAch->unlock_time));
        } else {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(10, 110);
            M5.Display.print("🔒 Locked");
        }
        
        // Points reward
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(150, 110);
        M5.Display.printf("Reward: %d pts", selectedAch->points_reward);
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
                selectItem();
                break;
            case ACTION_BACK:
                goBack();
                break;
        }
        
        drawAchievementScreen();
    }
    
    void navigateUp() {
        if (showingDetails) return;
        
        if (selectedAchievement > 0) {
            selectedAchievement--;
        }
    }
    
    void navigateDown() {
        if (showingDetails) return;
        
        std::vector<Achievement*> categoryAchievements;
        for (Achievement& ach : achievements) {
            if (ach.category == selectedCategory) {
                categoryAchievements.push_back(&ach);
            }
        }
        
        int maxOnPage = min(4, (int)categoryAchievements.size() - currentPage * 4);
        if (selectedAchievement < maxOnPage - 1) {
            selectedAchievement++;
        }
    }
    
    void navigateLeft() {
        if (showingDetails) return;
        
        if (selectedCategory > 0) {
            selectedCategory--;
            selectedAchievement = 0;
            currentPage = 0;
        }
    }
    
    void navigateRight() {
        if (showingDetails) return;
        
        if (selectedCategory < 6) {
            selectedCategory++;
            selectedAchievement = 0;
            currentPage = 0;
        }
    }
    
    void selectItem() {
        if (showingDetails) {
            showingDetails = false;
        } else {
            showingDetails = true;
        }
    }
    
    void goBack() {
        if (showingDetails) {
            showingDetails = false;
        } else {
            // Exit achievement system
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    Achievement* getSelectedAchievement() {
        std::vector<Achievement*> categoryAchievements;
        for (Achievement& ach : achievements) {
            if (ach.category == selectedCategory) {
                categoryAchievements.push_back(&ach);
            }
        }
        
        int index = currentPage * 4 + selectedAchievement;
        if (index < categoryAchievements.size()) {
            return categoryAchievements[index];
        }
        return nullptr;
    }
    
    String formatUnlockTime(unsigned long unlockTime) {
        if (unlockTime == 0) return "Unknown";
        
        unsigned long elapsed = millis() - unlockTime;
        int minutes = elapsed / 60000;
        
        if (minutes < 60) {
            return String(minutes) + "min ago";
        } else {
            return String(minutes / 60) + "h ago";
        }
    }
    
    void updateAnimations() {
        if (millis() - lastAnimUpdate > 100) {
            animationFrame++;
            if (animationFrame > 1000) animationFrame = 0;
            lastAnimUpdate = millis();
        }
    }
    
    void saveProgress() {
        // Placeholder - would save to SD card
        Serial.println("💾 Saving achievement progress...");
    }
    
    void loadProgress() {
        // Placeholder - would load from SD card  
        Serial.println("📂 Loading achievement progress...");
    }
    
    // ==================== API FUNCTIONS ====================
    
    // Called by other systems to report events
    void reportNetworkScan() { updateProgress("network_scan"); }
    void reportHandshakeCapture() { updateProgress("handshake_capture"); }
    void reportDeauthAttack() { updateProgress("deauth_attack"); }
    void reportEvilPortal() { updateProgress("evil_portal"); }
    void reportBluetoothAttack() { updateProgress("bluetooth_attack"); }
    void reportSuccessfulHack() { updateProgress("successful_hack"); }
    void reportGamePlayed() { updateProgress("game_played"); }
    void reportSushiEaten() { updateProgress("sushi_eaten"); }
    void reportPawTouched() { updateProgress("paw_touched"); }
    void reportSoundPlayed() { updateProgress("sound_played"); }
    void reportPersonalityChanged() { updateProgress("personality_changed"); }
    void reportButtonPress() { updateProgress("button_press"); }
    void reportHiddenNetwork() { updateProgress("hidden_network"); }
    void reportSignalAnalysis() { updateProgress("signal_analysis"); }
    void reportEasterEgg() { updateProgress("easter_egg"); }
    
    // Getters
    int getTotalAchievements() { return stats.total_achievements; }
    int getTotalPoints() { return stats.total_points; }
    int getUnlockedCount() { return stats.total_achievements; }
    int getTotalCount() { return achievements.size(); }
    PlayerStats& getStats() { return stats; }
    bool hasNewAchievements() { 
        for (Achievement& ach : achievements) {
            if (ach.is_new) return true;
        }
        return false;
    }
    
    void clearNewFlags() {
        for (Achievement& ach : achievements) {
            ach.is_new = false;
        }
    }
};

#endif // M5GOTCHI_NEKO_ACHIEVEMENTS_H