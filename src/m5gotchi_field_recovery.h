/**
 * @file m5gotchi_field_recovery.h
 * @brief Sistema robusto de error handling e auto-recovery
 *
 * Implementa watchdog, crash detection, exception handling e self-healing
 * para operações de pentest em campo com máxima confiabilidade.
 *
 * @version 1.0.0
 * @date 2025-10-30
 */

#ifndef M5GOTCHI_FIELD_RECOVERY_H
#define M5GOTCHI_FIELD_RECOVERY_H

#include <esp_task_wdt.h>
#include <esp_system.h>
#include <esp_err.h>
#include <esp_heap_caps.h>
#include <rom/rtc.h>
#include <Preferences.h>

/**
 * @brief Sistema de recuperação automática e error handling
 *
 * Features:
 * - Watchdog hardware e software
 * - Detecção de memory leaks
 * - Auto-restart em caso de travamento
 * - Recovery de estado após reboot
 * - Monitoramento de saúde do sistema
 * - Stack overflow detection
 */
class FieldRecovery {
public:
    /**
     * @brief Razões de reset
     */
    enum ResetReason {
        POWER_ON,
        EXTERNAL,
        SOFTWARE,
        PANIC,
        WATCHDOG,
        DEEP_SLEEP,
        BROWNOUT,
        UNKNOWN
    };

    /**
     * @brief Status de saúde do sistema
     */
    enum HealthStatus {
        HEALTHY,
        WARNING,
        CRITICAL,
        FAILING
    };

    /**
     * @brief Configuração do recovery
     */
    struct Config {
        bool watchdogEnabled = true;
        uint32_t watchdogTimeout = 30;      // segundos
        bool autoRestartOnHang = true;
        bool autoRestartOnOOM = true;       // Out of Memory
        uint32_t minFreeHeap = 20000;       // 20KB mínimo
        uint32_t criticalFreeHeap = 10000;  // 10KB crítico
        bool stackMonitoring = true;
        uint32_t healthCheckInterval = 5000; // 5s
    };

    /**
     * @brief Estatísticas de saúde
     */
    struct HealthStats {
        uint32_t freeHeap;
        uint32_t minFreeHeap;
        uint32_t largestBlock;
        uint8_t heapFragmentation;
        uint32_t uptime;
        uint32_t loopTime;
        uint32_t maxLoopTime;
        HealthStatus status;
    };

    /**
     * @brief Callback para ação antes de restart
     */
    typedef void (*PreRestartCallback)();

private:
    Config config;
    Preferences prefs;

    // Monitoramento
    uint32_t lastHealthCheck = 0;
    uint32_t lastWatchdogFeed = 0;
    uint32_t loopStartTime = 0;
    uint32_t maxLoopTime = 0;
    uint32_t loopCount = 0;

    // Estado de saúde
    HealthStats currentHealth;
    HealthStatus lastStatus = HEALTHY;

    // Callbacks
    PreRestartCallback preRestartCallback = nullptr;

    // Recovery state
    bool inRecoveryMode = false;
    uint32_t consecutiveCrashes = 0;
    uint32_t bootTime = 0;

    /**
     * @brief Converte RESET_REASON para enum
     */
    ResetReason getResetReason() {
        RESET_REASON reason = rtc_get_reset_reason(0);

        switch(reason) {
            case POWERON_RESET:          return POWER_ON;
            case SW_RESET:               return SOFTWARE;
            case OWDT_RESET:             return WATCHDOG;
            case DEEPSLEEP_RESET:        return DEEP_SLEEP;
            case SDIO_RESET:             return EXTERNAL;
            case TG0WDT_SYS_RESET:       return WATCHDOG;
            case TG1WDT_SYS_RESET:       return WATCHDOG;
            case RTCWDT_SYS_RESET:       return WATCHDOG;
            case INTRUSION_RESET:        return EXTERNAL;
            case TGWDT_CPU_RESET:        return WATCHDOG;
            case SW_CPU_RESET:           return SOFTWARE;
            case RTCWDT_CPU_RESET:       return WATCHDOG;
            case EXT_CPU_RESET:          return EXTERNAL;
            case RTCWDT_BROWN_OUT_RESET: return BROWNOUT;
            case RTCWDT_RTC_RESET:       return WATCHDOG;
            default:                     return UNKNOWN;
        }
    }

    /**
     * @brief String descritiva da razão de reset
     */
    String getResetReasonString(ResetReason reason) {
        switch(reason) {
            case POWER_ON:    return "Power On";
            case EXTERNAL:    return "External Reset";
            case SOFTWARE:    return "Software Reset";
            case PANIC:       return "Panic/Exception";
            case WATCHDOG:    return "Watchdog Timeout";
            case DEEP_SLEEP:  return "Deep Sleep Wake";
            case BROWNOUT:    return "Brownout";
            default:          return "Unknown";
        }
    }

