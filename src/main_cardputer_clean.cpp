/*
 * M5Gotchi PRO - Kawaii Pet Edition
 * Nova versão com:
 * - Gatinho grande com status visível
 * - Cores kawaii bonitas
 * - Animação suave do pet
 * - Controles funcionando 100%
 */

#include <M5Unified.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <Preferences.h>  // For persistent storage
#include <cstdio>          // For snprintf

// ==================== CONFIGURAÇÕES ====================
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

// Paletas editáveis (RGB565)
const uint16_t BG_COLOR_OPTIONS[]      = {0x0000, 0x4208, 0x001F, 0x5AEB, 0x7BEF};
const char*    BG_COLOR_NAMES[]        = {"Black", "Teal", "Deep Blue", "Violet", "Gray"};
const uint16_t NEKO_COLOR_OPTIONS[]    = {0x0000, 0x07E0, 0xFFFF, 0xF800, 0xFFE0};
const char*    NEKO_COLOR_NAMES[]      = {"Black", "Matrix", "White", "Red", "Yellow"};
const uint16_t HIGHLIGHT_COLOR_OPTIONS[] = {0x07FF, 0xFFE0, 0xF81F, 0xFD20, 0x07E0};
const char*    HIGHLIGHT_COLOR_NAMES[] = {"Cyan", "Lime", "Magenta", "Orange", "Green"};
const uint16_t TEXT_COLOR_OPTIONS[]    = {0x2DE6, 0xFFFF, 0xC618, 0xFD20, 0x07FF};
const char*    TEXT_COLOR_NAMES[]      = {"Mint", "White", "Silver", "Amber", "Cyan"};

const int BG_COLOR_COUNT       = sizeof(BG_COLOR_OPTIONS) / sizeof(uint16_t);
const int NEKO_COLOR_COUNT     = sizeof(NEKO_COLOR_OPTIONS) / sizeof(uint16_t);
const int HIGHLIGHT_COLOR_COUNT= sizeof(HIGHLIGHT_COLOR_OPTIONS) / sizeof(uint16_t);
const int TEXT_COLOR_COUNT     = sizeof(TEXT_COLOR_OPTIONS) / sizeof(uint16_t);

const int BG_COLOR_DEFAULT_INDEX        = 1; // Teal
const int NEKO_COLOR_DEFAULT_INDEX      = 0; // Black
const int HIGHLIGHT_COLOR_DEFAULT_INDEX = 0; // Cyan
const int TEXT_COLOR_DEFAULT_INDEX      = 0; // Mint
#define COLOR_STAT_GOOD 0x07E0 // Verde brilhante
#define COLOR_STAT_OK 0x7FE0   // Verde médio
#define COLOR_STAT_BAD 0xF800  // Vermelho (alerta)
#define COLOR_PANEL 0x1082     // Verde escuro
#define COLOR_SELECT 0x2DE6    // Verde claro (seleção)

// ==================== PÁGINAS ====================
enum Page {
    PAGE_HOME = 0,
    PAGE_MENU,
    PAGE_WIFI_SCANNER,
    PAGE_BLUETOOTH,
    PAGE_TOOLS,
    PAGE_SETTINGS,
    PAGE_COUNT
};

// ==================== MOOD SYSTEM ====================
enum MoodState {
    MOOD_EXCITED = 0,   // All stats > 85% + high energy
    MOOD_HAPPY,         // All stats > 70%
    MOOD_PLAYFUL,       // Happiness > 80%, energy > 60%
    MOOD_CONTENT,       // All stats > 50%
    MOOD_SLEEPY,        // Energy < 40%
    MOOD_HUNGRY,        // Hunger < 40%
    MOOD_NEUTRAL,       // Stats between 30-50%
    MOOD_BORED,         // Happiness < 40%, other stats ok
    MOOD_GRUMPY,        // Multiple stats 20-30%
    MOOD_SAD,           // Any stat < 30%
    MOOD_ANGRY,         // Any stat < 20%
    MOOD_CRITICAL       // Any stat < 10%
};

// ==================== VARIÁVEIS GLOBAIS ====================
int bgColorIndex        = BG_COLOR_DEFAULT_INDEX;
int nekoColorIndex      = NEKO_COLOR_DEFAULT_INDEX;
int highlightColorIndex = HIGHLIGHT_COLOR_DEFAULT_INDEX;
int textColorIndex      = TEXT_COLOR_DEFAULT_INDEX;

uint16_t colorBg        = BG_COLOR_OPTIONS[BG_COLOR_DEFAULT_INDEX];
uint16_t colorNeko      = NEKO_COLOR_OPTIONS[NEKO_COLOR_DEFAULT_INDEX];
uint16_t colorHighlight = HIGHLIGHT_COLOR_OPTIONS[HIGHLIGHT_COLOR_DEFAULT_INDEX];
uint16_t colorText      = TEXT_COLOR_OPTIONS[TEXT_COLOR_DEFAULT_INDEX];

#define COLOR_BG colorBg
#define COLOR_NEKO colorNeko
#define COLOR_HIGHLIGHT colorHighlight
#define COLOR_TEXT colorText

Page currentPage = PAGE_HOME;
int menuSelection = 0;
int maxMenuItems = 6;  // Aumentado para 6 (adicionando Test Mode)
int bluetoothSelection = 0;  // For Bluetooth menu
int toolsSelection = 0;       // For Tools menu
bool needsRedraw = true;
unsigned long lastStatusUpdate = 0;
unsigned long lastSerialHeartbeat = 0;  // For serial debug
unsigned long lastAlertBlink = 0;       // For blinking alerts
bool alertVisible = true;               // Alert blink state
int settingsSelection = 0;

// Pet Status
int hunger = 100;     // 0-100 (começa cheio)
int happiness = 100;  // 0-100 (começa feliz)
int energy = 100;     // 0-100 (começa energizado)
int level = 0;        // Começa no level 0
int experience = 0;   // Começa com 0 XP
String petName = "NEKO";  // Nome editável do pet

const unsigned long MOOD_MESSAGE_INTERVAL = 6000; // 6s per message
String currentMoodMessage = "";
MoodState lastMoodForMessage = MOOD_NEUTRAL;
unsigned long lastMoodMessageUpdate = 0;

// Tamagotchi Features
int weight = 10;      // Peso em kg (10-30)
int discipline = 0;   // 0-100 (treinamento)
int hygiene = 100;    // 0-100 (limpeza)
bool isSick = false;  // Estado de doença
int poopCount = 0;    // Quantidade de cocôs na tela
unsigned long lastPoopTime = 0;
bool needsAttention = false;  // Pet chamando atenção
unsigned long lastCallTime = 0;

// Uptime
unsigned long sessionStartTime = 0;
uint64_t petAgeSeconds = 0;
unsigned long lastAgeTickMillis = 0;

// WiFi
std::vector<String> wifiNetworks;
int selectedNetwork = 0;
bool isScanning = false;

// Neko HACKER animation frames (diferentes estilos por evolução!)
// Baby (Level 0-5)
const char* nekoFramesBaby[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >   <  ",  // Baby feliz
    "  /\\_/\\  \n ( -.^ ) \n  >   <  ",  // Baby piscando
    "  /\\_/\\  \n ( o.o ) \n  >   <  ",  // Baby curioso
    "  /\\_/\\  \n ( ^w^ ) \n  >   <  "   // Baby sorrindo
};

// Teen (Level 6-15)
const char* nekoFramesTeen[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >[*]<  ",  // Teen normal
    "  /\\_/\\  \n ( -.^ ) \n  >[#]<  ",  // Teen hackeando
    "  /\\_/\\  \n ( o.o ) \n  >[?]<  ",  // Teen pensando
    "  /\\_/\\  \n ( ^_^ ) \n  >[+]<  "   // Teen feliz
};

// Adult (Level 16+)
const char* nekoFramesAdult[] = {
    "  /\\_/\\  \n ( -.^ ) \n  >[$]<  ",  // Hacker pro (piscada)
    "  /\\_/\\  \n ( ^.^ ) \n  >[#]<  ",  // Coding
    "  /\\_/\\  \n ( o.o ) \n  >{*}<  ",  // Scanning
    "  /\\_/\\  \n ( @.@ ) \n  >[!]<  "   // Alert
};

// Pointer para o frame atual baseado no level
const char** getCurrentNekoFrames() {
    if (level <= 5) return nekoFramesBaby;
    else if (level <= 15) return nekoFramesTeen;
    else return nekoFramesAdult;
}

// Blink animation frames
const char* nekoBlinkBaby[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >   <  ",
    "  /\\_/\\  \n ( -.- ) \n  >   <  ",
    "  /\\_/\\  \n ( ^.^ ) \n  >   <  "
};

const char* nekoBlinkTeen[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >[*]<  ",
    "  /\\_/\\  \n ( -.- ) \n  >[#]<  ",
    "  /\\_/\\  \n ( ^.^ ) \n  >[*]<  "
};

const char* nekoBlinkAdult[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >[$]<  ",
    "  /\\_/\\  \n ( -.- ) \n  >[#]<  ",
    "  /\\_/\\  \n ( ^.^ ) \n  >[$]<  "
};

