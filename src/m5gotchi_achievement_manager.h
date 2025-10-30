/**
 * @file m5gotchi_achievement_manager.h
 * @brief Sistema de gerenciamento de conquistas (lógica pura)
 * @version 2.0
 * @date 2025-10-26
 * 
 * @details
 * Refatoração do sistema de achievements separando LÓGICA de UI.
 * Esta classe gerencia apenas a lógica de achievements:
 * - Progress tracking
 * - Unlock detection
 * - Statistics
 * - Persistence
 * 
 * A UI está em m5gotchi_achievement_ui.h
 * 
 * MELHORIAS v2.0:
 * - Complexidade ciclomática reduzida (CC < 10)
 * - Event-driven com lookup table (O(1))
 * - Cache de achievements desbloqueados
 * - Separação de concerns (SRP)
 * 
 * @example
 * ```cpp
 * AchievementManager mgr;
 * mgr.init();
 * 
 * // Reportar evento
 * mgr.onEvent(EVENT_WIFI_SCAN);
 * 
 * // Checar novos unlocks
 * if (mgr.hasNewUnlocks()) {
 *     auto unlocked = mgr.getNewUnlocks();
 *     // Mostrar notificação
 * }
 * ```
 */

#ifndef M5GOTCHI_ACHIEVEMENT_MANAGER_H
#define M5GOTCHI_ACHIEVEMENT_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <map>
#include <vector>
#include "ConfigManager.h"
#include "m5gotchi_progmem_strings.h"

// ==================== CONFIGURATION ====================
namespace AchievementConfig {
    constexpr size_t MAX_ACHIEVEMENTS = static_cast<size_t>(Config::Achievements::MAX_ACHIEVEMENTS);
    constexpr size_t MAX_EVENT_TYPES = 30;
    constexpr size_t CACHE_SIZE = 20;
}

// ==================== EVENT TYPES ====================
/**
 * @enum AchievementEvent
 * @brief Tipos de eventos que podem desbloquear achievements
 */
enum AchievementEvent {
    EVENT_WIFI_SCAN = 0,
    EVENT_WIFI_HANDSHAKE,
    EVENT_WIFI_DEAUTH,
    EVENT_PORTAL_CREATE,
    EVENT_BLE_SPAM,
    EVENT_BLE_SCAN,
    EVENT_GAME_PLAY,
    EVENT_GAME_WIN,
    EVENT_HIGH_SCORE,
    EVENT_SOUND_PLAY,
    EVENT_PERSONALITY_UNLOCK,
    EVENT_THEME_UNLOCK,
    EVENT_FRIEND_ADD,
    EVENT_RFID_SCAN,
    EVENT_RFID_CLONE,
    EVENT_GPS_WAYPOINT,
    EVENT_IR_TRANSMIT,
    EVENT_RF433_CAPTURE,
    EVENT_ATTACK_SUCCESS,
    EVENT_BUTTON_PRESS,
    EVENT_SCREEN_VIEW,
    EVENT_EASTER_EGG,
    EVENT_COUNT
};

// ==================== ACHIEVEMENT DATA ====================

/**
 * @enum AchievementCategory
 * @brief Categorias de achievements
 */
enum AchievementCategory {
    CAT_HACKING = 0,
    CAT_GAMING,
    CAT_KAWAII,
    CAT_SOUND,
    CAT_DATA,
    CAT_SOCIAL,
    CAT_SECRET,
    CAT_COUNT
};

/**
 * @enum AchievementRarity
 * @brief Raridade dos achievements
 */
enum AchievementRarity {
    RARITY_BRONZE = 0,
    RARITY_SILVER,
    RARITY_GOLD,
    RARITY_PLATINUM,
    RARITY_KAWAII,
    RARITY_COUNT
};

/**
 * @struct Achievement
 * @brief Dados de um achievement (otimizado)
 */
struct Achievement {
    uint8_t id;
    uint8_t category;
    uint8_t rarity;
    uint16_t points;
    
    // Progress
    uint16_t current;
    uint16_t required;
    
    // Flags compactadas
    union {
        uint8_t flags;
        struct {
            uint8_t unlocked : 1;
            uint8_t isNew : 1;
            uint8_t hidden : 1;
            uint8_t reserved : 5;
        };
    };
    
    // Timestamp
    uint32_t unlockTime;
    
    // Metadata em PROGMEM
    const char* name_P;
    const char* desc_P;
    const char* emoji_P;
    
    // Helpers
    bool isUnlocked() const { return unlocked; }
    void setUnlocked(bool val) { unlocked = val; }
    bool getIsNew() const { return isNew; }
    void setIsNew(bool val) { isNew = val; }
    
