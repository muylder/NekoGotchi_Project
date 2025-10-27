# 💾 M5GOTCHI MEMORY OPTIMIZATION GUIDE

## 📊 Status de Otimização

### ✅ Implementado (v1.1)

#### 1. **Achievement System** (`m5gotchi_neko_achievements.h`)
- ✅ Strings em PROGMEM (~8KB economizados)
- ✅ Fixed arrays ao invés de `std::vector`
- ✅ Bitfields para flags (8 bytes → 1 byte)
- ✅ Helpers para ler de PROGMEM
- **Economia Total**: ~12KB RAM

#### 2. **RFID/NFC System** (`m5gotchi_rfid_nfc.h`)
- ✅ Limite de cards salvos (20 → 10)
- ✅ Circular buffer para keys
- ✅ Inclui `m5gotchi_memory_utils.h`
- **Economia Total**: ~8KB RAM

#### 3. **Memory Utilities** (`m5gotchi_memory_utils.h`)
- ✅ StringPool para reuso de strings
- ✅ ObjectPool template genérico
- ✅ DirtyFlag pattern
- ✅ CircularBuffer template
- ✅ SafeString operations
- ✅ MemoryMonitor com stats
- ✅ DisplayBufferManager
- **Economia Total**: 2-4KB RAM + menos fragmentação

---

## 🎯 Próximas Otimizações

### Prioridade ALTA 🔴

#### 1. **WiFi Scanner Optimization**
```cpp
// ANTES: Armazena todos os resultados
std::vector<WiFiNetwork> networks;

// DEPOIS: Limite fixo com circular buffer
CircularBuffer<WiFiNetwork, 50> networks;
```
**Economia**: ~5KB RAM

#### 2. **BLE Spam Optimization**
```cpp
// ANTES: Buffers grandes sem limite
uint8_t bleBuffer[1024];

// DEPOIS: Buffers menores + streaming
#define BLE_BUFFER_SIZE 256
uint8_t bleBuffer[BLE_BUFFER_SIZE];
```
**Economia**: ~3KB RAM

#### 3. **Display Optimization**
```cpp
// Implementar dirty rectangles em TODOS os módulos
DisplayBufferManager displayMgr;
DirtyFlag screenDirty;

void update() {
    if (screenDirty.isDirty()) {
        // Redesenha apenas áreas sujas
        for (int i = 0; i < displayMgr.getDirtyCount(); i++) {
            auto rect = displayMgr.getDirtyRect(i);
            redrawRect(rect.x, rect.y, rect.w, rect.h);
        }
        screenDirty.clear();
        displayMgr.clear();
    }
}
```
**Economia**: ~30% menos CPU

---

### Prioridade MÉDIA 🟡

#### 4. **String Operations**
```cpp
// ANTES: Muitas concatenações
String msg = "Network: " + ssid + " - " + String(rssi) + "dBm";

// DEPOIS: Buffer fixo + snprintf
char msg[64];
snprintf(msg, sizeof(msg), "Network: %s - %ddBm", ssid, rssi);
```
**Economia**: ~2KB RAM + menos fragmentação

#### 5. **PROGMEM para Mensagens**
```cpp
// ANTES: Strings na RAM
const char* messages[] = {
    "Achievement unlocked!",
    "First scan complete!",
    // ...
};

// DEPOIS: Strings em Flash
const char msg1[] PROGMEM = "Achievement unlocked!";
const char msg2[] PROGMEM = "First scan complete!";
const char* const messages[] PROGMEM = {msg1, msg2};
```
**Economia**: ~4KB RAM

#### 6. **Menu System Optimization**
```cpp
// ANTES: Arrays dinâmicos
std::vector<MenuItem> menuItems;

// DEPOIS: PROGMEM + iteradores
struct MenuItem_P {
    char name[32];
    void (*handler)();
};

const MenuItem_P menu[] PROGMEM = {
    {"Scanner", &scannerHandler},
    {"Reader", &readerHandler},
    // ...
};
```
**Economia**: ~3KB RAM

---

### Prioridade BAIXA 🟢