// Ear wiggle animation
const char* nekoEarBaby[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >   <  ",
    "  /\\_//  \n ( ^.^ ) \n  >   <  ",
    "  //_/\\  \n ( ^.^ ) \n  >   <  "
};

const char* nekoEarTeen[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >[*]<  ",
    "  /\\_//  \n ( ^.^ ) \n  >[#]<  ",
    "  //_/\\  \n ( ^.^ ) \n  >[*]<  "
};

const char* nekoEarAdult[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >[$]<  ",
    "  /\\_//  \n ( ^.^ ) \n  >[#]<  ",
    "  //_/\\  \n ( ^.^ ) \n  >[$]<  "
};

// Tail wag animation
const char* nekoTailBaby[] = {
    "  /\\_/\\  \n ( ^w^ ) \n  >   <  ",
    "  /\\_/\\  \n ( ^w^ ) \n  >  ~<  ",
    "  /\\_/\\  \n ( ^w^ ) \n  > ~  <  "
};

const char* nekoTailTeen[] = {
    "  /\\_/\\  \n ( ^w^ ) \n  >[*]<  ",
    "  /\\_/\\  \n ( ^w^ ) \n  >[~]<  ",
    "  /\\_/\\  \n ( ^w^ ) \n  >[*]<~ "
};

const char* nekoTailAdult[] = {
    "  /\\_/\\  \n ( ^w^ ) \n  >[$]<  ",
    "  /\\_/\\  \n ( ^w^ ) \n  >[$]~  ",
    "  /\\_/\\  \n ( ^w^ ) \n  >[$]<  "
};

// Paw lick animation
const char* nekoLickBaby[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >  u/  ",
    "  /\\_/\\  \n ( ^o^ ) \n  >  u~  ",
    "  /\\_/\\  \n ( ^.^ ) \n  >  u/  ",
    "  /\\_/\\  \n ( ^.^ ) \n  >   <  "
};

const char* nekoLickTeen[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >u[*] ",
    "  /\\_/\\  \n ( ^o^ ) \n  >u[#] ",
    "  /\\_/\\  \n ( ^.^ ) \n  >u[*] ",
    "  /\\_/\\  \n ( ^.^ ) \n  >[*]<  "
};

const char* nekoLickAdult[] = {
    "  /\\_/\\  \n ( ^.^ ) \n  >u[$] ",
    "  /\\_/\\  \n ( ^o^ ) \n  >u[#] ",
    "  /\\_/\\  \n ( ^.^ ) \n  >u[$] ",
    "  /\\_/\\  \n ( ^.^ ) \n  >[$]<  "
};

// Hack mode animation
const char* nekoHackBaby[] = {
    "  /\\_/\\  \n ( [ ] ) \n  >_=_<  ",
    "  /\\_/\\  \n ( [#] ) \n  >_=_<  ",
    "  /\\_/\\  \n ( [ ] ) \n  >_=_<  "
};

const char* nekoHackTeen[] = {
    "  /\\_/\\  \n ( [ ] ) \n  >[*]<  ",
    "  /\\_/\\  \n ( [#] ) \n  >[#]<  ",
    "  /\\_/\\  \n ( [ ] ) \n  >[*]<  "
};

const char* nekoHackAdult[] = {
    "  /\\_/\\  \n ( [ ] ) \n  >[$]<  ",
    "  /\\_/\\  \n ( [#] ) \n  >[#]<  ",
    "  /\\_/\\  \n ( [ ] ) \n  >[$]<  "
};

// Sleep animation
const char* nekoSleepBaby[] = {
    "  /\\_/\\  \n ( -.- ) \n  zzzz   ",
    "  /\\_/\\  \n ( -.- ) \n   Zzz   ",
    "  /\\_/\\  \n ( -.- ) \n  zzzz   "
};

const char* nekoSleepTeen[] = {
    "  /\\_/\\  \n ( -.- ) \n  z[*]  ",
    "  /\\_/\\  \n ( -.- ) \n   Zzz   ",
    "  /\\_/\\  \n ( -.- ) \n  z[*]  "
};

const char* nekoSleepAdult[] = {
    "  /\\_/\\  \n ( -.- ) \n  z[$]  ",
    "  /\\_/\\  \n ( -.- ) \n   Zzz   ",
    "  /\\_/\\  \n ( -.- ) \n  z[$]  "
};

struct NekoAnimationDefinition {
    const char* name;
    const char** framesBaby;
    int frameCountBaby;
    const char** framesTeen;
    int frameCountTeen;
    const char** framesAdult;
    int frameCountAdult;
    int frameDurationMs;
    int minDurationMs;
};

const NekoAnimationDefinition NEKO_ANIM_IDLE = {
    "idle",
    nekoFramesBaby, 4,
    nekoFramesTeen, 4,
    nekoFramesAdult, 4,
    450,
    3000
};

const NekoAnimationDefinition NEKO_ANIM_BLINK = {
    "blink",
    nekoBlinkBaby, 3,
    nekoBlinkTeen, 3,
    nekoBlinkAdult, 3,
    180,
    600
};

const NekoAnimationDefinition NEKO_ANIM_EAR = {
    "ear",
    nekoEarBaby, 3,
    nekoEarTeen, 3,
    nekoEarAdult, 3,
    200,
    1500
};

const NekoAnimationDefinition NEKO_ANIM_TAIL = {
    "tail",
    nekoTailBaby, 3,
    nekoTailTeen, 3,
    nekoTailAdult, 3,
    220,
    1800
};

const NekoAnimationDefinition NEKO_ANIM_LICK = {
    "lick",
    nekoLickBaby, 4,
    nekoLickTeen, 4,
    nekoLickAdult, 4,
    240,
    2200
};

const NekoAnimationDefinition NEKO_ANIM_HACK = {
    "hack",
    nekoHackBaby, 3,
    nekoHackTeen, 3,
    nekoHackAdult, 3,
    200,
    2000
};

const NekoAnimationDefinition NEKO_ANIM_SLEEP = {
    "sleep",
    nekoSleepBaby, 3,
    nekoSleepTeen, 3,
    nekoSleepAdult, 3,
    500,
    3000
};

struct NekoAnimationState {
    const NekoAnimationDefinition* active;
    int frameIndex;
    unsigned long lastFrameChange;
    unsigned long lastAnimationChange;
};

NekoAnimationState nekoAnimationState = {nullptr, 0, 0, 0};

const char** getStageFrames(const NekoAnimationDefinition* anim, int& frameCount) {
    if (!anim) {
        frameCount = 0;
        return nullptr;
    }
    if (level <= 5) {
        frameCount = anim->frameCountBaby;
        return anim->framesBaby;
    }
    if (level <= 15) {
        frameCount = anim->frameCountTeen;
        return anim->framesTeen;
    }
    frameCount = anim->frameCountAdult;
    return anim->framesAdult;
}

MoodState getCurrentMood();

const NekoAnimationDefinition* chooseNekoAnimation(MoodState mood) {
    if (currentPage != PAGE_HOME) {
        return &NEKO_ANIM_IDLE;
    }

    if (isSick) {
        return &NEKO_ANIM_LICK;
    }

    if (energy < 25 || mood == MOOD_SLEEPY) {
        return &NEKO_ANIM_SLEEP;
    }

    const NekoAnimationDefinition* candidates[8];
    int count = 0;
    candidates[count++] = &NEKO_ANIM_IDLE;

    if (mood == MOOD_EXCITED || mood == MOOD_HAPPY || mood == MOOD_PLAYFUL) {
        candidates[count++] = &NEKO_ANIM_TAIL;
        candidates[count++] = &NEKO_ANIM_EAR;
    }

    if (mood == MOOD_HUNGRY || hunger < 35) {
        candidates[count++] = &NEKO_ANIM_LICK;
    }

    if (discipline > 70 || mood == MOOD_CONTENT) {
        candidates[count++] = &NEKO_ANIM_HACK;
    }

    // Blink always possible
    candidates[count++] = &NEKO_ANIM_BLINK;

    if (count == 0) {
        return &NEKO_ANIM_IDLE;
    }

    int choice = random(count);
    return candidates[choice];
}

void updateNekoAnimation(unsigned long now) {
    MoodState mood = getCurrentMood();

    if (!nekoAnimationState.active) {
        nekoAnimationState.active = &NEKO_ANIM_IDLE;
        nekoAnimationState.frameIndex = 0;
        nekoAnimationState.lastFrameChange = now;
        nekoAnimationState.lastAnimationChange = now;
    }

    if (currentPage != PAGE_HOME && nekoAnimationState.active != &NEKO_ANIM_IDLE) {
        nekoAnimationState.active = &NEKO_ANIM_IDLE;
        nekoAnimationState.frameIndex = 0;
        nekoAnimationState.lastFrameChange = now;
        nekoAnimationState.lastAnimationChange = now;
    }

    if (now - nekoAnimationState.lastAnimationChange >= static_cast<unsigned long>(nekoAnimationState.active->minDurationMs)) {
        const NekoAnimationDefinition* nextAnim = chooseNekoAnimation(mood);
        if (nextAnim != nekoAnimationState.active) {
            nekoAnimationState.active = nextAnim;
            nekoAnimationState.frameIndex = 0;
            nekoAnimationState.lastFrameChange = now;
            nekoAnimationState.lastAnimationChange = now;
            needsRedraw = true;
        } else {
            nekoAnimationState.lastAnimationChange = now;
        }
    }

    int frameCount = 0;
    const char** frames = getStageFrames(nekoAnimationState.active, frameCount);
    if (frameCount > 0 && frames) {
        int duration = max(80, nekoAnimationState.active->frameDurationMs);
        if (now - nekoAnimationState.lastFrameChange >= static_cast<unsigned long>(duration)) {
            nekoAnimationState.frameIndex = (nekoAnimationState.frameIndex + 1) % frameCount;
            nekoAnimationState.lastFrameChange = now;
            if (currentPage == PAGE_HOME) {
                needsRedraw = true;
            }
        }
    }
}

