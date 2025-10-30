/**
 * @file m5gotchi_field_ops_mode.h
 * @brief Modo operacional para uso profissional em campo
 *
 * Permite alternar entre modo kawaii/tamagotchi e modo operacional profissional,
 * otimizando recursos, bateria e apresentação para pentest em campo.
 *
 * @version 1.0.0
 * @date 2025-10-30
 */

#ifndef M5GOTCHI_FIELD_OPS_MODE_H
#define M5GOTCHI_FIELD_OPS_MODE_H

#include <Preferences.h>
#include <M5Unified.h>

/**
 * @brief Gerenciador de modos operacionais
 *
 * Oferece diferentes perfis de operação:
 * - NEKO_MODE: Modo tamagotchi completo com animações e personalidade
 * - PROFESSIONAL: Modo profissional com interface limpa
 * - STEALTH: Modo furtivo com mínimo de feedback visual/sonoro
 * - BATTERY_SAVER: Otimização extrema de bateria
 */
class FieldOpsMode {
public:
    /**
     * @brief Modos de operação
     */
    enum Mode {
        NEKO_MODE,          // Modo completo tamagotchi
        PROFESSIONAL,       // Modo profissional
        STEALTH,           // Modo stealth/furtivo
        BATTERY_SAVER,     // Economia de bateria
        CUSTOM             // Configuração customizada
    };

    /**
     * @brief Perfil de configuração para cada modo
     */
    struct Profile {
        // Visual
        bool animations = true;
        bool particles = true;
        bool nekoReactions = true;
        bool statusIcons = true;
        bool colorTheme = true;
        uint8_t brightness = 128;  // 0-255
        uint8_t screenTimeout = 60; // segundos (0 = nunca)

        // Audio
        bool sounds = true;
        bool music = true;
        bool beeps = true;
        uint8_t volume = 50;       // 0-100

        // Feedback
        bool vibration = true;
        bool ledIndicators = true;
        bool serialOutput = true;
        bool verboseLogging = false;

        // Performance
        uint16_t displayFPS = 30;
        bool reducedAnimations = false;
        bool minimalUI = false;

        // Energia
        bool cpuPowerSave = false;
        bool wifiPowerSave = false;
        bool autoDimming = true;
        uint32_t sleepTimeout = 300000; // 5min

        // Pentest
        bool autoSaveCaptures = true;
        bool continuousMonitor = false;
        bool aggressiveScanning = false;
    };

    /**
     * @brief Estatísticas de bateria
     */
    struct BatteryStats {
        uint8_t percentage;
        uint16_t voltage;          // mV
        int16_t current;           // mA
        bool charging;
        uint32_t estimatedRuntime; // minutos
        String status;
    };

private:
    Preferences prefs;
    Mode currentMode = NEKO_MODE;
    Profile currentProfile;

    // Estatísticas
    uint32_t modeStartTime = 0;
    uint32_t screenOnTime = 0;
    uint32_t lastActivity = 0;

    /**
     * @brief Perfis pré-configurados
     */
    Profile getPresetProfile(Mode mode) {
        Profile profile;

        switch(mode) {
            case NEKO_MODE:
                // Modo completo com todas features
                profile.animations = true;
                profile.particles = true;
                profile.nekoReactions = true;
                profile.sounds = true;
                profile.music = true;
                profile.vibration = true;
                profile.brightness = 180;
                profile.displayFPS = 30;
                profile.verboseLogging = false;
                profile.minimalUI = false;
                break;

            case PROFESSIONAL:
                // Modo profissional clean
                profile.animations = false;
                profile.particles = false;
                profile.nekoReactions = false;
                profile.sounds = false;
                profile.music = false;
                profile.vibration = false;
                profile.beeps = true;
                profile.brightness = 150;
                profile.displayFPS = 20;
                profile.verboseLogging = true;
                profile.minimalUI = true;
                profile.statusIcons = true;
                break;

            case STEALTH:
                // Modo stealth - mínimo feedback
                profile.animations = false;
                profile.particles = false;
                profile.nekoReactions = false;
                profile.sounds = false;
                profile.music = false;
                profile.vibration = false;
                profile.beeps = false;
                profile.ledIndicators = false;
                profile.brightness = 80;
                profile.displayFPS = 15;
                profile.verboseLogging = false;
                profile.minimalUI = true;
                profile.autoDimming = true;
                profile.screenTimeout = 30;
                break;

            case BATTERY_SAVER:
                // Economia máxima de bateria
                profile.animations = false;
                profile.particles = false;
                profile.nekoReactions = false;
                profile.sounds = false;
                profile.music = false;
                profile.vibration = false;
                profile.beeps = false;
                profile.brightness = 50;
                profile.displayFPS = 10;
                profile.cpuPowerSave = true;
                profile.wifiPowerSave = true;
                profile.autoDimming = true;
                profile.screenTimeout = 15;
                profile.sleepTimeout = 60000; // 1min
                profile.reducedAnimations = true;
                profile.minimalUI = true;
                break;

            case CUSTOM:
                // Mantém configuração atual
                profile = currentProfile;
                break;
        }

        return profile;
    }

