/**
 * @file m5gotchi_field_integration.h
 * @brief Integração dos sistemas de campo robustos com firmware existente
 *
 * Este arquivo mostra como integrar os novos sistemas de campo (persistence,
 * recovery, logging, ops mode, exporter) com o firmware NekoGotchi existente.
 *
 * @version 1.0.0
 * @date 2025-10-30
 */

#ifndef M5GOTCHI_FIELD_INTEGRATION_H
#define M5GOTCHI_FIELD_INTEGRATION_H

#include "m5gotchi_field_persistence.h"
#include "m5gotchi_field_recovery.h"
#include "m5gotchi_field_logger.h"
#include "m5gotchi_field_ops_mode.h"
#include "m5gotchi_field_exporter.h"

/**
 * @brief Sistema integrado de operações de campo
 *
 * Centraliza todos os sistemas robustos para uso profissional em pentest.
 * Fornece interface unificada e gerenciamento de ciclo de vida.
 */
class NekoGotchiFieldSystem {
public:
    // Subsistemas
    FieldPersistence persistence;
    FieldRecovery recovery;
    FieldLogger logger;
    FieldOpsMode opsMode;
    FieldExporter exporter;

    // Estado
    bool initialized = false;
    bool operationalMode = false;
    String currentMission = "";

    /**
     * @brief Inicializa todos os sistemas de campo
     *
     * @param enableOperationalMode Se true, inicia em modo profissional
     * @return Status da inicialização
     */
    bool begin(bool enableOperationalMode = false) {
        Serial.println("\n");
        Serial.println("╔══════════════════════════════════════════════════════════╗");
        Serial.println("║                                                          ║");
        Serial.println("║     🐱 NEKOGOTCHI FIELD FIRMWARE v1.0.0                 ║");
        Serial.println("║        Professional Pentest Platform                     ║");
        Serial.println("║                                                          ║");
        Serial.println("╠══════════════════════════════════════════════════════════╣");
        Serial.println("║  Initializing robust field systems...                    ║");
        Serial.println("╚══════════════════════════════════════════════════════════╝\n");

        delay(500);

        // 1. Recovery primeiro (detecta crashes, configura watchdog)
        Serial.println("▶ Iniciando Recovery System...");
        FieldRecovery::Config recoveryConfig;
        recoveryConfig.watchdogEnabled = true;
        recoveryConfig.watchdogTimeout = 30;
        recoveryConfig.autoRestartOnOOM = true;

        if (!recovery.begin(recoveryConfig)) {
            Serial.println("❌ Falha ao iniciar Recovery System!");
            return false;
        }

        // Define callback antes de restart (salvar estado crítico)
        recovery.setPreRestartCallback([]() {
            Serial.println("[Callback] 💾 Salvando estado antes de restart...");
            // Aqui você chamaria persistence.saveCriticalState()
        });

        Serial.println("✅ Recovery System ativo\n");
        delay(200);

        // 2. Persistence (carrega estado, detecta crash recovery)
        Serial.println("▶ Iniciando Persistence System...");
        FieldPersistence::Config persistConfig;
        persistConfig.autoSaveEnabled = true;
        persistConfig.autoSaveInterval = 30000; // 30s

        FieldPersistence::Status persistStatus = persistence.begin(persistConfig);

        if (persistStatus == FieldPersistence::ERROR_NO_STORAGE) {
            Serial.println("⚠️  Storage limitado - apenas memória flash");
        } else if (persistStatus == FieldPersistence::RECOVERED_FROM_CRASH) {
            Serial.println("🔄 Estado recuperado de crash anterior!");

            // Carrega estado crítico
            JsonDocument state(4096);
            if (persistence.loadCriticalState(state)) {
                Serial.println("✅ Estado crítico recuperado com sucesso");
                // Aqui você restauraria o estado do sistema
            }
        }

        Serial.println("✅ Persistence System ativo\n");
        delay(200);

        // 3. Logger (timeline, eventos, capturas)
        Serial.println("▶ Iniciando Logger System...");
        FieldLogger::Config loggerConfig;
        loggerConfig.writeToSD = persistence.hasSDCard();
        loggerConfig.writeToSerial = true;
        loggerConfig.minLevel = FieldLogger::INFO;

        if (!logger.begin(loggerConfig)) {
            Serial.println("⚠️  Logger em modo degradado (sem SD)");
        } else {
            Serial.println("✅ Logger System ativo\n");
        }
        delay(200);

        // 4. Ops Mode (gerenciamento de modos operacionais)
        Serial.println("▶ Iniciando Ops Mode Manager...");
        opsMode.begin();

        if (enableOperationalMode) {
            opsMode.enableProfessionalMode();
            operationalMode = true;
            logger.info("OpsMode", "Modo PROFESSIONAL ativado");
        } else {
            // Mantém modo Neko por padrão
            operationalMode = false;
        }

        Serial.println("✅ Ops Mode Manager ativo\n");
        delay(200);

        // 5. Exporter (export de dados)
        Serial.println("▶ Iniciando Export System...");
        if (exporter.begin()) {
            Serial.println("✅ Export System ativo\n");
        } else {
            Serial.println("⚠️  Export System indisponível (sem SD)\n");
        }
        delay(200);

        initialized = true;

        // Log de inicialização completa
        logger.log(FieldLogger::SYSTEM_BOOT, FieldLogger::SUCCESS,
                  "FieldSystem", "Todos os sistemas iniciados com sucesso");

        Serial.println("╔══════════════════════════════════════════════════════════╗");
        Serial.println("║  ✅ TODOS OS SISTEMAS OPERACIONAIS                       ║");
        Serial.println("╠══════════════════════════════════════════════════════════╣");
        Serial.printf( "║  Modo: %-48s  ║\n",
                      operationalMode ? "PROFESSIONAL" : "NEKO MODE");
        Serial.printf( "║  Session: %-45s  ║\n",
                      logger.getSessionID().c_str());
        Serial.printf( "║  Recovery Mode: %-3s                                     ║\n",
                      recovery.isInRecoveryMode() ? "YES" : "NO");
        Serial.println("╚══════════════════════════════════════════════════════════╝\n");

        return true;
    }