    float getProgress() const {
        if (required == 0) return unlocked ? 100.0f : 0.0f;
        return (current * 100.0f) / required;
    }
};

/**
 * @struct PlayerStats
 * @brief Estatísticas do jogador (compactado)
 */
struct PlayerStats {
    // Counters (16-bit suficiente)
    uint16_t networksScanned;
    uint16_t handshakesCaptured;
    uint16_t deauthAttacks;
    uint16_t portalsCreated;
    uint16_t bleAttacks;
    uint16_t gamesPlayed;
    uint16_t soundsPlayed;
    uint16_t cardsScanned;
    
    // Achievement stats
    uint16_t totalAchievements;
    uint16_t totalPoints;
    uint8_t achievementsByRarity[RARITY_COUNT];
    
    // Totals
    uint32_t totalUptime;
    uint16_t buttonPresses;
};

// ==================== ACHIEVEMENT MANAGER ====================

/**
 * @class AchievementManager
 * @brief Gerenciador de lógica de achievements
 * 
 * @details
 * Classe responsável APENAS pela lógica de achievements.
 * Não contém código de UI/display.
 * 
 * Padrões aplicados:
 * - Single Responsibility Principle
 * - Event-driven architecture
 * - Caching para performance
 * - Low cyclomatic complexity (CC < 10)
 */
class AchievementManager {
private:
    // Data
    Achievement achievements[AchievementConfig::MAX_ACHIEVEMENTS];
    uint8_t achievementCount;
    PlayerStats stats;
    
    // Event mapping (O(1) lookup)
    std::map<AchievementEvent, std::vector<uint8_t>> eventToAchievements;
    
    // Cache de unlocked
    uint8_t unlockedCache[AchievementConfig::CACHE_SIZE];
    uint8_t unlockedCacheSize;
    
    // New unlocks queue
    uint8_t newUnlocks[10];
    uint8_t newUnlockCount;
    
    // Storage
    Preferences prefs;
    
    // State
    bool initialized;
    bool dirty; // Precisa salvar?
    
public:
    AchievementManager() {
        achievementCount = 0;
        unlockedCacheSize = 0;
        newUnlockCount = 0;
        initialized = false;
        dirty = false;
        memset(&stats, 0, sizeof(PlayerStats));
    }
    
    // ==================== INITIALIZATION ====================
    
    /**
     * @brief Inicializa o sistema
     * @return true se sucesso
     */
    bool init() {
        Serial.println(F("🏆 Initializing Achievement Manager..."));
        
        if (!prefs.begin("m5g_achv", false)) {
            Serial.println(F("❌ Failed to open preferences"));
            return false;
        }
        
        initAchievements();
        buildEventMap();
        loadProgress();
        updateCache();
        
        initialized = true;
        Serial.printf_P(PSTR("✅ %d achievements loaded\n"), achievementCount);
        
        return true;
    }
    
    /**
     * @brief Inicializa lista de achievements
     */
    void initAchievements() {
        // Será preenchido com dados reais
        // Por enquanto, placeholder
        achievementCount = 0;
        
        // Exemplo: First Scan
        addAchievement(0, "First Scan", "Scan your first network", "📡",
                      CAT_HACKING, RARITY_BRONZE, 1, 50);
        
        // ... adicionar todos achievements
    }
    
    /**
     * @brief Adiciona um achievement
     */
    void addAchievement(uint8_t id, const char* name, const char* desc, 
                       const char* emoji, uint8_t category, uint8_t rarity,
                       uint16_t required, uint16_t points) {
    if (achievementCount >= AchievementConfig::MAX_ACHIEVEMENTS) return;
        
        Achievement& ach = achievements[achievementCount++];
        ach.id = id;
        ach.category = category;
        ach.rarity = rarity;
        ach.points = points;
        ach.current = 0;
        ach.required = required;
        ach.flags = 0;
        ach.unlockTime = 0;
        
        // Alocar strings em heap (ou usar PROGMEM)
        ach.name_P = name;  // TODO: Copiar para PROGMEM
        ach.desc_P = desc;
        ach.emoji_P = emoji;
    }
    
    /**
     * @brief Constrói mapa de eventos para achievements
     * @details Performance O(1) ao invés de O(n) em eventos
     */
    void buildEventMap() {
        eventToAchievements.clear();
        
        // Mapear achievements por evento
        // Por exemplo:
        eventToAchievements[EVENT_WIFI_SCAN] = {0, 1, 2};  // IDs dos achievements
        eventToAchievements[EVENT_WIFI_HANDSHAKE] = {3, 4};
        // ... mapear todos
        
        Serial.println(F("📊 Event map built"));
    }
    
