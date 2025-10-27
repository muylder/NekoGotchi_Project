/**
 * @file m5gotchi_tutorial_system.h
 * @brief Sistema de tutorial interativo para first-time users
 * @version 1.0
 * @date 2025-10-26
 * 
 * @details
 * Sistema completo de onboarding e ajuda contextual para M5Gotchi Pro.
 * Features:
 * - First-run wizard com passo-a-passo
 * - Contextual help system (press ? key)
 * - Tooltips dinâmicos
 * - Progress tracking
 * - Skip/Resume tutorial
 * - Multi-idioma (PT/EN)
 * 
 * @example
 * ```cpp
 * TutorialSystem tutorial;
 * tutorial.init();
 * 
 * if (tutorial.isFirstRun()) {
 *     tutorial.startWizard();
 * }
 * 
 * // Em cada tela
 * if (tutorial.shouldShowHelp()) {
 *     tutorial.showContextualHelp(CONTEXT_WIFI_SCANNER);
 * }
 * ```
 * 
 * @copyright M5Gotchi Pro Project 2025
 * @license MIT
 */

#ifndef M5GOTCHI_TUTORIAL_SYSTEM_H
#define M5GOTCHI_TUTORIAL_SYSTEM_H

#include <M5Unified.h>
#include <Preferences.h>
#include "m5gotchi_universal_controls.h"
#include "m5gotchi_progmem_strings.h"

// ==================== CONFIGURATION ====================
#define TUTORIAL_VERSION 1
#define MAX_TUTORIAL_STEPS 20
#define MAX_HELP_TOPICS 30
#define TOOLTIP_DISPLAY_TIME 3000
#define HINT_DISPLAY_TIME 5000

// ==================== TUTORIAL CONTEXTS ====================
/**
 * @enum TutorialContext
 * @brief Contextos diferentes onde ajuda contextual está disponível
 */
enum TutorialContext {
    CONTEXT_MAIN_MENU = 0,
    CONTEXT_WIFI_SCANNER,
    CONTEXT_WIFI_DEAUTH,
    CONTEXT_WIFI_HANDSHAKE,
    CONTEXT_EVIL_PORTAL,
    CONTEXT_BLE_SPAM,
    CONTEXT_BLE_SCANNER,
    CONTEXT_RFID_SCANNER,
    CONTEXT_RFID_READER,
    CONTEXT_RFID_CLONER,
    CONTEXT_GPS_WARDRIVING,
    CONTEXT_IR_UNIVERSAL,
    CONTEXT_RF433,
    CONTEXT_SUBGHZ,
    CONTEXT_CHAMELEON,
    CONTEXT_ACHIEVEMENTS,
    CONTEXT_GAMES,
    CONTEXT_NEKO_PET,
    CONTEXT_NEKOGRAM,
    CONTEXT_SETTINGS,
    CONTEXT_COUNT
};

// ==================== TUTORIAL STEPS ====================
/**
 * @enum TutorialStep
 * @brief Passos do wizard de primeiro uso
 */
enum TutorialStep {
    STEP_WELCOME = 0,
    STEP_CONTROLS,
    STEP_NAVIGATION,
    STEP_WIFI_INTRO,
    STEP_SCANNER_DEMO,
    STEP_ACHIEVEMENTS,
    STEP_KAWAII_FEATURES,
    STEP_SAFETY_WARNING,
    STEP_COMPLETE,
    STEP_COUNT
};

/**
 * @enum HelpType
 * @brief Tipos de ajuda disponível
 */
enum HelpType {
    HELP_TOOLTIP = 0,    ///< Dica rápida (3 segundos)
    HELP_HINT,           ///< Sugestão contextual (5 segundos)
    HELP_FULL,           ///< Ajuda completa (até dismiss)
    HELP_VIDEO           ///< Demo animada (futuro)
};

/**
 * @enum Language
 * @brief Idiomas suportados
 */
enum Language {
    LANG_EN = 0,
    LANG_PT_BR,
    LANG_COUNT
};

// ==================== STRUCTURES ====================