    /**
     * @brief Atualiza todos os sistemas (chame no loop principal!)
     */
    void update() {
        if (!initialized) return;

        // 1. Recovery - alimenta watchdog e monitora saúde
        recovery.loopBegin();

        // 2. Persistence - auto-save periódico
        persistence.update();

        // 3. Ops Mode - gerenciamento de energia, timeouts
        opsMode.update();

        // Fim do loop
        recovery.loopEnd();
    }

    /**
     * @brief Registra atividade do usuário (para power management)
     */
    void userActivity() {
        opsMode.userActivity();
    }

    /**
     * @brief Salva estado crítico (chame periodicamente ou antes de operações arriscadas)
     */
    bool saveState() {
        if (!initialized) return false;

        JsonDocument state(4096);

        // Exemplo de estado a salvar
        state["mode"] = opsMode.getMode();
        state["mission"] = currentMission;
        state["uptime"] = millis();
        state["operational"] = operationalMode;
        // Adicione mais estados conforme necessário

        bool success = persistence.saveCriticalState(state);

        if (success) {
            logger.info("FieldSystem", "Estado salvo com sucesso");
        } else {
            logger.error("FieldSystem", "Erro ao salvar estado");
        }

        return success;
    }

    /**
     * @brief Inicia uma missão/operação
     */
    void startMission(const String& missionName) {
        currentMission = missionName;

        logger.log(FieldLogger::SESSION_START, FieldLogger::SUCCESS,
                  "Mission", "Iniciando: " + missionName);

        Serial.println("\n╔══════════════════════════════════════════════════════════╗");
        Serial.println("║            🎯 MISSÃO INICIADA                            ║");
        Serial.println("╠══════════════════════════════════════════════════════════╣");
        Serial.printf( "║  Nome: %-50s ║\n", missionName.c_str());
        Serial.printf( "║  Session: %-45s  ║\n", logger.getSessionID().c_str());
        Serial.println("╚══════════════════════════════════════════════════════════╝\n");

        saveState();
    }

    /**
     * @brief Finaliza missão e exporta dados
     */
    void endMission() {
        if (currentMission.length() == 0) return;

        logger.log(FieldLogger::SESSION_END, FieldLogger::INFO,
                  "Mission", "Finalizando: " + currentMission);

        Serial.println("\n╔══════════════════════════════════════════════════════════╗");
        Serial.println("║          📊 FINALIZANDO MISSÃO                           ║");
        Serial.println("╠══════════════════════════════════════════════════════════╣");

        // Gera relatórios
        logger.printSessionStats();
        logger.generateReport();

        // Lista exports disponíveis
        exporter.listExports();

        Serial.println("╠══════════════════════════════════════════════════════════╣");
        Serial.println("║  ✅ Missão finalizada com sucesso                        ║");
        Serial.println("║  📦 Dados prontos para análise offline                   ║");
        Serial.println("╚══════════════════════════════════════════════════════════╝\n");

        currentMission = "";
    }

