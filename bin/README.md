# 📦 M5Gotchi Pro - Binários

Esta pasta contém os binários compilados do M5Gotchi Pro prontos para flash.

## 🚀 Quick Flash

### Usando script automático (Linux/Mac)

```bash
# M5Stack Core
./flash_m5stack-core-esp32.sh /dev/ttyUSB0

# M5Stack Fire
./flash_m5stack-fire.sh /dev/ttyUSB0

# M5Stack Core2
./flash_m5stack-core2.sh /dev/ttyUSB0
```

### Windows

```cmd
flash_m5stack-core-esp32.sh COM3
```

## 📥 Flash Manual

### Instalar esptool

```bash
pip install esptool
```

### Comandos de Flash

#### M5Stack Core ESP32
```bash
esptool.py --chip esp32 \
    --port /dev/ttyUSB0 \
    --baud 921600 \
    --before default_reset \
    --after hard_reset \
    write_flash -z \
    --flash_mode dio \
    --flash_freq 40m \
    --flash_size detect \
    0x1000 bootloader_m5stack-core-esp32.bin \
    0x8000 partitions_m5stack-core-esp32.bin \
    0x10000 M5Gotchi_Pro_m5stack-core-esp32.bin
```

#### M5Stack Fire
```bash
esptool.py --chip esp32 \
    --port /dev/ttyUSB0 \
    --baud 921600 \
    write_flash -z \
    0x1000 bootloader_m5stack-fire.bin \
    0x8000 partitions_m5stack-fire.bin \
    0x10000 M5Gotchi_Pro_m5stack-fire.bin
```

## 📋 Arquivos

| Arquivo | Endereço | Descrição |
|---------|----------|-----------|
| `bootloader_*.bin` | 0x1000 | ESP32 bootloader |
| `partitions_*.bin` | 0x8000 | Tabela de partições |
| `M5Gotchi_Pro_*.bin` | 0x10000 | Firmware principal |

## 🛠️ Troubleshooting

### Dispositivo não detectado
- Instalar driver CH340/CP210x
- Verificar cabo USB (deve suportar dados)
- Testar portas: `ls /dev/tty*` (Linux/Mac)

### Erro ao fazer flash
- Segurar botão A durante reset
- Tentar baud menor: `--baud 115200`
- Verificar permissões (Linux): `sudo usermod -a -G dialout $USER`

### Permission denied (Linux)
```bash
# Adicionar usuário ao grupo dialout
sudo usermod -a -G dialout $USER

# Fazer logout e login novamente
```

## 🔍 Verificar Flash

Após flash bem-sucedido, o M5Stack deve:
1. Mostrar "M5GOTCHI PRO" no boot
2. Exibir menu principal
3. SD Card detectado (se inserido)

## 📊 Tamanho dos Arquivos

Tamanho aproximado por dispositivo:
- Bootloader: ~27KB
- Partitions: ~3KB  
- Firmware: ~1.2MB

Total: ~1.23MB por dispositivo

## 🔄 Atualização

Para atualizar firmware existente:
```bash
# Apenas o firmware (mais rápido)
esptool.py --port /dev/ttyUSB0 \
    write_flash 0x10000 M5Gotchi_Pro_m5stack-core-esp32.bin
```

## 📞 Suporte

Problemas com flash?
- Verifique documentação: [../docs/](../docs/)
- Abra issue: [GitHub Issues](https://github.com/seu-usuario/M5Gotchi_Pro/issues)

---

**Compiled binaries for M5Gotchi Pro**
*Ready to flash and use!*
