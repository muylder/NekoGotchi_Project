# 🐱🔒 NekoGotchi Field Firmware

**Firmware robusto para operações profissionais de pentest em campo**

---

## ⚡ Quick Start

```cpp
#include "m5gotchi_field_integration.h"

NekoGotchiFieldSystem fieldSystem;

void setup() {
    M5.begin();

    // Modo profissional
    fieldSystem.begin(true);

    // Inicia operação
    fieldSystem.startMission("Client-Pentest-2025");
}

void loop() {
    M5.update();
    fieldSystem.update();  // OBRIGATÓRIO!

    // Seu código de pentest aqui...
}
```

---

## 📦 O que há de novo?

### Sistemas Implementados

| Sistema | Arquivo | Função |
|---------|---------|--------|
| **Persistence** | `m5gotchi_field_persistence.h` | Auto-save, backup, recovery de estado |
| **Recovery** | `m5gotchi_field_recovery.h` | Watchdog, crash detection, auto-restart |
| **Logger** | `m5gotchi_field_logger.h` | Timeline, eventos, capturas profissionais |
| **Ops Mode** | `m5gotchi_field_ops_mode.h` | Modos operacionais (Neko/Pro/Stealth/Battery) |
| **Exporter** | `m5gotchi_field_exporter.h` | Export PCAP, CSV, JSON, Wigle, KML |
| **Integration** | `m5gotchi_field_integration.h` | Orquestração de todos os sistemas |

---

## 🎯 Principais Features

### 1. Auto-Recovery de Crashes

```
╔════════════════════════════════════════╗
║  ⚠️  CRASH RECOVERY DETECTADO!        ║
╠════════════════════════════════════════╣
║  Crashes totais: 1                     ║
║  Tentando recuperar estado...          ║
║  ✅ Estado recuperado com sucesso      ║
╚════════════════════════════════════════╝
```

**O sistema:**
- Detecta crashes automaticamente
- Recupera último estado salvo
- Continua operação onde parou
- Entra em safe mode após 3+ crashes

---

### 2. Logging Profissional

**Timeline completa de eventos:**

```json
{
  "id": 42,
  "timestamp": 1234567890,
  "time": "02:15:30.123",
  "type": "WiFi Handshake Captured",
  "level": "CAPTURE",
  "module": "WiFi",
  "description": "Handshake capturado: CorpWiFi [AA:BB:CC:DD:EE:FF] on CH6",
  "metadata": {
    "ssid": "CorpWiFi",
    "bssid": "AA:BB:CC:DD:EE:FF",
    "channel": 6,
    "file": "/captures/wifi/CorpWiFi_1234567890.cap"
  }
}
```

**Arquivos gerados:**
- `session_[ID].log` - Log de texto
- `session_[ID]_timeline.json` - Timeline estruturada
- `session_[ID]_captures.json` - Índice de capturas

---

### 3. Export Multi-Formato

**Um comando, 5 formatos:**

```cpp
fieldSystem.exportWiFiSession(aps, count);
```

Gera:
- ✅ `session.csv` - Excel, Google Sheets
- ✅ `session.json` - Scripts, automação
- ✅ `session.wigle` - Wigle WiFi wardriving
- ✅ `session.kml` - Google Earth mapping
- ✅ `session.pcap` - Wireshark, Aircrack-ng

---

### 4. Modos Operacionais

| Modo | Bateria | Uso | Feedback |
|------|---------|-----|----------|
| 🐱 **Neko** | 3-4h | Demos, aprendizado | Completo (animações, sons) |
| 💼 **Professional** | 5-6h | Pentest padrão | Limpo, objetivo |
| 🥷 **Stealth** | 6-7h | Red team, discreto | Mínimo (sem som/vibração) |
| 🔋 **Battery Saver** | 8-10h | Operações longas | Reduzido (CPU 80MHz) |

**Toggle rápido:**
```cpp
fieldSystem.quickToggleMode();  // Neko ↔ Professional
```

---

### 5. Monitoramento de Saúde

**Watchdog automático:**

```cpp
// No loop (automático)
fieldSystem.update();  // Alimenta watchdog, monitora saúde
```

**Alertas automáticos:**
- ⚠️ Memória baixa (<20KB)
- ⚠️ Bateria crítica (<20%)
- ⚠️ Loop lento (>500ms)
- 🚨 Heap fragmentado (>50%)

