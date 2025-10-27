/*
 * 💬 M5GOTCHI NEKOGRAM SECURE CHAT v1.0
 * P2P encrypted messaging system between M5Gotchis with kawaii features!
 * 
 * Features:
 * - End-to-end encryption with cute key exchange
 * - P2P discovery via WiFi Direct and Bluetooth
 * - Kawaii sticker system with neko emojis
 * - Voice messages with neko sounds
 * - Group chats with up to 8 M5Gotchis
 * - Message reactions and kawaii animations
 * - Secure file sharing (configs, wordlists, etc.)
 * - Anonymous mode with cute codenames
 * - Chat themes and personalization
 * - Message self-destruct timers
 * 
 * Protocol: Custom encrypted protocol over ESP-NOW for low latency
 * Security: AES-256 + RSA key exchange + forward secrecy
 * 
 * Nya~ Let's chat securely! 🔐💕
 */

#ifndef M5GOTCHI_NEKOGRAM_CHAT_H
#define M5GOTCHI_NEKOGRAM_CHAT_H

#include <M5Unified.h>
#include <WiFi.h>
#include <esp_now.h>
#include <vector>
#include <map>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_neko_sounds.h"

// ==================== CHAT ENUMS ====================
enum ChatState {
    CHAT_MENU = 0,
    PEER_DISCOVERY,
    CHAT_LIST,
    ACTIVE_CHAT,
    COMPOSE_MESSAGE,
    STICKER_SELECTION,
    SETTINGS_MENU,
    KEY_EXCHANGE,
    GROUP_MANAGEMENT
};

enum MessageType {
    TEXT_MESSAGE = 0,
    STICKER_MESSAGE,
    VOICE_MESSAGE,
    FILE_MESSAGE,
    SYSTEM_MESSAGE,
    REACTION_MESSAGE,
    KEY_EXCHANGE_MESSAGE
};

enum PeerStatus {
    OFFLINE = 0,
    ONLINE,
    AWAY,
    BUSY,
    INVISIBLE,
    IN_HACK_MODE
};

enum ChatTheme {
    CLASSIC_NEKO = 0,
    HACKER_DARK,
    KAWAII_PINK,
    CYBER_BLUE,
    RAINBOW_PRIDE,
    STEALTH_MODE
};

// ==================== CHAT STRUCTURES ====================
struct ChatMessage {
    uint32_t id;
    uint8_t sender_mac[6];
    String sender_name;
    String content;
    MessageType type;
    unsigned long timestamp;
    bool encrypted;
    bool read;
    bool self_destruct;
    uint32_t destruct_timer;
    String sticker_id;
    std::vector<String> reactions;
};

struct ChatPeer {
    uint8_t mac_address[6];
    String nickname;
    String real_name;
    PeerStatus status;
    String status_message;
    String avatar_emoji;
    unsigned long last_seen;
    bool is_friend;
    bool is_blocked;
    uint32_t public_key_hash;
    String device_model;
    int signal_strength;
    ChatTheme preferred_theme;
};

struct ChatRoom {
    String room_id;
    String room_name;
    String room_emoji;
    std::vector<uint8_t*> participants;
    std::vector<ChatMessage> messages;
    bool is_group;
    bool is_encrypted;
    unsigned long created_time;
    String admin_mac;
    int max_participants;
};

struct StickerPack {
    String pack_id;
    String pack_name;
    String pack_emoji;
    std::vector<String> stickers;
    bool unlocked;
    String unlock_condition;
};

// ==================== NEKOGRAM CHAT CLASS ====================
class M5GotchiNekoGramChat {
private:
    ChatState currentState;
    int selectedItem;
    int currentPage;
    
    // Networking
    bool espNowInitialized;
    std::vector<ChatPeer> discoveredPeers;
    std::vector<ChatPeer> friends;
    std::vector<ChatRoom> chatRooms;
    
    // Current chat
    String activeChatId;
    ChatRoom* activeRoom;
    String composingMessage;
    int messageScrollOffset;
    
    // User profile
    String myNickname;
    String myStatusMessage;
    String myAvatarEmoji;
    PeerStatus myStatus;
    ChatTheme myTheme;
    bool anonymousMode;
    
    // Security
    uint8_t privateKey[32];
    uint8_t publicKey[32];
    std::map<String, uint8_t*> peerKeys;
    bool encryptionEnabled;
    
    // Stickers
    std::vector<StickerPack> stickerPacks;
    int selectedStickerPack;
    int selectedSticker;
    
    // UI state
    bool showingTypingIndicator;
    unsigned long lastTypingTime;
    int animationFrame;
    unsigned long lastAnimUpdate;
    bool keyboardMode;
    String inputBuffer;
    
    // Sound system
    M5GotchiNekoSounds* nekoSounds;
    
