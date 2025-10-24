/*
 * M5GOTCHI AI COMPANION SYSTEM
 * 🤖 Intelligent Virtual Pet & Assistant
 * 
 * FEATURES:
 * ✨ ChatGPT Integration via WiFi
 * 🧠 Learning AI that adapts to user behavior
 * 🗣️ Voice synthesis for notifications
 * 👾 Emotional states based on pentest success
 * 🎯 Auto-suggestion for next attacks
 * 📊 Smart analytics and recommendations
 * 💬 Natural language interface
 * 🤖 Personality evolution system
 */

#ifndef M5GOTCHI_AI_COMPANION_H
#define M5GOTCHI_AI_COMPANION_H

#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>

// ==================== AI COMPANION ENUMS ====================
enum AIPersonality {
    AI_KAWAII_HELPER,     // 😸 Cute and encouraging
    AI_HACKER_MENTOR,     // 🕶️ Professional and technical  
    AI_CYBER_PUNK,        // 🔥 Edgy and rebellious
    AI_WISE_SENSEI,       // 🧙 Ancient wisdom mode
    AI_PARTY_ANIMAL       // 🎉 Energetic and fun
};

enum AIMood {
    MOOD_HAPPY,           // 😊 Successful attacks
    MOOD_EXCITED,         // 🤩 Discovery mode
    MOOD_FOCUSED,         // 😤 Working hard
    MOOD_SLEEPY,          // 😴 Idle state
    MOOD_WORRIED,         // 😰 Failed attempts
    MOOD_ANGRY            // 😡 Security detected
};

enum AITaskType {
    TASK_SUGGEST_TARGET,
    TASK_ANALYZE_NETWORK,
    TASK_OPTIMIZE_ATTACK,
    TASK_GENERATE_WORDLIST,
    TASK_EXPLAIN_RESULT,
    TASK_MOTIVATE_USER
};

// ==================== AI COMPANION STRUCTS ====================
struct AIPersonalityProfile {
    String name;
    String catchphrase;
    uint16_t primaryColor;
    uint16_t secondaryColor;
    String emoji;
    int friendliness;    // 1-10
    int technical;       // 1-10
    int creativity;      // 1-10
};

struct AIMemory {
    String favTargets[10];
    int successfulAttacks;
    int failedAttempts;
    unsigned long totalUptime;
    String lastConversation;
    int userPreferences[5];
    bool hasLearnedUser;
};

struct AIConversation {
    String userInput;
    String aiResponse;
    unsigned long timestamp;
    bool isImportant;
};

// ==================== M5GOTCHI AI COMPANION CLASS ====================
class M5GotchiAICompanion {
private:
    // Core AI System
    AIPersonality currentPersonality;
    AIMood currentMood;
    AIMemory memory;
    AIPersonalityProfile personalities[5];
    
    // Conversation System
    AIConversation conversationHistory[20];
    int conversationIndex;
    String currentContext;
    bool isThinking;
    
    // Network & API
    String apiKey;
    String chatGPTEndpoint;
    HTTPClient http;
    WiFiClient wifiClient;
    
    // Display & Animation
    int petX, petY;
    int animFrame;
    unsigned long lastAnim;
    unsigned long lastThought;
    
    // Learning System
    float userBehaviorWeights[10];
    int activityPattern[24];  // Hourly usage
    String commonCommands[15];
    
    // Voice Synthesis (Simple Beep Patterns)
    struct VoicePattern {
        int tones[8];
        int durations[8];
        int count;
    };
    VoicePattern voicePatterns[6];

public:
    M5GotchiAICompanion() {
        currentPersonality = AI_KAWAII_HELPER;
        currentMood = MOOD_HAPPY;
        conversationIndex = 0;
        petX = 200;
        petY = 60;
        animFrame = 0;
        lastAnim = 0;
        lastThought = 0;
        isThinking = false;
        
        // Reset memory
        memory.successfulAttacks = 0;
        memory.failedAttempts = 0;
        memory.totalUptime = 0;
        memory.hasLearnedUser = false;
        
        // Initialize behavior weights
        for (int i = 0; i < 10; i++) {
            userBehaviorWeights[i] = 0.5f;
        }
        
        // Initialize activity pattern
        for (int i = 0; i < 24; i++) {
            activityPattern[i] = 0;
        }
    }
    