    /**
     * @brief Aplica perfil ao sistema
     */
    void applyProfile(const Profile& profile) {
        currentProfile = profile;

        // Aplica configurações de display
        M5.Display.setBrightness(profile.brightness);

        // Aplica configurações de CPU
        if (profile.cpuPowerSave) {
            setCpuFrequencyMhz(80); // Reduz para 80MHz
        } else {
            setCpuFrequencyMhz(240); // Full speed
        }

        // Aplica configurações de WiFi
        if (profile.wifiPowerSave) {
            // esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
            // TODO: Implementar power save WiFi
        }

        Serial.println("[OpsMode] ⚙️ Perfil aplicado");
    }

    /**
     * @brief Salva perfil nas preferências
     */
    void saveProfile() {
        prefs.putUChar("mode", currentMode);
        prefs.putBool("animations", currentProfile.animations);
        prefs.putBool("sounds", currentProfile.sounds);
        prefs.putUChar("brightness", currentProfile.brightness);
        prefs.putUChar("fps", currentProfile.displayFPS);
        // ... salvar outros parâmetros conforme necessário
    }

    /**
     * @brief Carrega perfil das preferências
     */
    void loadProfile() {
        currentMode = (Mode)prefs.getUChar("mode", NEKO_MODE);
        currentProfile = getPresetProfile(currentMode);

        // Sobrescreve com valores customizados se existirem
        if (prefs.isKey("animations")) {
            currentProfile.animations = prefs.getBool("animations");
        }
        // ... carregar outros parâmetros
    }

public:
    /**
     * @brief Inicializa gerenciador de modos
     */
    bool begin() {
        prefs.begin("opsmode", false);

        // Carrega modo salvo
        loadProfile();

        modeStartTime = millis();
        lastActivity = millis();

        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║     ⚙️  FIELD OPS MODE MANAGER        ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Current Mode: %-20s   ║\n", getModeString().c_str());
        Serial.printf( "║  Brightness: %-3d                     ║\n",
                      currentProfile.brightness);
        Serial.printf( "║  FPS: %-2d                             ║\n",
                      currentProfile.displayFPS);
        Serial.printf( "║  Animations: %-3s                     ║\n",
                      currentProfile.animations ? "ON" : "OFF");
        Serial.println("╚════════════════════════════════════════╝\n");

        // Aplica perfil
        applyProfile(currentProfile);

        return true;
    }

    /**
     * @brief Alterna para um modo específico
     */
    void setMode(Mode mode) {
        if (mode == currentMode) return;

        Serial.printf("[OpsMode] 🔄 Alternando: %s → %s\n",
                     getModeString().c_str(),
                     getModeString(mode).c_str());

        currentMode = mode;
        Profile newProfile = getPresetProfile(mode);
        applyProfile(newProfile);
        saveProfile();

        modeStartTime = millis();
    }

    /**
     * @brief Alterna entre modos ciclicamente
     */
    void toggleMode() {
        Mode nextMode = (Mode)((currentMode + 1) % 4);
        setMode(nextMode);
    }

    /**
     * @brief Ativa modo NEKO completo
     */
    void enableNekoMode() {
        setMode(NEKO_MODE);
    }

    /**
     * @brief Ativa modo profissional
     */
    void enableProfessionalMode() {
        setMode(PROFESSIONAL);
    }

    /**
     * @brief Ativa modo stealth
     */
    void enableStealthMode() {
        setMode(STEALTH);
    }

    /**
     * @brief Ativa economia de bateria
     */
    void enableBatterySaver() {
        setMode(BATTERY_SAVER);
    }

    /**
     * @brief Registra atividade do usuário
     */
    void userActivity() {
        lastActivity = millis();

        // Se tela estava apagada, reacende
        if (M5.Display.getBrightness() == 0) {
            M5.Display.setBrightness(currentProfile.brightness);
        }
    }

    /**
     * @brief Atualiza gerenciamento de energia (chame no loop)
     */
    void update() {
        uint32_t now = millis();
        uint32_t idleTime = now - lastActivity;

        // Auto-dimming
        if (currentProfile.autoDimming && currentProfile.screenTimeout > 0) {
            uint32_t timeout = currentProfile.screenTimeout * 1000;

            if (idleTime >= timeout) {
                // Desliga tela
                if (M5.Display.getBrightness() > 0) {
                    M5.Display.setBrightness(0);
                    Serial.println("[OpsMode] 💤 Tela desligada (timeout)");
                }
            }
        }

        // Auto-sleep
        if (currentProfile.sleepTimeout > 0 && idleTime >= currentProfile.sleepTimeout) {
            Serial.println("[OpsMode] 😴 Entrando em sleep mode...");
            // TODO: Implementar deep sleep
        }
    }

