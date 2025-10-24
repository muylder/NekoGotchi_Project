# 🎮 M5Gotchi Pro - Cardputer Edition

## 📱 Visão Geral

Versão **otimizada** do M5Gotchi Pro para **M5Stack Cardputer**, aproveitando ao máximo:
- 🖥️ Tela compacta 240x135 pixels
- ⌨️ Teclado QWERTY integrado
- 🧠 ESP32-S3 com PSRAM
- 💾 SD Card via SPI customizado

---

## 🎯 Diferenciais da Versão Cardputer

### ✨ Otimizações Implementadas

| Recurso | M5Stack Original | Cardputer Edition |
|---------|------------------|-------------------|
| **Controles** | 3 botões físicos (A, B, C) | Teclado QWERTY completo |
| **Interface** | 320x240 (fonte tamanho 2) | 240x135 (fonte tamanho 1) |
| **Layout** | Headers grandes | Headers compactos |
| **Navegação** | Botões únicos | Teclas + atalhos |
| **Redes visíveis** | 7 redes | 8 redes (otimizado) |
| **Status bar** | 3 controles | Texto completo de help |

### 🚀 Vantagens Exclusivas

1. **Atalhos Rápidos**
   - Acesso direto via teclas numéricas
   - Não precisa navegar por menus
   
2. **Portabilidade**
   - Menor e mais leve que M5Stack
   - Teclado integrado (sem periféricos)
   
3. **Eficiência de Tela**
   - Layout otimizado para 135px altura
   - Mais informação em menos espaço

---

## ⌨️ Mapeamento Completo do Teclado

### 🏠 Menu Principal

```
╔═══════════════════════════════════════╗
║  M5GOTCHI PRO - CARDPUTER             ║
╠═══════════════════════════════════════╣
║                                       ║
║  [1] Handshake Capture                ║
║  [2] Clone + Deauth                   ║
║  [3] Evil Portal                      ║
║                                       ║
║  SD Card: OK                          ║
║  Tela: 240x135 | Otimizado            ║
║                                       ║
╠═══════════════════════════════════════╣
║ 1=HS | 2=Clone | 3=Portal | ESC=Sair ║
╚═══════════════════════════════════════╝
```

| Tecla | Ação |
|-------|------|
| `1` | Iniciar Handshake Capture |
| `2` | Iniciar Clone & Deauth |
| `3` | Iniciar Evil Portal |
| `ESC` | Sair (quando disponível) |

---

### 📡 Modo: Handshake Capture

```
╔═══════════════════════════════════════╗
║  HANDSHAKE CAPTURE                    ║
╠═══════════════════════════════════════╣
║  Canal: 06 | PRIORITARIO              ║
║  Pacotes: 45832                       ║
║  EAPOL: 12                            ║
║  PMKID: 3                             ║
║  Arquivo: 1.pcap (2456KB)             ║
║  Runtime: 15m 32s                     ║
╠═══════════════════════════════════════╣
║ T=Modo | R=Reset | ESC=Menu           ║
╚═══════════════════════════════════════╝
```

| Tecla | Ação | Descrição |
|-------|------|-----------|
| `T` | Toggle modo canal | Alterna entre PRIORITÁRIO (1,6,11) e TODOS (1-13) |
| `R` | Reset contadores | Zera pacotes, EAPOL e PMKID |
| `ESC` | Voltar ao menu | Para captura e fecha arquivo PCAP |

**💡 Dicas:**
- Modo PRIORITÁRIO: Foca nos canais mais usados (1, 6, 11) → 80% das redes
- Modo TODOS: Varre todos os 13 canais → Mais lento, mas completo
- PMKID capturado = handshake completo SEM deauth! 🎯

---

### 🎯 Modo: Clone & Deauth

```
╔═══════════════════════════════════════╗
║  CLONE & DEAUTH                       ║
╠═══════════════════════════════════════╣
║  Home WiFi                        -45dB
║▓▓My Network 5G                    -52dB
║  Starbucks WiFi                   -67dB
║  NETGEAR_2G                       -71dB
║  TP-Link_5678                     -73dB
║  iPhone de João                   -78dB
║                                       ║
║  DEAUTH ATIVO: 125 pkts               ║
╠═══════════════════════════════════════╣
║ UP/DN=Nav | ENTER=Clone | SPACE=Deauth║
╚═══════════════════════════════════════╝
```