    // ==================== EVENT HANDLING ====================
    
    /**
     * @brief Processa um evento
     * @param event Tipo do evento
     * @param value Valor opcional (ex: quantidade)
     * @return Número de achievements desbloqueados
     */
    uint8_t onEvent(AchievementEvent event, uint16_t value = 1) {
        if (!initialized) return 0;
        
        uint8_t unlocked = 0;
        
        // Atualizar estatísticas
        updateStats(event, value);
        
        // Verificar achievements relacionados (O(1) lookup!)
        auto it = eventToAchievements.find(event);
        if (it != eventToAchievements.end()) {
            for (uint8_t achId : it->second) {
                if (checkAndUpdate(achId, value)) {
                    unlocked++;
                    dirty = true;
                }
            }
        }
        
        return unlocked;
    }
    
    /**
     * @brief Atualiza estatísticas baseado no evento
     * @param event Tipo do evento
     * @param value Valor
     */
    void updateStats(AchievementEvent event, uint16_t value) {
        switch (event) {
            case EVENT_WIFI_SCAN:
                stats.networksScanned += value;
                break;
            case EVENT_WIFI_HANDSHAKE:
                stats.handshakesCaptured += value;
                break;
            case EVENT_WIFI_DEAUTH:
                stats.deauthAttacks += value;
                break;
            case EVENT_PORTAL_CREATE:
                stats.portalsCreated += value;
                break;
            case EVENT_BLE_SPAM:
            case EVENT_BLE_SCAN:
                stats.bleAttacks += value;
                break;
            case EVENT_GAME_PLAY:
                stats.gamesPlayed += value;
                break;
            case EVENT_SOUND_PLAY:
                stats.soundsPlayed += value;
                break;
            case EVENT_RFID_SCAN:
                stats.cardsScanned += value;
                break;
            case EVENT_BUTTON_PRESS:
                stats.buttonPresses += value;
                break;
            default:
                break;
        }
    }
    
