# ✅ M5Gotchi Safe Pet - Pronto para SD Card!

## 📦 Arquivo Criado

**Firmware Merged (Tudo-em-Um):**
- 📁 `launcher_package/M5Gotchi_Safe_Pet_MERGED.bin` **(875 KB)**
- 📁 `bin/M5Gotchi_Safe_Pet_MERGED.bin` **(cópia)**

Este é um arquivo **único** contendo:
- ✅ Bootloader ESP32-S3
- ✅ Tabela de partições
- ✅ Firmware do pet kawaii

---

## 🚀 Como Usar (Super Simples!)

### 1️⃣ Copie para o SD Card

```
SD_CARD/
└── M5Gotchi_Safe_Pet_MERGED.bin  (875 KB)
```

### 2️⃣ No Cardputer:

1. Insira o SD card
2. Ligue o Cardputer
3. Abra o **M5Launcher**
4. Vá em: **Apps → Custom** (ou **Browser → SD**)
5. Selecione `M5Gotchi_Safe_Pet_MERGED.bin`
6. Pressione **ENTER**
7. Aguarde o flash (NÃO DESLIGUE!)
8. Pressione **RESET**
9. Pronto! 🐱

---

## 💻 OU flasheie via USB:

```bash
# Detectar porta
[System.IO.Ports.SerialPort]::getportnames()

# Flash (substitua COM3)
esptool.py --chip esp32s3 --port COM3 write_flash 0x0 M5Gotchi_Safe_Pet_MERGED.bin
```

**Mais fácil ainda!** Note que o offset é `0x0` (começa do zero) porque é um firmware merged!

---

## 🎮 Controles do Pet

```
G       = Menu
;       = Cima ▲
.       = Baixo ▼
Enter   = Selecionar ✅
Space   = Fazer carinho 🐾
O       = Alimentar 🍕
```

---

## 🛡️ 100% Seguro

✅ NÃO salva senhas WiFi  
✅ NÃO tem pentesting  
✅ Só diversão com pet virtual!

---

## 📁 Localizações do Arquivo

O firmware merged está disponível em **dois lugares**:

1. **`bin/M5Gotchi_Safe_Pet_MERGED.bin`**
   - Para backup e distribuição

2. **`launcher_package/M5Gotchi_Safe_Pet_MERGED.bin`**
   - Com JSON e README para SD card

**Use qualquer um - são idênticos!** (875 KB)

---

## 📊 Especificações

| Spec | Valor |
|------|-------|
| Tamanho | 875 KB (0.85 MB) |
| Tipo | Firmware merged (all-in-one) |
| Offset | 0x0000 (começa do zero) |
| Chip | ESP32-S3 |
| RAM Usage | 14.3% (46.8 KB) |
| Flash Usage | 26.4% (830 KB) |

---

## 🔧 Diferenças dos Arquivos

### Arquivos Separados (bin/):
```
bootloader.bin    13.7 KB  → offset 0x0000
partitions.bin     3.0 KB  → offset 0x8000
firmware.bin     811.4 KB  → offset 0x10000
```
**Total:** ~828 KB em 3 arquivos

### Arquivo Merged (launcher_package/):
```
M5Gotchi_Safe_Pet_MERGED.bin  875 KB  → offset 0x0000
```
**Total:** 875 KB em 1 arquivo único

**O merged é mais fácil para SD card!** Só copiar e instalar. 🎯

---

## ⚠️ Limpeza de Segurança

Se usou firmware de pentesting antes:

1. **Delete do SD card:**
   - `/config.json`
   - `/credentials.txt`
   - `/session.json`
   - Pastas: `/logs/`, `/handshakes/`, `/portals/`

2. **MUDE SUA SENHA WIFI!**
   - Acesse: http://192.168.1.1
   - Login no roteador
   - Wireless → Security → Mude senha

Veja `CLEANUP_QUICKSTART.md` para detalhes!

---

## 📚 Documentação Completa

- **`launcher_package/README_SDCARD.md`** - Guia de instalação via SD
- **`launcher_package/M5Gotchi_Safe_Pet.json`** - Metadados completos
- **`bin/INSTALLATION_GUIDE.md`** - Guia geral de instalação
- **`bin/PACKAGE_INFO.md`** - Info do pacote bin/
- **`CLEANUP_QUICKSTART.md`** - Limpeza de segurança

---

## ✨ Pronto para Diversão!

Seu **M5Gotchi Safe Pet** está 100% pronto!

Copie `M5Gotchi_Safe_Pet_MERGED.bin` para o SD card e instale via Launcher.

**Divirta-se com seu pet kawaii!** 🐱💗✨

---

**Gerado em:** 2025-10-30  
**Build:** PlatformIO 6.4.0  
**Status:** ✅ Firmware merged criado com sucesso  
**Método:** esptool merge_bin  
**Checksum:** SHA256 updated in image

🐾 **Cuide bem do seu pet!**