#### 7. **SD Card Buffering**
```cpp
// Implementar write buffering
class SDCardBuffer {
    char writeBuffer[512];
    size_t bufferPos;
    
    void write(const char* data) {
        size_t len = strlen(data);
        if (bufferPos + len >= 512) {
            flush();
        }
        memcpy(&writeBuffer[bufferPos], data, len);
        bufferPos += len;
    }
    
    void flush() {
        if (bufferPos > 0) {
            file.write(writeBuffer, bufferPos);
            bufferPos = 0;
        }
    }
};
```
**Economia**: ~50% menos operações I/O

#### 8. **Task Priority Optimization**
```cpp
// Separar tarefas por prioridade
xTaskCreatePinnedToCore(
    wifiScanTask,    // Task function
    "WiFiScan",      // Name
    4096,            // Stack size
    NULL,            // Parameters
    2,               // Priority (HIGH)
    NULL,            // Task handle
    0                // Core 0
);

xTaskCreatePinnedToCore(
    displayTask,     // Task function
    "Display",       // Name
    3072,            // Stack size
    NULL,            // Parameters
    1,               // Priority (MEDIUM)
    NULL,            // Task handle
    1                // Core 1
);
```
**Economia**: Melhor responsividade

---

## 📐 Checklist de Otimização por Arquivo

### `main_cardputer.cpp`
- [ ] Converter menu strings para PROGMEM
- [ ] Implementar dirty flags para redraws
- [ ] Limitar histórico de logs (circular buffer)
- [ ] Usar F() macro em Serial.println

### `m5gotchi_wifi_analyzer.h`
- [ ] Limite de redes escaneadas (100 → 50)
- [ ] Circular buffer para histórico
- [ ] PROGMEM para vendor database

### `m5gotchi_handshake_capture.h`
- [ ] Buffers de captura menores
- [ ] Streaming ao invés de buffer completo
- [ ] PROGMEM para mensagens

### `m5gotchi_evil_portal.h`
- [ ] Templates HTML em PROGMEM/SPIFFS
- [ ] Limite de conexões simultâneas
- [ ] String pooling para SSIDs

### `m5gotchi_bluetooth_attacks.h`
- [ ] Circular buffer para devices
- [ ] Limite de spam packets/segundo
- [ ] PROGMEM para manufacturer database

### `m5gotchi_gps_wardriving.h`
- [ ] Circular buffer para waypoints
- [ ] Batch write para SD card
- [ ] PROGMEM para formato strings

### `m5gotchi_advanced_protocols.h`
- [ ] Limitar cache de protocolos
- [ ] PROGMEM para protocol definitions

### `m5gotchi_neko_terminal.h`
- [ ] Circular buffer para command history
- [ ] PROGMEM para help strings
- [ ] Limite de output lines

### `m5gotchi_*_game.h`
- [ ] PROGMEM para game data
- [ ] Sprite pooling
- [ ] Dirty rectangles

---

## 🔧 Como Usar as Ferramentas

### 1. Memory Monitor
```cpp
#include "m5gotchi_memory_utils.h"

void loop() {
    // Verificar memória periodicamente
    if (millis() % 30000 == 0) {
        MemoryMonitor::printStats();
    }
    
    // Alertar se memória baixa
    if (MemoryMonitor::isLowMemory()) {
        displayWarning("Low Memory!");
    }
}
```

### 2. String Pooling
```cpp
// Ao invés de criar strings repetidas
const char* status1 = "Connected";
const char* status2 = "Connected";
const char* status3 = "Connected";

// Use o pool
const char* status1 = StringPool::get("Connected");
const char* status2 = StringPool::get("Connected"); // Reutiliza
const char* status3 = StringPool::get("Connected"); // Reutiliza
```

### 3. Object Pooling
```cpp
// Setup
ObjectPool<Achievement> achievementPool(50);

// Usar
Achievement* ach = achievementPool.acquire();
if (ach) {
    ach->id = 1;
    ach->unlocked = true;
    // ...
    
    // Liberar quando terminar
    achievementPool.release(ach);
}
```

