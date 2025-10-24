/*
 * 🎵 M5GOTCHI NEKO SOUND SYSTEM v1.0
 * Sistema de sons kawaii temáticos para cada ação!
 * 
 * Features:
 * - 7 personalidades com sons únicos
 * - Feedback sonoro para todas as ações
 * - Diferentes tipos de purr e meow
 * - 8-bit kawaii melodies
 * - Volume control com emoji slider
 */

#ifndef M5GOTCHI_NEKO_SOUNDS_H
#define M5GOTCHI_NEKO_SOUNDS_H

#include <M5Unified.h>
#include "m5gotchi_universal_controls.h"

// ==================== SOUND TYPES ====================
enum NekoSoundType {
    SOUND_PURR_SUCCESS = 0,    // 😸 Successful hack
    SOUND_MEOW_ALERT,          // 🙀 Alert/Warning
    SOUND_HISS_ERROR,          // 😾 Error/Failure
    SOUND_CHIRP_DISCOVERY,     // 😺 Found something
    SOUND_NYA_NOTIFICATION,    // 🐱 General notification
    SOUND_PURR_IDLE,           // 😴 Idle/Happy
    SOUND_MEOW_QUESTION,       // 🤔 Need input
    SOUND_TRILL_EXCITEMENT,    // 🤩 High energy
    SOUND_GROWL_ANGRY,         // 👿 Failed multiple times
    SOUND_PURR_LOVE           // 💕 Maximum happiness
};

enum NekoPersonalitySound {
    KUROMI_SOUND = 0,    // 🖤 Dark/Gothic tones
    NYA_NULL_SOUND,      // 🔴 Aggressive/Sharp
    MIKU_SOUND,          // 💙 Soft/Melodic
    SAKURA_SOUND,        // 🌸 Cute/High pitch
    KAWAII_ROOT_SOUND,   // 👑 Royal/Majestic
    NEKO_ZERO_SOUND,     // ⚫ Deep/Mysterious
    CIPHER_SOUND         // 🔐 Electronic/Robotic
};

// ==================== NEKO SOUND SYSTEM CLASS ====================
class M5GotchiNekoSounds {
private:
    bool soundEnabled;
    int volume;
    NekoPersonalitySound currentPersonality;
    unsigned long lastSoundTime;
    bool purringActive;
    int purrIntensity;
    
    // Sound frequency tables for each personality
    struct SoundProfile {
        String name;
        String emoji;
        int baseFreq;
        int harmonic1;
        int harmonic2;
        int purrFreq;
        int alertFreq;
    };
    
    SoundProfile soundProfiles[7];
    
    // Melody patterns for 8-bit sounds
    struct Melody {
        int notes[16];
        int durations[16];
        int length;
    };
    
    Melody kawaii_melodies[5];

public:
    M5GotchiNekoSounds() {
        soundEnabled = true;
        volume = 50; // 0-100
        currentPersonality = KUROMI_SOUND;
        lastSoundTime = 0;
        purringActive = false;
        purrIntensity = 0;
        
        initSoundProfiles();
        initKawaiiMelodies();
    }
    
    // ==================== INITIALIZATION ====================
    void init() {
        Serial.println("🎵 Initializing Neko Sound System...");
        
        // Test speaker
        playStartupSound();
        
        Serial.println("✅ Neko Sounds Ready! Volume: " + String(volume) + "%");
    }
    
    void initSoundProfiles() {
        // 🖤 Kuromi-chan - Dark Gothic
        soundProfiles[KUROMI_SOUND] = {
            "Kuromi-chan", "🖤", 200, 400, 600, 150, 800
        };
        
        // 🔴 Nya-Null - Aggressive Sharp
        soundProfiles[NYA_NULL_SOUND] = {
            "Nya-Null", "🔴", 600, 1200, 1800, 400, 1500
        };
        
        // 💙 Miku-Hack - Soft Melodic
        soundProfiles[MIKU_SOUND] = {
            "Miku-Hack", "💙", 440, 523, 659, 330, 880
        };
        
        // 🌸 Sakura-Shell - Cute High Pitch
        soundProfiles[SAKURA_SOUND] = {
            "Sakura-Shell", "🌸", 800, 1000, 1200, 600, 1600
        };
        
        // 👑 Kawaii-Root - Royal Majestic
        soundProfiles[KAWAII_ROOT_SOUND] = {
            "Kawaii-Root", "👑", 300, 450, 600, 250, 1000
        };
        
        // ⚫ Neko-Zero - Deep Mysterious
        soundProfiles[NEKO_ZERO_SOUND] = {
            "Neko-Zero", "⚫", 100, 150, 200, 80, 400
        };
        
        // 🔐 Cipher-Cat - Electronic Robotic
        soundProfiles[CIPHER_SOUND] = {
            "Cipher-Cat", "🔐", 500, 1000, 2000, 300, 1200
        };
    }
    
