# 🎮 M5Gotchi Pro - Edição Cardputer
## Guia Rápido de Referência (Português)

### 📱 Visão Geral
Suite de pentest WiFi otimizada para M5Stack Cardputer com teclado QWERTY e tela 240x135.

---

## ⌨️ Sistema de Navegação Simples

**Todos os modos usam os mesmos controles:**
- **↑ (Seta Cima)**: Navegar para cima / Opção anterior
- **↓ (Seta Baixo)**: Navegar para baixo / Próxima opção  
- **ENTER**: Confirmar seleção / Executar ação
- **ESC (DEL)**: Voltar ao menu principal

**Não precisa memorizar teclas!** Apenas use setas e ENTER.

---

## 🏠 Menu Principal

```
╔═══════════════════════════════════════╗
║  M5GOTCHI PRO - CARDPUTER             ║
╠═══════════════════════════════════════╣
║                                       ║
║  > Handshake Capture    <-- cursor    ║
║    Clone + Deauth                     ║
║    Evil Portal                        ║
║                                       ║
║  SD Card: OK                          ║
║                                       ║
╠═══════════════════════════════════════╣
║ UP/DN: Navegar | ENTER: Selecionar   ║
╚═══════════════════════════════════════╝
```

| Tecla | Ação |
|-------|------|
| ↑/↓ | Navegar opções |
| ENTER | Entrar no modo selecionado |

---

## 📡 Modo 1: Captura de Handshake

### Display
```
╔═══════════════════════════════════════╗
║  HANDSHAKE CAPTURE                    ║
╠═══════════════════════════════════════╣
║  Canal: 06 | PRIORITARIO              ║
║  Pacotes: 45832                       ║
║  EAPOL: 12                            ║
║  PMKID: 3                             ║
║  Arquivo: 1.pcap (2456KB)             ║
║                                       ║
║  > Alternar Modo       <-- cursor     ║
║    Resetar Contadores                 ║
║    Voltar ao Menu                     ║
╠═══════════════════════════════════════╣
║ UP/DN: Selecionar | ENTER: Executar  ║
╚═══════════════════════════════════════╝
```

### Menu de Ações
| Opção | Ação |
|-------|------|
| Alternar Modo | Alterna entre PRIORITÁRIO (1,6,11) e TODOS (1-13) os canais |
| Resetar Contadores | Zera contadores de pacotes/EAPOL/PMKID |
| Voltar ao Menu | Para captura e retorna ao menu principal |

### O que faz
- Captura handshakes WPA/WPA2 passivamente
- Salva em arquivos .pcap no cartão SD
- Captura PMKID (handshake SEM deauth!)

---

## 🎯 Modo 2: Clone & Deauth

### Display
```
╔═══════════════════════════════════════╗
║  CLONE & DEAUTH                       ║
╠═══════════════════════════════════════╣
║▓▓WiFi Casa                        -45dB  <-- selecionada
║  Minha Rede 5G                    -52dB
║  Starbucks WiFi                   -67dB
║  NET_VIRTUA                       -71dB
║  TP-Link_5678                     -73dB
║  iPhone João                      -78dB
║                                       ║
║  > Clonar Rede         <-- cursor     ║
║    Alternar Deauth                    ║
║    Voltar ao Menu                     ║
║                                       ║
║  DEAUTH ATIVO: 125 pkts               ║
╠═══════════════════════════════════════╣
║ UP/DN: Selecionar | ENTER: Executar  ║
╚═══════════════════════════════════════╝
```

### Navegação em Dois Níveis
1. **Lista de Redes**: Use ↑/↓ para selecionar rede alvo
2. **Menu de Ações**: Continue pressionando ↑/↓ para alcançar ações

### Menu de Ações
| Opção | Ação |
|-------|------|
| Clonar Rede | Cria AP falso com o SSID da rede selecionada |
| Alternar Deauth | Liga/desliga ataque deauth (força desconexão) |
| Voltar ao Menu | Para ataque e retorna ao menu principal |

### Fluxo de Ataque
1. Navegue até rede alvo → ENTER em "Clonar Rede"
2. AP é clonado, deauth inicia automaticamente
3. Navegue até "Alternar Deauth" → ENTER para parar ataque
4. Navegue até "Voltar ao Menu" → ENTER para sair

---

## 🕷️ Modo 3: Evil Portal