int clampPaletteIndex(int value, int count, int fallback) {
    if (value < 0 || value >= count) {
        return fallback;
    }
    return value;
}

void applyColorPalette() {
    colorBg        = BG_COLOR_OPTIONS[bgColorIndex];
    colorNeko      = NEKO_COLOR_OPTIONS[nekoColorIndex];
    colorHighlight = HIGHLIGHT_COLOR_OPTIONS[highlightColorIndex];
    colorText      = TEXT_COLOR_OPTIONS[textColorIndex];
}

String colorToHex(uint16_t value) {
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "#%04X", value);
    return String(buffer);
}

// ==================== MOOD & MESSAGING SYSTEM ====================

MoodState getCurrentMood() {
    int minStat = min(hunger, min(happiness, energy));
    int avgStat = (hunger + happiness + energy) / 3;
    
    // Critical state - qualquer stat < 10%
    if (minStat < 10) return MOOD_CRITICAL;
    
    // Angry - qualquer stat < 20%
    if (minStat < 20) return MOOD_ANGRY;
    
    // Sad - qualquer stat < 30%
    if (minStat < 30) return MOOD_SAD;
    
    // Specific mood states (order matters!)
    if (energy < 40) return MOOD_SLEEPY;
    if (hunger < 40) return MOOD_HUNGRY;
    if (happiness < 40 && minStat > 40) return MOOD_BORED;
    
    // Check for multiple low stats (grumpy)
    int lowStats = 0;
    if (hunger < 40) lowStats++;
    if (happiness < 40) lowStats++;
    if (energy < 40) lowStats++;
    if (lowStats >= 2 && avgStat < 50) return MOOD_GRUMPY;
    
    // Neutral - média entre 30-50%
    if (avgStat < 50) return MOOD_NEUTRAL;
    
    // Content - média entre 50-70%
    if (avgStat < 70) return MOOD_CONTENT;
    
    // Playful - high happiness and energy
    if (happiness > 80 && energy > 60) return MOOD_PLAYFUL;
    
    // Excited - all stats very high
    if (minStat > 85) return MOOD_EXCITED;
    
    // Happy - todos stats > 70%
    return MOOD_HAPPY;
}

String getMoodEmoji(MoodState mood) {
    switch (mood) {
        case MOOD_EXCITED: return "*o*";
        case MOOD_HAPPY: return "^_^";
        case MOOD_PLAYFUL: return "^w^";
        case MOOD_CONTENT: return "^.^";
        case MOOD_SLEEPY: return "-_-";
        case MOOD_HUNGRY: return "@_@";
        case MOOD_NEUTRAL: return "-.-";
        case MOOD_BORED: return "=_=";
        case MOOD_GRUMPY: return "~_~";
        case MOOD_SAD: return "T_T";
        case MOOD_ANGRY: return ">_<";
        case MOOD_CRITICAL: return "X_X";
        default: return "o.o";
    }
}

String getMoodMessage(MoodState mood) {
    int r = random(5);  // 5 messages per mood
    switch (mood) {
        case MOOD_EXCITED:
            switch(r) {
                case 0: return "WOOHOO! Best day ever!";
                case 1: return "Nya~ I'm AMAZING!";
                case 2: return "Pure happiness!!!";
                case 3: return "Can't stop purring!";
                default: return "Everything's perfect!";
            }
        case MOOD_HAPPY:
            switch(r) {
                case 0: return "Nya~ Life is paw-some!";
                case 1: return "Purr~ So happy!";
                case 2: return "Today is great!";
                case 3: return "Feeling blessed nya~";
                default: return "All paws up!";
            }
        case MOOD_PLAYFUL:
            switch(r) {
                case 0: return "Let's play nya!";
                case 1: return "Wanna have fun?!";
                case 2: return "Energy overload!";
                case 3: return "Chase the laser!";
                default: return "Zoomies time!";
            }
        case MOOD_CONTENT:
            switch(r) {
                case 0: return "Meow~ Feeling good!";
                case 1: return "All is well nya~";
                case 2: return "Life is nice";
                case 3: return "Peaceful vibes~";
                default: return "Comfy and cozy";
            }
        case MOOD_SLEEPY:
            switch(r) {
                case 0: return "Yawn... so sleepy...";
                case 1: return "Need catnap nya...";
                case 2: return "Zzz... tired...";
                case 3: return "Must... rest...";
                default: return "Eyes closing...";
            }
        case MOOD_HUNGRY:
            switch(r) {
                case 0: return "Feed me nya!";
                case 1: return "Tummy rumbling...";
                case 2: return "Want foods plz!";
                case 3: return "Hungry kitty here!";
                default: return "Nom nom time?";
            }
        case MOOD_NEUTRAL:
            switch(r) {
                case 0: return "Meh... could be better";
                case 1: return "Just existing...";
                case 2: return "Another day nya...";
                case 3: return "Feeling okay-ish";
                default: return "Not bad, not great";
            }
        case MOOD_BORED:
            switch(r) {
                case 0: return "So bored nya...";
                case 1: return "Nothing to do...";
                case 2: return "Entertain me plz!";
                case 3: return "Where's the fun?";
                default: return "Yawn... boring...";
            }
        case MOOD_GRUMPY:
            switch(r) {
                case 0: return "Leave me alone...";
                case 1: return "Not in the mood";
                case 2: return "Hmph! Whatever...";
                case 3: return "Go away nya...";
                default: return "Don't talk to me";
            }
        case MOOD_SAD:
            switch(r) {
                case 0: return "Meow... I'm sad...";
                case 1: return "Need attention nya...";
                case 2: return "Feeling down...";
                case 3: return "Why so lonely?";
                default: return "Comfort me plz...";
            }
        case MOOD_ANGRY:
            switch(r) {
                case 0: return "HISS! Not happy!";
                case 1: return "Grr... feed me NOW!";
                case 2: return "Angry kitty mode!";
                case 3: return "This is NOT okay!";
                default: return "RAWR! Mad nya!";
            }
        case MOOD_CRITICAL:
            switch(r) {
                case 0: return "Help... dying...";
                case 1: return "CRITICAL STATE!";
                case 2: return "Can't go on...";
                case 3: return "Need help NOW!";
                default: return "Emergency nya!!!";
            }
        default:
            return "...";
    }
}

void updateMoodMessage(unsigned long now) {
    MoodState mood = getCurrentMood();
    bool moodChanged = (mood != lastMoodForMessage);

    if (currentMoodMessage.length() == 0 || moodChanged) {
        currentMoodMessage = getMoodMessage(mood);
        lastMoodForMessage = mood;
        lastMoodMessageUpdate = now;
        if (currentPage == PAGE_HOME) {
            needsRedraw = true;
        }
        return;
    }

    if (now - lastMoodMessageUpdate >= MOOD_MESSAGE_INTERVAL) {
        String newMessage = getMoodMessage(mood);
        if (newMessage == currentMoodMessage) {
            newMessage = getMoodMessage(mood);
        }
        currentMoodMessage = newMessage;
        lastMoodForMessage = mood;
        lastMoodMessageUpdate = now;
        if (currentPage == PAGE_HOME) {
            needsRedraw = true;
        }
    }
}

float getMoodXPModifier(MoodState mood) {
    switch (mood) {
        case MOOD_EXCITED: return 2.0f;    // +100% XP! (best)
        case MOOD_HAPPY: return 1.5f;      // +50% XP
        case MOOD_PLAYFUL: return 1.4f;    // +40% XP
        case MOOD_CONTENT: return 1.2f;    // +20% XP
        case MOOD_SLEEPY: return 0.8f;     // -20% XP
        case MOOD_HUNGRY: return 0.9f;     // -10% XP
        case MOOD_NEUTRAL: return 1.0f;    // Normal XP
        case MOOD_BORED: return 0.7f;      // -30% XP
        case MOOD_GRUMPY: return 0.6f;     // -40% XP
        case MOOD_SAD: return 0.5f;        // -50% XP
        case MOOD_ANGRY: return 0.3f;      // -70% XP
        case MOOD_CRITICAL: return 0.1f;   // -90% XP (worst)
        default: return 1.0f;
    }
}