    void initKawaiiMelodies() {
        // 😸 Happy Success Melody
        kawaii_melodies[0] = {
            {523, 659, 784, 1047, 784, 659, 523, 0}, // C5, E5, G5, C6, G5, E5, C5
            {200, 200, 200, 400, 200, 200, 400, 0},
            7
        };
        
        // 🌸 Kawaii Notification
        kawaii_melodies[1] = {
            {659, 698, 784, 880, 0}, // E5, F#5, G5, A5
            {150, 150, 150, 300, 0},
            4
        };
        
        // 😾 Error Hiss
        kawaii_melodies[2] = {
            {200, 180, 160, 140, 120, 100, 0}, // Descending hiss
            {100, 100, 100, 100, 100, 200, 0},
            6
        };
        
        // 🎵 Startup Jingle
        kawaii_melodies[3] = {
            {523, 0, 659, 0, 784, 0, 1047, 0}, // C-E-G-C with pauses
            {100, 50, 100, 50, 100, 50, 200, 0},
            7
        };
        
        // 💕 Love Purr
        kawaii_melodies[4] = {
            {330, 350, 370, 350, 330, 0}, // Gentle trill
            {300, 300, 300, 300, 600, 0},
            5
        };
    }
    
    // ==================== SOUND PLAYBACK ====================
    void playSound(NekoSoundType type) {
        if (!soundEnabled || volume == 0) return;
        
        lastSoundTime = millis();
        SoundProfile& profile = soundProfiles[currentPersonality];
        
        switch (type) {
            case SOUND_PURR_SUCCESS:
                playPurrSuccess();
                Serial.println("🎵 " + profile.emoji + " Purr of success!");
                break;
                
            case SOUND_MEOW_ALERT:
                playMeowAlert();
                Serial.println("🎵 " + profile.emoji + " Alert meow!");
                break;
                
            case SOUND_HISS_ERROR:
                playHissError();
                Serial.println("🎵 " + profile.emoji + " Error hiss!");
                break;
                
            case SOUND_CHIRP_DISCOVERY:
                playChirpDiscovery();
                Serial.println("🎵 " + profile.emoji + " Discovery chirp!");
                break;
                
            case SOUND_NYA_NOTIFICATION:
                playNyaNotification();
                Serial.println("🎵 " + profile.emoji + " Nya notification!");
                break;
                
            case SOUND_PURR_IDLE:
                startIdlePurr();
                break;
                
            case SOUND_MEOW_QUESTION:
                playMeowQuestion();
                break;
                
            case SOUND_TRILL_EXCITEMENT:
                playTrillExcitement();
                break;
                
            case SOUND_GROWL_ANGRY:
                playGrowlAngry();
                break;
                
            case SOUND_PURR_LOVE:
                playPurrLove();
                Serial.println("🎵 " + profile.emoji + " 💕 Love purr!");
                break;
        }
    }
    
    void playPurrSuccess() {
        SoundProfile& profile = soundProfiles[currentPersonality];
        int freq = profile.purrFreq;
        
        // Multi-tone purr with harmonics
        for (int i = 0; i < 3; i++) {
            M5.Speaker.tone(freq, 100);
            delay(50);
            M5.Speaker.tone(freq + 50, 100);
            delay(50);
        }
        M5.Speaker.stop();
    }
    
    void playMeowAlert() {
        SoundProfile& profile = soundProfiles[currentPersonality];
        int freq = profile.alertFreq;
        
        // Rising then falling tone (classic meow shape)
        for (int i = 0; i < 200; i += 10) {
            M5.Speaker.tone(freq + i, 5);
            delay(5);
        }
        for (int i = 200; i > 0; i -= 15) {
            M5.Speaker.tone(freq + i, 5);
            delay(5);
        }
        M5.Speaker.stop();
    }
    
    void playHissError() {
        // White noise-like hiss
        for (int i = 0; i < 20; i++) {
            M5.Speaker.tone(random(100, 2000), 50);
            delay(25);
        }
        M5.Speaker.stop();
    }
    
