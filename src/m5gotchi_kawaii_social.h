/*
 * 🌐 M5GOTCHI KAWAII SOCIAL NETWORK v1.0
 * Sistema de rede social kawaii para compartilhar discoveries e achievements!
 * 
 * Features:
 * - Feed social com posts de discoveries, achievements e hacks
 * - Profile customizável com stats e badges
 * - Following/followers system entre M5Gotchis
 * - Like, comment e share em posts
 * - Trending discoveries e popular techniques
 * - Leaderboards globais e locais
 * - Groups por interests (WiFi masters, Bluetooth ninjas, etc.)
 * - Event system para challenges colaborativos
 * - Anonymous posting para sensitive discoveries
 * - Reputation system baseado em contributions
 * - Kawaii reactions (nya, purr, hiss, etc.)
 * - Photo sharing de successful hacks
 * - Mentorship system - senpai/kohai relationships
 * - Real-time chat integration com NekoGram
 * 
 * Tudo integrado com achievements e pet system!
 * Building the kawaii hacker community! 💕🔗
 */

#ifndef M5GOTCHI_KAWAII_SOCIAL_H
#define M5GOTCHI_KAWAII_SOCIAL_H

#include <M5Unified.h>
#include <WiFi.h>
#include <esp_now.h>
#include <vector>
#include <map>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_neko_sounds.h"

// ==================== SOCIAL ENUMS ====================
enum SocialScreen {
    SOCIAL_FEED = 0,
    SOCIAL_PROFILE,
    SOCIAL_DISCOVER,
    SOCIAL_GROUPS,
    SOCIAL_LEADERBOARD,
    SOCIAL_NOTIFICATIONS,
    SOCIAL_POST_CREATE,
    SOCIAL_SETTINGS
};

enum PostType {
    DISCOVERY_POST = 0,      // Network discovered, vulnerability found
    ACHIEVEMENT_POST,        // Achievement unlocked
    HACK_SUCCESS_POST,       // Successful attack
    TIP_POST,               // Helpful technique/tip
    MEME_POST,              // Kawaii meme or joke
    QUESTION_POST,          // Help request
    EVENT_POST,             // Community event
    PET_POST               // Virtual pet milestone
};

enum ReactionType {
    LIKE_REACTION = 0,      // 👍 Basic like
    NYA_REACTION,           // 😸 Kawaii approval
    PURR_REACTION,          // 😻 Love it
    HISS_REACTION,          // 😾 Dislike
    WOW_REACTION,           // 🤩 Amazing
    LEARN_REACTION,         // 🧠 Educational
    FIRE_REACTION,          // 🔥 Epic
    HEART_REACTION          // 💖 Kawaii love
};

enum UserReputation {
    NEWBIE = 0,             // 0-99 rep
    APPRENTICE,             // 100-299 rep
    SKILLED,                // 300-699 rep
    EXPERT,                 // 700-1499 rep
    MASTER,                 // 1500-2999 rep
    SENPAI,                 // 3000+ rep
    LEGENDARY_SENPAI        // 5000+ rep with special achievements
};

enum GroupType {
    WIFI_MASTERS = 0,
    BLUETOOTH_NINJAS,
    SOCIAL_ENGINEERS,
    CRYPTO_CATS,
    MOBILE_HACKERS,
    IOT_EXPLORERS,
    KAWAII_COLLECTORS,
    PET_LOVERS
};

// ==================== SOCIAL STRUCTURES ====================
struct SocialPost {
    String post_id;
    String author_id;
    String author_name;
    String author_emoji;
    PostType type;
    String title;
    String content;
    String image_data;       // Base64 encoded screenshot
    unsigned long timestamp;
    
    // Engagement
    std::map<ReactionType, int> reactions;
    std::vector<String> comments;
    int shares;
    bool is_trending;
    
    // Privacy
    bool is_anonymous;
    bool is_public;
    String group_id;         // If posted to specific group
    
    // Metadata
    String location_hint;    // Vague location (city, not exact)
    String device_model;
    std::vector<String> tags;
};

struct UserProfile {
    String user_id;
    String nickname;
    String real_name;
    String bio;
    String avatar_emoji;
    uint16_t theme_color;
    
    // Stats
    int reputation_points;
    UserReputation reputation_level;
    int posts_count;
    int followers_count;
    int following_count;
    int likes_received;
    int comments_made;
    
    // Activity
    unsigned long join_date;
    unsigned long last_active;
    bool is_online;
    String current_status;
    
    // Achievements showcase
    std::vector<String> featured_achievements;
    std::vector<String> featured_badges;
    
    // Privacy settings
    bool allow_anonymous_follow;
    bool show_real_stats;
    bool allow_direct_messages;
    
    // Pet integration
    String pet_name;
    String pet_emoji;
    int pet_level;
};

struct SocialGroup {
    String group_id;
    String group_name;
    String group_emoji;
    String description;
    GroupType type;
    
    std::vector<String> members;
    std::vector<String> admins;
    String creator_id;
    unsigned long created_date;
    
    // Group stats
    int total_posts;
    int active_members;
    bool is_public;
    bool requires_approval;
    
    // Featured content
    std::vector<String> pinned_posts;
    String group_banner;
    
    // Events
    std::vector<String> upcoming_events;
};

struct SocialNotification {
    String notification_id;
    String from_user_id;
    String from_user_name;
    String content;
    String action;           // "liked", "commented", "followed", etc.
    String target_post_id;
    unsigned long timestamp;
    bool is_read;
    bool is_important;
};