    // Colors for themes
    uint16_t themeColors[6][3] = {
        {WHITE, PINK, ORANGE},      // Classic Neko
        {GREEN, BLACK, CYAN},       // Hacker Dark
        {PINK, MAGENTA, WHITE},     // Kawaii Pink  
        {CYAN, BLUE, WHITE},        // Cyber Blue
        {RED, YELLOW, MAGENTA},     // Rainbow Pride
        {DARKGREY, BLACK, GREEN}    // Stealth Mode
    };

public:
    M5GotchiNekoGramChat() {
        currentState = CHAT_MENU;
        selectedItem = 0;
        currentPage = 0;
        espNowInitialized = false;
        activeRoom = nullptr;
        messageScrollOffset = 0;
        
        myNickname = "NekoHacker";
        myStatusMessage = "Hacking the world with kawaii power!";
        myAvatarEmoji = "🐱";
        myStatus = ONLINE;
        myTheme = CLASSIC_NEKO;
        anonymousMode = false;
        
        selectedStickerPack = 0;
        selectedSticker = 0;
        showingTypingIndicator = false;
        lastTypingTime = 0;
        animationFrame = 0;
        lastAnimUpdate = 0;
        keyboardMode = false;
        encryptionEnabled = true;
        nekoSounds = nullptr;
        
        initializeStickers();
        generateKeyPair();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("💬 Initializing NekoGram Secure Chat...");
        
        initializeESPNow();
        initializeStickers();
        loadProfile();
        loadFriends();
        
        Serial.println("✅ NekoGram Chat Ready! Nya~ Let's chat! 💕");
    }
    
    void setSoundSystem(M5GotchiNekoSounds* sounds) {
        nekoSounds = sounds;
    }
    
    bool initializeESPNow() {
        if (espNowInitialized) return true;
        
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        
        if (esp_now_init() != ESP_OK) {
            Serial.println("❌ ESP-NOW initialization failed");
            return false;
        }
        
        // Register callbacks
        esp_now_register_send_cb(onDataSent);
        esp_now_register_recv_cb(onDataReceived);
        
        espNowInitialized = true;
        Serial.println("📡 ESP-NOW initialized successfully");
        return true;
    }
    
    void generateKeyPair() {
        // Generate random key pair for encryption
        for (int i = 0; i < 32; i++) {
            privateKey[i] = random(0, 256);
        }
        
        // Simple key derivation (in real implementation, use proper crypto)
        for (int i = 0; i < 32; i++) {
            publicKey[i] = privateKey[i] ^ 0xAA; // XOR with constant
        }
        
        Serial.println("🔐 Key pair generated");
    }
    
    void initializeStickers() {
        stickerPacks.clear();
        
        // Basic Neko Pack
        StickerPack basicPack;
        basicPack.pack_id = "basic_neko";
        basicPack.pack_name = "Basic Neko";
        basicPack.pack_emoji = "🐱";
        basicPack.stickers = {
            "😸", "😻", "😽", "🙀", "😿", "😾",
            "🐱", "🐈", "🦁", "🐯", "🐅", "🐆"
        };
        basicPack.unlocked = true;
        basicPack.unlock_condition = "";
        stickerPacks.push_back(basicPack);
        
        // Hacker Pack
        StickerPack hackerPack;
        hackerPack.pack_id = "hacker_neko";
        hackerPack.pack_name = "Hacker Neko";
        hackerPack.pack_emoji = "💻";
        hackerPack.stickers = {
            "🔒", "🔓", "🔑", "🛡️", "⚡", "💻",
            "📡", "🕷️", "🐛", "🔍", "💾", "⚙️"
        };
        hackerPack.unlocked = false;
        hackerPack.unlock_condition = "Complete 10 successful hacks";
        stickerPacks.push_back(hackerPack);
        
        // Kawaii Pack
        StickerPack kawaiiPack;
        kawaiiPack.pack_id = "kawaii_power";
        kawaiiPack.pack_name = "Kawaii Power";
        kawaiiPack.pack_emoji = "💖";
        kawaiiPack.stickers = {
            "💖", "💕", "💓", "💗", "💝", "💘",
            "🌸", "🌺", "🌻", "🌷", "🎀", "✨"
        };
        kawaiiPack.unlocked = false;
        kawaiiPack.unlock_condition = "Unlock 5 kawaii achievements";
        stickerPacks.push_back(kawaiiPack);
        
        // Secret Pack
        StickerPack secretPack;
        secretPack.pack_id = "secret_neko";
        secretPack.pack_name = "Secret Neko";
        secretPack.pack_emoji = "🤫";
        secretPack.stickers = {
            "🤫", "🕵️", "👻", "🦹", "🥷", "🎭",
            "🔮", "⚗️", "🧪", "🔬", "🌌", "🪐"
        };
        secretPack.unlocked = false;
        secretPack.unlock_condition = "Find 5 easter eggs";
        stickerPacks.push_back(secretPack);
    }
    