    /**
     * @brief Obtém estatísticas de bateria
     */
    BatteryStats getBatteryStats() {
        BatteryStats stats;

        // M5Stack Cardputer tem gerenciamento de bateria
        // TODO: Implementar leitura real da bateria via I2C/ADC

        stats.percentage = 85; // Placeholder
        stats.voltage = 3700;  // mV
        stats.current = -150;  // mA (negativo = descarga)
        stats.charging = false;
        stats.estimatedRuntime = 120; // minutos

        if (stats.charging) {
            stats.status = "Charging";
        } else if (stats.percentage > 80) {
            stats.status = "Good";
        } else if (stats.percentage > 20) {
            stats.status = "Normal";
        } else {
            stats.status = "Low";
        }

        return stats;
    }

    /**
     * @brief Imprime status de bateria
     */
    void printBatteryStatus() {
        BatteryStats bat = getBatteryStats();

        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║         🔋 BATTERY STATUS             ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Level: %3d%%                          ║\n", bat.percentage);
        Serial.printf( "║  Voltage: %4d mV                     ║\n", bat.voltage);
        Serial.printf( "║  Current: %4d mA                     ║\n", bat.current);
        Serial.printf( "║  Status: %-28s ║\n", bat.status.c_str());
        Serial.printf( "║  Charging: %-3s                       ║\n",
                      bat.charging ? "YES" : "NO");
        Serial.printf( "║  Est. Runtime: ~%3lu min              ║\n",
                      bat.estimatedRuntime);
        Serial.println("╚════════════════════════════════════════╝\n");
    }

    /**
     * @brief Imprime configuração atual
     */
    void printCurrentConfig() {
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║      📋 CURRENT CONFIGURATION         ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Mode: %-30s ║\n", getModeString().c_str());
        Serial.println("╟────────────────────────────────────────╢");
        Serial.printf( "║  Animations: %-3s   Particles: %-3s    ║\n",
                      currentProfile.animations ? "ON" : "OFF",
                      currentProfile.particles ? "ON" : "OFF");
        Serial.printf( "║  Sounds: %-3s       Music: %-3s        ║\n",
                      currentProfile.sounds ? "ON" : "OFF",
                      currentProfile.music ? "ON" : "OFF");
        Serial.printf( "║  Vibration: %-3s   LED: %-3s          ║\n",
                      currentProfile.vibration ? "ON" : "OFF",
                      currentProfile.ledIndicators ? "ON" : "OFF");
        Serial.printf( "║  Brightness: %-3d   FPS: %-2d          ║\n",
                      currentProfile.brightness, currentProfile.displayFPS);
        Serial.printf( "║  Screen Timeout: %-3ds                ║\n",
                      currentProfile.screenTimeout);
        Serial.println("╚════════════════════════════════════════╝\n");
    }

    /**
     * @brief Obtém string do modo
     */
    String getModeString(Mode mode) const {
        switch(mode) {
            case NEKO_MODE:       return "🐱 Neko Mode";
            case PROFESSIONAL:    return "💼 Professional";
            case STEALTH:        return "🥷 Stealth";
            case BATTERY_SAVER:  return "🔋 Battery Saver";
            case CUSTOM:         return "⚙️ Custom";
            default:             return "❓ Unknown";
        }
    }

    String getModeString() const {
        return getModeString(currentMode);
    }

    // Getters para módulos consultarem configuração
    const Profile& getProfile() const { return currentProfile; }
    Mode getMode() const { return currentMode; }

    bool isAnimationsEnabled() const { return currentProfile.animations; }
    bool isParticlesEnabled() const { return currentProfile.particles; }
    bool isNekoReactionsEnabled() const { return currentProfile.nekoReactions; }
    bool isSoundsEnabled() const { return currentProfile.sounds; }
    bool isMusicEnabled() const { return currentProfile.music; }
    bool isVibrationEnabled() const { return currentProfile.vibration; }
    bool isMinimalUI() const { return currentProfile.minimalUI; }
    uint8_t getDisplayFPS() const { return currentProfile.displayFPS; }
    uint8_t getBrightness() const { return currentProfile.brightness; }

    // Setters para configuração customizada
    void setAnimations(bool enabled) {
        currentProfile.animations = enabled;
        saveProfile();
    }

    void setBrightness(uint8_t level) {
        currentProfile.brightness = level;
        M5.Display.setBrightness(level);
        saveProfile();
    }

    void setDisplayFPS(uint8_t fps) {
        currentProfile.displayFPS = fps;
        saveProfile();
    }
};

#endif // M5GOTCHI_FIELD_OPS_MODE_H