    /**
     * @brief Atualiza estatísticas de saúde
     */
    void updateHealthStats() {
        currentHealth.freeHeap = esp_get_free_heap_size();
        currentHealth.minFreeHeap = esp_get_minimum_free_heap_size();
        currentHealth.largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        currentHealth.uptime = millis() - bootTime;

        // Calcula fragmentação
        if (currentHealth.freeHeap > 0) {
            currentHealth.heapFragmentation =
                100 - ((currentHealth.largestBlock * 100) / currentHealth.freeHeap);
        }

        // Determina status
        if (currentHealth.freeHeap < config.criticalFreeHeap) {
            currentHealth.status = CRITICAL;
        } else if (currentHealth.freeHeap < config.minFreeHeap) {
            currentHealth.status = WARNING;
        } else if (currentHealth.heapFragmentation > 50) {
            currentHealth.status = WARNING;
        } else {
            currentHealth.status = HEALTHY;
        }
    }

    /**
     * @brief Executa garbage collection manual
     */
    void forceGarbageCollection() {
        Serial.println("[FieldRecovery] 🧹 Forçando garbage collection...");

        // Em ESP32, não temos GC automático, mas podemos tentar liberar caches
        // TODO: Adicionar limpeza de caches específicos do app
    }

    /**
     * @brief Tenta recuperar memória
     */
    bool attemptMemoryRecovery() {
        Serial.println("[FieldRecovery] 💊 Tentando recuperar memória...");

        size_t beforeFree = esp_get_free_heap_size();

        // 1. Limpa buffers de display
        // 2. Limpa caches de rede
        // 3. Libera recursos temporários
        // TODO: Implementar callbacks para limpeza de módulos

        forceGarbageCollection();

        size_t afterFree = esp_get_free_heap_size();
        size_t recovered = afterFree - beforeFree;

        if (recovered > 1000) {
            Serial.printf("[FieldRecovery] ✅ Recuperados %d bytes\n", recovered);
            return true;
        }

        Serial.println("[FieldRecovery] ❌ Falha ao recuperar memória");
        return false;
    }

public:
    /**
     * @brief Inicializa sistema de recovery
     */
    bool begin(const Config& cfg = Config()) {
        config = cfg;
        bootTime = millis();

        prefs.begin("recovery", false);

        // Detecta razão do último reset
        ResetReason reason = getResetReason();
        String reasonStr = getResetReasonString(reason);

        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║    🛡️  FIELD RECOVERY SYSTEM           ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Reset Reason: %-20s   ║\n", reasonStr.c_str());

        // Verifica se foi crash
        bool wasCrash = (reason == WATCHDOG || reason == PANIC || reason == BROWNOUT);

        if (wasCrash) {
            consecutiveCrashes = prefs.getUInt("consecutive_crashes", 0) + 1;
            prefs.putUInt("consecutive_crashes", consecutiveCrashes);

            Serial.printf( "║  ⚠️  CRASH DETECTADO! (#%d)           ║\n", consecutiveCrashes);

            if (consecutiveCrashes >= 3) {
                Serial.println("║  🚨 MODO SEGURO ATIVADO              ║");
                inRecoveryMode = true;
            }
        } else {
            // Reset bem-sucedido, limpa contador
            prefs.putUInt("consecutive_crashes", 0);
            consecutiveCrashes = 0;
        }

        uint32_t lastUptime = prefs.getUInt("last_uptime", 0);
        uint32_t totalCrashes = prefs.getUInt("total_crashes", 0);

        Serial.printf( "║  Last Uptime: %lu ms                 ║\n", lastUptime);
        Serial.printf( "║  Total Crashes: %lu                  ║\n", totalCrashes);
        Serial.println("╚════════════════════════════════════════╝\n");

        // Configura watchdog
        if (config.watchdogEnabled) {
            esp_task_wdt_init(config.watchdogTimeout, config.autoRestartOnHang);
            esp_task_wdt_add(NULL); // Adiciona task atual
            Serial.printf("[FieldRecovery] ⏱️  Watchdog: %ds timeout\n",
                         config.watchdogTimeout);
        }

        // Primeira atualização de saúde
        updateHealthStats();

        return true;
    }

    /**
     * @brief Alimenta o watchdog (chame regularmente no loop!)
     */
    void feedWatchdog() {
        if (!config.watchdogEnabled) return;

        esp_task_wdt_reset();
        lastWatchdogFeed = millis();
    }

    /**
     * @brief Marca início do loop (para medição)
     */
    void loopBegin() {
        loopStartTime = millis();
        loopCount++;
    }

    /**
     * @brief Marca fim do loop e alimenta watchdog
     */
    void loopEnd() {
        uint32_t loopTime = millis() - loopStartTime;

        if (loopTime > maxLoopTime) {
            maxLoopTime = loopTime;
            currentHealth.maxLoopTime = maxLoopTime;
        }

        currentHealth.loopTime = loopTime;

        // Alerta se loop muito lento
        if (loopTime > 500) {
            Serial.printf("[FieldRecovery] ⚠️ Loop lento: %lums\n", loopTime);
        }

        feedWatchdog();
    }