**Recovery automático:**
- Garbage collection
- Limpeza de caches
- Auto-restart se necessário

---

## 🛠️ Integração com Código Existente

### Passo 1: Incluir headers

```cpp
#include "m5gotchi_field_integration.h"
```

### Passo 2: Instanciar sistema

```cpp
NekoGotchiFieldSystem fieldSystem;
```

### Passo 3: Inicializar no setup()

```cpp
void setup() {
    M5.begin();

    // true = Professional Mode
    // false = Neko Mode
    fieldSystem.begin(false);

    // Opcional: configurar callback
    fieldSystem.recovery.setPreRestartCallback([]() {
        // Salvar estado antes de crash
        fieldSystem.saveState();
    });
}
```

### Passo 4: Atualizar no loop()

```cpp
void loop() {
    M5.update();

    // OBRIGATÓRIO - alimenta watchdog + monitora saúde
    fieldSystem.update();

    // Input do usuário
    if (M5.Keyboard.isChange()) {
        fieldSystem.userActivity();  // Reset power management
    }

    // Seu código normal...
}
```

### Passo 5: Usar logging

```cpp
// Em vez de Serial.println()
fieldSystem.logger.info("WiFi", "Scan iniciado");
fieldSystem.logger.success("WiFi", "42 APs encontrados");
fieldSystem.logger.error("WiFi", "Falha na captura");

// Eventos específicos
fieldSystem.logger.log(
    FieldLogger::WIFI_HANDSHAKE_CAPTURED,
    FieldLogger::CAPTURE,
    "WiFi",
    "Handshake: " + ssid
);
```

### Passo 6: Salvar capturas

```cpp
// Quando capturar handshake
uint8_t packetData[256];
size_t packetLen = captureHandshake(packetData);

fieldSystem.persistence.saveWiFiCapture(
    ssid,
    packetData,
    packetLen
);

fieldSystem.logger.logWiFiCapture(
    ssid, bssid, channel,
    "Handshake",
    "/captures/wifi/..."
);
```

### Passo 7: Export final

```cpp
// No fim da operação
FieldExporter::WiFiAP aps[100];

// Popular aps[] com dados escaneados...

fieldSystem.exportWiFiSession(aps, apCount);
fieldSystem.endMission();
```

---

## 📊 Exemplo Real: WiFi Assessment

```cpp
void doWiFiAssessment() {
    // Inicia missão
    fieldSystem.startMission("Client-ACME-WiFi-2025");

    // Scan inicial
    fieldSystem.logger.log(
        FieldLogger::WIFI_SCAN_START,
        FieldLogger::INFO,
        "WiFi", "Scanning all channels..."
    );

    // ... executar scan ...
    int apCount = scanWiFi();

    fieldSystem.logger.log(
        FieldLogger::WIFI_SCAN_COMPLETE,
        FieldLogger::SUCCESS,
        "WiFi",
        String(apCount) + " APs found"
    );

    // Capturar handshakes
    for (int i = 0; i < targetCount; i++) {
        String ssid = targets[i].ssid;

        // Deauth
        sendDeauth(targets[i]);
        fieldSystem.logger.log(
            FieldLogger::WIFI_DEAUTH_SENT,
            FieldLogger::INFO,
            "WiFi",
            "Deauth sent: " + ssid
        );

        // Aguardar handshake
        if (waitForHandshake(targets[i])) {
            fieldSystem.logger.logWiFiCapture(
                ssid,
                targets[i].bssid,
                targets[i].channel,
                "Handshake",
                "/captures/wifi/" + ssid + ".cap"
            );
        }
    }

    // Export tudo
    fieldSystem.exportWiFiSession(aps, apCount);

    // Finaliza
    fieldSystem.endMission();
}
```

---

## 🔧 Configuração Avançada

### Customizar Persistence

```cpp
FieldPersistence::Config persistCfg;
persistCfg.autoSaveEnabled = true;
persistCfg.autoSaveInterval = 60000;  // 1 minuto
persistCfg.backupEnabled = true;
persistCfg.maxBackups = 5;

fieldSystem.persistence.begin(persistCfg);
```

### Customizar Recovery

