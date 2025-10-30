# NekoGotchi Field Operations Guide 🐱🔒

**Versão:** 1.0.0
**Data:** 2025-10-30
**Classificação:** Uso Profissional em Pentest

---

## 📋 Índice

1. [Visão Geral](#visão-geral)
2. [Preparação Pré-Operacional](#preparação-pré-operacional)
3. [Modos de Operação](#modos-de-operação)
4. [Operações em Campo](#operações-em-campo)
5. [Coleta e Export de Evidências](#coleta-e-export-de-evidências)
6. [Recovery e Troubleshooting](#recovery-e-troubleshooting)
7. [Melhores Práticas](#melhores-práticas)
8. [Referência de Comandos](#referência-de-comandos)

---

## 🎯 Visão Geral

O **NekoGotchi Field Firmware** transforma seu M5Stack Cardputer em uma plataforma robusta e profissional para operações de pentest em campo, mantendo a personalidade kawaii/tamagotchi opcional.

### Características Principais

- ✅ **Auto-recovery** de crashes com preservação de estado
- ✅ **Logging profissional** com timeline completa de eventos
- ✅ **Export automático** em formatos padrão (PCAP, CSV, JSON, Wigle, KML)
- ✅ **Múltiplos modos** operacionais (Neko, Professional, Stealth, Battery Saver)
- ✅ **Persistência robusta** de capturas e configurações
- ✅ **Monitoramento** de saúde do sistema e bateria
- ✅ **Watchdog** hardware para máxima confiabilidade

---

## 🛠️ Preparação Pré-Operacional

### Checklist antes de sair a campo

```
[ ] Cartão SD formatado (FAT32, mínimo 8GB recomendado)
[ ] Bateria carregada (verificar com Menu → System → Battery Status)
[ ] Firmware atualizado (versão 1.0.0+)
[ ] Teste de sistemas críticos realizado
[ ] Backup de configurações importantes
[ ] GPS calibrado (se wardriving)
[ ] Antenas conectadas (se aplicável)
```

### Teste de Sistema

Execute no serial monitor após boot:

```cpp
// No setup() ou menu de teste
fieldSystem.printSystemStatus();
```

Verifique se **TODOS os sistemas** reportam `✅ OK`:
- Recovery System
- Persistence System
- Logger System
- Ops Mode Manager
- Export System

### Configuração Inicial

1. **Inserir cartão SD** antes de ligar
2. Boot do sistema (30-40 segundos)
3. Verificar mensagens de inicialização
4. Escolher modo operacional

---

## ⚙️ Modos de Operação

### 1. 🐱 Neko Mode (Padrão)

**Quando usar:** Demonstrações, aprendizado, uso casual

**Características:**
- Interface completa com animações
- Reações do pet neko
- Sons e música
- Tutorial interativo
- Achievement system ativo

**Consumo de bateria:** ~3-4 horas

---

### 2. 💼 Professional Mode

**Quando usar:** Operações profissionais de pentest

**Características:**
- Interface limpa e objetiva
- Sem animações desnecessárias
- Logging verbose ativado
- UI minimalista focada em dados
- Beeps apenas para alertas importantes

**Consumo de bateria:** ~5-6 horas

**Como ativar:**
```cpp
fieldSystem.opsMode.enableProfessionalMode();
```

Ou via menu: `Settings → Ops Mode → Professional`

---

### 3. 🥷 Stealth Mode

**Quando usar:** Operações discretas, red team

**Características:**
- **SEM sons** (completamente silencioso)
- **SEM vibração**
- **SEM LEDs** indicadores
- Brightness reduzido (80/255)
- Tela auto-dimming agressivo (30s)
- FPS reduzido para economia

**Consumo de bateria:** ~6-7 horas

**Como ativar:**
```cpp
fieldSystem.opsMode.enableStealthMode();
```

⚠️ **ATENÇÃO:** Neste modo, feedback visual é mínimo. Monitore via serial se possível.

---

### 4. 🔋 Battery Saver Mode

**Quando usar:** Operações prolongadas (>6 horas)

**Características:**
- CPU em 80MHz (vs 240MHz)
- Brightness mínimo (50/255)
- WiFi power save ativado
- Tela desliga após 15s de inatividade
- Sleep automático após 1 minuto
- FPS 10 (vs 30)

**Consumo de bateria:** ~8-10 horas

**Trade-offs:**
- Performance reduzida em scans
- Latência maior na interface
- Não recomendado para ataques ativos

---

## 🎮 Operações em Campo

### Workflow Típico de Operação

#### 1. Boot e Verificação

```
[BOOT] → [Verify Systems] → [Choose Mode] → [Start Mission]
```

#### 2. Iniciar Missão

```cpp
fieldSystem.startMission("Client-ACME-Corp-WiFi-Assessment");
```

Isso cria:
- Session ID único
- Diretório de logs dedicado
- Checkpoint de estado inicial
- Timestamp de início

#### 3. Executar Operações

**Exemplo: WiFi Assessment**

```
1. WiFi Scan → Identifica APs
2. Export inicial (CSV para planilha)
3. Targeted Deauth (se autorizado)
4. Handshake Capture
5. PMKID extraction
6. Export final (PCAP + JSON + Wigle)
```

**Logs automáticos:**
- Cada scan gera entrada no timeline
- Handshakes salvos automaticamente em `/captures/wifi/`
- Timeline JSON atualizada em tempo real

#### 4. Monitoramento Contínuo

O sistema **auto-monitora**:
- ✅ Memória (alerta se <20KB livre)
- ✅ Bateria (alerta se <20%)
- ✅ Saúde do watchdog
- ✅ Fragmentação de heap
- ✅ Tempo de loop (alerta se >500ms)

**Ver status em tempo real:**
```cpp
fieldSystem.recovery.printHealth();
fieldSystem.opsMode.printBatteryStatus();
```

#### 5. Finalizar Missão

```cpp
fieldSystem.endMission();
```

Isso executa:
- Export automático de todos os dados
- Geração de relatórios
- Fechamento de arquivos de log
- Listagem de arquivos exportados
- Estatísticas da sessão

---

## 📦 Coleta e Export de Evidências

### Formatos Suportados

#### 1. **PCAP (Wireshark/Aircrack-ng)**

```cpp
File pcap = fieldSystem.exporter.createPCAP("session_captures");

// Durante captura de pacotes
FieldExporter::Packet pkt;
pkt.timestamp = micros();
pkt.length = packetLen;
pkt.data = packetData;
pkt.channel = currentChannel;

fieldSystem.exporter.addPacketToPCAP(pcap, pkt);

pcap.close();
```

**Uso offline:**
```bash
# Análise com Wireshark
wireshark session_captures.pcap

# Crack com Aircrack-ng
aircrack-ng -w wordlist.txt session_captures.pcap
```

---

#### 2. **CSV (Análise em Planilha)**

```cpp
FieldExporter::WiFiAP aps[100];
// ... popular aps ...

fieldSystem.exporter.exportWiFiToCSV(aps, count, "wifi_survey");
```

**Colunas:**
- SSID, BSSID, RSSI, Channel, Encryption
- Timestamp, Latitude, Longitude
- Handshake (YES/NO), PMKID (YES/NO)

**Uso offline:**
- Abrir em Excel/LibreOffice
- Filtrar por encryption type
- Ordenar por RSSI
- Criar gráficos de cobertura

---

#### 3. **JSON (Automação/Scripts)**

```cpp
fieldSystem.exporter.exportWiFiToJSON(aps, count, "wifi_data");
```

**Estrutura:**
```json
{
  "export_type": "wifi_scan",
  "timestamp": 1234567890,
  "ap_count": 42,
  "access_points": [
    {
      "ssid": "CorpNet",
      "bssid": "AA:BB:CC:DD:EE:FF",
      "channel": 6,
      "rssi": -45,
      "encryption": "WPA2-PSK",
      "flags": {
        "handshake": true,
        "pmkid": false
      }
    }
  ]
}
```

**Uso offline:**
```python
import json

with open('wifi_data.json') as f:
    data = json.load(f)

# Filtrar APs vulneráveis
vulnerable = [ap for ap in data['access_points']
              if 'WEP' in ap['encryption']]
```

---

#### 4. **Wigle WiFi (Wardriving)**

```cpp
fieldSystem.exporter.exportToWigle(aps, count, "wardriving_session");
```

**Upload para Wigle.net:**
1. Acesse https://wigle.net
2. Login/Create account
3. Upload → Select `.wigle` file
4. View your contributions on map

---

#### 5. **KML (Google Earth)**

```cpp
fieldSystem.exporter.exportToKML(aps, count, "wifi_map");
```

**Visualização:**
1. Abra Google Earth
2. File → Open → Selecione `.kml`
3. APs aparecem como pins coloridos:
   - 🟢 Verde = Open
   - 🔵 Azul = WPA/WPA2
   - 🔴 Vermelho = WEP

---

### Chain of Custody (Cadeia de Custódia)

Cada arquivo exportado contém metadata:

```
Filename: session_01abcdef_20251030.pcap
Session ID: session_01abcdef
Timestamp: 2025-10-30 14:35:22 UTC
Device: M5Stack Cardputer (ESP32-S3)
Firmware: NekoGotchi Field v1.0.0
Operator: [configurável]
Mission: Client-ACME-Corp-WiFi-Assessment
```

**Recomendação:** Documente em relatório separado:
- Quem operou o dispositivo
- Autorização para o pentest
- Escopo da operação
- Hora de início/fim

---

## 🔧 Recovery e Troubleshooting

### Crash Recovery Automático

O sistema detecta crashes automaticamente:

```
╔════════════════════════════════════════╗
║  ⚠️  CRASH RECOVERY DETECTADO!        ║
╠════════════════════════════════════════╣
║  Crashes totais: 3                     ║
║  Última razão: Out of Memory           ║
║  Tentando recuperar estado...          ║
╚════════════════════════════════════════╝

[FieldPersistence] 🔄 Restaurado de backup
[Recovery] ✅ Estado recuperado com sucesso
```

**O sistema:**
1. ✅ Detecta o crash no próximo boot
2. ✅ Carrega último estado salvo
3. ✅ Restaura configurações
4. ✅ Continua logging na mesma sessão (ou cria nova)

**Se 3+ crashes consecutivos:**
- Sistema entra em **SAFE MODE**
- Features não-essenciais desabilitadas
- Apenas operações básicas disponíveis

---

### Problemas Comuns

#### 1. "ERROR_NO_STORAGE"

**Causa:** Cartão SD não detectado

**Solução:**
1. Desligar dispositivo
2. Verificar cartão SD inserido corretamente
3. Tentar outro cartão (FAT32, <32GB funciona melhor)
4. Sistema ainda funciona, mas sem persistência

---

#### 2. "Out of Memory" / Heap baixo

**Sintomas:**
```
[FieldRecovery] ⚠️ Sistema em WARNING
[FieldRecovery] 🧹 Forçando garbage collection...
```

**Causa:** Muitos dados em memória (scans grandes, buffers)

**Solução automática:**
- Sistema tenta liberar memória automaticamente
- Caches são limpos
- Se falhar, auto-restart

**Solução manual:**
- Reduzir tamanho de scans
- Ativar Battery Saver mode (menos buffers)
- Fazer exports mais frequentes

---

#### 3. Watchdog Timeout

**Sintomas:**
```
Reset Reason: Watchdog Timeout
```

**Causa:** Loop travado (operação >30s sem alimentar watchdog)

**Prevenção:**
- Sistema auto-alimenta watchdog no `update()`
- Certifique-se de chamar `fieldSystem.update()` regularmente
- Evite `delay()` longos (use `yield()` ou tasks)

---

#### 4. Bateria baixa inesperada

**Sintomas:** Desliga antes do esperado

**Causas possíveis:**
- Brightness alto
- WiFi scan contínuo
- CPU full speed
- Bateria degradada

**Solução:**
```cpp
// Ativa battery saver imediatamente
fieldSystem.opsMode.enableBatterySaver();

// Verifica saúde da bateria
fieldSystem.opsMode.printBatteryStatus();
```

---

## 💡 Melhores Práticas

### Antes da Operação

1. ✅ **Teste completo** em ambiente controlado
2. ✅ **Backup de configs** importantes
3. ✅ **Cartão SD formatado** e testado
4. ✅ **Bateria 100%** (ou power bank)
5. ✅ **Autorização documentada** por escrito

### Durante a Operação

1. ✅ **Inicie missão** com nome descritivo
2. ✅ **Salve estado** periodicamente (auto ou manual)
3. ✅ **Monitore bateria** a cada hora
4. ✅ **Export intermediário** a cada fase
5. ✅ **Documente achados** em notes

### Após a Operação

1. ✅ **Finalize missão** formalmente
2. ✅ **Export completo** de todos os dados
3. ✅ **Backup imediato** do cartão SD
4. ✅ **Verificar integridade** dos arquivos exportados
5. ✅ **Limpar dados sensíveis** (se aplicável)

---

## 📚 Referência de Comandos

### Inicialização

```cpp
#include "m5gotchi_field_integration.h"

NekoGotchiFieldSystem fieldSystem;

void setup() {
    // Inicializa em modo profissional
    fieldSystem.begin(true);  // false = neko mode
}

void loop() {
    // OBRIGATÓRIO: chame em todo loop!
    fieldSystem.update();

    // Seu código aqui...
}
```

---

### Gestão de Missões

```cpp
// Iniciar
fieldSystem.startMission("Nome-da-Missao");

// Finalizar
fieldSystem.endMission();

// Status
fieldSystem.printSystemStatus();
```

---

### Modos Operacionais

```cpp
// Alternar modos
fieldSystem.opsMode.enableNekoMode();
fieldSystem.opsMode.enableProfessionalMode();
fieldSystem.opsMode.enableStealthMode();
fieldSystem.opsMode.enableBatterySaver();

// Toggle rápido (neko ↔ professional)
fieldSystem.quickToggleMode();

// Verificar modo atual
String mode = fieldSystem.opsMode.getModeString();

// Configurações customizadas
fieldSystem.opsMode.setBrightness(100);
fieldSystem.opsMode.setAnimations(false);
```

---

### Logging

```cpp
// Log simples
fieldSystem.logger.info("ModuleName", "Mensagem");
fieldSystem.logger.error("ModuleName", "Erro!");
fieldSystem.logger.success("ModuleName", "Sucesso!");

// Log de evento específico
fieldSystem.logger.log(
    FieldLogger::WIFI_SCAN_COMPLETE,
    FieldLogger::SUCCESS,
    "WiFi",
    "42 APs encontrados"
);

// Log de captura WiFi
fieldSystem.logger.logWiFiCapture(
    "MyNetwork",          // SSID
    "AA:BB:CC:DD:EE:FF",  // BSSID
    6,                    // Channel
    "Handshake",          // Type
    "/path/to/capture.cap" // File
);

// GPS location
fieldSystem.logger.logGPSLocation(
    -23.550520,  // Latitude
    -46.633308,  // Longitude
    760.0        // Altitude
);
```

---

### Persistence

```cpp
// Salvar estado
fieldSystem.saveState();

// Salvar captura WiFi
fieldSystem.persistence.saveWiFiCapture(
    "NetworkName",
    packetData,
    packetLength
);

// Log de evento no arquivo
fieldSystem.persistence.logEvent(
    "Handshake captured",
    "SSID: MyNetwork, CH: 6"
);
```

---

### Export

```cpp
// Criar array de APs
FieldExporter::WiFiAP aps[100];

// Popular dados (exemplo)
aps[0].ssid = "CorpWiFi";
aps[0].bssid = "AA:BB:CC:DD:EE:FF";
aps[0].rssi = -45;
aps[0].channel = 6;
aps[0].encryption = "WPA2-PSK";
aps[0].latitude = -23.550520;
aps[0].longitude = -46.633308;
aps[0].hasHandshake = true;

// Export em vários formatos
fieldSystem.exporter.exportWiFiToCSV(aps, count, "session");
fieldSystem.exporter.exportWiFiToJSON(aps, count, "session");
fieldSystem.exporter.exportToWigle(aps, count, "session");
fieldSystem.exporter.exportToKML(aps, count, "session");

// Listar exports
fieldSystem.exporter.listExports();
```

---

### Recovery e Monitoramento

```cpp
// Verificar saúde
const FieldRecovery::HealthStats& health =
    fieldSystem.recovery.getHealth();

if (health.status == FieldRecovery::WARNING) {
    Serial.println("⚠️ Sistema em WARNING!");
}

// Imprimir saúde
fieldSystem.recovery.printHealth();

// Verificar bateria
FieldOpsMode::BatteryStats bat =
    fieldSystem.opsMode.getBatteryStats();

Serial.printf("Bateria: %d%%\n", bat.percentage);

// Registrar atividade de usuário (reset timeout)
fieldSystem.userActivity();
```

---

### Emergências

```cpp
// Salvar e reiniciar
fieldSystem.saveState();
fieldSystem.recovery.restart("Manual restart");

// Handler de emergência
fieldSystem.emergencyHandler("Critical error occurred");

// Shutdown gracioso
fieldSystem.shutdown();
```

---

## 🎓 Exemplo Completo de Uso

```cpp
#include <M5Cardputer.h>
#include "m5gotchi_field_integration.h"

NekoGotchiFieldSystem fieldSystem;

void setup() {
    auto cfg = M5.begin();

    // Inicializa sistemas de campo
    bool professionalMode = true;  // ou false para neko mode
    fieldSystem.begin(professionalMode);

    // Inicia missão
    fieldSystem.startMission("WiFi-Pentest-Building-A");
}

void loop() {
    M5.update();

    // OBRIGATÓRIO: atualiza sistemas
    fieldSystem.update();

    // Detecta input do usuário
    if (M5.Keyboard.isChange()) {
        fieldSystem.userActivity();  // Reset power management

        if (M5.Keyboard.isKeyPressed('m')) {
            // Toggle modo operacional
            fieldSystem.quickToggleMode();
        }

        if (M5.Keyboard.isKeyPressed('s')) {
            // Scan WiFi
            fieldSystem.logger.log(
                FieldLogger::WIFI_SCAN_START,
                FieldLogger::INFO,
                "WiFi", "Iniciando scan..."
            );

            // ... executar scan ...

            fieldSystem.logger.log(
                FieldLogger::WIFI_SCAN_COMPLETE,
                FieldLogger::SUCCESS,
                "WiFi", "Scan completo: 42 APs"
            );
        }

        if (M5.Keyboard.isKeyPressed('e')) {
            // Export
            FieldExporter::WiFiAP aps[50];
            // ... popular aps ...

            fieldSystem.exportWiFiSession(aps, apCount);
        }

        if (M5.Keyboard.isKeyPressed('q')) {
            // Finalizar missão
            fieldSystem.endMission();
        }
    }

    // Auto-save a cada 5 minutos
    static uint32_t lastSave = 0;
    if (millis() - lastSave > 300000) {
        fieldSystem.saveState();
        lastSave = millis();
    }
}
```

---

## 📞 Suporte e Troubleshooting

### Logs de Debug

Todos os logs são salvos em:
```
/nekogotchi/logs/[session_id].log
/nekogotchi/logs/[session_id]_timeline.json
```

### Dados de Telemetria

Para análise offline de problemas:

```cpp
fieldSystem.printSystemStatus();  // Captura completa do estado
```

Copie output do serial e anexe em issue/report.

---

## 📄 Licença e Disclaimer

⚠️ **ATENÇÃO**: Este firmware é destinado **exclusivamente** para:
- Testes de segurança autorizados
- Pentest com contrato/autorização
- Pesquisa educacional
- CTF e laboratórios

❌ **NUNCA** use sem autorização explícita por escrito.

O uso indevido pode violar leis como:
- Computer Fraud and Abuse Act (EUA)
- Lei Carolina Dieckmann (Brasil)
- GDPR (Europa)

Os desenvolvedores **NÃO** se responsabilizam por uso inadequado.

---

## 🚀 Roadmap Futuro

- [ ] Criptografia AES-256 de capturas
- [ ] Sincronização cloud (S3, FTP)
- [ ] Interface web para análise remota
- [ ] Machine learning para detecção de anomalias
- [ ] Integração com Metasploit/BurpSuite
- [ ] Modo mesh networking (múltiplos dispositivos)

---

**Desenvolvido com 🐱 por NekoGotchi Team**

*"Making pentest fun again, one neko at a time!"*