    /**
     * @brief Verifica e atualiza um achievement específico
     * @param achId ID do achievement
     * @param increment Incremento no progresso
     * @return true se foi desbloqueado nesta chamada
     */
    bool checkAndUpdate(uint8_t achId, uint16_t increment) {
        if (achId >= achievementCount) return false;
        
        Achievement& ach = achievements[achId];
        
        // Já desbloqueado
        if (ach.isUnlocked()) return false;
        
        // Atualizar progresso
        ach.current += increment;
        
        // Verificar se completou
        if (ach.current >= ach.required) {
            unlockAchievement(achId);
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief Desbloqueia um achievement
     * @param achId ID do achievement
     */
    void unlockAchievement(uint8_t achId) {
        if (achId >= achievementCount) return;
        
        Achievement& ach = achievements[achId];
        if (ach.isUnlocked()) return;
        
        ach.setUnlocked(true);
        ach.setIsNew(true);
        ach.unlockTime = millis();
        
        // Atualizar estatísticas
        stats.totalAchievements++;
        stats.totalPoints += ach.points;
        stats.achievementsByRarity[ach.rarity]++;
        
        // Adicionar à fila de novos
        if (newUnlockCount < 10) {
            newUnlocks[newUnlockCount++] = achId;
        }
        
        // Atualizar cache
        updateCache();
        
        Serial.printf_P(PSTR("🏆 Achievement unlocked: %d\n"), achId);
        dirty = true;
    }
    
    // ==================== CACHING ====================
    
    /**
     * @brief Atualiza cache de achievements desbloqueados
     * @details Melhora performance de queries frequentes
     */
    void updateCache() {
        unlockedCacheSize = 0;
        
    for (uint8_t i = 0; i < achievementCount && unlockedCacheSize < AchievementConfig::CACHE_SIZE; i++) {
            if (achievements[i].isUnlocked()) {
                unlockedCache[unlockedCacheSize++] = i;
            }
        }
    }
    
    /**
     * @brief Verifica se achievement está no cache (rápido)
     */
    bool isInCache(uint8_t achId) const {
        for (uint8_t i = 0; i < unlockedCacheSize; i++) {
            if (unlockedCache[i] == achId) return true;
        }
        return false;
    }
    
    // ==================== QUERIES ====================
    
    /**
     * @brief Retorna achievement por ID
     */
    const Achievement* getAchievement(uint8_t achId) const {
        if (achId >= achievementCount) return nullptr;
        return &achievements[achId];
    }
    
    /**
     * @brief Retorna todos achievements
     */
    const Achievement* getAllAchievements() const {
        return achievements;
    }
    
    uint8_t getCount() const { return achievementCount; }
    
    /**
     * @brief Conta achievements por categoria
     */
    uint8_t countByCategory(uint8_t category) const {
        uint8_t count = 0;
        for (uint8_t i = 0; i < achievementCount; i++) {
            if (achievements[i].category == category) count++;
        }
        return count;
    }
    
    /**
     * @brief Conta achievements desbloqueados
     */
    uint8_t countUnlocked() const {
        return stats.totalAchievements;
    }
    
    /**
     * @brief Retorna progresso geral (0-100%)
     */
    float getOverallProgress() const {
        if (achievementCount == 0) return 0.0f;
        return (stats.totalAchievements * 100.0f) / achievementCount;
    }
    
    /**
     * @brief Retorna estatísticas
     */
    const PlayerStats& getStats() const {
        return stats;
    }
    
    /**
     * @brief Verifica se há novos unlocks
     */
    bool hasNewUnlocks() const {
        return newUnlockCount > 0;
    }
    
    /**
     * @brief Retorna e limpa fila de novos unlocks
     */
    std::vector<uint8_t> getNewUnlocks() {
        std::vector<uint8_t> result;
        for (uint8_t i = 0; i < newUnlockCount; i++) {
            result.push_back(newUnlocks[i]);
        }
        newUnlockCount = 0;
        return result;
    }
    
    /**
     * @brief Limpa flags isNew
     */
    void clearNewFlags() {
        for (uint8_t i = 0; i < achievementCount; i++) {
            achievements[i].setIsNew(false);
        }
    }
    
    // ==================== PERSISTENCE ====================
    
    /**
     * @brief Salva progresso
     */
    void save() {
        if (!dirty) return;
        
        Serial.println(F("💾 Saving achievement progress..."));
        
        // Salvar stats
        prefs.putBytes("stats", &stats, sizeof(PlayerStats));
        
        // Salvar achievements (apenas progresso e flags)
        for (uint8_t i = 0; i < achievementCount; i++) {
            char key[16];
            snprintf(key, sizeof(key), "ach_%d", i);
            
            struct SaveData {
                uint16_t current;
                uint8_t flags;
                uint32_t unlockTime;
            } data;
            
            data.current = achievements[i].current;
            data.flags = achievements[i].flags;
            data.unlockTime = achievements[i].unlockTime;
            
            prefs.putBytes(key, &data, sizeof(SaveData));
        }
        
        dirty = false;
        Serial.println(F("✅ Progress saved"));
    }
    
    /**
     * @brief Carrega progresso
     */
    void loadProgress() {
        Serial.println(F("📂 Loading achievement progress..."));
        
        // Carregar stats
        if (prefs.getBytesLength("stats") == sizeof(PlayerStats)) {
            prefs.getBytes("stats", &stats, sizeof(PlayerStats));
        }
        
        // Carregar achievements
        for (uint8_t i = 0; i < achievementCount; i++) {
            char key[16];
            snprintf(key, sizeof(key), "ach_%d", i);
            
            struct SaveData {
                uint16_t current;
                uint8_t flags;
                uint32_t unlockTime;
            } data;
            
            if (prefs.getBytesLength(key) == sizeof(SaveData)) {
                prefs.getBytes(key, &data, sizeof(SaveData));
                
                achievements[i].current = data.current;
                achievements[i].flags = data.flags;
                achievements[i].unlockTime = data.unlockTime;
            }
        }
        
        Serial.printf_P(PSTR("✅ Progress loaded: %d/%d unlocked\n"),
                       stats.totalAchievements, achievementCount);
    }
    
    /**
     * @brief Reseta todo o progresso
     */
    void reset() {
        prefs.clear();
        memset(&stats, 0, sizeof(PlayerStats));
        
        for (uint8_t i = 0; i < achievementCount; i++) {
            achievements[i].current = 0;
            achievements[i].flags = 0;
            achievements[i].unlockTime = 0;
        }
        
        unlockedCacheSize = 0;
        newUnlockCount = 0;
        dirty = true;
        
        Serial.println(F("🔄 Achievement progress reset"));
    }
    
    /**
     * @brief Auto-save periódico
     */
    void update() {
        static unsigned long lastSave = 0;
        
        // Auto-save a cada 5 minutos
        if (dirty && millis() - lastSave > 300000) {
            save();
            lastSave = millis();
        }
    }
};

#endif // M5GOTCHI_ACHIEVEMENT_MANAGER_H
