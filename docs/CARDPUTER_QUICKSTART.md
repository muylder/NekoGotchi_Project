# Compilação Rápida - M5Gotchi Pro Cardputer

## Windows (PowerShell)

```powershell
# Instalar PlatformIO (primeira vez)
pip install platformio

# Compilar para Cardputer
pio run -e m5stack-cardputer

# Compilar e fazer upload
pio run -e m5stack-cardputer --target upload

# Monitorar serial
pio device monitor
```

## Linux/macOS (Bash)

```bash
# Instalar PlatformIO (primeira vez)
pip3 install platformio

# Compilar para Cardputer
pio run -e m5stack-cardputer

# Compilar e fazer upload
pio run -e m5stack-cardputer --target upload

# Monitorar serial
pio device monitor
```

## Usando Makefile

```bash
# Compilar
make build-cardputer

# Upload
make upload-cardputer

# Tudo de uma vez
make build-cardputer && make upload-cardputer
```

## Usando Build Script

```bash
# Apenas Cardputer
./build.sh cardputer

# Todos os dispositivos
./build.sh all
```

## Troubleshooting

### Porta serial não encontrada

**Windows:**
```powershell
# Listar portas
pio device list

# Upload especificando porta
pio run -e m5stack-cardputer --target upload --upload-port COM3
```

**Linux/macOS:**
```bash
# Listar portas
ls /dev/tty*

# Upload especificando porta
pio run -e m5stack-cardputer --target upload --upload-port /dev/ttyUSB0

# Se permissão negada
sudo usermod -a -G dialout $USER
# Logout e login novamente
```

### Erro de compilação

```bash
# Limpar build anterior
pio run --target clean

# Reinstalar dependências
pio pkg install

# Tentar novamente
pio run -e m5stack-cardputer
```

### SD Card não detectado

1. Verificar formato: **FAT32 obrigatório**
2. Máximo **32GB** recomendado
3. Verificar pinos no código:
   ```cpp
   #define SCK  36
   #define MISO 35
   #define MOSI 37
   #define SS   34
   ```

## Estrutura do SD Card

```
SD Card (FAT32)
├── portal.html              # Template WiFi genérico
├── portal_netflix.html      # Template Netflix
├── portal_facebook.html     # Template Facebook
├── portal_google.html       # Template Google
├── credentials.txt          # Auto-gerado
└── *.pcap                   # Auto-gerado
```

## Primeiros Passos

1. **Compilar firmware**
   ```bash
   pio run -e m5stack-cardputer --target upload
   ```

2. **Preparar SD Card**
   - Formatar como FAT32
   - Copiar arquivos de `data/portal_templates/`

3. **Usar o dispositivo**
   - Pressionar `1` = Handshake Capture
   - Pressionar `2` = Clone & Deauth
   - Pressionar `3` = Evil Portal

4. **Ver documentação completa**
   - [docs/CARDPUTER_GUIDE.md](CARDPUTER_GUIDE.md)

## Controles Rápidos

```
MENU:        1=HS | 2=Clone | 3=Portal
HANDSHAKE:   T=Modo | R=Reset | ESC=Sair
CLONE:       ↑↓=Nav | ENTER=Clone | SPACE=Deauth | ESC=Sair
PORTAL:      H=HTML | ESC=Sair
```

## Atalhos de Teclado

| Tecla | Função |
|-------|--------|
| `1`, `2`, `3` | Selecionar modo |
| `ESC` | Voltar ao menu |
| `T` | Toggle modo canal (Handshake) |
| `R` | Reset contadores (Handshake) |
| `↑` `↓` | Navegar (Clone) |
| `ENTER` | Clonar rede (Clone) |
| `SPACE` | Toggle Deauth (Clone) |
| `H` | Trocar template HTML (Portal) |

## Performance Esperada

- **Handshake Capture**: 5-15 handshakes em 30min
- **Deauth Attack**: ~100 pacotes/min
- **Evil Portal**: 3-10 clientes simultâneos
- **Autonomia**: ~8h com bateria 1400mAh

---

**⚠️ LEMBRETE:** Use apenas em redes próprias ou com autorização!