    /**
     * @brief Exemplo: Captura WiFi handshake
     */
    void exampleCaptureHandshake(const String& ssid, const String& bssid,
                                 uint8_t channel, const uint8_t* data, size_t len) {

        // 1. Salva captura raw
        persistence.saveWiFiCapture(ssid, data, len);

        // 2. Log do evento
        logger.logWiFiCapture(ssid, bssid, channel, "Handshake",
                             "/captures/wifi/" + ssid + ".cap");

        // 3. Feedback visual (se não em stealth)
        if (!opsMode.isMinimalUI()) {
            Serial.println("✨ Handshake capturado! (Neko está feliz!)");
        } else {
            Serial.println("[+] Handshake captured");
        }

        // 4. Vibração (se habilitado)
        if (opsMode.getProfile().vibration) {
            // M5.Speaker.tone(1000, 100);
        }

        // 5. Auto-save do estado
        saveState();
    }

    /**
     * @brief Exemplo: Export completo de sessão WiFi
     */
    void exportWiFiSession(const FieldExporter::WiFiAP* aps, size_t count) {
        String sessionID = logger.getSessionID();

        Serial.println("\n📦 Exportando sessão WiFi...");

        // Export em múltiplos formatos
        exporter.exportWiFiToCSV(aps, count, sessionID);
        exporter.exportWiFiToJSON(aps, count, sessionID);
        exporter.exportToWigle(aps, count, sessionID);
        exporter.exportToKML(aps, count, sessionID);

        // Gera relatório
        JsonDocument stats(512);
        stats["wifi_aps_found"] = count;
        stats["handshakes"] = 0; // Contar handshakes reais
        stats["pmkids"] = 0;     // Contar PMKIDs reais

        exporter.generateTextReport(sessionID, stats);

        logger.success("Exporter", "Sessão exportada em 4 formatos");
    }

    /**
     * @brief Alterna modo operacional rapidamente
     */
    void quickToggleMode() {
        operationalMode = !operationalMode;

        if (operationalMode) {
            opsMode.enableProfessionalMode();
            logger.info("OpsMode", "Alternado para modo PROFESSIONAL");
        } else {
            opsMode.enableNekoMode();
            logger.info("OpsMode", "Alternado para modo NEKO");
        }

        Serial.printf("\n🔄 Modo alternado: %s\n\n",
                     operationalMode ? "PROFESSIONAL" : "NEKO");
    }

    /**
     * @brief Imprime status completo do sistema
     */
    void printSystemStatus() {
        Serial.println("\n");
        Serial.println("╔══════════════════════════════════════════════════════════╗");
        Serial.println("║          📊 NEKOGOTCHI FIELD SYSTEM STATUS               ║");
        Serial.println("╚══════════════════════════════════════════════════════════╝");

        opsMode.printCurrentConfig();
        recovery.printHealth();
        opsMode.printBatteryStatus();
        persistence.printStats();
        logger.printSessionStats();

        Serial.println("╔══════════════════════════════════════════════════════════╗");
        Serial.println("║  Sistema operando normalmente                            ║");
        Serial.println("╚══════════════════════════════════════════════════════════╝\n");
    }

    /**
     * @brief Shutdown gracioso do sistema
     */
    void shutdown() {
        Serial.println("\n🔄 Iniciando shutdown gracioso...");

        // Salva estado final
        saveState();

        // Finaliza missão se houver
        if (currentMission.length() > 0) {
            endMission();
        }

        // Fecha logger
        logger.end();

        // Marca persistence como não rodando
        persistence.end();

        Serial.println("👋 Sistema finalizado com sucesso\n");

        delay(1000);
    }

    /**
     * @brief Handler de emergência (chame em caso de erro crítico)
     */
    void emergencyHandler(const String& reason) {
        Serial.println("\n🚨 EMERGÊNCIA: " + reason);

        // Tenta salvar estado
        persistence.emergencySave(reason);
        recovery.restart(reason);
    }
};

// Instância global (opcional)
// extern NekoGotchiFieldSystem fieldSystem;

#endif // M5GOTCHI_FIELD_INTEGRATION_H