/**
 * @struct TutorialStepData
 * @brief Dados de um passo do tutorial
 */
struct TutorialStepData {
    const char* title_P;        ///< Título em PROGMEM
    const char* description_P;  ///< Descrição em PROGMEM
    const char* action_P;       ///< Ação esperada em PROGMEM
    const char* emoji_P;        ///< Emoji decorativo
    uint16_t color;            ///< Cor temática do passo
    bool requiresAction;       ///< Requer ação do usuário?
    bool canSkip;              ///< Pode pular este passo?
};

/**
 * @struct HelpTopic
 * @brief Tópico de ajuda contextual
 */
struct HelpTopic {
    TutorialContext context;
    const char* title_P;
    const char* shortHelp_P;    ///< Ajuda curta (tooltip)
    const char* longHelp_P;     ///< Ajuda completa
    const char* controls_P;     ///< Descrição de controles
    const char* tips_P;         ///< Dicas e truques
};

/**
 * @struct TutorialProgress
 * @brief Progresso do usuário no tutorial
 */
struct TutorialProgress {
    bool completed;
    uint8_t currentStep;
    uint8_t stepsCompleted;
    uint32_t startTime;
    uint32_t completionTime;
    bool skipped;
};

// ==================== TUTORIAL SYSTEM CLASS ====================

/**
 * @class TutorialSystem
 * @brief Sistema principal de tutorial e ajuda
 * 
 * @details
 * Gerencia todo o fluxo de onboarding de novos usuários, incluindo:
 * - Wizard de first-run
 * - Ajuda contextual
 * - Tooltips automáticos
 * - Progress tracking
 * - Persistência de estado
 */
class TutorialSystem {
private:
    // State
    bool isActive;
    bool firstRun;
    bool helpEnabled;
    TutorialStep currentStep;
    TutorialContext currentContext;
    Language language;
    
    // Progress
    TutorialProgress progress;
    uint8_t hintsShown[CONTEXT_COUNT];
    bool contextVisited[CONTEXT_COUNT];
    
    // Display state
    bool showingHelp;
    bool showingTooltip;
    unsigned long tooltipStartTime;
    String tooltipMessage;
    
    // Storage
    Preferences prefs;
    
    // UI state
    int selectedOption;
    bool waitingForAction;
    
    // Animation
    int animationFrame;
    unsigned long lastAnimUpdate;
    
public:
    /**
     * @brief Construtor
     */
    TutorialSystem() {
        isActive = false;
        firstRun = true;
        helpEnabled = true;
        currentStep = STEP_WELCOME;
        currentContext = CONTEXT_MAIN_MENU;
        language = LANG_PT_BR;
        
        memset(&progress, 0, sizeof(TutorialProgress));
        memset(hintsShown, 0, sizeof(hintsShown));
        memset(contextVisited, 0, sizeof(contextVisited));
        
        showingHelp = false;
        showingTooltip = false;
        tooltipStartTime = 0;
        selectedOption = 0;
        waitingForAction = false;
        animationFrame = 0;
        lastAnimUpdate = 0;
    }
    
    // ==================== INITIALIZATION ====================
    
    /**
     * @brief Inicializa o sistema de tutorial
     * @return true se inicializado com sucesso
     */
    bool init() {
        Serial.println(F("📚 Initializing Tutorial System..."));
        
        // Abrir preferences
        if (!prefs.begin("m5gotchi_tut", false)) {
            Serial.println(F("❌ Failed to open tutorial preferences"));
            return false;
        }
        
        // Verificar se é primeira execução
        firstRun = !prefs.getBool("completed", false);
        helpEnabled = prefs.getBool("help_enabled", true);
        language = (Language)prefs.getUChar("language", LANG_PT_BR);
        
        if (firstRun) {
            Serial.println(F("🎉 First run detected! Starting wizard..."));
            progress.startTime = millis();
        } else {
            Serial.println(F("✅ Welcome back!"));
            loadProgress();
        }
        
        Serial.println(F("✅ Tutorial System ready!"));
        return true;
    }
    