struct Leaderboard {
    String category;
    String period;           // "daily", "weekly", "monthly", "all_time"
    std::vector<std::pair<String, int>> rankings; // user_id, score
    unsigned long last_updated;
};

// ==================== KAWAII SOCIAL NETWORK CLASS ====================
class M5GotchiKawaiiSocial {
private:
    SocialScreen currentScreen;
    int selectedItem;
    int scrollOffset;
    
    // User data
    UserProfile myProfile;
    std::vector<UserProfile> followingUsers;
    std::vector<UserProfile> followers;
    
    // Content
    std::vector<SocialPost> feedPosts;
    std::vector<SocialPost> myPosts;
    std::vector<SocialGroup> joinedGroups;
    std::vector<SocialGroup> discoverGroups;
    
    // Notifications
    std::vector<SocialNotification> notifications;
    int unreadNotifications;
    
    // Leaderboards
    std::map<String, Leaderboard> leaderboards;
    
    // Current post creation
    String newPostTitle;
    String newPostContent;
    PostType newPostType;
    bool newPostAnonymous;
    
    // UI state
    int animationFrame;
    unsigned long lastAnimUpdate;
    bool showingPostDetails;
    int selectedPostIndex;
    
    // Network
    bool socialNetworkConnected;
    unsigned long lastSync;
    
    // Sound system
    M5GotchiNekoSounds* nekoSounds;
    
    // Colors for reputation levels
    uint16_t reputationColors[7] = {
        DARKGREY,    // Newbie
        BLUE,        // Apprentice
        GREEN,       // Skilled
        YELLOW,      // Expert
        ORANGE,      // Master
        MAGENTA,     // Senpai
        WHITE        // Legendary Senpai
    };
    
    // Post type emojis and colors
    String postTypeEmojis[8] = {"🔍", "🏆", "⚡", "💡", "😹", "❓", "🎉", "🐱"};
    uint16_t postTypeColors[8] = {CYAN, YELLOW, GREEN, BLUE, PINK, ORANGE, MAGENTA, WHITE};

public:
    M5GotchiKawaiiSocial() {
        currentScreen = SOCIAL_FEED;
        selectedItem = 0;
        scrollOffset = 0;
        showingPostDetails = false;
        selectedPostIndex = 0;
        
        unreadNotifications = 0;
        socialNetworkConnected = false;
        lastSync = 0;
        
        newPostType = DISCOVERY_POST;
        newPostAnonymous = false;
        
        animationFrame = 0;
        lastAnimUpdate = 0;
        nekoSounds = nullptr;
        
        initializeProfile();
        initializeGroups();
        generateSampleContent();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🌐 Initializing Kawaii Social Network...");
        
        initializeProfile();
        loadUserData();
        initializeLeaderboards();
        connectToSocialNetwork();
        
        Serial.println("✅ Kawaii Social Ready! Let's share discoveries! 💕");
    }
    
    void setSoundSystem(M5GotchiNekoSounds* sounds) {
        nekoSounds = sounds;
    }
    
    void initializeProfile() {
        myProfile.user_id = generateUserId();
        myProfile.nickname = "NekoHacker";
        myProfile.real_name = "Anonymous";
        myProfile.bio = "Kawaii hacker exploring the digital world! 🐱💻";
        myProfile.avatar_emoji = "😸";
        myProfile.theme_color = PINK;
        
        myProfile.reputation_points = 50;
        myProfile.reputation_level = NEWBIE;
        myProfile.posts_count = 0;
        myProfile.followers_count = 0;
        myProfile.following_count = 0;
        myProfile.likes_received = 0;
        myProfile.comments_made = 0;
        
        myProfile.join_date = millis();
        myProfile.last_active = millis();
        myProfile.is_online = true;
        myProfile.current_status = "Ready to hack! 💪";
        
        myProfile.allow_anonymous_follow = true;
        myProfile.show_real_stats = true;
        myProfile.allow_direct_messages = true;
        
        myProfile.pet_name = "Neko";
        myProfile.pet_emoji = "🐱";
        myProfile.pet_level = 1;
    }
    
    void initializeGroups() {
        // Create sample groups
        SocialGroup wifiGroup;
        wifiGroup.group_id = "wifi_masters";
        wifiGroup.group_name = "WiFi Masters";
        wifiGroup.group_emoji = "📡";
        wifiGroup.description = "Masters of wireless network hacking";
        wifiGroup.type = WIFI_MASTERS;
        wifiGroup.creator_id = "system";
        wifiGroup.created_date = millis() - 86400000; // 1 day ago
        wifiGroup.is_public = true;
        wifiGroup.requires_approval = false;
        wifiGroup.total_posts = 42;
        wifiGroup.active_members = 156;
        discoverGroups.push_back(wifiGroup);
        
        SocialGroup kawaiGroup;
        kawaiGroup.group_id = "kawaii_collectors";
        kawaiGroup.group_name = "Kawaii Collectors";
        kawaiGroup.group_emoji = "💖";
        kawaiGroup.description = "Collecting all the cute achievements!";
        kawaiGroup.type = KAWAII_COLLECTORS;
        kawaiGroup.creator_id = "neko_senpai";
        kawaiGroup.created_date = millis() - 172800000; // 2 days ago
        kawaiGroup.is_public = true;
        kawaiGroup.requires_approval = false;
        kawaiGroup.total_posts = 89;
        kawaiGroup.active_members = 203;
        discoverGroups.push_back(kawaiGroup);
    }
    