### Display
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
║                                       ║
║  > Trocar Template     <-- cursor     ║
║    Voltar ao Menu                     ║
╠═══════════════════════════════════════╣
║ UP/DN: Selecionar | ENTER: Executar  ║
╚═══════════════════════════════════════╝
```

### Menu de Ações
| Opção | Ação |
|-------|------|
| Trocar Template | Alterna entre: portal.html → netflix → facebook → google → portal |
| Voltar ao Menu | Para portal e retorna ao menu principal |

### Templates Disponíveis
1. **portal.html** - Design gradiente moderno
2. **portal_netflix.html** - Interface estilo Netflix
3. **portal_facebook.html** - Interface estilo Facebook
4. **portal_google.html** - Google Material Design

### O que faz
- Cria WiFi falso "Free WiFi"
- Captura credenciais em `/credentials.txt`
- DNS spoofing redireciona todos sites para portal

---

## 🚀 Início Rápido

### 1. Compilar e Fazer Upload
```bash
# Opção 1: PlatformIO
pio run -e m5stack-cardputer --target upload

# Opção 2: Makefile
make upload-cardputer

# Opção 3: Script de build
./build.sh cardputer
```

### 2. Preparar Cartão SD
- Formatar como **FAT32**
- Copiar templates HTML de `data/portal_templates/`
- Inserir no Cardputer

### 3. Primeiro Uso
1. Ligar → Menu principal aparece
2. Use setas ↑/↓ para navegar
3. Pressione ENTER para selecionar modo
4. Siga menu de ações na tela
5. Pressione ESC a qualquer momento para voltar ao menu

---

## 💡 Dicas & Truques

### Captura de Handshake
- **Modo PRIORITÁRIO** foca nos canais 1, 6, 11 (80% das redes)
- **Modo TODOS** escaneia todos os 13 canais (mais lento mas completo)
- PMKID = handshake completo sem ataque deauth!
- Deixe rodando 10-15 minutos para melhores resultados

### Clone & Deauth
- Selecione rede com sinal mais forte (maior dB)
- Clone cria AP falso, deauth força desconexão
- Clientes reconectam no AP falso
- Use ESC para parar tudo rapidamente

### Evil Portal
- Funciona melhor em áreas movimentadas
- Troque template para combinar com público-alvo
- Credenciais salvas mesmo se dispositivo perder energia
- Confira `/credentials.txt` no cartão SD

---

## 🐛 Resolução de Problemas

### Cartão SD não detectado
- Formatar como FAT32 (não exFAT)
- Máximo 32GB recomendado
- Limpar contatos com álcool isopropílico

### Teclado não responde
- Delay configurado em 50ms (responsivo)
- Tente reiniciar o Cardputer
- Atualize biblioteca M5Unified

### Nenhum handshake capturado
- Use modo PRIORITÁRIO primeiro (mais rápido)
- Aguarde pelo menos 10 minutos
- Handshakes só ocorrem quando dispositivo conecta

### Portal não redireciona
- iOS: Popup aparece automaticamente
- Android: Notificação "Login necessário"
- Desktop: Acesse manualmente `192.168.4.1`

---

## ⚖️ Uso Legal e Ético

**APENAS PARA FINS EDUCACIONAIS**

✅ **Permitido:**
- Testar suas próprias redes
- Auditorias de segurança autorizadas
- Ambientes de laboratório controlados
- Pesquisa acadêmica com permissão

❌ **Proibido:**
- Atacar redes de terceiros
- Capturar dados de outras pessoas
- Qualquer uso malicioso
- Violações de privacidade

**Uso não autorizado é CRIME**

---

## 📊 Performance

- **Captura Handshake**: 5-15 handshakes em 30 min
- **Ataque Deauth**: ~100 pacotes/min
- **Evil Portal**: 3-10 clientes simultâneos
- **Bateria**: ~8h (1400mAh)

---

## 🎯 Resumo de Controles

```
EM QUALQUER LUGAR:
  ↑ ↓      = Navegar opções
  ENTER    = Confirmar / Executar
  ESC      = Voltar ao menu principal

Só isso! Simples e consistente.
```

---

**Versão:** 2.0.0 Edição Cardputer  
**Última Atualização:** 2025-10-24  

Para informações detalhadas, veja documentação completa na pasta `docs/`.