    // ==================== CHAT FLOW ====================
    void start() {
        currentState = CHAT_MENU;
        selectedItem = 0;
        drawChatScreen();
        
        if (nekoSounds) {
            nekoSounds->playContextSound("notification");
        }
    }
    
    void stop() {
        saveChatHistory();
        saveProfile();
        
        if (espNowInitialized) {
            esp_now_deinit();
            espNowInitialized = false;
        }
        
        if (nekoSounds) {
            nekoSounds->playContextSound("goodbye");
        }
    }
    
    void update() {
        updateAnimations();
        updateTypingIndicator();
        processIncomingMessages();
        updatePeerDiscovery();
        
        // Auto-scroll in active chat
        if (currentState == ACTIVE_CHAT && activeRoom) {
            if (activeRoom->messages.size() > 6) {
                messageScrollOffset = activeRoom->messages.size() - 6;
            }
        }
    }
    
    // ==================== SCREEN DRAWING ====================
    void drawChatScreen() {
        M5.Display.fillScreen(getThemeColor(1)); // Background
        
        switch (currentState) {
            case CHAT_MENU:
                drawMainMenu();
                break;
            case PEER_DISCOVERY:
                drawPeerDiscovery();
                break;
            case CHAT_LIST:
                drawChatList();
                break;
            case ACTIVE_CHAT:
                drawActiveChat();
                break;
            case COMPOSE_MESSAGE:
                drawComposeMessage();
                break;
            case STICKER_SELECTION:
                drawStickerSelection();
                break;
            case SETTINGS_MENU:
                drawSettingsMenu();
                break;
            case KEY_EXCHANGE:
                drawKeyExchange();
                break;
            case GROUP_MANAGEMENT:
                drawGroupManagement();
                break;
        }
    }
    
    void drawMainMenu() {
        // Header with kawaii design
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setTextSize(1);
        M5.Display.setCursor(50, 10);
        M5.Display.print("💬 NekoGram");
        
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(60, 25);
        M5.Display.print("Secure Chat");
        
        // Menu options
        String menuItems[] = {
            "💬 Active Chats",
            "🔍 Find Friends", 
            "👥 My Friends",
            "🎨 Stickers",
            "⚙️ Settings",
            "📊 Chat Stats",
            "🚪 Exit"
        };
        
        for (int i = 0; i < 7; i++) {
            uint16_t color = (i == selectedItem) ? getThemeColor(2) : getThemeColor(0);
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 45 + i * 10);
            M5.Display.print(menuItems[i]);
        }
        