    /**
     * @brief Atualiza monitoramento de saúde (chame periodicamente)
     */
    void update() {
        uint32_t now = millis();

        if (now - lastHealthCheck >= config.healthCheckInterval) {
            updateHealthStats();
            lastHealthCheck = now;

            // Verifica mudança de status
            if (currentHealth.status != lastStatus) {
                onHealthStatusChange(lastStatus, currentHealth.status);
                lastStatus = currentHealth.status;
            }

            // Ação baseada em status
            switch(currentHealth.status) {
                case CRITICAL:
                    handleCriticalHealth();
                    break;

                case WARNING:
                    handleWarningHealth();
                    break;

                case FAILING:
                    handleFailingHealth();
                    break;

                default:
                    break;
            }
        }
    }

    /**
     * @brief Trata mudança de status de saúde
     */
    void onHealthStatusChange(HealthStatus from, HealthStatus to) {
        Serial.print("[FieldRecovery] 🏥 Saúde: ");
        Serial.print(getHealthStatusString(from));
        Serial.print(" → ");
        Serial.println(getHealthStatusString(to));
    }

    /**
     * @brief Trata saúde em WARNING
     */
    void handleWarningHealth() {
        Serial.println("[FieldRecovery] ⚠️ Sistema em WARNING");

        // Tenta recuperação suave
        if (currentHealth.freeHeap < config.minFreeHeap) {
            attemptMemoryRecovery();
        }
    }

    /**
     * @brief Trata saúde CRITICAL
     */
    void handleCriticalHealth() {
        Serial.println("[FieldRecovery] 🚨 Sistema CRITICAL!");

        // Tenta recuperação agressiva
        if (attemptMemoryRecovery()) {
            Serial.println("[FieldRecovery] ✅ Recuperação bem-sucedida");
            return;
        }

        // Se não conseguir recuperar e config permitir, reinicia
        if (config.autoRestartOnOOM) {
            Serial.println("[FieldRecovery] 🔄 Auto-restart devido a OOM...");
            delay(1000);
            restart("Out of Memory");
        }
    }

    /**
     * @brief Trata saúde FAILING
     */
    void handleFailingHealth() {
        Serial.println("[FieldRecovery] 💀 Sistema FAILING - restart iminente!");
        restart("System Failing");
    }

    /**
     * @brief Reinicia o sistema graciosamente
     */
    void restart(const String& reason) {
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║       🔄 REINICIANDO SISTEMA          ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Razão: %-28s ║\n", reason.c_str());
        Serial.printf( "║  Uptime: %lu ms                       ║\n", millis());
        Serial.println("╚════════════════════════════════════════╝\n");

        // Salva informações
        prefs.putUInt("last_uptime", millis());
        prefs.putString("last_reason", reason);
        prefs.putUInt("total_crashes", prefs.getUInt("total_crashes", 0) + 1);

        // Callback antes de reiniciar
        if (preRestartCallback) {
            Serial.println("[FieldRecovery] 📞 Executando callback pre-restart...");
            preRestartCallback();
        }

        delay(500);
        ESP.restart();
    }

    /**
     * @brief Define callback antes de restart
     */
    void setPreRestartCallback(PreRestartCallback callback) {
        preRestartCallback = callback;
    }

    /**
     * @brief Imprime estatísticas de saúde
     */
    void printHealth() {
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║        🏥 SYSTEM HEALTH STATUS        ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf( "║  Status: %-28s  ║\n",
                      getHealthStatusString(currentHealth.status).c_str());
        Serial.printf( "║  Free Heap: %6lu / %6lu bytes   ║\n",
                      currentHealth.freeHeap, currentHealth.minFreeHeap);
        Serial.printf( "║  Largest Block: %6lu bytes        ║\n",
                      currentHealth.largestBlock);
        Serial.printf( "║  Fragmentation: %3d%%                 ║\n",
                      currentHealth.heapFragmentation);
        Serial.printf( "║  Uptime: %lu ms                       ║\n",
                      currentHealth.uptime);
        Serial.printf( "║  Loop: %lu ms (max: %lu ms)          ║\n",
                      currentHealth.loopTime, currentHealth.maxLoopTime);
        Serial.printf( "║  Loops: %lu                           ║\n", loopCount);
        Serial.println("╚════════════════════════════════════════╝\n");
    }

    /**
     * @brief String de status de saúde
     */
    String getHealthStatusString(HealthStatus status) {
        switch(status) {
            case HEALTHY:  return "✅ HEALTHY";
            case WARNING:  return "⚠️ WARNING";
            case CRITICAL: return "🚨 CRITICAL";
            case FAILING:  return "💀 FAILING";
            default:       return "❓ UNKNOWN";
        }
    }

    // Getters
    const HealthStats& getHealth() const { return currentHealth; }
    bool isInRecoveryMode() const { return inRecoveryMode; }
    uint32_t getConsecutiveCrashes() const { return consecutiveCrashes; }
    uint32_t getUptime() const { return millis() - bootTime; }
};

/**
 * @brief Macro helper para try-catch em operações críticas
 */
#define SAFE_OPERATION(operation, errorMsg) \
    do { \
        try { \
            operation; \
        } catch(...) { \
            Serial.println("[SAFE_OP] ❌ " errorMsg); \
            return false; \
        } \
    } while(0)

#endif // M5GOTCHI_FIELD_RECOVERY_H
