# Source Code - M5Gotchi Pro

## Arquivos Principais

### `main.cpp` - Versão M5Stack Core/Fire/Core2
Versão original otimizada para dispositivos M5Stack com:
- Tela 320x240 pixels
- 3 botões físicos (A, B, C)
- Fontes tamanho 2
- Interface espaçada

**Dispositivos suportados:**
- M5Stack Core ESP32
- M5Stack Fire
- M5Stack Core2

**PlatformIO environments:**
```ini
[env:m5stack-core-esp32]
[env:m5stack-fire]
[env:m5stack-core2]
```

---

### `main_cardputer.cpp` - Versão M5Stack Cardputer
Versão otimizada para Cardputer com:
- Tela 240x135 pixels (menor)
- Teclado QWERTY completo
- Fontes tamanho 1 (compactas)
- Interface condensada
- 10+ atalhos de teclado

**Dispositivo suportado:**
- M5Stack Cardputer (ESP32-S3)

**PlatformIO environment:**
```ini
[env:m5stack-cardputer]
```

---

## Diferenças Principais

| Característica | main.cpp | main_cardputer.cpp |
|----------------|----------|-------------------|
| **Resolução** | 320x240 | 240x135 |
| **Controles** | 3 botões (A/B/C) | Teclado QWERTY |
| **Tamanho fonte** | 2 | 1 |
| **Headers** | 35px altura | 14px altura |
| **Status bar** | 3 labels curtas | Texto completo |
| **Redes visíveis** | 7 | 8 |
| **Atalhos** | 0 | 10+ |
| **Navegação** | Botões A/C | Teclas ↑/↓ |
| **Loop delay** | 10ms | 50ms |

---

## Compilação Seletiva

### Compilar apenas uma versão

```bash
# M5Stack Core
pio run -e m5stack-core-esp32

# Cardputer
pio run -e m5stack-cardputer
```

### Como funciona?

O `platformio.ini` define qual arquivo usar:

```ini
[env:m5stack-core-esp32]
# Usa src/main.cpp automaticamente

[env:m5stack-cardputer]
# Também usa src/main.cpp, mas com flag -DCARDPUTER
# (Nota: Pode ser configurado para usar main_cardputer.cpp)
```

Para forçar uso do arquivo específico, adicione no `platformio.ini`:

```ini
[env:m5stack-cardputer]
build_src_filter = 
    +<main_cardputer.cpp>
    -<main.cpp>
```

---

## Código Compartilhado

Ambas versões compartilham:

### ✅ Mesma lógica WiFi
- Sniffer de pacotes
- Detecção EAPOL
- Captura PMKID
- Deauth attack
- Evil Portal

### ✅ Mesmas estruturas
```cpp
typedef struct {
    uint32_t magic_number;
    uint16_t version_major;
    // ... PCAP header
} pcap_hdr_t;

struct NetworkInfo {
    String ssid;
    uint8_t bssid[6];
    // ... rede WiFi
};
```

### ✅ Mesmos algoritmos
- `isEAPOL()` - Detecta handshake
- `hasPMKID()` - Detecta PMKID
- `sendDeauthPacket()` - Envia deauth
- `handleRoot()` - Serve portal HTML

---

## Diferenças de Implementação

### 1. Display Functions

**main.cpp:**
```cpp
void drawHeader(const char* title) {
    M5.Display.setTextSize(2);  // Fonte grande
    M5.Display.setCursor((320 - strlen(title) * 12) / 2, 10);
    M5.Display.drawLine(0, 35, 320, 35, GREEN);
}
```

**main_cardputer.cpp:**
```cpp
void drawHeaderCompact(const char* title, uint16_t color = GREEN) {
    M5.Display.setTextSize(1);  // Fonte pequena
    M5.Display.setCursor(5, 3);
    M5.Display.drawLine(0, 14, SCREEN_WIDTH, 14, color);
}
```

---

### 2. Input Handling

**main.cpp:**
```cpp
// Loop principal
M5.update();
if (M5.BtnA.wasPressed()) {
    // Ação botão A
}
if (M5.BtnB.wasPressed()) {
    // Ação botão B
}
```

**main_cardputer.cpp:**
```cpp
// Função dedicada
void handleKeyboard() {
    if (M5Cardputer.Keyboard.isChange()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        
        if (status.word == "1") {
            // Modo 1
        }
        if (status.enter) {
            // Enter pressionado
        }
    }
}
```

---

### 3. Menu Display

**main.cpp (espaçado):**
```cpp
M5.Display.setTextSize(2);
int y = 60;
M5.Display.setCursor(20, y);
M5.Display.println("1. Handshake Capture");
y += 30;  // Espaçamento grande
M5.Display.setCursor(20, y);
M5.Display.println("2. Clone + Deauth");
```

**main_cardputer.cpp (compacto):**
```cpp
M5.Display.setTextSize(1);
int y = 22;
M5.Display.setCursor(10, y);
M5.Display.println("[1] Handshake Capture");
y += 15;  // Espaçamento pequeno
M5.Display.setCursor(10, y);
M5.Display.println("[2] Clone + Deauth");
```

---

## Manutenção

### Adicionar nova feature

Para adicionar uma feature em **ambas** versões:

1. **Implementar lógica comum** (ex: novo tipo de ataque)
   ```cpp
   // Adicionar em AMBOS arquivos
   void newAttackMode() {
       // Código WiFi (igual em ambos)
   }
   ```

2. **Adaptar display** para cada versão
   
   **main.cpp:**
   ```cpp
   void displayNewAttack() {
       M5.Display.setTextSize(2);  // Grande
       M5.Display.setCursor(10, 50);
       M5.Display.println("New Attack");
   }
   ```
   
   **main_cardputer.cpp:**
   ```cpp
   void displayNewAttackCompact() {
       M5.Display.setTextSize(1);  // Pequeno
       M5.Display.setCursor(5, 22);
       M5.Display.println("New Attack");
   }
   ```

3. **Adaptar controles**
   
   **main.cpp:**
   ```cpp
   if (M5.BtnA.wasPressed()) {
       startNewAttack();
   }
   ```
   
   **main_cardputer.cpp:**
   ```cpp
   if (status.word == "n" || status.word == "N") {
       startNewAttack();
   }
   ```

---

## Debugging

### Serial Monitor

Ambas versões usam Serial para debug:

```cpp
Serial.begin(115200);
Serial.println("M5Gotchi PRO iniciado!");
Serial.printf("EAPOL #%lu capturado!\n", eapolCount);
```

Ver logs:
```bash
pio device monitor -b 115200
```

### Debug Levels

Ajustar no `platformio.ini`:

```ini
build_flags = 
    -DCORE_DEBUG_LEVEL=5  # 0=none, 5=verbose
```

---

## Referências

- [M5Unified API](https://github.com/m5stack/M5Unified)
- [ESP32 WiFi API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html)
- [PlatformIO Docs](https://docs.platformio.org/)

---

## Licença

MIT License - Veja [../LICENSE](../LICENSE)