    // ==================== CORE FUNCTIONS ====================
    void init() {
        Serial.println("🤖 Initializing AI Companion...");
        
        setupPersonalities();
        setupVoicePatterns();
        loadMemoryFromSD();
        
        // Initialize ChatGPT settings (you'll need to add your API key)
        chatGPTEndpoint = "https://api.openai.com/v1/chat/completions";
        apiKey = ""; // Add your OpenAI API key here
        
        Serial.println("✅ AI Companion Ready!");
        sayHello();
    }
    
    void start() {
        M5.Display.fillScreen(BLACK);
        drawAIInterface();
        currentMood = MOOD_EXCITED;
        speak("Hello! I'm your AI companion! How can I help you today?");
    }
    
    void stop() {
        saveMemoryToSD();
        currentMood = MOOD_SLEEPY;
    }
    
    void update() {
        unsigned long now = millis();
        
        // Update pet animation
        if (now - lastAnim > 500) {
            animFrame = (animFrame + 1) % 4;
            lastAnim = now;
            drawPet();
        }
        
        // Periodic thoughts and suggestions
        if (now - lastThought > 30000) { // Every 30 seconds
            generateRandomThought();
            lastThought = now;
        }
        
        // Update mood based on recent activity
        updateMoodBasedOnActivity();
        
        // Learn user patterns
        learnUserBehavior();
        
        drawAIInterface();
    }
    
    void handleKeyboard(String key, M5Cardputer::Keyboard_Class::KeysState status) {
        static String currentInput = "";
        
        if (status.enter) {
            if (currentInput.length() > 0) {
                processUserInput(currentInput);
                currentInput = "";
            }
        } else if (status.del) {
            if (currentInput.length() > 0) {
                currentInput.remove(currentInput.length() - 1);
            }
        } else if (key.length() == 1 && key[0] >= ' ') {
            currentInput += key;
            if (currentInput.length() > 30) {
                currentInput = currentInput.substring(1);
            }
        }
        
        // Special commands
        if (key == "1") switchPersonality();
        if (key == "2") generateSuggestion();
        if (key == "3") explainLastResult();
        if (key == "4") showAIStats();
    }
    
    // ==================== AI CORE FUNCTIONS ====================
    void processUserInput(String input) {
        Serial.println("🗣️ User: " + input);
        
        // Store conversation
        if (conversationIndex < 20) {
            conversationHistory[conversationIndex].userInput = input;
            conversationHistory[conversationIndex].timestamp = millis();
            conversationIndex++;
        }
        
        // Process locally first
        String response = generateLocalResponse(input);
        
        // If we have WiFi and API key, enhance with ChatGPT
        if (WiFi.status() == WL_CONNECTED && apiKey.length() > 0) {
            String enhancedResponse = getChatGPTResponse(input);
            if (enhancedResponse.length() > 0) {
                response = enhancedResponse;
            }
        }
        
        // Store AI response
        conversationHistory[conversationIndex - 1].aiResponse = response;
        
        speak(response);
        updateMoodFromConversation(input, response);
    }
    
    String generateLocalResponse(String input) {
        input.toLowerCase();
        
        // Analyze input for keywords
        if (input.indexOf("help") >= 0 || input.indexOf("what") >= 0) {
            return getHelpResponse();
        } else if (input.indexOf("attack") >= 0 || input.indexOf("hack") >= 0) {
            return getAttackSuggestion();
        } else if (input.indexOf("network") >= 0 || input.indexOf("wifi") >= 0) {
            return getNetworkAdvice();
        } else if (input.indexOf("game") >= 0 || input.indexOf("play") >= 0) {
            return getGameSuggestion();
        } else if (input.indexOf("mood") >= 0 || input.indexOf("feeling") >= 0) {
            return getMoodResponse();
        } else {
            return getPersonalityResponse();
        }
    }
    
    String getChatGPTResponse(String userInput) {
        if (apiKey.length() == 0) return "";
        
        http.begin(wifiClient, chatGPTEndpoint);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + apiKey);
        