    /**
     * @brief Salva progresso
     */
    void saveProgress() {
        prefs.putBool("completed", progress.completed);
        prefs.putUChar("current_step", currentStep);
        prefs.putUChar("steps_completed", progress.stepsCompleted);
        prefs.putBool("skipped", progress.skipped);
        
        if (progress.completed) {
            prefs.putUInt("completion_time", progress.completionTime);
        }
    }
    
    /**
     * @brief Carrega progresso
     */
    void loadProgress() {
        progress.completed = prefs.getBool("completed", false);
        currentStep = (TutorialStep)prefs.getUChar("current_step", STEP_WELCOME);
        progress.stepsCompleted = prefs.getUChar("steps_completed", 0);
        progress.skipped = prefs.getBool("skipped", false);
        progress.completionTime = prefs.getUInt("completion_time", 0);
    }
    
    // ==================== WIZARD FLOW ====================
    
    /**
     * @brief Inicia o wizard de primeiro uso
     */
    void startWizard() {
        isActive = true;
        currentStep = STEP_WELCOME;
        progress.currentStep = 0;
        progress.startTime = millis();
        
        Serial.println(F("🌟 Starting tutorial wizard..."));
        drawCurrentStep();
    }
    
    /**
     * @brief Avança para próximo passo
     */
    void nextStep() {
        if (currentStep < STEP_COUNT - 1) {
            currentStep = (TutorialStep)(currentStep + 1);
            progress.currentStep = currentStep;
            progress.stepsCompleted++;
            
            if (currentStep == STEP_COMPLETE) {
                completeTutorial();
            } else {
                drawCurrentStep();
            }
            
            saveProgress();
        }
    }
    
    /**
     * @brief Volta para passo anterior
     */
    void previousStep() {
        if (currentStep > STEP_WELCOME) {
            currentStep = (TutorialStep)(currentStep - 1);
            progress.currentStep = currentStep;
            drawCurrentStep();
        }
    }
    
    /**
     * @brief Pula tutorial
     */
    void skipTutorial() {
        progress.skipped = true;
        progress.completed = true;
        progress.completionTime = millis() - progress.startTime;
        
        isActive = false;
        saveProgress();
        
        Serial.println(F("⏭️ Tutorial skipped"));
    }
    
    /**
     * @brief Completa tutorial
     */
    void completeTutorial() {
        progress.completed = true;
        progress.completionTime = millis() - progress.startTime;
        
        isActive = false;
        firstRun = false;
        
        prefs.putBool("completed", true);
        saveProgress();
        
        Serial.println(F("🎉 Tutorial completed!"));
        Serial.printf_P(PSTR("Time: %d seconds\n"), progress.completionTime / 1000);
    }
    
    // ==================== STEP RENDERING ====================
    
    /**
     * @brief Desenha o passo atual do tutorial
     */
    void drawCurrentStep() {
        M5.Display.fillScreen(BLACK);
        
        // Header com progress bar
        drawProgressBar();
        
        // Conteúdo do passo
        switch (currentStep) {
            case STEP_WELCOME:
                drawWelcomeStep();
                break;
            case STEP_CONTROLS:
                drawControlsStep();
                break;
            case STEP_NAVIGATION:
                drawNavigationStep();
                break;
            case STEP_WIFI_INTRO:
                drawWiFiIntroStep();
                break;
            case STEP_SCANNER_DEMO:
                drawScannerDemoStep();
                break;
            case STEP_ACHIEVEMENTS:
                drawAchievementsStep();
                break;
            case STEP_KAWAII_FEATURES:
                drawKawaiiFeaturesStep();
                break;
            case STEP_SAFETY_WARNING:
                drawSafetyWarningStep();
                break;
            case STEP_COMPLETE:
                drawCompleteStep();
                break;
            default:
                break;
        }
        
        // Footer com controles
        drawNavigationFooter();
    }
    
    /**
     * @brief Desenha barra de progresso
     */
    void drawProgressBar() {
        // Background
        M5.Display.fillRect(0, 0, 240, 20, DARKGREY);
        
        // Progress
        int progressWidth = (currentStep * 240) / STEP_COUNT;
        M5.Display.fillRect(0, 0, progressWidth, 20, GREEN);
        
        // Text
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(90, 5);
        M5.Display.printf("%d/%d", currentStep + 1, STEP_COUNT);
    }
    