    void initializeLeaderboards() {
        // Initialize different leaderboard categories
        leaderboards["reputation"] = {"Reputation Leaders", "all_time", {}, millis()};
        leaderboards["discoveries"] = {"Discovery Masters", "weekly", {}, millis()};
        leaderboards["achievements"] = {"Achievement Hunters", "monthly", {}, millis()};
        leaderboards["social"] = {"Social Butterflies", "weekly", {}, millis()};
        
        // Add sample data
        leaderboards["reputation"].rankings.push_back({"cyber_senpai", 4567});
        leaderboards["reputation"].rankings.push_back({"neko_master", 3421});
        leaderboards["reputation"].rankings.push_back({"kawaii_hacker", 2890});
        leaderboards["reputation"].rankings.push_back({"wifi_ninja", 2340});
        leaderboards["reputation"].rankings.push_back({myProfile.user_id, myProfile.reputation_points});
    }
    
    void generateSampleContent() {
        // Generate sample posts for demo
        SocialPost post1;
        post1.post_id = "post_001";
        post1.author_id = "cyber_senpai";
        post1.author_name = "CyberSenpai";
        post1.author_emoji = "🥷";
        post1.type = DISCOVERY_POST;
        post1.title = "Hidden Network Found!";
        post1.content = "Discovered a hidden corporate network using advanced scanning techniques. The SSID was using Unicode characters to hide! 🕵️";
        post1.timestamp = millis() - 3600000; // 1 hour ago
        post1.reactions[NYA_REACTION] = 15;
        post1.reactions[WOW_REACTION] = 8;
        post1.reactions[LEARN_REACTION] = 23;
        post1.shares = 5;
        post1.is_trending = true;
        post1.is_anonymous = false;
        post1.is_public = true;
        post1.device_model = "M5Stack Cardputer";
        post1.tags = {"discovery", "hidden_network", "unicode", "corporate"};
        feedPosts.push_back(post1);
        
        SocialPost post2;
        post2.post_id = "post_002";
        post2.author_id = "kawaii_hacker";
        post2.author_name = "KawaiiHacker";
        post2.author_emoji = "😻";
        post2.type = ACHIEVEMENT_POST;
        post2.title = "Sushi SQL Master Unlocked!";
        post2.content = "Finally got the Sushi SQL Master achievement! 🍣 50 perfect SQL injections in the kawaii game! The wasabi levels were intense! 🌶️";
        post2.timestamp = millis() - 7200000; // 2 hours ago
        post2.reactions[HEART_REACTION] = 34;
        post2.reactions[FIRE_REACTION] = 12;
        post2.reactions[NYA_REACTION] = 28;
        post2.shares = 8;
        post2.is_trending = false;
        post2.is_anonymous = false;
        post2.is_public = true;
        post2.tags = {"achievement", "sushi_sql", "gaming", "kawaii"};
        feedPosts.push_back(post2);
        
        SocialPost post3;
        post3.post_id = "post_003";
        post3.author_id = "neko_master";
        post3.author_name = "NekoMaster";
        post3.author_emoji = "👑";
        post3.type = TIP_POST;
        post3.title = "Pro Tip: Handshake Timing";
        post3.content = "For better handshake capture success, wait exactly 3 seconds after deauth before starting capture. This timing works 90% of the time! ⏰";
        post3.timestamp = millis() - 10800000; // 3 hours ago
        post3.reactions[LEARN_REACTION] = 67;
        post3.reactions[LIKE_REACTION] = 45;
        post3.reactions[WOW_REACTION] = 19;
        post3.shares = 23;
        post3.is_trending = true;
        post3.is_anonymous = false;
        post3.is_public = true;
        post3.tags = {"tip", "handshake", "timing", "deauth"};
        feedPosts.push_back(post3);
        
        SocialPost post4;
        post4.post_id = "post_004";
        post4.author_id = "anonymous";
        post4.author_name = "Anonymous Neko";
        post4.author_emoji = "🎭";
        post4.type = QUESTION_POST;
        post4.title = "Help with WPA3 Networks?";
        post4.content = "Has anyone had success with WPA3 networks? My usual techniques aren't working. Any kawaii methods to try? 🤔";
        post4.timestamp = millis() - 14400000; // 4 hours ago
        post4.reactions[LEARN_REACTION] = 8;
        post4.reactions[LIKE_REACTION] = 12;
        post4.shares = 2;
        post4.is_trending = false;
        post4.is_anonymous = true;
        post4.is_public = true;
        post4.tags = {"question", "wpa3", "help", "techniques"};
        feedPosts.push_back(post4);
    }
    