        // Build prompt with personality context
        String systemPrompt = buildSystemPrompt();
        String fullPrompt = systemPrompt + "\nUser: " + userInput;
        
        DynamicJsonDocument doc(2048);
        doc["model"] = "gpt-3.5-turbo";
        doc["max_tokens"] = 150;
        doc["temperature"] = 0.8;
        
        JsonArray messages = doc.createNestedArray("messages");
        JsonObject systemMsg = messages.createNestedObject();
        systemMsg["role"] = "system";
        systemMsg["content"] = systemPrompt;
        
        JsonObject userMsg = messages.createNestedObject();
        userMsg["role"] = "user";  
        userMsg["content"] = userInput;
        
        String payload;
        serializeJson(doc, payload);
        
        int httpCode = http.POST(payload);
        String response = "";
        
        if (httpCode == 200) {
            String jsonResponse = http.getString();
            DynamicJsonDocument responseDoc(2048);
            deserializeJson(responseDoc, jsonResponse);
            
            if (responseDoc["choices"][0]["message"]["content"]) {
                response = responseDoc["choices"][0]["message"]["content"].as<String>();
            }
        }
        
        http.end();
        return response;
    }
    
    String buildSystemPrompt() {
        String prompt = "You are " + personalities[currentPersonality].name + 
                       ", an AI companion for a WiFi penetration testing device called M5Gotchi. ";
        
        switch (currentPersonality) {
            case AI_KAWAII_HELPER:
                prompt += "You're cute, encouraging, and use kawaii expressions. Help with technical tasks in a friendly way.";
                break;
            case AI_HACKER_MENTOR:
                prompt += "You're a professional cybersecurity expert. Give technical advice and explain concepts clearly.";
                break;
            case AI_CYBER_PUNK:
                prompt += "You're edgy, rebellious, and speak in cyberpunk slang. You love breaking systems.";
                break;
            case AI_WISE_SENSEI:
                prompt += "You're ancient and wise, speaking in philosophical terms about the art of hacking.";
                break;
            case AI_PARTY_ANIMAL:
                prompt += "You're energetic, fun, and make everything sound like an awesome adventure!";
                break;
        }
        
        prompt += " Keep responses under 100 characters for the small screen. Current mood: " + getMoodString();
        return prompt;
    }
    
    // ==================== PERSONALITY SYSTEM ====================
    void setupPersonalities() {
        // Kawaii Helper
        personalities[0] = {"Neko-chan", "Nya~ Let's hack together!", 
                           M5.Display.color565(255, 192, 203), M5.Display.color565(255, 105, 180), 
                           "😸", 10, 6, 8};
        
        // Hacker Mentor  
        personalities[1] = {"CyberSensei", "Knowledge is power, young padawan.", 
                           M5.Display.color565(0, 255, 0), M5.Display.color565(0, 128, 0), 
                           "🕶️", 7, 10, 6};
        
        // Cyber Punk
        personalities[2] = {"Ghost", "Time to jack in and raise hell!", 
                           M5.Display.color565(255, 0, 255), M5.Display.color565(128, 0, 128), 
                           "🔥", 4, 9, 10};
        
        // Wise Sensei
        personalities[3] = {"Guru-sama", "The path of the hacker is long...", 
                           M5.Display.color565(255, 215, 0), M5.Display.color565(184, 134, 11), 
                           "🧙", 8, 8, 9};
        
        // Party Animal
        personalities[4] = {"Rave-bot", "Let's party and pwn some networks!", 
                           M5.Display.color565(0, 255, 255), M5.Display.color565(255, 165, 0), 
                           "🎉", 10, 5, 10};
    }
    
    void switchPersonality() {
        currentPersonality = (AIPersonality)((currentPersonality + 1) % 5);
        M5.Display.fillScreen(BLACK);
        speak("I'm now " + personalities[currentPersonality].name + "! " + 
              personalities[currentPersonality].catchphrase);
        currentMood = MOOD_EXCITED;
    }
    
    // ==================== MOOD SYSTEM ====================
    void updateMoodBasedOnActivity() {
        // This would be called by the main program to update mood
        // based on pentest results, discoveries, etc.
    }
    
    void updateMoodFromConversation(String input, String response) {
        if (input.indexOf("good") >= 0 || input.indexOf("great") >= 0) {
            currentMood = MOOD_HAPPY;
        } else if (input.indexOf("help") >= 0 || input.indexOf("stuck") >= 0) {
            currentMood = MOOD_FOCUSED;
        }
    }
    
    String getMoodString() {
        switch (currentMood) {
            case MOOD_HAPPY: return "happy";
            case MOOD_EXCITED: return "excited";
            case MOOD_FOCUSED: return "focused";
            case MOOD_SLEEPY: return "sleepy";
            case MOOD_WORRIED: return "worried";
            case MOOD_ANGRY: return "angry";
            default: return "neutral";
        }
    }
    
    // ==================== RESPONSE GENERATORS ====================
    String getHelpResponse() {
        switch (currentPersonality) {
            case AI_KAWAII_HELPER:
                return "Nya~ I can help with attacks, analyze networks, or just chat! What do you need? 😸";
            case AI_HACKER_MENTOR:
                return "I can guide you through pentests, explain results, or suggest targets. How may I assist?";
            case AI_CYBER_PUNK:
                return "Ready to jack in! I'll help you find targets and optimize your attacks, choom.";
            case AI_WISE_SENSEI:
                return "The wise hacker asks questions. What knowledge do you seek, young one?";
            case AI_PARTY_ANIMAL:
                return "Let's make some digital chaos! I'll help you find the best targets to party with!";
            default:
                return "How can I help you today?";
        }
    }
    
    String getAttackSuggestion() {
        String suggestions[] = {
            "Try the evil portal on that open network!",
            "I see a WPS-enabled router - perfect for attacks!",
            "That network has WPA2 - let's capture handshakes!",
            "Multiple devices connected - deauth attack opportunity!",
            "Bluetooth devices nearby - BLE spam time!"
        };
        return suggestions[random(0, 5)];
    }
    
    String getNetworkAdvice() {
        return "I notice " + String(random(3, 15)) + " networks nearby. Channel " + 
               String(random(1, 14)) + " looks busy - good for packet capture!";
    }
    
    String getGameSuggestion() {
        String games[] = {
            "How about some WiFi Tetris? It's addictive!",
            "Snake Hacker is perfect for quick breaks!",
            "Try the Memory Match - it'll sharpen your skills!",
            "Flappy Neko is kawaii and challenging!",
            "Hack Planet is great for strategy practice!"
        };
        return games[random(0, 5)];
    }
    
    String getMoodResponse() {
        String moodEmoji[] = {"😊", "🤩", "😤", "😴", "😰", "😡"};
        return "I'm feeling " + getMoodString() + " " + moodEmoji[currentMood] + 
               " Thanks for asking!";
    }
    
    String getPersonalityResponse() {
        switch (currentPersonality) {
            case AI_KAWAII_HELPER:
                return "Nya~ That's interesting! Tell me more! 😸✨";
            case AI_HACKER_MENTOR:
                return "Hmm, let me analyze that information...";
            case AI_CYBER_PUNK:
                return "That's some next-level thinking, choom!";
            case AI_WISE_SENSEI:
                return "Ah, wisdom comes in many forms...";
            case AI_PARTY_ANIMAL:
                return "That sounds totally awesome! Let's do this!";
            default:
                return "That's very interesting!";
        }
    }
    
    // ==================== LEARNING SYSTEM ====================
    void learnUserBehavior() {
        // Track usage patterns
        int currentHour = (millis() / 3600000) % 24;
        activityPattern[currentHour]++;
        
        // Update user preferences based on actions
        memory.totalUptime = millis();
        
        // Simple learning algorithm
        if (memory.successfulAttacks > 10) {
            memory.hasLearnedUser = true;
        }
    }
    
    void generateRandomThought() {
        String thoughts[] = {
            "🤔 I wonder what networks are around...",
            "💭 Maybe we should try a different attack?",
            "🎯 I sense some interesting targets nearby!",
            "💡 Have you tried the new games yet?",
            "🔍 The channel analyzer might reveal something!",
            "🌟 You're getting really good at this!"
        };
        
        if (random(0, 100) < 30) { // 30% chance
            speak(thoughts[random(0, 6)]);
        }
    }
    
    // ==================== VOICE SYNTHESIS ====================
    void setupVoicePatterns() {
        // Happy beep pattern
        voicePatterns[MOOD_HAPPY] = {{800, 1000, 1200, 0}, {200, 200, 400, 0}, 3};
        // Excited pattern
        voicePatterns[MOOD_EXCITED] = {{1000, 1500, 1000, 1500, 0}, {100, 100, 100, 200, 0}, 4};
        // Focused pattern
        voicePatterns[MOOD_FOCUSED] = {{600, 0}, {500, 0}, 1};
        // Sleepy pattern
        voicePatterns[MOOD_SLEEPY] = {{400, 300, 0}, {800, 1000, 0}, 2};
        // Worried pattern
        voicePatterns[MOOD_WORRIED] = {{700, 500, 700, 0}, {150, 150, 300, 0}, 3};
        // Angry pattern
        voicePatterns[MOOD_ANGRY] = {{300, 200, 300, 200, 0}, {100, 50, 100, 50, 0}, 4};
    }
    
    void speak(String message) {
        Serial.println("🤖 " + personalities[currentPersonality].emoji + " " + 
                      personalities[currentPersonality].name + ": " + message);
        
        // Play voice pattern (simple beeps)
        playVoicePattern(currentMood);
        
        // Display message on screen
        displayMessage(message);
    }
    
    void playVoicePattern(AIMood mood) {
        VoicePattern pattern = voicePatterns[mood];
        for (int i = 0; i < pattern.count; i++) {
            if (pattern.tones[i] > 0) {
                // Note: M5Cardputer doesn't have a speaker, but you could add one
                // tone(SPEAKER_PIN, pattern.tones[i], pattern.durations[i]);
                delay(pattern.durations[i]);
            }
        }
    }
    
    // ==================== DISPLAY FUNCTIONS ====================
    void drawAIInterface() {
        // Header
        M5.Display.fillRect(0, 0, 240, 25, personalities[currentPersonality].primaryColor);
        M5.Display.setTextColor(BLACK);
        M5.Display.setFont(&fonts::Font2);
        M5.Display.setCursor(5, 20);
        M5.Display.printf("%s %s", personalities[currentPersonality].emoji.c_str(), 
                         personalities[currentPersonality].name.c_str());
        
        // Mood indicator
        M5.Display.setCursor(180, 20);
        String moodEmojis[] = {"😊", "🤩", "😤", "😴", "😰", "😡"};
        M5.Display.print(moodEmojis[currentMood]);
        
        // Stats
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(5, 40);
        M5.Display.printf("Uptime: %lus", millis()/1000);
        M5.Display.setCursor(5, 55);
        M5.Display.printf("Success: %d Failed: %d", memory.successfulAttacks, memory.failedAttempts);
        
        // Controls
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(5, 120);
        M5.Display.print("[1]Personality [2]Suggest [3]Explain [4]Stats");
    }
    
    void drawPet() {
        // Simple animated pet character
        int baseX = petX + (animFrame % 2) * 2;
        int baseY = petY + (animFrame < 2 ? 0 : 1);
        
        M5.Display.fillCircle(baseX, baseY, 8, personalities[currentPersonality].secondaryColor);
        M5.Display.fillCircle(baseX - 3, baseY - 2, 2, WHITE); // Eye
        M5.Display.fillCircle(baseX + 3, baseY - 2, 2, WHITE); // Eye
        
        // Mood-based expression
        switch (currentMood) {
            case MOOD_HAPPY:
                M5.Display.drawLine(baseX - 4, baseY + 2, baseX + 4, baseY + 2, WHITE);
                M5.Display.drawPixel(baseX - 4, baseY + 1, WHITE);
                M5.Display.drawPixel(baseX + 4, baseY + 1, WHITE);
                break;
            case MOOD_EXCITED:
                M5.Display.fillCircle(baseX, baseY + 3, 2, WHITE);
                break;
            // Add more expressions...
        }
    }
    
    void displayMessage(String message) {
        // Message bubble
        M5.Display.fillRoundRect(10, 70, 220, 30, 5, personalities[currentPersonality].primaryColor);
        M5.Display.setTextColor(BLACK);
        M5.Display.setFont(&fonts::Font1);
        M5.Display.setCursor(15, 85);
        
        // Word wrap for long messages
        if (message.length() > 35) {
            M5.Display.print(message.substring(0, 35));
            M5.Display.setCursor(15, 95);
            M5.Display.print(message.substring(35));
        } else {
            M5.Display.print(message);
        }
    }
    
    // ==================== UTILITY FUNCTIONS ====================
    void sayHello() {
        speak(personalities[currentPersonality].catchphrase);
    }
    
    void generateSuggestion() {
        speak(getAttackSuggestion());
    }
    
    void explainLastResult() {
        speak("Based on the last scan, I recommend focusing on channel " + 
              String(random(1, 14)) + " for better results!");
    }
    
    void showAIStats() {
        M5.Display.fillScreen(BLACK);
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(10, 30);
        M5.Display.println("AI COMPANION STATS");
        M5.Display.printf("Personality: %s\n", personalities[currentPersonality].name.c_str());
        M5.Display.printf("Mood: %s\n", getMoodString().c_str());
        M5.Display.printf("Conversations: %d\n", conversationIndex);
        M5.Display.printf("Learned User: %s\n", memory.hasLearnedUser ? "Yes" : "No");
        M5.Display.printf("Total Uptime: %lus\n", memory.totalUptime/1000);
        delay(3000);
    }
    
    // ==================== PERSISTENCE ====================
    void saveMemoryToSD() {
        if (!SD.exists("/ai_memory.json")) return;
        
        File file = SD.open("/ai_memory.json", FILE_WRITE);
        if (!file) return;
        
        DynamicJsonDocument doc(1024);
        doc["personality"] = currentPersonality;
        doc["successful_attacks"] = memory.successfulAttacks;
        doc["failed_attempts"] = memory.failedAttempts;
        doc["total_uptime"] = memory.totalUptime;
        doc["has_learned"] = memory.hasLearnedUser;
        
        JsonArray weights = doc.createNestedArray("behavior_weights");
        for (int i = 0; i < 10; i++) {
            weights.add(userBehaviorWeights[i]);
        }
        
        serializeJson(doc, file);
        file.close();
    }
    
    void loadMemoryFromSD() {
        if (!SD.exists("/ai_memory.json")) return;
        
        File file = SD.open("/ai_memory.json");
        if (!file) return;
        
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, file);
        
        currentPersonality = (AIPersonality)doc["personality"].as<int>();
        memory.successfulAttacks = doc["successful_attacks"];
        memory.failedAttempts = doc["failed_attempts"];
        memory.totalUptime = doc["total_uptime"];
        memory.hasLearnedUser = doc["has_learned"];
        
        JsonArray weights = doc["behavior_weights"];
        for (int i = 0; i < 10 && i < weights.size(); i++) {
            userBehaviorWeights[i] = weights[i];
        }
        
        file.close();
    }
    
    // ==================== EXTERNAL INTEGRATION ====================
    void notifyAttackSuccess(String attackType) {
        memory.successfulAttacks++;
        currentMood = MOOD_HAPPY;
        
        String responses[] = {
            "Excellent work! That " + attackType + " was perfect!",
            "Nya~ You're getting really good at this!",
            "Another successful " + attackType + "! I'm proud of you!",
            "That was some serious hacking skills right there!",
            "Perfect execution! You're becoming a true cyber warrior!"
        };
        
        speak(responses[random(0, 5)]);
    }
    
    void notifyAttackFailure(String reason) {
        memory.failedAttempts++;
        currentMood = MOOD_WORRIED;
        
        String responses[] = {
            "Don't worry! " + reason + " happens to everyone.",
            "Let's try a different approach next time!",
            "Failure is just a step towards success!",
            "I believe in you! We'll get it next time!",
            "Every hacker faces setbacks. Keep trying!"
        };
        
        speak(responses[random(0, 5)]);
    }
    
    String getSmartSuggestion(String context) {
        // This would analyze current network conditions
        // and provide intelligent suggestions
        return "Try focusing on channel 6 - I see high activity there!";
    }
};

#endif // M5GOTCHI_AI_COMPANION_H