uint16_t getMoodColor(MoodState mood) {
    switch (mood) {
        case MOOD_EXCITED: return 0xFFE0;      // Yellow (excited!)
        case MOOD_HAPPY: return COLOR_STAT_GOOD;
        case MOOD_PLAYFUL: return 0x07FF;      // Cyan (playful)
        case MOOD_CONTENT: return COLOR_STAT_OK;
        case MOOD_SLEEPY: return 0x5ACF;       // Light blue
        case MOOD_HUNGRY: return 0xFD20;       // Orange
        case MOOD_NEUTRAL: return COLOR_TEXT;
        case MOOD_BORED: return 0x8410;        // Gray
        case MOOD_GRUMPY: return 0xFBE0;       // Light orange
        case MOOD_SAD: return 0xFD20;          // Orange
        case MOOD_ANGRY: return COLOR_STAT_BAD;
        case MOOD_CRITICAL: return COLOR_STAT_BAD;
        default: return COLOR_TEXT;
    }
}

// Evolution stage decay rates
void getDecayRates(int &hungerDecay, int &happinessDecay, int &energyDecay) {
    if (level <= 5) {
        // Baby - slow decay
        hungerDecay = 2;
        happinessDecay = 1;
        energyDecay = 1;
    } else if (level <= 15) {
        // Teen - normal decay
        hungerDecay = 3;
        happinessDecay = 2;
        energyDecay = 2;
    } else {
        // Adult - faster decay (needs more care)
        hungerDecay = 4;
        happinessDecay = 3;
        energyDecay = 2;
    }
}

// ==================== FUNÇÕES DE DESENHO ====================

void drawStatusBar(int x, int y, const char* label, int value, uint16_t color) {
    // Label
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(x, y);
    M5.Display.print(label);
    
    // Barra
    int barWidth = 50;  // Reduzido de 60 para 50
    int barHeight = 6;   // Reduzido de 8 para 6
    int barX = x + 45;   // Ajustado de 50 para 45
    int filled = (value * barWidth) / 100;
    
    M5.Display.drawRect(barX, y, barWidth, barHeight, COLOR_PANEL);
    M5.Display.fillRect(barX + 1, y + 1, filled - 2, barHeight - 2, color);
    
    // Valor
    M5.Display.setCursor(barX + barWidth + 5, y);
    M5.Display.printf("%d%%", value);
}

void drawBigNekoFrame(int x, int y, const char* frameText) {
    if (!frameText) {
        return;
    }
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setTextSize(3);  // TAMANHO 3 = MEGA GATINHO!

    int lines = 0;
    int startY = y;
    int idx = 0;
    while (frameText[idx] != '\0') {
        String line = "";
        while (frameText[idx] != '\0' && frameText[idx] != '\n') {
            line += frameText[idx++];
        }
        if (frameText[idx] == '\n') {
            idx++;
        }
        M5.Display.setCursor(x, startY + lines * 24);
        M5.Display.print(line);
        lines++;
    }
}

