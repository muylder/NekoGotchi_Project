# 🔧 Guia de Instalação via Arduino IDE

## ⚠️ PROBLEMA IDENTIFICADO

O firmware compila e grava perfeitamente, mas **não executa** no Cardputer.  
Isso indica incompatibilidade entre PlatformIO/Arduino Framework e o hardware.

---

## 🚀 SOLUÇÃO: ARDUINO IDE 2.x

### 📥 1. BAIXAR E INSTALAR

**Arduino IDE 2.3.2:**
https://www.arduino.cc/en/software

### ⚙️ 2. CONFIGURAR ESP32

1. Abra Arduino IDE
2. Vá em: **File → Preferences**
3. Em "Additional Board Manager URLs", adicione:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Clique OK
5. Vá em: **Tools → Board → Boards Manager**
6. Procure por "esp32"
7. Instale: **esp32 by Espressif Systems** (versão 3.0.0 ou superior)

### 📚 3. INSTALAR BIBLIOTECAS

Vá em: **Sketch → Include Library → Manage Libraries**

Instale as seguintes bibliotecas:

```
1. M5Unified (by M5Stack)
2. M5Cardputer (by M5Stack)
3. ArduinoJson (by Benoit Blanchon)
4. NimBLE-Arduino (by h2zero)
5. IRremoteESP8266 (by crankyoldgit)
6. rc-switch (by sui77)
7. LoRa (by Sandeep Mistry)
8. TinyGPSPlus (by Mikal Hart)
9. MFRC522 (by GithubCommunity)
10. Adafruit PN532
```

### 🎯 4. CONFIGURAR PLACA

1. **Tools → Board → esp32 → ESP32S3 Dev Module**

2. Configure os seguintes parâmetros:
   ```
   Upload Speed: 1500000
   USB Mode: Hardware CDC and JTAG
   USB CDC On Boot: Enabled
   USB Firmware MSC On Boot: Disabled
   USB DFU On Boot: Disabled
   Upload Mode: UART0 / Hardware CDC
   CPU Frequency: 240MHz (WiFi)
   Flash Mode: DIO
   Flash Size: 8MB (64Mb)
   Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
   Core Debug Level: None
   PSRAM: Disabled
   Arduino Runs On: Core 1
   Events Run On: Core 1
   ```

3. **Tools → Port → COMx** (selecione a porta do Cardputer)

### 📝 5. ABRIR O CÓDIGO

1. Abra o arquivo: `src/main.cpp` no Arduino IDE
2. OU copie todo o conteúdo de `src/main.cpp` para uma nova sketch

### 🔄 6. COMPILAR E GRAVAR

1. Clique em **Verify** (✓) para compilar
2. Se compilar sem erros, clique em **Upload** (→)
3. Aguarde o upload completar
4. Pressione **RESET** no Cardputer

---

## 🆘 SE AINDA NÃO FUNCIONAR

### Opção A: Testar com Blink Básico

Teste primeiro com código SUPER básico:

```cpp
void setup() {
  pinMode(21, OUTPUT);
}

void loop() {
  digitalWrite(21, HIGH);
  delay(500);
  digitalWrite(21, LOW);
  delay(500);
}
```

Se o LED não piscar = **PROBLEMA DE HARDWARE OU BOOTLOADER**

### Opção B: Usar M5Burner

1. Baixe M5Burner: https://docs.m5stack.com/en/download
2. Conecte o Cardputer
3. Selecione "M5Cardputer"
4. Escolha um firmware oficial (ex: Factory Test)
5. Grave
6. Teste se funciona

Se o firmware oficial funcionar = **Incompatibilidade do nosso código**  
Se não funcionar = **Hardware ou bootloader corrompido**

### Opção C: Regravar Bootloader ESP32-S3

```bash
# Download bootloader oficial ESP32-S3
curl -O https://github.com/espressif/arduino-esp32/raw/master/tools/sdk/esp32s3/bin/bootloader_dio_80m.bin

# Grave no offset 0x0
python -m esptool --port COM3 --chip esp32s3 write_flash 0x0 bootloader_dio_80m.bin
```

---

## 📊 DIAGNÓSTICO DO PROBLEMA

### Sintomas:
- ✅ Compilação OK
- ✅ Upload OK
- ✅ Hash verificado
- ❌ Firmware não executa
- ❌ LED não pisca
- ❌ Serial sem output

### Causa Provável:
**Bootloader incompatível** ou **Arduino Framework** do PlatformIO não suporta completamente o Cardputer.

### Solução:
Arduino IDE usa toolchain diferente que pode ser mais compatível.

---

## 🔗 LINKS ÚTEIS

- Arduino IDE: https://www.arduino.cc/en/software
- ESP32 Board Support: https://github.com/espressif/arduino-esp32
- M5Burner: https://docs.m5stack.com/en/download
- M5Cardputer Docs: https://docs.m5stack.com/en/core/M5Cardputer

---

**🙏 Boa sorte! Se conseguir fazer funcionar com Arduino IDE, me avise!**