```cpp
FieldRecovery::Config recoveryCfg;
recoveryCfg.watchdogEnabled = true;
recoveryCfg.watchdogTimeout = 60;     // 60 segundos
recoveryCfg.minFreeHeap = 30000;      // 30KB mínimo
recoveryCfg.autoRestartOnOOM = false; // Manual restart

fieldSystem.recovery.begin(recoveryCfg);
```

### Customizar Logger

```cpp
FieldLogger::Config loggerCfg;
loggerCfg.minLevel = FieldLogger::DEBUG;  // Log tudo
loggerCfg.writeToSerial = false;          // Apenas SD
loggerCfg.maxLogSize = 10485760;          // 10MB

fieldSystem.logger.begin(loggerCfg);
```

---

## 📁 Estrutura de Arquivos

### No Cartão SD

```
/nekogotchi/
├── state_critical.json          # Estado do sistema
├── state_critical.json.bak      # Backup
├── config.json                  # Configurações
├── neko_state.json             # Estado do pet
├── captures/
│   ├── wifi/
│   │   ├── CorpWiFi_1234.cap
│   │   ├── GuestNet_5678.cap
│   │   └── ...
│   ├── ble/
│   └── rfid/
├── logs/
│   ├── session_01abcd.log
│   ├── session_01abcd_timeline.json
│   ├── session_01abcd_captures.json
│   └── events.log
└── export/
    ├── session_01abcd.csv
    ├── session_01abcd.json
    ├── session_01abcd.wigle
    ├── session_01abcd.kml
    ├── session_01abcd.pcap
    └── session_01abcd_report.txt
```

---

## 🚨 Troubleshooting

### Problema: Crash loop

```
Reset Reason: Watchdog Timeout
Crashes totais: 5
```

**Solução:**
- Sistema entra em SAFE MODE após 3 crashes
- Features não-essenciais desabilitadas
- Verificar logs: `/nekogotchi/logs/events.log`
- Reduzir operações pesadas

---

### Problema: Sem espaço no SD

```
[FieldPersistence] ❌ Erro: No space left
```

**Solução:**
```cpp
// Limpeza manual
fieldSystem.persistence.cleanup(86400000); // Remove >24h

// Verificar uso
fieldSystem.persistence.printStats();
```

---

### Problema: Bateria drena rápido

**Solução:**
```cpp
// Ativa battery saver
fieldSystem.opsMode.enableBatterySaver();

// Ou ajustes manuais
fieldSystem.opsMode.setBrightness(30);
fieldSystem.opsMode.setDisplayFPS(10);
```

---

## 📖 Documentação Completa

Consulte: [`FIELD_OPERATIONS_GUIDE.md`](FIELD_OPERATIONS_GUIDE.md)

**Tópicos:**
- ✅ Preparação pré-operacional
- ✅ Modos de operação detalhados
- ✅ Workflow de operações
- ✅ Export e análise de dados
- ✅ Recovery e troubleshooting
- ✅ Melhores práticas
- ✅ Referência completa de comandos

---

## ⚖️ Disclaimer Legal

⚠️ **USO EXCLUSIVO PARA:**
- Pentesting autorizado
- Pesquisa educacional
- CTF e competições
- Laboratórios controlados

❌ **NUNCA use sem autorização explícita por escrito!**

Violação pode incorrer em crimes como:
- Computer Fraud and Abuse Act (EUA)
- Lei Carolina Dieckmann (Brasil)
- Computer Misuse Act (UK)
- GDPR violations (EU)

**Os desenvolvedores NÃO se responsabilizam por uso inadequado.**

---

## 🤝 Contribuições

PRs bem-vindos para:
- [ ] Novos formatos de export
- [ ] Integrações com ferramentas (Metasploit, etc)
- [ ] Otimizações de performance
- [ ] Novos modos operacionais
- [ ] Documentação e exemplos

---

## 📜 Changelog

### v1.0.0 (2025-10-30)

**Inicial release com:**
- ✅ Sistema de persistência robusta
- ✅ Auto-recovery de crashes
- ✅ Logging profissional
- ✅ 4 modos operacionais
- ✅ Export multi-formato
- ✅ Documentação completa

---

## 📞 Suporte

**Issues:** https://github.com/muylder/NekoGotchi_Project/issues

**Documentação:** [`/docs`](../docs/)

---

**Desenvolvido com 🐱 e ☕ pelo NekoGotchi Team**

*"Pentest profissional com personalidade neko!"*