    // ==================== SOCIAL FLOW ====================
    void start() {
        currentScreen = SOCIAL_FEED;
        selectedItem = 0;
        scrollOffset = 0;
        drawSocialScreen();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("social_open");
        }
    }
    
    void stop() {
        saveUserData();
        syncToNetwork();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("social_close");
        }
    }
    
    void update() {
        updateAnimations();
        updateNetworkStatus();
        processNotifications();
        updateTrendingContent();
        
        // Auto-refresh feed every 5 minutes
        if (millis() - lastSync > 300000) {
            refreshFeed();
            lastSync = millis();
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawSocialScreen() {
        M5.Display.fillScreen(BLACK);
        
        switch (currentScreen) {
            case SOCIAL_FEED:
                drawFeedScreen();
                break;
            case SOCIAL_PROFILE:
                drawProfileScreen();
                break;
            case SOCIAL_DISCOVER:
                drawDiscoverScreen();
                break;
            case SOCIAL_GROUPS:
                drawGroupsScreen();
                break;
            case SOCIAL_LEADERBOARD:
                drawLeaderboardScreen();
                break;
            case SOCIAL_NOTIFICATIONS:
                drawNotificationsScreen();
                break;
            case SOCIAL_POST_CREATE:
                drawPostCreateScreen();
                break;
            case SOCIAL_SETTINGS:
                drawSettingsScreen();
                break;
        }
        
        // Always draw navigation bar at bottom
        drawNavigationBar();
    }
    
    void drawFeedScreen() {
        // Header
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🌐 Social Feed");
        
        if (socialNetworkConnected) {
            M5.Display.setTextColor(GREEN);
            M5.Display.setCursor(200, 5);
            M5.Display.print("●");
        } else {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(200, 5);
            M5.Display.print("●");
        }
        
        // Posts list
        int postY = 20;
        int visiblePosts = 4;
        int startIdx = scrollOffset;
        
        for (int i = 0; i < visiblePosts && (startIdx + i) < feedPosts.size(); i++) {
            SocialPost& post = feedPosts[startIdx + i];
            int currentY = postY + i * 25;
            
            // Selection highlight
            if (i == selectedItem) {
                M5.Display.fillRect(0, currentY - 2, 240, 22, DARKGREY);
            }
            
            // Post header
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(5, currentY);
            M5.Display.print(post.author_emoji + " " + post.author_name);
            
            // Trending indicator
            if (post.is_trending) {
                M5.Display.setTextColor(ORANGE);
                M5.Display.setCursor(180, currentY);
                M5.Display.print("🔥");
            }
            
            // Anonymous indicator
            if (post.is_anonymous) {
                M5.Display.setTextColor(PURPLE);
                M5.Display.setCursor(200, currentY);
                M5.Display.print("🎭");
            }
            
            // Post title and type
            M5.Display.setTextColor(postTypeColors[post.type]);
            M5.Display.setCursor(5, currentY + 8);
            M5.Display.print(postTypeEmojis[post.type] + " " + post.title);
            
            // Engagement stats
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(5, currentY + 16);
            int totalReactions = 0;
            for (auto& reaction : post.reactions) {
                totalReactions += reaction.second;
            }
            M5.Display.printf("%d reactions | %d shares", totalReactions, post.shares);
            
            // Timestamp
            M5.Display.setCursor(150, currentY + 16);
            M5.Display.print(formatTimestamp(post.timestamp));
        }
        
        // Scroll indicators
        if (scrollOffset > 0) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(115, 20);
            M5.Display.print("▲");
        }
        if (scrollOffset + visiblePosts < feedPosts.size()) {
            M5.Display.setTextColor(WHITE);
            M5.Display.setCursor(115, 115);
            M5.Display.print("▼");
        }
    }
    
    void drawProfileScreen() {
        // Profile header
        M5.Display.setTextColor(myProfile.theme_color);
        M5.Display.setCursor(60, 5);
        M5.Display.print("👤 My Profile");
        
        // Avatar and basic info
        M5.Display.setTextSize(2);
        M5.Display.setCursor(20, 20);
        M5.Display.print(myProfile.avatar_emoji);
        
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(50, 25);
        M5.Display.print(myProfile.nickname);
        
        // Reputation
        M5.Display.setTextColor(reputationColors[myProfile.reputation_level]);
        M5.Display.setCursor(50, 35);
        M5.Display.print(getReputationName(myProfile.reputation_level));
        M5.Display.setCursor(50, 45);
        M5.Display.printf("%d rep points", myProfile.reputation_points);
        
        // Stats
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 60);
        M5.Display.print("=== STATS ===");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 75);
        M5.Display.printf("Posts: %d", myProfile.posts_count);
        M5.Display.setCursor(80, 75);
        M5.Display.printf("Followers: %d", myProfile.followers_count);
        M5.Display.setCursor(160, 75);
        M5.Display.printf("Following: %d", myProfile.following_count);
        
        M5.Display.setCursor(10, 85);
        M5.Display.printf("Likes received: %d", myProfile.likes_received);
        M5.Display.setCursor(10, 95);
        M5.Display.printf("Comments made: %d", myProfile.comments_made);
        
        // Bio
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(10, 105);
        M5.Display.print("Bio: " + myProfile.bio.substring(0, 30) + "...");
        
        // Pet integration
        if (!myProfile.pet_name.isEmpty()) {
            M5.Display.setTextColor(PINK);
            M5.Display.setCursor(10, 115);
            M5.Display.printf("Pet: %s %s (Lv.%d)", 
                             myProfile.pet_emoji.c_str(), 
                             myProfile.pet_name.c_str(), 
                             myProfile.pet_level);
        }
    }
    
    void drawDiscoverScreen() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(60, 5);
        M5.Display.print("🔍 Discover");
        
        // Trending tags
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 20);
        M5.Display.print("=== TRENDING TAGS ===");
        
        String trendingTags[] = {"#handshake", "#kawaii", "#wpa3", "#discovery", "#achievement"};
        uint16_t tagColors[] = {CYAN, PINK, ORANGE, GREEN, YELLOW};
        
        for (int i = 0; i < 5; i++) {
            M5.Display.setTextColor(tagColors[i]);
            M5.Display.setCursor(10 + (i % 2) * 100, 35 + (i / 2) * 15);
            M5.Display.print(trendingTags[i]);
        }
        
        // Suggested users
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 70);
        M5.Display.print("=== SUGGESTED FOLLOWS ===");
        
        String suggestedUsers[] = {"CyberSenpai 🥷", "NekoMaster 👑", "KawaiiHacker 😻"};
        for (int i = 0; i < 3; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 85 + i * 12);
            M5.Display.print(suggestedUsers[i]);
        }
    }
    
    void drawGroupsScreen() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(70, 5);
        M5.Display.print("👥 Groups");
        
        // My groups
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 20);
        M5.Display.printf("My Groups (%d):", joinedGroups.size());
        
        // Available groups
        M5.Display.setCursor(10, 50);
        M5.Display.print("Discover Groups:");
        
        for (int i = 0; i < min(4, (int)discoverGroups.size()); i++) {
            SocialGroup& group = discoverGroups[i];
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 65 + i * 15);
            M5.Display.print(group.group_emoji + " " + group.group_name);
            
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(15, 65 + i * 15 + 8);
            M5.Display.printf("%d members | %d posts", group.active_members, group.total_posts);
        }
    }
    
    void drawLeaderboardScreen() {
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🏆 Leaderboards");
        
        // Category tabs
        String categories[] = {"Rep", "Disc", "Ach", "Social"};
        for (int i = 0; i < 4; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : DARKGREY;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10 + i * 50, 20);
            M5.Display.print(categories[i]);
        }
        
        // Rankings (show reputation by default)
        Leaderboard& board = leaderboards["reputation"];
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 35);
        M5.Display.print("=== " + board.category + " ===");
        
        for (int i = 0; i < min(5, (int)board.rankings.size()); i++) {
            String userId = board.rankings[i].first;
            int score = board.rankings[i].second;
            
            uint16_t color = (userId == myProfile.user_id) ? CYAN : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 50 + i * 12);
            M5.Display.printf("%d. %s - %d", i + 1, 
                             getUserDisplayName(userId).c_str(), score);
        }
    }
    
    void drawNotificationsScreen() {
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(50, 5);
        M5.Display.print("🔔 Notifications");
        
        if (unreadNotifications > 0) {
            M5.Display.setTextColor(RED);
            M5.Display.setCursor(180, 5);
            M5.Display.printf("(%d)", unreadNotifications);
        }
        
        // Notifications list
        for (int i = 0; i < min(5, (int)notifications.size()); i++) {
            SocialNotification& notif = notifications[i];
            
            uint16_t color = notif.is_read ? DARKGREY : WHITE;
            if (i == selectedItem) color = YELLOW;
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 25 + i * 18);
            
            // Notification icon based on action
            String icon = "📢";
            if (notif.action == "liked") icon = "👍";
            else if (notif.action == "commented") icon = "💬";
            else if (notif.action == "followed") icon = "👥";
            else if (notif.action == "shared") icon = "🔄";
            
            M5.Display.print(icon + " " + notif.from_user_name);
            
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(10, 25 + i * 18 + 8);
            String shortContent = notif.content;
            if (shortContent.length() > 28) {
                shortContent = shortContent.substring(0, 28) + "...";
            }
            M5.Display.print(shortContent);
        }
        
        if (notifications.empty()) {
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(60, 60);
            M5.Display.print("No notifications");
            M5.Display.setCursor(80, 75);
            M5.Display.print("yet! 😸");
        }
    }
    
    void drawPostCreateScreen() {
        M5.Display.setTextColor(PINK);
        M5.Display.setCursor(60, 5);
        M5.Display.print("✏️ New Post");
        
        // Post type selection
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.print("Type: " + String(postTypeEmojis[newPostType]));
        
        String postTypes[] = {"Discovery", "Achievement", "Hack", "Tip", "Meme", "Question", "Event", "Pet"};
        M5.Display.setCursor(60, 25);
        M5.Display.print(postTypes[newPostType]);
        
        // Title input
        M5.Display.setCursor(10, 40);
        M5.Display.print("Title:");
        M5.Display.drawRect(10, 50, 220, 15, WHITE);
        M5.Display.setCursor(15, 55);
        M5.Display.print(newPostTitle + "_");
        
        // Content input
        M5.Display.setCursor(10, 70);
        M5.Display.print("Content:");
        M5.Display.drawRect(10, 80, 220, 30, WHITE);
        M5.Display.setCursor(15, 85);
        
        // Word wrap content
        String displayContent = newPostContent;
        if (displayContent.length() > 60) {
            displayContent = displayContent.substring(0, 60) + "...";
        }
        M5.Display.print(displayContent);
        
        // Options
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 115);
        M5.Display.print("Anonymous: " + String(newPostAnonymous ? "YES" : "NO"));
        
        M5.Display.setCursor(120, 115);
        M5.Display.print("⏎ Post | ← Cancel");
    }
    
    void drawSettingsScreen() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(70, 5);
        M5.Display.print("⚙️ Settings");
        
        String settingsItems[] = {
            "📝 Edit Profile",
            "🎨 Change Theme",
            "🔔 Notifications: ON",
            "🥷 Anonymous Mode: " + String(myProfile.allow_anonymous_follow ? "ON" : "OFF"),
            "📊 Show Real Stats: " + String(myProfile.show_real_stats ? "YES" : "NO"),
            "💬 Allow DMs: " + String(myProfile.allow_direct_messages ? "YES" : "NO"),
            "🔄 Sync Data",
            "← Back"
        };
        
        for (int i = 0; i < 8; i++) {
            uint16_t color = (i == selectedItem) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 25 + i * 11);
            M5.Display.print(settingsItems[i]);
        }
    }
    
    void drawNavigationBar() {
        // Bottom navigation
        M5.Display.fillRect(0, 125, 240, 10, DARKGREY);
        
        // Navigation icons
        String navIcons[] = {"🏠", "👤", "🔍", "👥", "🏆", "🔔", "✏️", "⚙️"};
        for (int i = 0; i < 8; i++) {
            uint16_t color = (i == currentScreen) ? YELLOW : WHITE;
            M5.Display.setTextColor(color);
            M5.Display.setCursor(5 + i * 28, 127);
            M5.Display.print(navIcons[i]);
        }
    }
    
    // ==================== CONTENT CREATION ====================
    void createPost(String title, String content, PostType type, bool anonymous = false) {
        SocialPost newPost;
        newPost.post_id = "post_" + String(millis());
        newPost.author_id = anonymous ? "anonymous" : myProfile.user_id;
        newPost.author_name = anonymous ? "Anonymous Neko" : myProfile.nickname;
        newPost.author_emoji = anonymous ? "🎭" : myProfile.avatar_emoji;
        newPost.type = type;
        newPost.title = title;
        newPost.content = content;
        newPost.timestamp = millis();
        newPost.is_anonymous = anonymous;
        newPost.is_public = true;
        newPost.device_model = "M5Stack Cardputer";
        
        // Add to feeds
        myPosts.push_back(newPost);
        feedPosts.insert(feedPosts.begin(), newPost); // Add to top of feed
        
        // Update stats
        myProfile.posts_count++;
        myProfile.reputation_points += 5; // Small reward for posting
        
        // Play sound
        if (nekoSounds) {
            nekoSounds->playContextSound("post_created");
        }
        
        Serial.printf("📝 Created new post: %s\n", title.c_str());
    }
    
    void reactToPost(String postId, ReactionType reaction) {
        // Find and update post
        for (SocialPost& post : feedPosts) {
            if (post.post_id == postId) {
                post.reactions[reaction]++;
                
                // Create notification for post author
                if (post.author_id != myProfile.user_id && post.author_id != "anonymous") {
                    SocialNotification notif;
                    notif.notification_id = "notif_" + String(millis());
                    notif.from_user_id = myProfile.user_id;
                    notif.from_user_name = myProfile.nickname;
                    notif.content = "reacted to your post: " + post.title;
                    notif.action = "liked";
                    notif.target_post_id = postId;
                    notif.timestamp = millis();
                    notif.is_read = false;
                    notif.is_important = false;
                    
                    // Would send to other user in real implementation
                }
                
                // Small reputation bonus
                myProfile.reputation_points += 1;
                
                if (nekoSounds) {
                    nekoSounds->playContextSound("reaction_added");
                }
                break;
            }
        }
    }
    
    void followUser(String userId) {
        // Add to following list (simplified)
        myProfile.following_count++;
        
        // Create notification
        SocialNotification notif;
        notif.notification_id = "notif_" + String(millis());
        notif.from_user_id = myProfile.user_id;
        notif.from_user_name = myProfile.nickname;
        notif.content = "started following you!";
        notif.action = "followed";
        notif.timestamp = millis();
        notif.is_read = false;
        notif.is_important = true;
        
        // Reputation bonus for social activity
        myProfile.reputation_points += 2;
        
        if (nekoSounds) {
            nekoSounds->playContextSound("follow_user");
        }
        
        Serial.printf("👥 Following user: %s\n", userId.c_str());
    }
    
    void joinGroup(String groupId) {
        // Find group and add to joined list
        for (SocialGroup& group : discoverGroups) {
            if (group.group_id == groupId) {
                group.members.push_back(myProfile.user_id);
                group.active_members++;
                joinedGroups.push_back(group);
                
                // Remove from discover list
                for (int i = 0; i < discoverGroups.size(); i++) {
                    if (discoverGroups[i].group_id == groupId) {
                        discoverGroups.erase(discoverGroups.begin() + i);
                        break;
                    }
                }
                
                // Reputation bonus
                myProfile.reputation_points += 10;
                
                if (nekoSounds) {
                    nekoSounds->playContextSound("join_group");
                }
                
                Serial.printf("👥 Joined group: %s\n", group.group_name.c_str());
                break;
            }
        }
    }
    
    // ==================== SOCIAL INTEGRATION ====================
    void shareAchievement(String achievementName, String achievementEmoji) {
        String title = "Achievement Unlocked: " + achievementName + "!";
        String content = "Just unlocked the " + achievementName + " " + achievementEmoji + 
                        " achievement! The kawaii hacker journey continues! 🎉";
        
        createPost(title, content, ACHIEVEMENT_POST, false);
    }
    
    void shareDiscovery(String networkName, String discoveryType) {
        String title = "Network Discovery: " + discoveryType;
        String content = "Found interesting network behavior! Type: " + discoveryType + 
                        ". Always learning something new in the field! 🔍";
        
        createPost(title, content, DISCOVERY_POST, true); // Anonymous for security
    }
    
    void shareHackSuccess(String hackType, String targetInfo) {
        String title = "Successful " + hackType + "!";
        String content = "Just completed a successful " + hackType + 
                        " attack! The technique worked perfectly. Educational purposes only! ⚡";
        
        createPost(title, content, HACK_SUCCESS_POST, true); // Anonymous for safety
    }
    
    void sharePetMilestone(String petName, String milestone) {
        String title = petName + " " + milestone + "!";
        String content = "My virtual pet " + petName + " just " + milestone + 
                        "! So proud of my little hacker companion! 🐱💕";
        
        createPost(title, content, PET_POST, false);
    }
    
    void askForHelp(String question, String details) {
        String title = "Need Help: " + question;
        String content = details + " Any kawaii suggestions from the community? 🤔";
        
        createPost(title, content, QUESTION_POST, true); // Anonymous for privacy
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
        
        drawSocialScreen();
    }
    
    void navigateUp() {
        if (currentScreen == SOCIAL_FEED) {
            if (selectedItem > 0) {
                selectedItem--;
            } else if (scrollOffset > 0) {
                scrollOffset--;
            }
        } else {
            if (selectedItem > 0) selectedItem--;
        }
    }
    
    void navigateDown() {
        if (currentScreen == SOCIAL_FEED) {
            int visiblePosts = 4;
            if (selectedItem < visiblePosts - 1 && selectedItem < feedPosts.size() - 1 - scrollOffset) {
                selectedItem++;
            } else if (scrollOffset + visiblePosts < feedPosts.size()) {
                scrollOffset++;
            }
        } else {
            int maxItems = getMaxItemsForCurrentScreen();
            if (selectedItem < maxItems - 1) selectedItem++;
        }
    }
    
    void navigateLeft() {
        if (currentScreen > SOCIAL_FEED) {
            currentScreen = (SocialScreen)(currentScreen - 1);
            selectedItem = 0;
            scrollOffset = 0;
        }
    }
    
    void navigateRight() {
        if (currentScreen < SOCIAL_SETTINGS) {
            currentScreen = (SocialScreen)(currentScreen + 1);
            selectedItem = 0;
            scrollOffset = 0;
        }
    }
    
    void selectCurrentItem() {
        switch (currentScreen) {
            case SOCIAL_FEED:
                if (selectedItem + scrollOffset < feedPosts.size()) {
                    selectedPostIndex = selectedItem + scrollOffset;
                    showPostDetails();
                }
                break;
                
            case SOCIAL_DISCOVER:
                if (selectedItem < 3) {
                    // Follow suggested user
                    String userIds[] = {"cyber_senpai", "neko_master", "kawaii_hacker"};
                    followUser(userIds[selectedItem]);
                }
                break;
                
            case SOCIAL_GROUPS:
                if (selectedItem < discoverGroups.size()) {
                    joinGroup(discoverGroups[selectedItem].group_id);
                }
                break;
                
            case SOCIAL_NOTIFICATIONS:
                if (selectedItem < notifications.size()) {
                    notifications[selectedItem].is_read = true;
                    if (unreadNotifications > 0) unreadNotifications--;
                }
                break;
                
            case SOCIAL_POST_CREATE:
                handlePostCreation();
                break;
                
            case SOCIAL_SETTINGS:
                handleSettingsSelection();
                break;
        }
    }
    
    void goBack() {
        if (showingPostDetails) {
            showingPostDetails = false;
        } else {
            currentScreen = SOCIAL_FEED;
            selectedItem = 0;
            scrollOffset = 0;
        }
    }
    
    void showPostDetails() {
        if (selectedPostIndex >= feedPosts.size()) return;
        
        SocialPost& post = feedPosts[selectedPostIndex];
        
        M5.Display.fillScreen(BLACK);
        
        // Post header
        M5.Display.setTextColor(postTypeColors[post.type]);
        M5.Display.setCursor(10, 10);
        M5.Display.print(postTypeEmojis[post.type] + " " + post.title);
        
        // Author info
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 25);
        M5.Display.print("By: " + post.author_name + " " + post.author_emoji);
        
        M5.Display.setTextColor(DARKGREY);
        M5.Display.setCursor(150, 25);
        M5.Display.print(formatTimestamp(post.timestamp));
        
        // Content
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(10, 40);
        M5.Display.print("Content:");
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 55);
        
        // Word wrap content
        String content = post.content;
        int lineY = 55;
        int charsPerLine = 35;
        for (int i = 0; i < content.length(); i += charsPerLine) {
            String line = content.substring(i, min(i + charsPerLine, (int)content.length()));
            M5.Display.setCursor(10, lineY);
            M5.Display.print(line);
            lineY += 10;
            if (lineY > 90) break; // Don't overflow screen
        }
        
        // Reactions
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(10, 100);
        M5.Display.print("Reactions:");
        
        String reactionEmojis[] = {"👍", "😸", "😻", "😾", "🤩", "🧠", "🔥", "💖"};
        int reactionX = 10;
        for (int i = 0; i < 8; i++) {
            if (post.reactions.find((ReactionType)i) != post.reactions.end() && 
                post.reactions[(ReactionType)i] > 0) {
                M5.Display.setCursor(reactionX, 110);
                M5.Display.printf("%s%d", reactionEmojis[i].c_str(), post.reactions[(ReactionType)i]);
                reactionX += 30;
            }
        }
        
        // Controls
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(10, 125);
        M5.Display.print("👍 React | 🔄 Share | ← Back");
        
        showingPostDetails = true;
    }
    
    void handlePostCreation() {
        // Simplified post creation - in real app would have text input
        if (newPostTitle.isEmpty()) {
            newPostTitle = "My Latest Discovery";
        }
        if (newPostContent.isEmpty()) {
            newPostContent = "Found something interesting today! Still learning and exploring the digital world with my kawaii companion! 🐱💻";
        }
        
        createPost(newPostTitle, newPostContent, newPostType, newPostAnonymous);
        
        // Reset form
        newPostTitle = "";
        newPostContent = "";
        newPostType = DISCOVERY_POST;
        newPostAnonymous = false;
        
        // Go back to feed
        currentScreen = SOCIAL_FEED;
        selectedItem = 0;
    }
    
    void handleSettingsSelection() {
        switch (selectedItem) {
            case 1: // Change Theme
                myProfile.theme_color = (myProfile.theme_color == PINK) ? CYAN :
                                       (myProfile.theme_color == CYAN) ? GREEN :
                                       (myProfile.theme_color == GREEN) ? ORANGE : PINK;
                break;
            case 3: // Anonymous Mode
                myProfile.allow_anonymous_follow = !myProfile.allow_anonymous_follow;
                break;
            case 4: // Show Real Stats
                myProfile.show_real_stats = !myProfile.show_real_stats;
                break;
            case 5: // Allow DMs
                myProfile.allow_direct_messages = !myProfile.allow_direct_messages;
                break;
            case 6: // Sync Data
                syncToNetwork();
                break;
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    String generateUserId() {
        return "user_" + String(millis() % 100000);
    }
    
    String getReputationName(UserReputation level) {
        switch (level) {
            case NEWBIE: return "Newbie Neko";
            case APPRENTICE: return "Apprentice";
            case SKILLED: return "Skilled Hacker";
            case EXPERT: return "Expert";
            case MASTER: return "Master";
            case SENPAI: return "Senpai";
            case LEGENDARY_SENPAI: return "Legendary Senpai";
            default: return "Unknown";
        }
    }
    
    String getUserDisplayName(String userId) {
        if (userId == myProfile.user_id) return myProfile.nickname;
        if (userId == "cyber_senpai") return "CyberSenpai";
        if (userId == "neko_master") return "NekoMaster";
        if (userId == "kawaii_hacker") return "KawaiiHacker";
        return "Unknown User";
    }
    
    String formatTimestamp(unsigned long timestamp) {
        unsigned long elapsed = millis() - timestamp;
        int minutes = elapsed / 60000;
        
        if (minutes < 1) return "now";
        if (minutes < 60) return String(minutes) + "m";
        int hours = minutes / 60;
        if (hours < 24) return String(hours) + "h";
        return String(hours / 24) + "d";
    }
    
    int getMaxItemsForCurrentScreen() {
        switch (currentScreen) {
            case SOCIAL_FEED: return feedPosts.size();
            case SOCIAL_DISCOVER: return 3;
            case SOCIAL_GROUPS: return discoverGroups.size();
            case SOCIAL_LEADERBOARD: return 4;
            case SOCIAL_NOTIFICATIONS: return notifications.size();
            case SOCIAL_SETTINGS: return 8;
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
    
    void updateNetworkStatus() {
        // Simulate network connectivity
        socialNetworkConnected = WiFi.status() == WL_CONNECTED;
    }
    
    void processNotifications() {
        // Simulate receiving notifications
        static unsigned long lastNotif = 0;
        if (millis() - lastNotif > 120000 && notifications.size() < 10) { // Every 2 minutes
            SocialNotification notif;
            notif.notification_id = "notif_" + String(millis());
            notif.from_user_id = "cyber_senpai";
            notif.from_user_name = "CyberSenpai";
            notif.content = "liked your recent discovery post!";
            notif.action = "liked";
            notif.timestamp = millis();
            notif.is_read = false;
            notif.is_important = false;
            
            notifications.insert(notifications.begin(), notif);
            unreadNotifications++;
            lastNotif = millis();
            
            if (nekoSounds) {
                nekoSounds->playContextSound("notification_received");
            }
        }
    }
    
    void updateTrendingContent() {
        // Update trending status based on engagement
        for (SocialPost& post : feedPosts) {
            int totalEngagement = post.shares * 3; // Shares worth more
            for (auto& reaction : post.reactions) {
                totalEngagement += reaction.second;
            }
            
            post.is_trending = (totalEngagement > 30);
        }
    }
    
    void refreshFeed() {
        // Simulate new posts arriving
        if (feedPosts.size() < 20) {
            // Add a new sample post occasionally
            generateSampleContent();
        }
        
        Serial.println("🔄 Feed refreshed");
    }
    
    void connectToSocialNetwork() {
        // Simulate connecting to social network
        socialNetworkConnected = true;
        Serial.println("🌐 Connected to Kawaii Social Network");
    }
    
    void syncToNetwork() {
        if (!socialNetworkConnected) return;
        
        // Simulate syncing data
        Serial.println("🔄 Syncing social data...");
        
        if (nekoSounds) {
            nekoSounds->playContextSound("sync_complete");
        }
    }
    
    void saveUserData() {
        Serial.println("💾 Saving social data...");
        // Would save to SD card in real implementation
    }
    
    void loadUserData() {
        Serial.println("📂 Loading social data...");
        // Would load from SD card in real implementation
    }
    
    // ==================== API FUNCTIONS ====================
    
    // Called by other systems to create social content
    void postAchievement(String name, String emoji) { shareAchievement(name, emoji); }
    void postDiscovery(String network, String type) { shareDiscovery(network, type); }
    void postHackSuccess(String hack, String info) { shareHackSuccess(hack, info); }
    void postPetMilestone(String pet, String milestone) { sharePetMilestone(pet, milestone); }
    void postQuestion(String question, String details) { askForHelp(question, details); }
    
    // Getters
    int getReputation() { return myProfile.reputation_points; }
    UserReputation getReputationLevel() { return myProfile.reputation_level; }
    int getFollowersCount() { return myProfile.followers_count; }
    int getFollowingCount() { return myProfile.following_count; }
    int getPostsCount() { return myProfile.posts_count; }
    int getUnreadNotifications() { return unreadNotifications; }
    bool isConnected() { return socialNetworkConnected; }
    String getUserId() { return myProfile.user_id; }
    String getNickname() { return myProfile.nickname; }
};

#endif // M5GOTCHI_KAWAII_SOCIAL_H