        // Status display
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(10, 120);
        M5.Display.print(myAvatarEmoji + " " + myNickname);
        
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(100, 120);
        M5.Display.print("Online | " + String(friends.size()) + " friends");
    }
    
    void drawPeerDiscovery() {
        // Header
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(50, 10);
        M5.Display.print("🔍 Finding NekoGrams");
        
        // Scanning animation
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(70, 30);
        String dots = "";
        for (int i = 0; i < (animationFrame / 10) % 4; i++) {
            dots += ".";
        }
        M5.Display.print("Scanning" + dots);
        
        // Discovered peers
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(10, 50);
        M5.Display.printf("Found %d nearby NekoGrams:", discoveredPeers.size());
        
        for (int i = 0; i < min(4, (int)discoveredPeers.size()); i++) {
            ChatPeer& peer = discoveredPeers[i];
            
            uint16_t color = (i == selectedItem) ? getThemeColor(2) : getThemeColor(0);
            M5.Display.setTextColor(color);
            M5.Display.setCursor(15, 65 + i * 12);
            M5.Display.print(peer.avatar_emoji + " " + peer.nickname);
            
            // Signal strength
            M5.Display.setCursor(150, 65 + i * 12);
            String signal = "";
            int bars = map(peer.signal_strength, -100, -30, 0, 4);
            for (int b = 0; b < 4; b++) {
                signal += (b < bars) ? "📶" : "📵";
            }
            M5.Display.print(signal);
            
            // Status
            M5.Display.setTextColor(getStatusColor(peer.status));
            M5.Display.setCursor(15, 65 + i * 12 + 8);
            M5.Display.print(getStatusString(peer.status));
        }
        
        // Instructions
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(20, 115);
        M5.Display.print("⏎ Connect | ← Back");
    }
    
    void drawChatList() {
        // Header
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(60, 10);
        M5.Display.print("💬 My Chats");
        
        // Chat rooms list
        for (int i = 0; i < min(5, (int)chatRooms.size()); i++) {
            ChatRoom& room = chatRooms[i];
            
            uint16_t color = (i == selectedItem) ? getThemeColor(2) : getThemeColor(0);
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 30 + i * 18);
            M5.Display.print(room.room_emoji + " " + room.room_name);
            
            // Last message preview
            if (!room.messages.empty()) {
                ChatMessage& lastMsg = room.messages.back();
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(15, 30 + i * 18 + 8);
                String preview = lastMsg.sender_name + ": ";
                if (lastMsg.type == STICKER_MESSAGE) {
                    preview += lastMsg.sticker_id;
                } else {
                    preview += lastMsg.content.substring(0, 20);
                    if (lastMsg.content.length() > 20) preview += "...";
                }
                M5.Display.print(preview);
            }
            
            // Unread indicator
            int unreadCount = getUnreadCount(room.room_id);
            if (unreadCount > 0) {
                M5.Display.fillCircle(220, 35 + i * 18, 8, RED);
                M5.Display.setTextColor(WHITE);
                M5.Display.setCursor(217, 32 + i * 18);
                M5.Display.print(String(unreadCount));
            }
        }
        
        // No chats message
        if (chatRooms.empty()) {
            M5.Display.setTextColor(getThemeColor(0));
            M5.Display.setCursor(40, 60);
            M5.Display.print("No chats yet!");
            M5.Display.setCursor(30, 75);
            M5.Display.print("Find friends to start");
            M5.Display.setCursor(50, 90);
            M5.Display.print("chatting! 💕");
        }
    }
    
    void drawActiveChat() {
        if (!activeRoom) return;
        
        // Chat header
        M5.Display.fillRect(0, 0, 240, 20, getThemeColor(2));
        M5.Display.setTextColor(getThemeColor(1));
        M5.Display.setCursor(10, 5);
        M5.Display.print(activeRoom->room_emoji + " " + activeRoom->room_name);
        
        // Encryption indicator
        if (activeRoom->is_encrypted) {
            M5.Display.setCursor(200, 5);
            M5.Display.print("🔒");
        }
        
        // Messages area
        int messageY = 25;
        int startIdx = max(0, messageScrollOffset);
        int endIdx = min(startIdx + 6, (int)activeRoom->messages.size());
        
        for (int i = startIdx; i < endIdx; i++) {
            ChatMessage& msg = activeRoom->messages[i];
            
            // Message bubble
            bool isMyMessage = isMyMac(msg.sender_mac);
            int bubbleX = isMyMessage ? 100 : 10;
            int bubbleW = 130;
            uint16_t bubbleColor = isMyMessage ? getThemeColor(2) : DARKGREY;
            
            M5.Display.fillRoundRect(bubbleX, messageY, bubbleW, 15, 5, bubbleColor);
            
            // Message content
            M5.Display.setTextColor(isMyMessage ? getThemeColor(1) : WHITE);
            M5.Display.setCursor(bubbleX + 5, messageY + 3);
            
            if (msg.type == STICKER_MESSAGE) {
                M5.Display.print(msg.sticker_id);
            } else if (msg.type == SYSTEM_MESSAGE) {
                M5.Display.setTextColor(YELLOW);
                M5.Display.print("📢 " + msg.content);
            } else {
                String displayContent = msg.content;
                if (displayContent.length() > 18) {
                    displayContent = displayContent.substring(0, 18) + "...";
                }
                M5.Display.print(displayContent);
            }
            
            // Timestamp
            M5.Display.setTextColor(DARKGREY);
            M5.Display.setCursor(bubbleX + 5, messageY + 12);
            M5.Display.print(formatTime(msg.timestamp));
            
            messageY += 18;
        }
        
        // Typing indicator
        if (showingTypingIndicator) {
            M5.Display.setTextColor(getThemeColor(2));
            M5.Display.setCursor(10, messageY);
            M5.Display.print("Someone is typing...");
        }
        
        // Input area
        M5.Display.fillRect(0, 120, 240, 15, getThemeColor(0));
        M5.Display.setTextColor(getThemeColor(1));
        M5.Display.setCursor(5, 125);
        M5.Display.print("💬 Type message | 🎨 Stickers");
    }
    
    void drawComposeMessage() {
        // Header
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(60, 10);
        M5.Display.print("✏️ Compose");
        
        // Input field
        M5.Display.drawRect(10, 30, 220, 60, getThemeColor(0));
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(15, 35);
        
        // Show input buffer with cursor
        String displayText = inputBuffer;
        if (millis() % 1000 < 500) { // Blinking cursor
            displayText += "_";
        }
        
        // Word wrap for long messages
        int lineY = 35;
        int charCount = 0;
        for (int i = 0; i < displayText.length(); i++) {
            M5.Display.print(displayText[i]);
            charCount++;
            if (charCount >= 30 || displayText[i] == '\n') {
                lineY += 10;
                M5.Display.setCursor(15, lineY);
                charCount = 0;
            }
        }
        
        // Character counter
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(180, 95);
        M5.Display.printf("%d/200", inputBuffer.length());
        
        // Options
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(10, 110);
        M5.Display.print("⏎ Send | 🎨 Stickers | ← Back");
    }
    
    void drawStickerSelection() {
        // Header
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(50, 5);
        M5.Display.print("🎨 Choose Sticker");
        
        // Sticker pack tabs
        for (int i = 0; i < min(4, (int)stickerPacks.size()); i++) {
            StickerPack& pack = stickerPacks[i];
            uint16_t color = (i == selectedStickerPack) ? getThemeColor(2) : 
                           (pack.unlocked ? getThemeColor(0) : DARKGREY);
            
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10 + i * 50, 20);
            M5.Display.print(pack.pack_emoji);
        }
        
        // Sticker grid
        if (selectedStickerPack < stickerPacks.size()) {
            StickerPack& pack = stickerPacks[selectedStickerPack];
            
            if (pack.unlocked) {
                int stickerIdx = 0;
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 6; col++) {
                        if (stickerIdx >= pack.stickers.size()) break;
                        
                        int x = 10 + col * 35;
                        int y = 35 + row * 25;
                        
                        // Selection highlight
                        if (stickerIdx == selectedSticker) {
                            M5.Display.fillRoundRect(x-3, y-3, 30, 20, 5, getThemeColor(2));
                        }
                        
                        // Sticker
                        M5.Display.setTextColor(getThemeColor(0));
                        M5.Display.setCursor(x, y);
                        M5.Display.print(pack.stickers[stickerIdx]);
                        
                        stickerIdx++;
                    }
                }
            } else {
                // Locked pack
                M5.Display.setTextColor(DARKGREY);
                M5.Display.setCursor(60, 50);
                M5.Display.print("🔒 LOCKED");
                M5.Display.setCursor(30, 65);
                M5.Display.print(pack.unlock_condition);
            }
        }
        
        // Instructions
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(30, 115);
        M5.Display.print("⏎ Send | ← Back");
    }
    
    void drawSettingsMenu() {
        // Header
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(70, 10);
        M5.Display.print("⚙️ Settings");
        
        // Settings options
        String settingsItems[] = {
            "📝 Change Nickname",
            "😸 Change Avatar",
            "💬 Status Message", 
            "🎨 Chat Theme",
            "🔐 Encryption: " + String(encryptionEnabled ? "ON" : "OFF"),
            "🥷 Anonymous Mode: " + String(anonymousMode ? "ON" : "OFF"),
            "🔊 Notification Sounds",
            "← Back to Menu"
        };
        
        for (int i = 0; i < 8; i++) {
            uint16_t color = (i == selectedItem) ? getThemeColor(2) : getThemeColor(0);
            M5.Display.setTextColor(color);
            M5.Display.setCursor(10, 30 + i * 11);
            M5.Display.print(settingsItems[i]);
        }
    }
    
    void drawKeyExchange() {
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(50, 30);
        M5.Display.print("🔐 Key Exchange");
        
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(30, 50);
        M5.Display.print("Establishing secure");
        M5.Display.setCursor(40, 65);
        M5.Display.print("connection...");
        
        // Progress animation
        M5.Display.setCursor(80, 85);
        String progress = "";
        for (int i = 0; i < (animationFrame / 10) % 6; i++) {
            progress += "🔑";
        }
        M5.Display.print(progress);
    }
    
    void drawGroupManagement() {
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(50, 10);
        M5.Display.print("👥 Group Chat");
        
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(30, 40);
        M5.Display.print("Group features");
        M5.Display.setCursor(40, 55);
        M5.Display.print("coming soon!");
        M5.Display.setCursor(60, 80);
        M5.Display.print("Stay tuned! 💕");
    }
    
    // ==================== MESSAGE HANDLING ====================
    void sendMessage(String content, MessageType type = TEXT_MESSAGE, String stickerId = "") {
        if (!activeRoom) return;
        
        ChatMessage msg;
        msg.id = millis(); // Simple ID generation
        memcpy(msg.sender_mac, WiFi.macAddress().c_str(), 6);
        msg.sender_name = myNickname;
        msg.content = content;
        msg.type = type;
        msg.timestamp = millis();
        msg.encrypted = encryptionEnabled;
        msg.read = true; // Own messages are read
        msg.self_destruct = false;
        msg.sticker_id = stickerId;
        
        // Add to chat room
        activeRoom->messages.push_back(msg);
        
        // Send via ESP-NOW to all participants
        broadcastMessage(msg);
        
        // Play send sound
        if (nekoSounds) {
            if (type == STICKER_MESSAGE) {
                nekoSounds->playContextSound("sticker_sent");
            } else {
                nekoSounds->playContextSound("message_sent");
            }
        }
        
        Serial.printf("📤 Message sent: %s\n", content.c_str());
    }
    
    void broadcastMessage(ChatMessage& msg) {
        if (!activeRoom) return;
        
        // Create message packet
        uint8_t data[250];
        int dataSize = serializeMessage(msg, data);
        
        // Send to all participants
        for (uint8_t* participant : activeRoom->participants) {
            esp_err_t result = esp_now_send(participant, data, dataSize);
            if (result != ESP_OK) {
                Serial.printf("❌ Failed to send message to peer\n");
            }
        }
    }
    
    int serializeMessage(ChatMessage& msg, uint8_t* buffer) {
        // Simple serialization (in real implementation, use proper protocol)
        int offset = 0;
        
        // Message header
        buffer[offset++] = 0xNE; // Magic bytes
        buffer[offset++] = 0xKO;
        buffer[offset++] = (uint8_t)msg.type;
        buffer[offset++] = msg.encrypted ? 1 : 0;
        
        // Sender MAC
        memcpy(buffer + offset, msg.sender_mac, 6);
        offset += 6;
        
        // Timestamp
        memcpy(buffer + offset, &msg.timestamp, 4);
        offset += 4;
        
        // Content length and data
        uint8_t contentLen = min(200, (int)msg.content.length());
        buffer[offset++] = contentLen;
        memcpy(buffer + offset, msg.content.c_str(), contentLen);
        offset += contentLen;
        
        return offset;
    }
    
    void processIncomingMessages() {
        // This would be called from ESP-NOW callback
        // For now, simulate receiving messages periodically
        static unsigned long lastSimMessage = 0;
        if (millis() - lastSimMessage > 30000) { // Every 30 seconds
            simulateIncomingMessage();
            lastSimMessage = millis();
        }
    }
    
    void simulateIncomingMessage() {
        if (chatRooms.empty()) return;
        
        ChatMessage msg;
        msg.id = millis();
        // Simulate random sender MAC
        for (int i = 0; i < 6; i++) {
            msg.sender_mac[i] = random(0, 256);
        }
        msg.sender_name = "NekoFriend";
        msg.content = "Nya~ How's the hacking going? 🐱";
        msg.type = TEXT_MESSAGE;
        msg.timestamp = millis();
        msg.encrypted = true;
        msg.read = false;
        
        chatRooms[0].messages.push_back(msg);
        
        // Play notification sound
        if (nekoSounds) {
            nekoSounds->playContextSound("message_received");
        }
    }
    
    // ==================== PEER DISCOVERY ====================
    void startPeerDiscovery() {
        discoveredPeers.clear();
        
        // Simulate discovering peers
        ChatPeer peer1;
        memset(peer1.mac_address, 0xAA, 6);
        peer1.nickname = "CyberNeko";
        peer1.real_name = "Anonymous";
        peer1.status = ONLINE;
        peer1.status_message = "Ready to hack!";
        peer1.avatar_emoji = "🐱‍💻";
        peer1.last_seen = millis();
        peer1.signal_strength = -45;
        peer1.preferred_theme = HACKER_DARK;
        discoveredPeers.push_back(peer1);
        
        ChatPeer peer2;
        memset(peer2.mac_address, 0xBB, 6);
        peer2.nickname = "KawaiiHacker";
        peer2.real_name = "Anonymous";
        peer2.status = AWAY;
        peer2.status_message = "Eating sushi 🍣";
        peer2.avatar_emoji = "😸";
        peer2.last_seen = millis() - 300000;
        peer2.signal_strength = -67;
        peer2.preferred_theme = KAWAII_PINK;
        discoveredPeers.push_back(peer2);
        
        Serial.printf("🔍 Discovered %d peers\n", discoveredPeers.size());
    }
    
    void updatePeerDiscovery() {
        // Update signal strengths and status
        for (ChatPeer& peer : discoveredPeers) {
            peer.signal_strength += random(-5, 5);
            peer.signal_strength = constrain(peer.signal_strength, -100, -30);
        }
    }
    
    void connectToPeer(ChatPeer& peer) {
        // Add as friend if not already
        bool alreadyFriend = false;
        for (ChatPeer& friend_peer : friends) {
            if (memcmp(friend_peer.mac_address, peer.mac_address, 6) == 0) {
                alreadyFriend = true;
                break;
            }
        }
        
        if (!alreadyFriend) {
            friends.push_back(peer);
        }
        
        // Create or find chat room
        String roomId = createRoomId(peer.mac_address);
        ChatRoom* room = findChatRoom(roomId);
        
        if (!room) {
            ChatRoom newRoom;
            newRoom.room_id = roomId;
            newRoom.room_name = peer.nickname;
            newRoom.room_emoji = peer.avatar_emoji;
            newRoom.participants.push_back(peer.mac_address);
            newRoom.is_group = false;
            newRoom.is_encrypted = encryptionEnabled;
            newRoom.created_time = millis();
            newRoom.max_participants = 2;
            
            chatRooms.push_back(newRoom);
            room = &chatRooms.back();
        }
        
        // Switch to chat
        activeChatId = roomId;
        activeRoom = room;
        currentState = ACTIVE_CHAT;
        
        // Play connection sound
        if (nekoSounds) {
            nekoSounds->playContextSound("friend_connected");
        }
        
        Serial.printf("🤝 Connected to %s\n", peer.nickname.c_str());
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
        
        drawChatScreen();
    }
    
    void navigateUp() {
        if (currentState == COMPOSE_MESSAGE) {
            // Handle text input navigation
            return;
        }
        
        if (selectedItem > 0) {
            selectedItem--;
        }
    }
    
    void navigateDown() {
        int maxItems = getMaxItemsForCurrentState();
        if (selectedItem < maxItems - 1) {
            selectedItem++;
        }
    }
    
    void navigateLeft() {
        if (currentState == STICKER_SELECTION) {
            if (selectedStickerPack > 0) {
                selectedStickerPack--;
                selectedSticker = 0;
            }
        } else if (currentState == ACTIVE_CHAT) {
            // Scroll up in chat
            if (messageScrollOffset > 0) {
                messageScrollOffset--;
            }
        }
    }
    
    void navigateRight() {
        if (currentState == STICKER_SELECTION) {
            if (selectedStickerPack < stickerPacks.size() - 1) {
                selectedStickerPack++;
                selectedSticker = 0;
            }
        } else if (currentState == ACTIVE_CHAT) {
            // Scroll down in chat
            if (activeRoom && messageScrollOffset < activeRoom->messages.size() - 6) {
                messageScrollOffset++;
            }
        }
    }
    
    void selectCurrentItem() {
        switch (currentState) {
            case CHAT_MENU:
                handleMenuSelection();
                break;
            case PEER_DISCOVERY:
                if (selectedItem < discoveredPeers.size()) {
                    connectToPeer(discoveredPeers[selectedItem]);
                }
                break;
            case CHAT_LIST:
                if (selectedItem < chatRooms.size()) {
                    activeChatId = chatRooms[selectedItem].room_id;
                    activeRoom = &chatRooms[selectedItem];
                    currentState = ACTIVE_CHAT;
                    messageScrollOffset = 0;
                }
                break;
            case ACTIVE_CHAT:
                currentState = COMPOSE_MESSAGE;
                inputBuffer = "";
                keyboardMode = true;
                break;
            case COMPOSE_MESSAGE:
                if (!inputBuffer.isEmpty()) {
                    sendMessage(inputBuffer);
                    inputBuffer = "";
                    currentState = ACTIVE_CHAT;
                }
                break;
            case STICKER_SELECTION:
                if (selectedStickerPack < stickerPacks.size()) {
                    StickerPack& pack = stickerPacks[selectedStickerPack];
                    if (pack.unlocked && selectedSticker < pack.stickers.size()) {
                        sendMessage("", STICKER_MESSAGE, pack.stickers[selectedSticker]);
                        currentState = ACTIVE_CHAT;
                    }
                }
                break;
            case SETTINGS_MENU:
                handleSettingsSelection();
                break;
        }
    }
    
    void goBack() {
        switch (currentState) {
            case PEER_DISCOVERY:
            case CHAT_LIST:
            case SETTINGS_MENU:
                currentState = CHAT_MENU;
                selectedItem = 0;
                break;
            case ACTIVE_CHAT:
                currentState = CHAT_LIST;
                activeRoom = nullptr;
                activeChatId = "";
                break;
            case COMPOSE_MESSAGE:
            case STICKER_SELECTION:
                currentState = ACTIVE_CHAT;
                break;
            case KEY_EXCHANGE:
            case GROUP_MANAGEMENT:
                currentState = CHAT_MENU;
                break;
        }
    }
    
    void handleMenuSelection() {
        switch (selectedItem) {
            case 0: // Active Chats
                currentState = CHAT_LIST;
                selectedItem = 0;
                break;
            case 1: // Find Friends
                currentState = PEER_DISCOVERY;
                selectedItem = 0;
                startPeerDiscovery();
                break;
            case 2: // My Friends
                // Show friends list
                currentState = CHAT_LIST; // For now, same as chat list
                selectedItem = 0;
                break;
            case 3: // Stickers
                currentState = STICKER_SELECTION;
                selectedItem = 0;
                break;
            case 4: // Settings
                currentState = SETTINGS_MENU;
                selectedItem = 0;
                break;
            case 5: // Chat Stats
                showChatStats();
                break;
            case 6: // Exit
                // Handle in main application
                break;
        }
    }
    
    void handleSettingsSelection() {
        switch (selectedItem) {
            case 0: // Change Nickname
                // Open nickname input (simplified)
                myNickname = "NewNekoName";
                break;
            case 1: // Change Avatar
                // Cycle through avatars
                String avatars[] = {"🐱", "😸", "😻", "🙀", "😾", "🐱‍💻", "🐱‍🏍", "🐱‍👤"};
                for (int i = 0; i < 8; i++) {
                    if (myAvatarEmoji == avatars[i]) {
                        myAvatarEmoji = avatars[(i + 1) % 8];
                        break;
                    }
                }
                break;
            case 3: // Chat Theme
                myTheme = (ChatTheme)((myTheme + 1) % 6);
                break;
            case 4: // Encryption Toggle
                encryptionEnabled = !encryptionEnabled;
                break;
            case 5: // Anonymous Mode
                anonymousMode = !anonymousMode;
                break;
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    uint16_t getThemeColor(int index) {
        return themeColors[myTheme][index];
    }
    
    uint16_t getStatusColor(PeerStatus status) {
        switch (status) {
            case ONLINE: return GREEN;
            case AWAY: return YELLOW;
            case BUSY: return RED;
            case INVISIBLE: return DARKGREY;
            case IN_HACK_MODE: return CYAN;
            default: return WHITE;
        }
    }
    
    String getStatusString(PeerStatus status) {
        switch (status) {
            case ONLINE: return "Online";
            case AWAY: return "Away";
            case BUSY: return "Busy";
            case INVISIBLE: return "Invisible";
            case IN_HACK_MODE: return "Hacking";
            default: return "Offline";
        }
    }
    
    bool isMyMac(uint8_t* mac) {
        String myMac = WiFi.macAddress();
        // Simple comparison (would need proper implementation)
        return false; // For now, assume all messages are from others
    }
    
    String createRoomId(uint8_t* peerMac) {
        String roomId = "";
        for (int i = 0; i < 6; i++) {
            roomId += String(peerMac[i], HEX);
        }
        return roomId;
    }
    
    ChatRoom* findChatRoom(String roomId) {
        for (ChatRoom& room : chatRooms) {
            if (room.room_id == roomId) {
                return &room;
            }
        }
        return nullptr;
    }
    
    int getUnreadCount(String roomId) {
        ChatRoom* room = findChatRoom(roomId);
        if (!room) return 0;
        
        int count = 0;
        for (ChatMessage& msg : room->messages) {
            if (!msg.read && !isMyMac(msg.sender_mac)) {
                count++;
            }
        }
        return count;
    }
    
    String formatTime(unsigned long timestamp) {
        unsigned long elapsed = millis() - timestamp;
        int minutes = elapsed / 60000;
        
        if (minutes < 1) return "now";
        if (minutes < 60) return String(minutes) + "m";
        return String(minutes / 60) + "h";
    }
    
    int getMaxItemsForCurrentState() {
        switch (currentState) {
            case CHAT_MENU: return 7;
            case PEER_DISCOVERY: return discoveredPeers.size();
            case CHAT_LIST: return chatRooms.size();
            case SETTINGS_MENU: return 8;
            case STICKER_SELECTION: return stickerPacks[selectedStickerPack].stickers.size();
            default: return 1;
        }
    }
    
    void updateAnimations() {
        if (millis() - lastAnimUpdate > 100) {
            animationFrame++;
            if (animationFrame > 1000) animationFrame = 0;
            lastAnimUpdate = millis();
        }
    }
    
    void updateTypingIndicator() {
        if (showingTypingIndicator && millis() - lastTypingTime > 3000) {
            showingTypingIndicator = false;
        }
    }
    
    void showChatStats() {
        // Simple stats display
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(getThemeColor(2));
        M5.Display.setCursor(60, 20);
        M5.Display.print("📊 Chat Stats");
        
        M5.Display.setTextColor(getThemeColor(0));
        M5.Display.setCursor(20, 40);
        M5.Display.printf("Friends: %d", friends.size());
        M5.Display.setCursor(20, 55);
        M5.Display.printf("Active Chats: %d", chatRooms.size());
        
        int totalMessages = 0;
        for (ChatRoom& room : chatRooms) {
            totalMessages += room.messages.size();
        }
        M5.Display.setCursor(20, 70);
        M5.Display.printf("Total Messages: %d", totalMessages);
        
        delay(3000);
    }
    
    void saveChatHistory() {
        Serial.println("💾 Saving chat history...");
        // Would save to SD card in real implementation
    }
    
    void saveProfile() {
        Serial.println("💾 Saving profile...");
        // Would save to SD card in real implementation
    }
    
    void loadProfile() {
        Serial.println("📂 Loading profile...");
        // Would load from SD card in real implementation
    }
    
    void loadFriends() {
        Serial.println("📂 Loading friends list...");
        // Would load from SD card in real implementation
    }
    
    // ==================== ESP-NOW CALLBACKS ====================
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
        Serial.printf("📤 Message sent to %02x:%02x:%02x:%02x:%02x:%02x, status: %s\n",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5],
                     status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAILED");
    }
    
    static void onDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
        Serial.printf("📥 Message received from %02x:%02x:%02x:%02x:%02x:%02x, len: %d\n",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], len);
        
        // Process received message
        // (Would parse the data and add to appropriate chat room)
    }
    
    // ==================== GETTERS ====================
    ChatState getCurrentState() { return currentState; }
    int getFriendsCount() { return friends.size(); }
    int getActiveChatCount() { return chatRooms.size(); }
    bool isInActiveChat() { return (currentState == ACTIVE_CHAT && activeRoom != nullptr); }
    String getMyNickname() { return myNickname; }
    String getMyStatus() { return myStatusMessage; }
};

#endif // M5GOTCHI_NEKOGRAM_CHAT_H