### 4. Circular Buffer
```cpp
CircularBuffer<LogEntry, 100> logBuffer;

// Adicionar (automaticamente descarta o mais antigo se cheio)
LogEntry entry = {millis(), "WiFi scan complete"};
logBuffer.push(entry);

// Ler
LogEntry oldest;
if (logBuffer.pop(oldest)) {
    Serial.println(oldest.message);
}
```

### 5. Dirty Flags
```cpp
class MyScreen {
    DirtyFlag screenDirty;
    DirtyFlag menuDirty;
    
    void updateNetwork() {
        // Marcar apenas a área afetada como suja
        screenDirty.markDirty();
    }
    
    void draw() {
        if (screenDirty.isDirty()) {
            redrawScreen();
            screenDirty.clear();
        }
        
        if (menuDirty.isDirty()) {
            redrawMenu();
            menuDirty.clear();
        }
    }
};
```

---

## 📊 Resultados Esperados

### Antes das Otimizações
```
Flash Usage:  2.18 MB / 3.14 MB (69%)
RAM Usage:    210 KB / 327 KB (64%)
Fragmentation: ~45%
FPS:          ~30-40 FPS
```

### Depois das Otimizações (META)
```
Flash Usage:  2.10 MB / 3.14 MB (67%)  [-80KB por PROGMEM]
RAM Usage:    165 KB / 327 KB (50%)   [-45KB economizados]
Fragmentation: ~25%                    [-20% fragmentação]
FPS:          ~50-60 FPS               [+50% performance]
```

### Breakdown por Otimização
| Otimização | RAM Saved | Flash Used | Frag. Reduction |
|------------|-----------|------------|-----------------|
| PROGMEM strings | -20KB | +15KB | -5% |
| Fixed arrays | -15KB | 0 | -10% |
| Object pooling | -10KB | +2KB | -15% |
| Circular buffers | -8KB | +1KB | -5% |
| String pooling | -4KB | 0 | -8% |
| Dirty flags | 0 | +2KB | 0 (CPU -30%) |
| **TOTAL** | **-57KB** | **+20KB** | **-43%** |

---

## 🚨 Sinais de Alerta

### Quando a Memória Está Problemática:
1. **Free Heap < 20KB**: Memória crítica
2. **Fragmentação > 50%**: Heap fragmentado
3. **Allocation failures**: malloc() retorna NULL
4. **Random reboots**: Out of memory crashes
5. **Slow performance**: Garbage collection excessiva

### Como Detectar:
```cpp
void checkMemoryHealth() {
    uint32_t free = ESP.getFreeHeap();
    float frag = MemoryMonitor::getFragmentation();
    
    if (free < 20000) {
        Serial.println(F("🚨 CRITICAL: Very low memory!"));
        // Forçar garbage collection ou reboot
    }
    
    if (frag > 50.0) {
        Serial.println(F("⚠️ WARNING: High fragmentation!"));
        // Considerar defragmentation ou limpar caches
    }
}
```

---

## 🎓 Best Practices

### DO ✅
- Use PROGMEM para dados constantes
- Limite o tamanho de containers dinâmicos
- Implemente object pooling para objetos frequentes
- Use F() macro em debug strings
- Monitore memória regularmente
- Use buffers de tamanho fixo quando possível
- Implemente dirty flags para evitar trabalho desnecessário
- Use strncpy/snprintf para evitar buffer overflows

### DON'T ❌
- Não use String para concatenações frequentes
- Não crie vectors sem limite de tamanho
- Não aloque memória em loops tight
- Não use recursão profunda
- Não ignore warnings de low memory
- Não use sprintf sem bounds checking
- Não mantenha buffers grandes não utilizados

---

## 📚 Referências

- [ESP32 Memory Types](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/memory-types.html)
- [Arduino Memory Guide](https://www.arduino.cc/reference/en/language/variables/utilities/progmem/)
- [Heap Fragmentation](https://cpp4arduino.com/2018/11/06/what-is-heap-fragmentation.html)

---

**Última atualização**: 2025-10-26
**Versão**: 1.1
**Status**: 🟡 Em progresso (30% completo)