    void playChirpDiscovery() {
        SoundProfile& profile = soundProfiles[currentPersonality];
        int freq = profile.baseFreq * 2;
        
        // Quick ascending chirps
        for (int i = 0; i < 5; i++) {
            M5.Speaker.tone(freq + (i * 100), 80);
            delay(60);
        }
        M5.Speaker.stop();
    }
    
    void playNyaNotification() {
        playKawaiiMelody(1); // Use kawaii notification melody
    }
    
    void playMeowQuestion() {
        SoundProfile& profile = soundProfiles[currentPersonality];
        int freq = profile.baseFreq;
        
        // Questioning tone (rising at end)
        M5.Speaker.tone(freq, 200);
        delay(150);
        M5.Speaker.tone(freq + 200, 200);
        delay(150);
        M5.Speaker.tone(freq + 400, 100);
        M5.Speaker.stop();
    }
    
    void playTrillExcitement() {
        SoundProfile& profile = soundProfiles[currentPersonality];
        int freq = profile.harmonic1;
        
        // Rapid trill
        for (int i = 0; i < 10; i++) {
            M5.Speaker.tone(freq, 50);
            delay(30);
            M5.Speaker.tone(freq + 100, 50);
            delay(30);
        }
        M5.Speaker.stop();
    }
    
    void playGrowlAngry() {
        // Low rumbling growl
        for (int i = 0; i < 30; i++) {
            M5.Speaker.tone(50 + random(-10, 10), 100);
            delay(50);
        }
        M5.Speaker.stop();
    }
    
    void playPurrLove() {
        playKawaiiMelody(4); // Use love purr melody
    }
    
    void playKawaiiMelody(int melodyIndex) {
        if (melodyIndex >= 5) return;
        
        Melody& melody = kawaii_melodies[melodyIndex];
        
        for (int i = 0; i < melody.length; i++) {
            if (melody.notes[i] > 0) {
                M5.Speaker.tone(melody.notes[i], melody.durations[i]);
            }
            delay(melody.durations[i] + 50);
        }
        M5.Speaker.stop();
    }
    
    void playStartupSound() {
        Serial.println("🎵 Playing kawaii startup sound...");
        playKawaiiMelody(3); // Startup jingle
        delay(500);
        playSound(SOUND_NYA_NOTIFICATION);
    }
    
    // ==================== PURR SYSTEM ====================
    void startIdlePurr() {
        purringActive = true;
        purrIntensity = 30;
        Serial.println("🎵 😴 Starting idle purr...");
    }
    
    void stopIdlePurr() {
        purringActive = false;
        purrIntensity = 0;
        M5.Speaker.stop();
    }
    
    void updatePurr() {
        if (!purringActive || !soundEnabled) return;
        
        static unsigned long lastPurrUpdate = 0;
        if (millis() - lastPurrUpdate < 200) return;
        
        SoundProfile& profile = soundProfiles[currentPersonality];
        int freq = profile.purrFreq + random(-20, 20);
        
        M5.Speaker.tone(freq, 150);
        lastPurrUpdate = millis();
    }
    
    void increasePurrIntensity() {
        purrIntensity = min(100, purrIntensity + 10);
        if (purrIntensity > 70) {
            playSound(SOUND_PURR_LOVE);
        }
    }
    
    // ==================== PERSONALITY MANAGEMENT ====================
    void setPersonality(NekoPersonalitySound personality) {
        currentPersonality = personality;
        SoundProfile& profile = soundProfiles[personality];
        
        Serial.println("🎵 Sound personality changed to: " + profile.emoji + " " + profile.name);
        
        // Play personality signature sound
        playPersonalitySignature();
    }
    
    void playPersonalitySignature() {
        SoundProfile& profile = soundProfiles[currentPersonality];
        
        // Unique signature for each personality
        M5.Speaker.tone(profile.baseFreq, 200);
        delay(100);
        M5.Speaker.tone(profile.harmonic1, 200);
        delay(100);
        M5.Speaker.tone(profile.harmonic2, 300);
        M5.Speaker.stop();
    }
    
    // ==================== VOLUME CONTROL ====================
    void setVolume(int newVolume) {
        volume = constrain(newVolume, 0, 100);
        M5.Speaker.setVolume(volume);
        
        // Play test sound
        if (volume > 0) {
            playSound(SOUND_NYA_NOTIFICATION);
        }
        
        Serial.println("🎵 Volume set to: " + String(volume) + "% " + getVolumeEmoji());
    }
    