    /**
     * @brief Passo: Welcome
     */
    void drawWelcomeStep() {
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(50, 35);
        M5.Display.print(F("🐱 M5Gotchi Pro"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(30, 55);
        if (language == LANG_PT_BR) {
            M5.Display.print(F("Bem-vindo ao mundo"));
            M5.Display.setCursor(40, 70);
            M5.Display.print(F("dos Neko Hackers!"));
        } else {
            M5.Display.print(F("Welcome to the world"));
            M5.Display.setCursor(50, 70);
            M5.Display.print(F("of Neko Hackers!"));
        }
        
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(20, 95);
        if (language == LANG_PT_BR) {
            M5.Display.print(F("Vamos fazer um tour rápido"));
        } else {
            M5.Display.print(F("Let's take a quick tour"));
        }
    }
    
    /**
     * @brief Passo: Controles
     */
    void drawControlsStep() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(60, 30);
        M5.Display.print(F("🎮 Controles"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(20, 50);
        M5.Display.print(F("▲▼ : Navegar"));
        M5.Display.setCursor(20, 65);
        M5.Display.print(F("◄► : Mudar página"));
        M5.Display.setCursor(20, 80);
        M5.Display.print(F("SEL: Selecionar"));
        M5.Display.setCursor(20, 95);
        M5.Display.print(F("BCK: Voltar"));
    }
    
    /**
     * @brief Passo: Navegação
     */
    void drawNavigationStep() {
        M5.Display.setTextColor(BLUE);
        M5.Display.setCursor(50, 30);
        M5.Display.print(F("🗺️ Navegação"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 50);
        M5.Display.print(F("4 páginas principais:"));
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(20, 65);
        M5.Display.print(F("1️⃣ WiFi Pentest"));
        M5.Display.setCursor(20, 75);
        M5.Display.print(F("2️⃣ Avançado"));
        M5.Display.setCursor(20, 85);
        M5.Display.print(F("3️⃣ Jogos"));
        M5.Display.setCursor(20, 95);
        M5.Display.print(F("4️⃣ Kawaii Neko 💖"));
    }
    
    /**
     * @brief Passo: WiFi Intro
     */
    void drawWiFiIntroStep() {
        M5.Display.setTextColor(MAGENTA);
        M5.Display.setCursor(50, 30);
        M5.Display.print(F("📡 WiFi Tools"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 50);
        M5.Display.print(F("Ferramentas disponíveis:"));
        
        M5.Display.setCursor(20, 65);
        M5.Display.print(F("• Scanner de redes"));
        M5.Display.setCursor(20, 75);
        M5.Display.print(F("• Captura handshake"));
        M5.Display.setCursor(20, 85);
        M5.Display.print(F("• Deauth attack"));
        M5.Display.setCursor(20, 95);
        M5.Display.print(F("• Evil Portal"));
    }
    
    /**
     * @brief Passo: Scanner Demo
     */
    void drawScannerDemoStep() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(40, 30);
        M5.Display.print(F("🔍 Experimente!"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 55);
        M5.Display.print(F("Vá até WiFi Scanner"));
        M5.Display.setCursor(15, 70);
        M5.Display.print(F("e escaneie redes"));
        
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(15, 90);
        M5.Display.print(F("Seu 1º achievement"));
        M5.Display.setCursor(15, 100);
        M5.Display.print(F("está esperando! 🏆"));
    }
    
    /**
     * @brief Passo: Achievements
     */
    void drawAchievementsStep() {
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(40, 30);
        M5.Display.print(F("🏆 Conquistas"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 50);
        M5.Display.print(F("Desbloqueie badges:"));
        
        M5.Display.setCursor(20, 65);
        M5.Display.print(F("🥉 Bronze"));
        M5.Display.setCursor(20, 75);
        M5.Display.print(F("🥈 Prata"));
        M5.Display.setCursor(20, 85);
        M5.Display.print(F("🥇 Ouro"));
        M5.Display.setCursor(20, 95);
        M5.Display.print(F("💎 Kawaii"));
    }
    
    /**
     * @brief Passo: Kawaii Features
     */
    void drawKawaiiFeaturesStep() {
        M5.Display.setTextColor(0xF81F);  // Pink
        M5.Display.setCursor(40, 30);
        M5.Display.print(F("💖 Kawaii Mode"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 50);
        M5.Display.print(F("Features especiais:"));
        
        M5.Display.setCursor(20, 65);
        M5.Display.print(F("🐱 Neko Virtual Pet"));
        M5.Display.setCursor(20, 75);
        M5.Display.print(F("💬 NekoGram Chat"));
        M5.Display.setCursor(20, 85);
        M5.Display.print(F("🎵 Neko Sounds"));
        M5.Display.setCursor(20, 95);
        M5.Display.print(F("🎮 Mini-games"));
    }
    
    /**
     * @brief Passo: Safety Warning
     */
    void drawSafetyWarningStep() {
        M5.Display.setTextColor(RED);
        M5.Display.setCursor(50, 30);
        M5.Display.print(F("⚠️ AVISO"));
        
        M5.Display.setTextColor(ORANGE);
        M5.Display.setCursor(15, 50);
        M5.Display.print(F("Uso apenas educacional"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 70);
        M5.Display.print(F("• Respeite privacidade"));
        M5.Display.setCursor(15, 80);
        M5.Display.print(F("• Não hackeie outros"));
        M5.Display.setCursor(15, 90);
        M5.Display.print(F("• Use responsavelmente"));
    }
    
    /**
     * @brief Passo: Complete
     */
    void drawCompleteStep() {
        M5.Display.setTextColor(GREEN);
        M5.Display.setCursor(40, 30);
        M5.Display.print(F("🎉 Pronto!"));
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 55);
        M5.Display.print(F("Você está pronto para"));
        M5.Display.setCursor(15, 70);
        M5.Display.print(F("começar sua jornada!"));
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(15, 90);
        M5.Display.print(F("Pressione ? para ajuda"));
        M5.Display.setCursor(15, 100);
        M5.Display.print(F("em qualquer tela"));
    }
    
    /**
     * @brief Desenha footer de navegação
     */
    void drawNavigationFooter() {
        M5.Display.fillRect(0, 115, 240, 20, DARKGREY);
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(5, 120);
        
        if (currentStep > STEP_WELCOME) {
            M5.Display.print(F("◄ Voltar"));
        }
        
        M5.Display.setCursor(85, 120);
        M5.Display.print(F("Pular"));
        
        if (currentStep < STEP_COMPLETE) {
            M5.Display.setCursor(170, 120);
            M5.Display.print(F("Próximo ►"));
        } else {
            M5.Display.setCursor(170, 120);
            M5.Display.print(F("Iniciar!"));
        }
    }
    
    // ==================== CONTEXTUAL HELP ====================
    
    /**
     * @brief Define o contexto atual
     */
    void setContext(TutorialContext ctx) {
        currentContext = ctx;
        contextVisited[ctx] = true;
        
        // Mostrar hint automático na primeira visita
        if (helpEnabled && hintsShown[ctx] == 0 && !firstRun) {
            showAutoHint(ctx);
        }
    }
    
    /**
     * @brief Mostra ajuda para o contexto atual
     */
    void showContextualHelp() {
        showingHelp = true;
        drawContextHelp(currentContext);
    }
    
    /**
     * @brief Mostra hint automático
     */
    void showAutoHint(TutorialContext ctx) {
        hintsShown[ctx]++;
        
        // Criar mensagem baseada no contexto
        String hint = "";
        switch (ctx) {
            case CONTEXT_WIFI_SCANNER:
                hint = F("Dica: Pressione SELECT para detalhes");
                break;
            case CONTEXT_RFID_SCANNER:
                hint = F("Dica: Aproxime o cartão da antena");
                break;
            // ... mais hints
            default:
                return;
        }
        
        showTooltip(hint, HINT_DISPLAY_TIME);
    }
    
    /**
     * @brief Desenha ajuda contextual completa
     */
    void drawContextHelp(TutorialContext ctx) {
        M5.Display.fillScreen(BLACK);
        M5.Display.drawRect(5, 5, 230, 125, CYAN);
        
        M5.Display.setTextColor(CYAN);
        M5.Display.setCursor(80, 10);
        M5.Display.print(F("❓ Ajuda"));
        
        M5.Display.setTextColor(WHITE);
        
        // Conteúdo específico por contexto
        switch (ctx) {
            case CONTEXT_WIFI_SCANNER:
                M5.Display.setCursor(10, 30);
                M5.Display.print(F("WiFi Scanner"));
                M5.Display.setCursor(10, 45);
                M5.Display.print(F("Escaneia redes próximas"));
                M5.Display.setCursor(10, 60);
                M5.Display.print(F("▲▼: Rolar lista"));
                M5.Display.setCursor(10, 70);
                M5.Display.print(F("SEL: Ver detalhes"));
                break;
                
            // ... mais contextos
                
            default:
                M5.Display.setCursor(10, 60);
                M5.Display.print(F("Sem ajuda disponível"));
                break;
        }
        
        M5.Display.setTextColor(YELLOW);
        M5.Display.setCursor(60, 110);
        M5.Display.print(F("BACK: Fechar"));
    }
    
    /**
     * @brief Mostra tooltip temporário
     */
    void showTooltip(String message, unsigned long duration) {
        tooltipMessage = message;
        tooltipStartTime = millis();
        showingTooltip = true;
    }
    
    /**
     * @brief Desenha tooltip na tela
     */
    void drawTooltip() {
        if (!showingTooltip) return;
        
        // Check timeout
        if (millis() - tooltipStartTime > TOOLTIP_DISPLAY_TIME) {
            showingTooltip = false;
            return;
        }
        
        // Desenhar tooltip no topo
        M5.Display.fillRect(10, 5, 220, 25, 0x2965);  // Dark blue
        M5.Display.drawRect(10, 5, 220, 25, YELLOW);
        
        M5.Display.setTextColor(WHITE);
        M5.Display.setCursor(15, 12);
        M5.Display.print(tooltipMessage.substring(0, 35));
    }
    
    // ==================== CONTROLS ====================
    
    /**
     * @brief Processa input do usuário no tutorial
     */
    void handleUniversalControls(int action) {
        if (!isActive) return;
        
        switch (action) {
            case ACTION_SELECT:
            case ACTION_RIGHT:
                nextStep();
                break;
                
            case ACTION_LEFT:
                previousStep();
                break;
                
            case ACTION_BACK:
                if (currentStep == STEP_WELCOME) {
                    // Primeira tela, oferecer pular
                    skipTutorial();
                } else {
                    previousStep();
                }
                break;
                
            default:
                break;
        }
    }
    
    // ==================== API ====================
    
    bool isFirstRun() const { return firstRun; }
    bool isTutorialActive() const { return isActive; }
    bool isHelpEnabled() const { return helpEnabled; }
    TutorialContext getCurrentContext() const { return currentContext; }
    
    void setLanguage(Language lang) {
        language = lang;
        prefs.putUChar("language", lang);
    }
    
    void setHelpEnabled(bool enabled) {
        helpEnabled = enabled;
        prefs.putBool("help_enabled", enabled);
    }
    
    void resetTutorial() {
        prefs.clear();
        firstRun = true;
        progress = {};
        memset(hintsShown, 0, sizeof(hintsShown));
        memset(contextVisited, 0, sizeof(contextVisited));
    }
    
    /**
     * @brief Update loop - desenha tooltips etc
     */
    void update() {
        if (showingTooltip) {
            drawTooltip();
        }
        
        // Animações
        if (millis() - lastAnimUpdate > 100) {
            animationFrame++;
            lastAnimUpdate = millis();
        }
    }
};

#endif // M5GOTCHI_TUTORIAL_SYSTEM_H