| Tecla | Ação | Descrição |
|-------|------|-----------|
| `↑` (`` ` ``) | Navegar cima | Seleciona rede anterior |
| `↓` (`;`) | Navegar baixo | Seleciona próxima rede |
| `ENTER` | Clonar rede | Cria AP clone da rede selecionada |
| `SPACE` | Toggle Deauth | Liga/desliga ataque deauth automático |
| `ESC` | Voltar ao menu | Para deauth e volta para menu |

**💡 Dicas:**
- ▓▓ = Rede selecionada (fundo verde)
- -45dB = Sinal forte (perto)
- -80dB = Sinal fraco (longe)
- Clone + Deauth = Força clientes a conectar no clone

**⚠️ Fluxo de Ataque:**
1. Escanear redes (automático)
2. Navegar com ↑↓ e selecionar alvo
3. Pressionar `ENTER` para clonar
4. Deauth inicia automaticamente
5. Clientes desconectam e conectam no clone

---

### 🕷️ Modo: Evil Portal

```
╔═══════════════════════════════════════╗
║  EVIL PORTAL                          ║
╠═══════════════════════════════════════╣
║  SSID: Free WiFi                      ║
║  IP: 192.168.4.1                      ║
║  Clientes: 3                          ║
║  Visitas: 15                          ║
║  Capturado: 2                         ║
║  Template:                            ║
║  /portal.html                         ║
╠═══════════════════════════════════════╣
║ H=HTML | ESC=Menu                     ║
╚═══════════════════════════════════════╝
```

| Tecla | Ação | Descrição |
|-------|------|-----------|
| `H` | Trocar HTML | Alterna entre templates disponíveis |
| `ESC` | Voltar ao menu | Para portal e volta para menu |

**🎨 Templates Disponíveis:**

Pressione `H` para alternar entre:

1. **`/portal.html`** (Padrão)
   - Design gradiente moderno
   - Roxo/azul
   
2. **`/portal_netflix.html`**
   - Interface Netflix
   - Vermelho/preto
   
3. **`/portal_facebook.html`**
   - Interface Facebook
   - Azul Facebook
   
4. **`/portal_google.html`**
   - Material Design
   - Cores Google

**💡 Dicas:**
- Visitas = Acessos à página inicial
- Capturado = Credenciais enviadas via POST
- Tudo salvo em `/credentials.txt` no SD Card
- DNS Spoofing: TODA URL redireciona para o portal

---

## 📦 Compilação e Upload

### Opção 1: PlatformIO CLI (Recomendado)

```bash
# Compilar para Cardputer
pio run -e m5stack-cardputer

# Compilar e fazer upload
pio run -e m5stack-cardputer --target upload

# Monitorar serial
pio device monitor
```

### Opção 2: Makefile

```bash
# Compilar
make build-cardputer

# Upload
make upload-cardputer

# Monitorar
make monitor
```

### Opção 3: Build Script

```bash
# Compilar apenas Cardputer
./build.sh cardputer

# Compilar todos os dispositivos
./build.sh all
```

### Opção 4: VS Code + PlatformIO

1. Instalar extensão PlatformIO
2. Abrir projeto
3. Selecionar env: `m5stack-cardputer`
4. Clicar em "Upload"

---

## 🔧 Configuração do SD Card

### Estrutura Recomendada

```
SD Card (FAT32)
├── portal.html              # Template padrão
├── portal_netflix.html      # Template Netflix
├── portal_facebook.html     # Template Facebook
├── portal_google.html       # Template Google
├── credentials.txt          # Credenciais capturadas (auto)
├── 1.pcap                   # Handshakes capturados (auto)
├── 2.pcap                   # Próximo arquivo (auto)
└── ...
```

### Como Preparar

1. **Formatar SD Card como FAT32**
   ```bash
   # Linux
   sudo mkfs.vfat -F 32 /dev/sdX1
   
   # Windows: Formatar > FAT32
   # macOS: Disk Utility > MS-DOS (FAT)
   ```

2. **Copiar templates HTML**
   ```bash
   cp data/portal_templates/*.html /media/sdcard/
   ```

3. **Inserir no Cardputer**
   - Slot SD na parte traseira
   - Push-push para travar

---

## 🎓 Guia de Uso Prático

### Cenário 1: Capturar Handshake WPA2

**Objetivo:** Capturar handshake de uma rede específica

```
1. Inserir SD Card no Cardputer
2. Ligar dispositivo
3. Pressionar [1] no menu
4. Aguardar "EAPOL" aparecer nos contadores
5. Se nada capturar em 5min, pressionar [T] (trocar modo)
6. Pressionar [ESC] para voltar ao menu
7. Remover SD Card e analisar .pcap no PC
```

**Análise no PC:**
```bash
# Ver conteúdo do PCAP
tcpdump -r 1.pcap

# Tentar quebrar com hashcat
hcxpcapngtool -o hash.hc22000 1.pcap
hashcat -m 22000 hash.hc22000 wordlist.txt
```

---

### Cenário 2: Clonar Rede e Forçar Desconexão

**Objetivo:** Criar AP clone e desconectar clientes da rede real

```
1. Ligar Cardputer
2. Pressionar [2] no menu
3. Aguardar scan (automático)
4. Usar [↑] [↓] para selecionar rede alvo
5. Pressionar [ENTER] para clonar
6. Aguardar 2s (clone sendo criado)
7. Deauth inicia automaticamente
8. Observar "DEAUTH ATIVO: X pkts"
9. Clientes desconectam e conectam no clone
10. Pressionar [SPACE] para parar deauth
11. Pressionar [ESC] para voltar
```

**⚠️ Atenção:**
- Só use em redes que você administra!
- Ideal para testar segurança de sua empresa

---

### Cenário 3: Capturar Credenciais com Portal

**Objetivo:** Criar portal captivo falso

```
1. Inserir SD Card com templates HTML
2. Ligar Cardputer
3. Pressionar [3] no menu
4. Portal cria AP "Free WiFi" automaticamente
5. Vítima conecta no AP
6. Browser abre automaticamente (captive portal)
7. Vítima digita SSID + senha
8. Credenciais salvas em /credentials.txt
9. Vítima vê mensagem "Conectado!"
10. Pressionar [H] para trocar template
11. Pressionar [ESC] quando terminar
```

**Ver credenciais capturadas:**
```bash
# No SD Card do PC
cat credentials.txt

=== Captura #1 ===
Time: 456789 ms
SSID: MyHomeWiFi
Password: SuperSecret123
IP: 192.168.4.2
---
```

---

## 🐛 Troubleshooting

### Problema: SD Card não detectado

**Sintomas:**
- "SD Card: FALHA" no menu
- Serial mostra "SD Card falhou!"

**Soluções:**
1. Verificar formato: FAT32 obrigatório
2. Verificar contatos: Limpar com álcool isopropílico
3. Testar outro cartão: Máximo 32GB recomendado
4. Verificar pinos SPI:
   ```cpp
   #define SCK  36
   #define MISO 35
   #define MOSI 37
   #define SS   34
   ```

---

### Problema: Teclado não responde

**Sintomas:**
- Pressionar teclas não faz nada
- Menu não muda ao pressionar [1] [2] [3]

**Soluções:**
1. Verificar delay no loop: Deve ser ≤ 50ms
2. Reiniciar Cardputer
3. Verificar biblioteca M5Unified atualizada:
   ```bash
   pio pkg update m5stack/M5Unified
   ```
4. Testar teclas no serial monitor (modo debug)

---

### Problema: Nenhum handshake capturado

**Sintomas:**
- EAPOL sempre em 0
- Arquivo PCAP vazio

**Soluções:**
1. **Usar modo PRIORITÁRIO** (padrão)
   - Pressionar [T] se estiver em TODOS
   
2. **Aguardar mais tempo**
   - Mínimo 10-15 minutos
   - Handshakes só ocorrem quando cliente conecta
   
3. **Forçar reconexão** (ético!)
   - Em OUTRO Cardputer: Modo [2] Deauth
   - Ou desligar/religar roteador
   
4. **Verificar canal**
   - Sua rede está em canal 1, 6 ou 11?
   - Se não, pressionar [T] para modo TODOS

---

### Problema: Portal não redireciona

**Sintomas:**
- Cliente conecta no "Free WiFi"
- Browser não abre automaticamente
- Ou abre mas não carrega página

**Soluções:**
1. **Testar acesso manual**
   - Abrir browser
   - Digitar: `http://192.168.4.1`
   
2. **Verificar DNS Spoofing**
   - No código, checar: `dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());`
   
3. **Testar template HTML**
   - Verificar `/portal.html` existe no SD
   - Se não existir, usa HTML embutido (ok)
   
4. **iOS/Android diferente**
   - iOS: Abre popup automático
   - Android: Notificação "Login necessário"
   - Desktop: Manual `http://192.168.4.1`

---

## 📊 Comparação de Performance

### M5Stack Core vs Cardputer

| Métrica | M5Stack Core | Cardputer |
|---------|-------------|-----------|
| **Tela útil** | 320x240 (76.8k px) | 240x135 (32.4k px) |
| **Redes por tela** | 7 redes | 8 redes |
| **FPS interface** | ~30 FPS | ~40 FPS (menor tela) |
| **Controles** | 3 botões | 47 teclas |
| **Atalhos** | 0 | 10+ atalhos |
| **Peso** | 56g | 36g |
| **Bateria** | 150mAh | 1400mAh |
| **Autonomia** | ~2h | ~8h |

**🏆 Vencedor:** Cardputer para mobilidade e autonomia!

---

## 🔐 Uso Ético e Legal

### ⚖️ IMPORTANTE

Este software é **EXCLUSIVAMENTE EDUCACIONAL**:

✅ **Permitido:**
- Testar segurança de suas próprias redes
- Ambientes de laboratório controlados
- Pesquisa acadêmica com autorização
- Auditorias de segurança contratadas

❌ **PROIBIDO:**
- Atacar redes de terceiros sem autorização
- Capturar dados de outras pessoas
- Uso malicioso de qualquer tipo
- Violação de privacidade

**⚠️ Consequências:**
- Uso não autorizado é **CRIME**
- Lei de Crimes Cibernéticos (Lei 12.737/2012)
- Pena: 3 meses a 1 ano + multa

---

## 🎯 Próximos Passos

### Melhorias Futuras Planejadas

- [ ] Suporte WPA3
- [ ] Beacon flooding via teclado
- [ ] Multiple SSID spoofing
- [ ] Exportar handshakes via WiFi
- [ ] Interface web de configuração
- [ ] Logs em tempo real no display
- [ ] Atalhos customizáveis
- [ ] Modo stealth (display off)

### Como Contribuir

1. Fork do projeto
2. Criar branch: `git checkout -b feature/minha-feature`
3. Commit: `git commit -m 'Adiciona feature X'`
4. Push: `git push origin feature/minha-feature`
5. Abrir Pull Request

---

## 📞 Suporte

### Documentação Adicional

- [README.md](../README.md) - Overview geral
- [PRO_GUIDE.md](PRO_GUIDE.md) - Guia completo M5Stack
- [QUICKSTART.md](QUICKSTART.md) - Setup rápido
- [COMPARISON.md](COMPARISON.md) - Comparações técnicas

### Logs de Debug

Para ativar logs detalhados:

```ini
# platformio.ini
build_flags = 
    ${env.build_flags}
    -DCORE_DEBUG_LEVEL=5  # 0=nada, 5=verbose
```

Depois:
```bash
pio device monitor -b 115200
```

---

## 📜 Licença

MIT License - Veja [LICENSE](../LICENSE)

**Disclaimer:** Os autores não se responsabilizam por uso indevido desta ferramenta.

---

## 🎮 Resumo de Teclas Rápido

```
MENU:        1=HS | 2=Clone | 3=Portal
HANDSHAKE:   T=Modo | R=Reset | ESC=Sair
CLONE:       ↑↓=Nav | ENTER=Clone | SPACE=Deauth | ESC=Sair
PORTAL:      H=HTML | ESC=Sair
```

**Boa sorte e pentest ético! 🛡️**