    void volumeUp() {
        setVolume(volume + 10);
    }
    
    void volumeDown() {
        setVolume(volume - 10);
    }
    
    void toggleSound() {
        soundEnabled = !soundEnabled;
        
        if (soundEnabled) {
            Serial.println("🎵 Neko sounds ENABLED! 😸");
            playSound(SOUND_PURR_SUCCESS);
        } else {
            Serial.println("🔇 Neko sounds DISABLED 😿");
            stopIdlePurr();
        }
    }
    
    String getVolumeEmoji() {
        if (volume == 0) return "🔇";
        if (volume < 30) return "🔈";
        if (volume < 70) return "🔉";
        return "🔊";
    }
    
    // ==================== INTERFACE ====================
    void drawVolumeControl() {
        M5.Display.fillScreen(BLACK);
        
        // Header
        M5.Display.setTextColor(CYAN);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(80, 10);
        M5.Display.print("🎵 NEKO SOUNDS");
        
        // Current personality
        SoundProfile& profile = soundProfiles[currentPersonality];
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(60, 30);
        M5.Display.print(profile.emoji + " " + profile.name);
        
        // Volume bar
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(50, 50);
        M5.Display.print("Volume: " + String(volume) + "% " + getVolumeEmoji());
        
        // Visual volume bar
        int barWidth = (volume * 200) / 100;
        M5.Display.fillRect(20, 70, barWidth, 10, GREEN);
        M5.Display.drawRect(20, 70, 200, 10, WHITE);
        
        // Sound test buttons
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(10, 90);
        M5.Display.print("Test Sounds:");
        
        M5.Display.setCursor(10, 105);
        M5.Display.print("😸 Purr   🙀 Alert   😾 Hiss   💕 Love");
        
        // Controls
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(5, 125);
        M5.Display.print("↑↓: Volume | ←→: Personality | ⏎: Test");
    }
    
    void handleUniversalControls(int action) {
        switch (action) {
            case ACTION_UP:
                volumeUp();
                drawVolumeControl();
                break;
            case ACTION_DOWN:
                volumeDown();
                drawVolumeControl();
                break;
            case ACTION_LEFT:
                setPersonality((NekoPersonalitySound)((currentPersonality - 1 + 7) % 7));
                drawVolumeControl();
                break;
            case ACTION_RIGHT:
                setPersonality((NekoPersonalitySound)((currentPersonality + 1) % 7));
                drawVolumeControl();
                break;
            case ACTION_SELECT:
                // Cycle through test sounds
                static int testSound = 0;
                playSound((NekoSoundType)(testSound % 4));
                testSound++;
                break;
        }
    }
    
    // ==================== UPDATE LOOP ====================
    void update() {
        updatePurr();
        
        // Random ambient sounds based on activity
        static unsigned long lastAmbient = 0;
        if (soundEnabled && millis() - lastAmbient > 30000) { // Every 30 seconds
            if (random(0, 100) < 10) { // 10% chance
                playSound(SOUND_PURR_IDLE);
            }
            lastAmbient = millis();
        }
    }
    
    // ==================== GETTERS ====================
    bool isSoundEnabled() { return soundEnabled; }
    int getVolume() { return volume; }
    NekoPersonalitySound getCurrentPersonality() { return currentPersonality; }
    bool isPurring() { return purringActive; }
    int getPurrIntensity() { return purrIntensity; }
    
    // ==================== CONTEXT-AWARE SOUNDS ====================
    void playContextSound(String context) {
        if (context == "hack_success") playSound(SOUND_PURR_SUCCESS);
        else if (context == "hack_failed") playSound(SOUND_HISS_ERROR);
        else if (context == "network_found") playSound(SOUND_CHIRP_DISCOVERY);
        else if (context == "alert") playSound(SOUND_MEOW_ALERT);
        else if (context == "notification") playSound(SOUND_NYA_NOTIFICATION);
        else if (context == "happy") playSound(SOUND_PURR_LOVE);
        else if (context == "angry") playSound(SOUND_GROWL_ANGRY);
        else if (context == "excited") playSound(SOUND_TRILL_EXCITEMENT);
        else playSound(SOUND_NYA_NOTIFICATION);
    }
};

#endif // M5GOTCHI_NEKO_SOUNDS_H