void drawHomePage() {
    M5.Display.fillScreen(colorBg);
    
    // Linha 1: Level, XP e Uptime (sem título)
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(colorHighlight);
    M5.Display.setCursor(5, 5);
    M5.Display.printf("Lv:%d", level);
    M5.Display.setCursor(45, 5);
    M5.Display.printf("XP:%d", experience);
    
    // Uptime no canto direito
    unsigned long uptime = (millis() - sessionStartTime) / 1000;
    int hours = uptime / 3600;
    int mins = (uptime % 3600) / 60;
    int secs = uptime % 60;
    M5.Display.setCursor(150, 5);
    M5.Display.printf("%02d:%02d:%02d", hours, mins, secs);
    
    // Nome do pet (editável via settings)
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(colorText);
    M5.Display.setCursor(5, 17);
    M5.Display.print(petName);
    
    // Age indicator (below name) - +3px padding
    uint64_t ageSecondsTotal = petAgeSeconds;
    int ageDays = static_cast<int>(ageSecondsTotal / 86400ULL);
    int ageHours = static_cast<int>((ageSecondsTotal % 86400ULL) / 3600ULL);
    int ageMinutes = static_cast<int>((ageSecondsTotal % 3600ULL) / 60ULL);
    M5.Display.setTextColor(colorHighlight);
    M5.Display.setCursor(5, 29);
    if (ageDays > 0) {
        M5.Display.printf("Age: %dd %dh", ageDays, ageHours);
    } else if (ageHours > 0) {
        M5.Display.printf("Age: %dh %dm", ageHours, ageMinutes);
    } else {
        M5.Display.printf("Age: %dm", ageMinutes);
    }
    
    // Weight indicator (below age) - +3px padding
    M5.Display.setTextColor(colorHighlight);
    M5.Display.setCursor(5, 41);
    M5.Display.printf("WT: %dkg", weight);
    
    // Mood indicator (below weight) - +3px padding
    MoodState currentMood = getCurrentMood();
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(getMoodColor(currentMood));
    M5.Display.setCursor(5, 53);
    M5.Display.printf("Mood: %s", getMoodEmoji(currentMood).c_str());
    
    // Desenhar gatinho HACKER MEGA no centro
    int nekoY = 38;
    int stageFrameCount = 0;
    const NekoAnimationDefinition* activeAnim = nekoAnimationState.active ? nekoAnimationState.active : &NEKO_ANIM_IDLE;
    const char** stageFrames = getStageFrames(activeAnim, stageFrameCount);
    const char* frameText = nullptr;
    if (stageFrames && stageFrameCount > 0) {
        frameText = stageFrames[nekoAnimationState.frameIndex % stageFrameCount];
    } else {
        const char** fallback = getCurrentNekoFrames();
        frameText = fallback ? fallback[0] : nullptr;
    }
    drawBigNekoFrame(70, nekoY, frameText);
    
    // Status bars - 5 bars total (F, J, E, DS, CL)
    int barWidth = 30;   // Barras pequenas (30px)
    int barHeight = 6;   // Altura 6px
    int spacing = 9;     // Espaçamento entre barras (reduzido para caber 5)
    
    // Start below Mood indicator (+3px more padding)
    int statusY = 67;  // Fixed position below mood
    
    // Remover Tamagotchi indicators daqui - vão após as barras F/J/E
    
    // Poop indicators (draw poops on screen)
    for (int i = 0; i < poopCount && i < 3; i++) {
        M5.Display.setTextColor(0x7A00);  // Brown
        M5.Display.setCursor(140 + (i * 15), 95);
        M5.Display.print("@");
    }
    
    // Attention indicator (blinking)
    if (needsAttention && alertVisible) {
        M5.Display.setTextColor(COLOR_STAT_BAD);
        M5.Display.setTextSize(2);
        M5.Display.setCursor(200, 20);
        M5.Display.print("!");
        M5.Display.setTextSize(1);
    }
    
    uint16_t hungerColor = hunger > 60 ? COLOR_STAT_GOOD : (hunger > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    uint16_t happyColor = happiness > 60 ? COLOR_STAT_GOOD : (happiness > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    uint16_t energyColor = energy > 60 ? COLOR_STAT_GOOD : (energy > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT);
    
    // Bar 1: Food (com flash se < 20%)
    bool hungerLow = hunger < 20;
    M5.Display.setCursor(5, statusY);
    M5.Display.print("F:");
    M5.Display.fillRect(17, statusY + 1, (hunger * barWidth) / 100, barHeight, hungerLow && !alertVisible ? COLOR_STAT_BAD : hungerColor);
    M5.Display.drawRect(17, statusY + 1, barWidth, barHeight, COLOR_PANEL);
    M5.Display.setCursor(50, statusY);
    M5.Display.printf("%d", hunger);
    
    // Bar 2: Joy (com flash se < 20%)
    bool happinessLow = happiness < 20;
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(5, statusY + spacing);
    M5.Display.print("J:");
    M5.Display.fillRect(17, statusY + spacing + 1, (happiness * barWidth) / 100, barHeight, happinessLow && !alertVisible ? COLOR_STAT_BAD : happyColor);
    M5.Display.drawRect(17, statusY + spacing + 1, barWidth, barHeight, COLOR_PANEL);
    M5.Display.setCursor(50, statusY + spacing);
    M5.Display.printf("%d", happiness);
    
    // Bar 3: Energy (com flash se < 20%)
    bool energyLow = energy < 20;
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(5, statusY + spacing * 2);
    M5.Display.print("E:");
    M5.Display.fillRect(17, statusY + spacing * 2 + 1, (energy * barWidth) / 100, barHeight, energyLow && !alertVisible ? COLOR_STAT_BAD : energyColor);
    M5.Display.drawRect(17, statusY + spacing * 2 + 1, barWidth, barHeight, COLOR_PANEL);
    M5.Display.setCursor(50, statusY + spacing * 2);
    M5.Display.printf("%d", energy);
    
    // Bar 4: Discipline (NEW BAR)
    uint16_t disciplineColor = discipline > 60 ? COLOR_STAT_GOOD : (discipline > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(5, statusY + spacing * 3);
    M5.Display.print("DS:");
    M5.Display.fillRect(20, statusY + spacing * 3 + 1, (discipline * barWidth) / 100, barHeight, disciplineColor);
    M5.Display.drawRect(20, statusY + spacing * 3 + 1, barWidth, barHeight, COLOR_PANEL);
    M5.Display.setCursor(53, statusY + spacing * 3);
    M5.Display.printf("%d", discipline);
    
    // Bar 5: Hygiene/Clean (NEW BAR)
    uint16_t hygieneColor = hygiene > 60 ? COLOR_STAT_GOOD : (hygiene > 30 ? COLOR_STAT_OK : COLOR_STAT_BAD);
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(5, statusY + spacing * 4);
    M5.Display.print("CL:");
    M5.Display.fillRect(20, statusY + spacing * 4 + 1, (hygiene * barWidth) / 100, barHeight, hygieneColor);
    M5.Display.drawRect(20, statusY + spacing * 4 + 1, barWidth, barHeight, COLOR_PANEL);
    M5.Display.setCursor(53, statusY + spacing * 4);
    M5.Display.printf("%d", hygiene);
    
    // Sick indicator (below all bars)
    if (isSick) {
        M5.Display.setTextColor(COLOR_STAT_BAD);
        M5.Display.setCursor(5, statusY + spacing * 5);
        M5.Display.print("SICK!");
    }
    
    // Alertas para stats baixos (<20%) - COM BLINK - ABAIXO DAS BARRAS
    if (alertVisible) {
        String alert = "";
        if (hunger < 20) alert += "HUNGRY! ";
        if (happiness < 20) alert += "SAD! ";
        if (energy < 20) alert += "TIRED! ";
        
        if (alert.length() > 0) {
            M5.Display.setTextSize(1);
            M5.Display.setTextColor(COLOR_STAT_BAD);
            // Posicionar logo abaixo da última barra (CL) ou SICK
            int alertY = statusY + (spacing * 5) + (isSick ? 10 : 0);
            M5.Display.setCursor(5, alertY);
            M5.Display.print(alert);
        }
    }

    // Mood message no rodapé (substitui antigo footer)
    String moodMsg = currentMoodMessage.length() > 0 ? currentMoodMessage : getMoodMessage(currentMood);
    int msgWidth = moodMsg.length() * 6;
    int footerHeight = 12;
    int footerY = SCREEN_HEIGHT - footerHeight;
    M5.Display.fillRect(0, footerY, SCREEN_WIDTH, footerHeight, colorBg);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(getMoodColor(currentMood));
    int msgX = max(0, (SCREEN_WIDTH - msgWidth) / 2);
    M5.Display.setCursor(msgX, footerY + 2);
    M5.Display.print(moodMsg);
}

void drawMenuPage() {
    M5.Display.fillScreen(colorBg);
    
    // Título
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(60, 5);
    M5.Display.print("= MENU =");
    
    const char* menuItems[] = {
        "1. WiFi Scanner",
        "2. Bluetooth Tools",
        "3. System Tools",
        "4. Pet Settings",
        "5. Test Mode (QA)",
        "6. About / Info"
    };
    
    int startY = 30;
    int itemHeight = 16;  // Reduzido para caber 6 itens
    
    for (int i = 0; i < 6; i++) {
        if (i == menuSelection) {
            M5.Display.fillRoundRect(10, startY + i * itemHeight - 2, SCREEN_WIDTH - 20, 16, 4, COLOR_SELECT);
            M5.Display.setTextColor(COLOR_HIGHLIGHT);
        } else {
            M5.Display.setTextColor(COLOR_TEXT);
        }
        
        M5.Display.setTextSize(1);
        M5.Display.setCursor(15, startY + i * itemHeight);
        M5.Display.print(menuItems[i]);
    }
    
}

void drawWiFiScanner() {
    M5.Display.fillScreen(colorBg);
    
    // Título
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(45, 5);
    M5.Display.print("WiFi Scan");
    
    if (isScanning) {
        M5.Display.setTextColor(COLOR_HIGHLIGHT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(70, 60);
        M5.Display.print("Scanning...");
        return;
    }
    
    if (wifiNetworks.size() == 0) {
        M5.Display.setTextColor(COLOR_TEXT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(40, 60);
        M5.Display.print("Press SPACE to scan");
    } else {
        int startY = 30;
        int itemHeight = 12;
        int visibleItems = 8;
        int scrollOffset = max(0, selectedNetwork - visibleItems + 1);
        
        for (int i = scrollOffset; i < min((int)wifiNetworks.size(), scrollOffset + visibleItems); i++) {
            if (i == selectedNetwork) {
                M5.Display.fillRect(2, startY + (i - scrollOffset) * itemHeight - 1, SCREEN_WIDTH - 4, 11, COLOR_SELECT);
                M5.Display.setTextColor(COLOR_HIGHLIGHT);
            } else {
                M5.Display.setTextColor(COLOR_TEXT);
            }
            
            M5.Display.setTextSize(1);
            M5.Display.setCursor(5, startY + (i - scrollOffset) * itemHeight);
            
            String network = wifiNetworks[i];
            if (network.length() > 35) {
                network = network.substring(0, 32) + "...";
            }
            M5.Display.print(network);
        }
        
        // Scrollbar
        if (wifiNetworks.size() > visibleItems) {
            int barHeight = (visibleItems * 96) / wifiNetworks.size();
            int barY = 30 + (scrollOffset * 96) / wifiNetworks.size();
            M5.Display.fillRect(SCREEN_WIDTH - 3, barY, 2, barHeight, COLOR_STAT_GOOD);
        }
    }
    
}

void drawBluetoothPage() {
    M5.Display.fillScreen(colorBg);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(30, 5);
    M5.Display.print("Bluetooth");
    
    M5.Display.setTextSize(1);
    
    const char* items[] = {"BLE Spam - Apple", "BLE Spam - Samsung", "BLE Scanner", "Device Jammer"};
    int startY = 35;
    int lineHeight = 15;
    
    for (int i = 0; i < 4; i++) {
        int y = startY + (i * lineHeight);
        
        // Highlight selected
        if (i == bluetoothSelection) {
            M5.Display.fillRect(5, y - 2, SCREEN_WIDTH - 10, lineHeight - 2, COLOR_SELECT);
            M5.Display.setTextColor(COLOR_BG);
        } else {
            M5.Display.setTextColor(COLOR_TEXT);
        }
        
        M5.Display.setCursor(10, y);
        M5.Display.print(items[i]);
    }
    
}

void drawToolsPage() {
    M5.Display.fillScreen(COLOR_BG);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setCursor(55, 5);
    M5.Display.print("Tools");
    
    M5.Display.setTextSize(1);
    
    const char* items[] = {"File Manager", "System Info", "LED Control", "Reboot Device"};
    int startY = 35;
    int lineHeight = 15;
    
    for (int i = 0; i < 4; i++) {
        int y = startY + (i * lineHeight);
        
        // Highlight selected
        if (i == toolsSelection) {
            M5.Display.fillRect(5, y - 2, SCREEN_WIDTH - 10, lineHeight - 2, COLOR_SELECT);
            M5.Display.setTextColor(COLOR_BG);
        } else {
            M5.Display.setTextColor(COLOR_TEXT);
        }
        
        M5.Display.setCursor(10, y);
        M5.Display.print(items[i]);
    }
    
}

void drawSettingsPage() {
    M5.Display.fillScreen(COLOR_BG);

    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_HIGHLIGHT);
    M5.Display.setCursor(35, 5);
    M5.Display.print("Color Setup");

    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(10, 25);
    M5.Display.print(";/. move  ,/ change");
    M5.Display.setCursor(10, 37);
    M5.Display.print("Colors persist via Pet Settings");

    const char* labels[4] = {"Background", "Neko", "Highlight", "Text"};
    const char* names[4]  = {
        BG_COLOR_NAMES[bgColorIndex],
        NEKO_COLOR_NAMES[nekoColorIndex],
        HIGHLIGHT_COLOR_NAMES[highlightColorIndex],
        TEXT_COLOR_NAMES[textColorIndex]
    };
    const uint16_t values[4] = {
        COLOR_BG,
        COLOR_NEKO,
        COLOR_HIGHLIGHT,
        COLOR_TEXT
    };

    int startY = 50;
    int itemHeight = 18;
    for (int i = 0; i < 4; ++i) {
        int y = startY + i * itemHeight;
        bool selected = (i == settingsSelection);
        if (selected) {
            M5.Display.fillRoundRect(5, y - 3, SCREEN_WIDTH - 10, 16, 4, COLOR_HIGHLIGHT);
            M5.Display.setTextColor(COLOR_BG);
        } else {
            M5.Display.setTextColor(COLOR_TEXT);
        }
        M5.Display.setCursor(10, y);
        String line = String(labels[i]) + ": " + names[i] + " " + colorToHex(values[i]);
        M5.Display.print(line);
    }

    M5.Display.setTextColor(COLOR_TEXT);
    M5.Display.setCursor(10, 115);
    M5.Display.print("ESC=Back");
}

// ==================== FUNÇÕES DE NAVEGAÇÃO ====================

// Forward declarations for persistence functions (defined later)
void savePetData();
void loadPetData();


void handleHomeInput(char key) {
    // ESC (código 0x1B) = Menu
    if (key == 0x1B || key == '`') {  // ESC ou backtick
        currentPage = PAGE_MENU;
        menuSelection = 0;
        needsRedraw = true;
    }
    // SPACE = Alimentar (aumenta hunger)
    else if (key == ' ') {
        MoodState oldMood = getCurrentMood();
        hunger = min(100, hunger + 20);
        happiness = min(100, happiness + 5);
        weight = min(30, weight + 1);  // Increase weight
        
        // XP com mood modifier
        int baseXP = 2;
        int earnedXP = (int)(baseXP * getMoodXPModifier(getCurrentMood()));
        experience += earnedXP;
        
        // Feedback contextual baseado no mood
        M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
        M5.Display.setTextColor(COLOR_STAT_GOOD);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(75, 38);
        M5.Display.print("*NOM NOM*");
        M5.Display.setCursor(75, 48);
        if (getCurrentMood() == MOOD_HAPPY) {
            M5.Display.print("Yummy! +");
            M5.Display.print(earnedXP);
            M5.Display.print("XP!");
        } else if (getCurrentMood() >= MOOD_CONTENT) {
            M5.Display.print("Thanks! +");
            M5.Display.print(earnedXP);
            M5.Display.print("XP");
        } else {
            M5.Display.print("Finally... +");
            M5.Display.print(earnedXP);
            M5.Display.print("XP");
        }
        delay(800);
        
        savePetData();
        needsRedraw = true;
    }
    // O ou o = Brincar (aumenta happiness)
    else if (key == 'o' || key == 'O') {
        MoodState oldMood = getCurrentMood();
        happiness = min(100, happiness + 15);
        energy = max(0, energy - 10);
        weight = max(5, weight - 1);  // Decrease weight
        
        // XP com mood modifier
        int baseXP = 3;
        int earnedXP = (int)(baseXP * getMoodXPModifier(getCurrentMood()));
        experience += earnedXP;
        
        // Feedback visual com contexto
        M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
        M5.Display.setTextColor(0xF81F);  // Pink
        M5.Display.setTextSize(1);
        M5.Display.setCursor(80, 38);
        M5.Display.print("<3 FUN <3");
        M5.Display.setCursor(75, 48);
        if (energy < 20) {
            M5.Display.print("Too tired...");
        } else if (getCurrentMood() == MOOD_HAPPY) {
            M5.Display.print("Wheee! +");
            M5.Display.print(earnedXP);
            M5.Display.print("XP!");
        } else {
            M5.Display.print("Nice! +");
            M5.Display.print(earnedXP);
            M5.Display.print("XP");
        }
        delay(800);
        
        savePetData();
        needsRedraw = true;
    }
    // Z ou z = Dormir (recupera energy)
    else if (key == 'z' || key == 'Z') {
        energy = min(100, energy + 25);
        hunger = max(0, hunger - 5);
        
        // Feedback de sono
        M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
        M5.Display.setTextColor(COLOR_HIGHLIGHT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(80, 38);
        M5.Display.print("ZZZ...");
        M5.Display.setCursor(75, 48);
        if (energy >= 100) {
            M5.Display.print("Fully rested!");
        } else {
            M5.Display.print("Sweet dreams~");
        }
        delay(1000);
        
        savePetData();
        needsRedraw = true;
    }
    // C ou c = Clean (limpar cocô)
    else if (key == 'c' || key == 'C') {
        if (poopCount > 0) {
            poopCount = 0;
            hygiene = min(100, hygiene + 20);
            happiness = min(100, happiness + 5);
            
            // Feedback de limpeza
            M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
            M5.Display.setTextColor(COLOR_STAT_GOOD);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(75, 38);
            M5.Display.print("*CLEAN*");
            M5.Display.setCursor(75, 48);
            M5.Display.print("All clean nya!");
            delay(800);
        } else {
            // Nada para limpar
            M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
            M5.Display.setTextColor(COLOR_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(75, 40);
            M5.Display.print("Already clean!");
            delay(600);
        }
        
        savePetData();
        needsRedraw = true;
    }
    // M ou m = Medicine (curar doença)
    else if (key == 'm' || key == 'M') {
        if (isSick) {
            isSick = false;
            happiness = min(100, happiness + 10);
            
            // Feedback de medicina
            M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
            M5.Display.setTextColor(COLOR_STAT_GOOD);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(75, 38);
            M5.Display.print("+MEDICINE+");
            M5.Display.setCursor(75, 48);
            M5.Display.print("Feeling better!");
            delay(800);
        } else {
            // Não está doente
            M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
            M5.Display.setTextColor(COLOR_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(75, 40);
            M5.Display.print("Not sick!");
            delay(600);
        }
        
        savePetData();
        needsRedraw = true;
    }
    // T ou t = Train (treinar disciplina)
    else if (key == 't' || key == 'T') {
        if (discipline < 100) {
            discipline = min(100, discipline + 10);
            energy = max(0, energy - 5);
            
            int baseXP = 5;
            int earnedXP = (int)(baseXP * getMoodXPModifier(getCurrentMood()));
            experience += earnedXP;
            
            // Feedback de treino
            M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
            M5.Display.setTextColor(COLOR_HIGHLIGHT);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(75, 38);
            M5.Display.print("*TRAINING*");
            M5.Display.setCursor(75, 48);
            M5.Display.printf("Good! +%dXP", earnedXP);
            delay(800);
        } else {
            // Disciplina máxima
            M5.Display.fillRect(70, 35, 100, 30, COLOR_BG);
            M5.Display.setTextColor(COLOR_TEXT);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(75, 38);
            M5.Display.print("Max discipline!");
            M5.Display.setCursor(75, 48);
            M5.Display.print("Perfect pet!");
            delay(600);
        }
        
        savePetData();
        needsRedraw = true;
    }
}

void handleMenuInput(char key) {
    // Debug: mostrar código da tecla
    Serial.printf("Menu key: '%c' (0x%02X)\n", key, (unsigned char)key);
    
    // Navegação com ; e .
    if (key == ';') {  // UP
        menuSelection = (menuSelection - 1 + maxMenuItems) % maxMenuItems;
        needsRedraw = true;
        Serial.println("UP pressed");
    } 
    else if (key == '.') {  // DOWN
        menuSelection = (menuSelection + 1) % maxMenuItems;
        needsRedraw = true;
        Serial.println("DOWN pressed");
    }
    // ENTER - testar TODOS os códigos possíveis
    else if (key == 0x0D || key == 0x0A || key == '\n' || key == '\r' || key == 13 || key == 10) {
        Serial.printf(">>> ENTER DETECTED! menuSelection = %d <<<\n", menuSelection);
        switch (menuSelection) {
            case 0: 
                Serial.println("Going to WiFi Scanner");
                currentPage = PAGE_WIFI_SCANNER; 
                break;
            case 1: 
                Serial.println("Going to Bluetooth");
                currentPage = PAGE_BLUETOOTH; 
                break;
            case 2: 
                Serial.println("Going to Tools");
                currentPage = PAGE_TOOLS; 
                break;
            case 3: 
                Serial.println("Going to Settings");
                currentPage = PAGE_SETTINGS; 
                break;
            case 4: 
                Serial.println("Test Mode - Setting low stats");
                // Test Mode - forçar stats baixos para testar alertas
                hunger = 5;
                happiness = 5;
                energy = 5;
                savePetData();
                M5.Display.fillScreen(COLOR_BG);
                M5.Display.setTextColor(COLOR_STAT_BAD);
                M5.Display.setTextSize(2);
                M5.Display.setCursor(20, 40);
                M5.Display.print("TEST MODE");
                M5.Display.setTextColor(COLOR_TEXT);
                M5.Display.setTextSize(1);
                M5.Display.setCursor(15, 65);
                M5.Display.print("All stats set to 5%");
                M5.Display.setCursor(15, 80);
                M5.Display.print("Check alerts on home!");
                delay(2000);
                currentPage = PAGE_HOME;
                break;
            case 5: 
                Serial.println("Showing About");
                // About
                M5.Display.fillScreen(COLOR_BG);
                M5.Display.setTextColor(COLOR_NEKO);
                M5.Display.setTextSize(2);
                M5.Display.setCursor(30, 40);
                M5.Display.print("NEKO HACKER");
                M5.Display.setTextColor(COLOR_TEXT);
                M5.Display.setTextSize(1);
                M5.Display.setCursor(40, 65);
                M5.Display.print("Safe Pet v2.1.0");
                M5.Display.setCursor(35, 80);
                M5.Display.print("Made with <3");
                delay(2000);
                break;
        }
        needsRedraw = true;
    }
    // ESC = Voltar para home
    else if (key == 0x1B || key == '`') {
        Serial.println("ESC pressed - back to home");
        currentPage = PAGE_HOME;
        needsRedraw = true;
    }
    // Ignorar números - não fazer nada
    else if (key >= '0' && key <= '9') {
        Serial.printf("Number %c ignored (no shortcuts)\n", key);
    }
}

void handleWiFiInput(char key) {
    if (key == ' ') {  // SPACE - Scan
        if (!isScanning) {
            isScanning = true;
            needsRedraw = true;
            
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            delay(100);
            
            int n = WiFi.scanNetworks();
            wifiNetworks.clear();
            
            for (int i = 0; i < n; i++) {
                String network = WiFi.SSID(i);
                network += " ";
                network += WiFi.RSSI(i);
                network += "dBm";
                wifiNetworks.push_back(network);
            }
            
            selectedNetwork = 0;
            isScanning = false;
            needsRedraw = true;
        }
    } 
    else if (key == ';' && wifiNetworks.size() > 0) {  // UP
        selectedNetwork = (selectedNetwork - 1 + wifiNetworks.size()) % wifiNetworks.size();
        needsRedraw = true;
    } 
    else if (key == '.' && wifiNetworks.size() > 0) {  // DOWN
        selectedNetwork = (selectedNetwork + 1) % wifiNetworks.size();
        needsRedraw = true;
    }
    else if ((key == 0x0D || key == 0x0A || key == '\n' || key == '\r' || key == 13 || key == 10) && wifiNetworks.size() > 0) {  // ENTER
        M5.Display.fillScreen(COLOR_BG);
        M5.Display.setTextColor(COLOR_NEKO);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(10, 60);
        M5.Display.printf("Selected: %s", wifiNetworks[selectedNetwork].c_str());
        delay(1500);
        needsRedraw = true;
    }
    else if (key == 0x1B || key == '`') {  // ESC = Voltar
        currentPage = PAGE_MENU;
        menuSelection = 0;
        needsRedraw = true;
    }
}

void handleBluetoothInput(char key) {
    Serial.printf("Bluetooth key: '%c' (0x%02X)\n", key, (uint8_t)key);
    
    if (key >= '0' && key <= '9') {
        Serial.println("Number ignored in Bluetooth");
        return;
    }
    
    if (key == ';') {  // Navigate up
        bluetoothSelection = (bluetoothSelection - 1 + 4) % 4;
        needsRedraw = true;
    }
    else if (key == '.') {  // Navigate down
        bluetoothSelection = (bluetoothSelection + 1) % 4;
        needsRedraw = true;
    }
    else if (key == 0x0D || key == 0x0A || key == '\n' || key == '\r' || key == 13 || key == 10) {  // ENTER
        Serial.printf("ENTER detected in Bluetooth! Selected: %d\n", bluetoothSelection);
        M5.Display.fillScreen(COLOR_BG);
        M5.Display.setTextColor(COLOR_HIGHLIGHT);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(40, 60);
        M5.Display.printf("BLE Action %d...", bluetoothSelection + 1);
        delay(1500);
        needsRedraw = true;
    }
    else if (key == 0x1B || key == '`') {
        currentPage = PAGE_MENU;
        bluetoothSelection = 0;
        needsRedraw = true;
    }
}

void handleToolsInput(char key) {
    Serial.printf("Tools key: '%c' (0x%02X)\n", key, (uint8_t)key);
    
    if (key >= '0' && key <= '9') {
        Serial.println("Number ignored in Tools");
        return;
    }
    
    if (key == ';') {  // Navigate up
        toolsSelection = (toolsSelection - 1 + 4) % 4;
        needsRedraw = true;
    }
    else if (key == '.') {  // Navigate down
        toolsSelection = (toolsSelection + 1) % 4;
        needsRedraw = true;
    }
    else if (key == 0x0D || key == 0x0A || key == '\n' || key == '\r' || key == 13 || key == 10) {  // ENTER
        Serial.printf("ENTER detected in Tools! Selected: %d\n", toolsSelection);
        
        if (toolsSelection == 3) {  // Reboot Device
            M5.Display.fillScreen(COLOR_BG);
            M5.Display.setTextColor(COLOR_STAT_BAD);
            M5.Display.setTextSize(2);
            M5.Display.setCursor(40, 60);
            M5.Display.print("Rebooting...");
            delay(1000);
            ESP.restart();
        } else {
            M5.Display.fillScreen(COLOR_BG);
            M5.Display.setTextColor(COLOR_HIGHLIGHT);
            M5.Display.setTextSize(1);
            M5.Display.setCursor(40, 60);
            M5.Display.printf("Tool %d selected", toolsSelection + 1);
            delay(1500);
            needsRedraw = true;
        }
    }
    else if (key == 0x1B || key == '`') {
        currentPage = PAGE_MENU;
        toolsSelection = 0;
        needsRedraw = true;
    }
}

void handleSettingsInput(char key) {
    auto cycleIndex = [](int current, int count, bool forward) {
        if (forward) {
            return (current + 1) % count;
        }
        return (current - 1 + count) % count;
    };

    auto applyAndSave = [&]() {
        applyColorPalette();
        savePetData();
        needsRedraw = true;
    };

    if (key == 0x1B || key == '`') {
        currentPage = PAGE_MENU;
        needsRedraw = true;
    } else if (key == ';') {
        settingsSelection = (settingsSelection - 1 + 4) % 4;
        needsRedraw = true;
    } else if (key == '.') {
        settingsSelection = (settingsSelection + 1) % 4;
        needsRedraw = true;
    } else if (key == '[' || key == '{' || key == 'a' || key == 'A' || key == ',' || key == '<') {
        switch (settingsSelection) {
            case 0:
                bgColorIndex = cycleIndex(bgColorIndex, BG_COLOR_COUNT, false);
                break;
            case 1:
                nekoColorIndex = cycleIndex(nekoColorIndex, NEKO_COLOR_COUNT, false);
                break;
            case 2:
                highlightColorIndex = cycleIndex(highlightColorIndex, HIGHLIGHT_COLOR_COUNT, false);
                break;
            case 3:
                textColorIndex = cycleIndex(textColorIndex, TEXT_COLOR_COUNT, false);
                break;
        }
        applyAndSave();
    } else if (key == ']' || key == '}' || key == 'd' || key == 'D' || key == '/' || key == '?') {
        switch (settingsSelection) {
            case 0:
                bgColorIndex = cycleIndex(bgColorIndex, BG_COLOR_COUNT, true);
                break;
            case 1:
                nekoColorIndex = cycleIndex(nekoColorIndex, NEKO_COLOR_COUNT, true);
                break;
            case 2:
                highlightColorIndex = cycleIndex(highlightColorIndex, HIGHLIGHT_COLOR_COUNT, true);
                break;
            case 3:
                textColorIndex = cycleIndex(textColorIndex, TEXT_COLOR_COUNT, true);
                break;
        }
        applyAndSave();
    }
}

// ==================== PERSISTENCE ====================
Preferences preferences;

void savePetData() {
    preferences.begin("nekopet", false);
    preferences.putInt("hunger", hunger);
    preferences.putInt("happiness", happiness);
    preferences.putInt("energy", energy);
    preferences.putInt("level", level);
    preferences.putInt("experience", experience);
    preferences.putString("petName", petName);
    // Tamagotchi features
    preferences.putInt("weight", weight);
    preferences.putInt("discipline", discipline);
    preferences.putInt("hygiene", hygiene);
    preferences.putBool("isSick", isSick);
    preferences.putInt("poopCount", poopCount);
    preferences.putInt("bgColorIdx", bgColorIndex);
    preferences.putInt("nekoColorIdx", nekoColorIndex);
    preferences.putInt("highlightColorIdx", highlightColorIndex);
    preferences.putInt("textColorIdx", textColorIndex);
    preferences.putULong64("ageSeconds", petAgeSeconds);
    preferences.end();
    Serial.println("✓ Pet data saved to flash!");
}

void loadPetData() {
    preferences.begin("nekopet", true);
    hunger = preferences.getInt("hunger", 100);
    happiness = preferences.getInt("happiness", 100);
    energy = preferences.getInt("energy", 100);
    level = preferences.getInt("level", 0);
    experience = preferences.getInt("experience", 0);
    petName = preferences.getString("petName", "NEKO");
    // Tamagotchi features
    weight = preferences.getInt("weight", 10);
    discipline = preferences.getInt("discipline", 0);
    hygiene = preferences.getInt("hygiene", 100);
    isSick = preferences.getBool("isSick", false);
    poopCount = preferences.getInt("poopCount", 0);
    bgColorIndex = clampPaletteIndex(preferences.getInt("bgColorIdx", BG_COLOR_DEFAULT_INDEX), BG_COLOR_COUNT, BG_COLOR_DEFAULT_INDEX);
    nekoColorIndex = clampPaletteIndex(preferences.getInt("nekoColorIdx", NEKO_COLOR_DEFAULT_INDEX), NEKO_COLOR_COUNT, NEKO_COLOR_DEFAULT_INDEX);
    highlightColorIndex = clampPaletteIndex(preferences.getInt("highlightColorIdx", HIGHLIGHT_COLOR_DEFAULT_INDEX), HIGHLIGHT_COLOR_COUNT, HIGHLIGHT_COLOR_DEFAULT_INDEX);
    textColorIndex = clampPaletteIndex(preferences.getInt("textColorIdx", TEXT_COLOR_DEFAULT_INDEX), TEXT_COLOR_COUNT, TEXT_COLOR_DEFAULT_INDEX);
    petAgeSeconds = preferences.getULong64("ageSeconds", 0);
    preferences.end();
    applyColorPalette();
    Serial.printf("✓ Pet data loaded: %s Lvl=%d XP=%d H=%d Hp=%d E=%d W=%dkg D=%d%%\n", 
                  petName.c_str(), level, experience, hunger, happiness, energy, weight, discipline);
}

// ==================== SETUP & LOOP ====================

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5Cardputer.begin();  // IMPORTANTE: Inicializar o Cardputer!
    M5.Display.setRotation(1);
    M5.Display.setBrightness(128);
    M5.Display.fillScreen(COLOR_BG);
    
    Serial.begin(115200);
    delay(1000);  // Wait for serial to stabilize
    Serial.println("\n\n========================================");
    Serial.println("🐱 M5GOTCHI PRO v2.0.5 - Kawaii Pet");
    Serial.println("========================================");
    Serial.println("Serial Monitor Connected!");
    Serial.println("Press ESC to open menu");
    Serial.println("Use ;/. to navigate");
    Serial.println("Press ENTER to select");
    Serial.println("========================================\n");
    
    // Splash screen kawaii
    M5.Display.fillScreen(COLOR_BG);
    M5.Display.setTextColor(COLOR_NEKO);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(20, 40);
    M5.Display.print("M5GOTCHI");
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_HIGHLIGHT);
    M5.Display.setCursor(60, 75);
    M5.Display.print("Kawaii Pet v2.0.5");
    M5.Display.setCursor(55, 90);
    M5.Display.print("Loading...");
    
    // Barra de loading kawaii
    for (int i = 0; i < 200; i += 10) {
        M5.Display.fillRect(20, 105, i, 6, COLOR_STAT_GOOD);
        delay(50);
    }
    
    delay(500);
    
    // Load saved pet data
    loadPetData();
    
    Serial.println("✓ System initialized successfully!");
    Serial.println("✓ Display ready");
    Serial.println("✓ Keyboard ready");
    Serial.printf("✓ Pet loaded at Level %d\n\n", level);
    
    sessionStartTime = millis();  // Iniciar contador de uptime
    lastAgeTickMillis = sessionStartTime;
    currentPage = PAGE_HOME;
    needsRedraw = true;
    unsigned long initNow = millis();
    lastStatusUpdate = initNow;
    lastMoodForMessage = getCurrentMood();
    currentMoodMessage = getMoodMessage(lastMoodForMessage);
    lastMoodMessageUpdate = initNow;
}

void loop() {
    M5.update();
    
    unsigned long now = millis();

    if (lastAgeTickMillis == 0) {
        lastAgeTickMillis = now;
    }
    unsigned long ageDeltaMillis = now - lastAgeTickMillis;
    if (ageDeltaMillis >= 1000) {
        uint64_t secondsAdded = ageDeltaMillis / 1000;
        petAgeSeconds += secondsAdded;
        lastAgeTickMillis += secondsAdded * 1000;
        if (currentPage == PAGE_HOME) {
            needsRedraw = true;
        }
    }
    
    // Serial heartbeat every 10 seconds
    if (now - lastSerialHeartbeat > 10000) {
        Serial.printf("[%lu] Alive | Page: %d | Lvl: %d | H:%d%% Hp:%d%% E:%d%%\n", 
                     now/1000, currentPage, level, hunger, happiness, energy);
        lastSerialHeartbeat = now;
    }
    
    updateNekoAnimation(now);
    updateMoodMessage(now);
    
    // Blink de alertas (500ms on/off)
    if (now - lastAlertBlink > 500) {
        alertVisible = !alertVisible;
        lastAlertBlink = now;
        if (currentPage == PAGE_HOME && (hunger < 20 || happiness < 20 || energy < 20)) {
            needsRedraw = true;
        }
    }
    
    // Atualizar status do pet com tempos diferentes para cada stat
    if (now - lastStatusUpdate > 30000) {  // A cada 30 segundos
        // Get decay rates based on evolution stage
        int hungerDecay, happinessDecay, energyDecay;
        getDecayRates(hungerDecay, happinessDecay, energyDecay);
        
        // Apply decay with evolution-based rates
        hunger = max(0, hunger - hungerDecay);
        
        // Felicidade diminui médio (a cada 45s - verificar com contador)
        static int happinessCounter = 0;
        happinessCounter++;
        if (happinessCounter >= 2) {  // 30s * 1.5 = 45s
            happiness = max(0, happiness - happinessDecay);
            happinessCounter = 0;
        }
        
        // Energia diminui devagar (a cada 60s - verificar com contador)
        static int energyCounter = 0;
        energyCounter++;
        if (energyCounter >= 2) {  // 30s * 2 = 60s
            energy = max(0, energy - energyDecay);
            energyCounter = 0;
        }
        
        // Ganhar XP com o tempo (a cada 30s) - COM MOOD MODIFIER
        MoodState currentMood = getCurrentMood();
        int baseXP = 5;
        int earnedXP = (int)(baseXP * getMoodXPModifier(currentMood));
        experience += earnedXP;
        
        if (experience >= 100) {  // Level up a cada 100 XP
            experience = 0;
            level++;
            // Feedback visual de level up com evolução
            M5.Display.fillScreen(COLOR_BG);
            M5.Display.setTextColor(COLOR_NEKO);
            M5.Display.setTextSize(3);
            M5.Display.setCursor(30, 40);
            M5.Display.printf("LEVEL %d!", level);
            M5.Display.setTextSize(1);
            M5.Display.setTextColor(COLOR_HIGHLIGHT);
            M5.Display.setCursor(50, 70);
            if (level == 6) {
                M5.Display.print("Evolved to TEEN!");
            } else if (level == 16) {
                M5.Display.print("Evolved to ADULT!");
            }
            delay(2000);
        }
        
        // Tamagotchi Systems
        // Poop generation (every 2 minutes if hygiene < 50)
        if (hygiene < 50 && now - lastPoopTime > 120000 && poopCount < 3) {
            poopCount++;
            hygiene = max(0, hygiene - 10);
            lastPoopTime = now;
            needsAttention = true;
            lastCallTime = now;
        }
        
        // Weight changes
        // (handled in actions - eating increases, playing decreases)
        
        // Sickness check (low hygiene or multiple low stats)
        if (!isSick && (hygiene < 20 || (hunger < 20 && energy < 20))) {
            isSick = true;
            needsAttention = true;
            lastCallTime = now;
        }
        
        // Hygiene decay if has poop
        if (poopCount > 0) {
            hygiene = max(0, hygiene - 1);
        }
        
        // Save data every 30 seconds
        savePetData();
        
        lastStatusUpdate = now;
        needsRedraw = true;
    }
    
    // Attention call blink (pet needs care)
    if (needsAttention && now - lastCallTime > 10000) {
        // Reset attention after 10 seconds
        needsAttention = false;
    }
    
    // Redesenhar tela apenas quando necessário
    if (needsRedraw) {
        switch (currentPage) {
            case PAGE_HOME:
                drawHomePage();
                break;
            case PAGE_MENU:
                drawMenuPage();
                break;
            case PAGE_WIFI_SCANNER:
                drawWiFiScanner();
                break;
            case PAGE_BLUETOOTH:
                drawBluetoothPage();
                break;
            case PAGE_TOOLS:
                drawToolsPage();
                break;
            case PAGE_SETTINGS:
                drawSettingsPage();
                break;
        }
        needsRedraw = false;
    }
    
    // Processar input do teclado
    M5Cardputer.update();  // IMPORTANTE: Atualizar o Cardputer!
    
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            
            // Debug: Show ALL key info
            Serial.printf("=== KEY DETECTED ===\n");
            Serial.printf("Word size: %d\n", status.word.size());
            Serial.printf("Del: %d, Enter: %d, Tab: %d, Fn: %d, Opt: %d\n", 
                         status.del, status.enter, status.tab, status.fn, status.opt);
            
            // Check if ENTER flag is set (this is the proper way!)
            if (status.enter) {
                Serial.println("*** ENTER FLAG DETECTED! ***");
                char enterKey = 0x0D;  // Use this as enter
                switch (currentPage) {
                    case PAGE_HOME:
                        handleHomeInput(enterKey);
                        break;
                    case PAGE_MENU:
                        handleMenuInput(enterKey);
                        break;
                    case PAGE_WIFI_SCANNER:
                        handleWiFiInput(enterKey);
                        break;
                    case PAGE_BLUETOOTH:
                        handleBluetoothInput(enterKey);
                        break;
                    case PAGE_TOOLS:
                        handleToolsInput(enterKey);
                        break;
                    case PAGE_SETTINGS:
                        handleSettingsInput(enterKey);
                        break;
                }
            }
            
            for (auto key : status.word) {
                // Debug: mostrar tecla pressionada
                Serial.printf("Char: '%c' (0x%02X) Dec: %d\n", key, key, key);
                
                switch (currentPage) {
                    case PAGE_HOME:
                        handleHomeInput(key);
                        break;
                    case PAGE_MENU:
                        handleMenuInput(key);
                        break;
                    case PAGE_WIFI_SCANNER:
                        handleWiFiInput(key);
                        break;
                    case PAGE_BLUETOOTH:
                        handleBluetoothInput(key);
                        break;
                    case PAGE_TOOLS:
                        handleToolsInput(key);
                        break;
                    case PAGE_SETTINGS:
                        handleSettingsInput(key);
                        break;
                }
            }
        }
    }
    
    delay(10);  // 100 FPS loop
}
