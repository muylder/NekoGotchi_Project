# 📦 M5Gotchi Safe Pet - Instalação via M5Launcher

## 🎯 Arquivos Gerados

Os seguintes arquivos `.bin` foram criados na pasta `bin/`:

| Arquivo | Tamanho | Endereço | Descrição |
|---------|---------|----------|-----------|
| `bootloader.bin` | 13.69 KB | 0x0000 | ESP32-S3 bootloader |
| `partitions.bin` | 3.00 KB | 0x8000 | Tabela de partições |
| `M5Gotchi_Safe_Pet.bin` | 811.42 KB | 0x10000 | **Firmware principal (SEGURO)** |

**Total:** ~830 KB

---

## 📱 Método 1: M5Burner (Recomendado)

### Passo a Passo:

1. **Baixe o M5Burner:**
   - Windows: https://m5burner.m5stack.com/
   - Instale e abra o M5Burner

2. **Conecte o M5Cardputer via USB-C**

3. **Selecione a porta COM** (ex: COM3)

4. **Vá em "Custom" → "Add"**

5. **Preencha:**
   - Nome: `M5Gotchi Safe Pet`
   - Categoria: `Entertainment`

6. **Adicione os arquivos:**
   - Offset `0x0000` → `bootloader.bin`
   - Offset `0x8000` → `partitions.bin`
   - Offset `0x10000` → `M5Gotchi_Safe_Pet.bin`

7. **Clique em "Burn"** e aguarde!

---

## 💻 Método 2: esptool.py (Linha de Comando)

### Instalação do esptool:

```bash
pip install esptool
```

### Flash completo:

```bash
esptool.py --chip esp32s3 --port COM3 --baud 921600 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 80m --flash_size detect \
  0x0000 bin/bootloader.bin \
  0x8000 bin/partitions.bin \
  0x10000 bin/M5Gotchi_Safe_Pet.bin
```

**Substitua `COM3` pela sua porta!**

---

## 🔧 Método 3: PlatformIO (Upload Direto)

Se tiver o Cardputer conectado:

```bash
# Detectar porta
[System.IO.Ports.SerialPort]::getportnames()

# Upload (substitua COM3)
pio run -e m5stack-cardputer -t upload --upload-port COM3
```

---

## 🛡️ Verificação de Segurança

Após instalar, verifique:

✅ **NÃO** deve salvar senhas WiFi
✅ **NÃO** deve criar `/credentials.txt`
✅ **NÃO** deve ter modo "Evil Portal"
✅ **SÓ** deve ter: Pet, WiFi Scanner (passivo), Menu

Para garantir segurança, delete arquivos antigos do SD:

```
/config.json
/credentials.txt
/session.json
/logs/*
/handshakes/*
/portals/*
```

---

## 🎮 Controles do Pet

| Tecla | Ação |
|-------|------|
| **G** | Abrir Menu |
| **;** | ▲ Cima |
| **.** | ▼ Baixo |
| **Enter** | ✅ Selecionar |
| **Space** | 🐾 Fazer carinho |
| **O** | 🍕 Alimentar |

---

## 📊 Informações Técnicas

- **Platform:** ESP32-S3 (240MHz)
- **RAM Usage:** 14.3% (46.8 KB / 320 KB)
- **Flash Usage:** 26.4% (830 KB / 3 MB)
- **Framework:** Arduino-ESP32 2.0.11
- **Libraries:**
  - M5Unified 0.2.10
  - M5Cardputer 1.1.1
  - WiFi (scan only)

---

## 🐱 Features do Pet

- **Status System:**
  - 🍕 Hunger (diminui a cada 30s)
  - 💗 Happiness (diminui a cada 45s)
  - ⚡ Energy (diminui a cada 60s)

- **Progression:**
  - 🏆 XP e Level
  - 🎨 Cat sprite 3x maior
  - 📡 WiFi scanner passivo

- **Tema Kawaii:**
  - 💗 Rosa: `0xFD20`
  - 🩵 Ciano: `0x07FF`
  - 🌙 Background: `0x0010`

---

## ⚠️ IMPORTANTE

Este é o firmware **SEGURO** sem recursos de pentesting!

Se precisar da versão completa (com handshakes, evil portal, etc.),
use `main_cardputer.cpp` - mas **APENAS em redes autorizadas!**

**NUNCA** use ferramentas de hacking em redes públicas ou sem permissão!

---

## 🔄 Reverter para Firmware Original

Para voltar ao firmware M5Stack original:

1. Abra o M5Burner
2. Procure por "Cardputer Launcher"
3. Faça o flash

---

## 📞 Suporte

- GitHub: https://github.com/muylder/NekoGotchi_Project
- Issues: https://github.com/muylder/NekoGotchi_Project/issues

---

**Aproveite seu pet virtual! 🐾✨**
