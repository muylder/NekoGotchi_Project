# 🎮 M5Gotchi Pro - Edição M5Stack
## Guia Rápido de Referência (Português)

### 📱 Visão Geral
Suite de pentest WiFi para M5Stack Core/Fire/Core2 com interface de 3 botões e tela 320x240.

---

## 🎮 Controles dos Botões

**Consistente em todos os modos:**
- **Botão A (Esquerda)**: Navegar cima / Anterior / Alternar
- **Botão B (Centro)**: Confirmar / Menu / Alternar ação
- **Botão C (Direita)**: Navegar baixo / Próximo / Voltar

---

## 🏠 Menu Principal

```
╔═══════════════════════════════════════╗
║       M5GOTCHI PRO                    ║
╠═══════════════════════════════════════╣
║                                       ║
║    1. Handshake Capture               ║
║                                       ║
║    2. Clone + Deauth                  ║
║                                       ║
║    3. Evil Portal                     ║
║                                       ║
║    SD: ATIVO                          ║
║                                       ║
╠═══════════════════════════════════════╣
║ [A]1    [B]2    [C]3                  ║
╚═══════════════════════════════════════╝
```

| Botão | Ação |
|-------|------|
| A | Entrar modo Handshake Capture |
| B | Entrar modo Clone & Deauth |
| C | Entrar modo Evil Portal |

---

## 📡 Modo 1: Captura de Handshake

### Display
```
╔═══════════════════════════════════════╗
║  HANDSHAKE CAPTURE                    ║
╠═══════════════════════════════════════╣
║  Canal: 06                            ║
║  Modo: PRIORITARIO                    ║
║  Pacotes: 45832                       ║
║  EAPOL: 12                            ║
║  PMKID: 3                             ║
╠═══════════════════════════════════════╣
║ [A]Modo [B]Menu [C]Reset              ║
╚═══════════════════════════════════════╝
```

| Botão | Ação |
|-------|------|
| A | Alternar entre PRIORITÁRIO (1,6,11) e TODOS (1-13) os canais |
| B | Voltar ao menu principal |
| C | Resetar contadores de pacotes/EAPOL/PMKID |

### O que faz
- Captura handshakes WPA/WPA2 passivamente
- Salva em arquivos .pcap no cartão SD
- Captura PMKID (handshake sem deauth!)

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
║  Rede Convidados                  -81dB
║                                       ║
║  DEAUTH: 125 pacotes                  ║
╠═══════════════════════════════════════╣
║ [A]Clone [B]Deauth [C]Menu            ║
╚═══════════════════════════════════════╝
```

| Botão | Ação |
|-------|------|
| A | Clonar rede selecionada |
| A (Segurar) | Navegar para cima na lista de redes |
| B | Ligar/desligar ataque deauth |
| C | Voltar ao menu principal |
| C (Segurar) | Navegar para baixo na lista de redes |

### Fluxo de Ataque
1. Use A/C (segurar) para selecionar rede alvo
2. Pressione A (rápido) para clonar
3. Deauth inicia automaticamente
4. Pressione B para parar deauth
5. Pressione C para sair

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
║  HTML: /portal.html                   ║
╠═══════════════════════════════════════╣
║ [A]Reload [B]Menu [C]Trocar           ║
╚═══════════════════════════════════════╝
```

| Botão | Ação |
|-------|------|
| A | Recarregar template HTML |
| B | Voltar ao menu principal |
| C | Alternar entre templates |

### Templates Disponíveis
Pressionando C alterna entre:
1. portal.html (gradiente moderno)
2. portal2.html (se existir)
3. Volta para portal.html

---

## 🚀 Início Rápido

### 1. Compilar e Fazer Upload
```bash
# Opção 1: PlatformIO
pio run -e m5stack-core-esp32 --target upload

# Opção 2: Makefile
make upload-core

# Opção 3: Script de build
./build.sh core
```

### 2. Preparar Cartão SD
- Formatar como **FAT32**
- Copiar templates HTML de `data/portal_templates/`
- Inserir no M5Stack

### 3. Primeiro Uso
1. Ligar → Menu principal aparece
2. Pressione A/B/C para selecionar modo
3. Siga os rótulos dos botões na parte inferior da tela
4. Pressione B para voltar ao menu (geralmente botão central)

---

## 💡 Dicas & Truques

### Captura de Handshake
- Modo PRIORITÁRIO (canais 1,6,11) é mais rápido
- Modo TODOS escaneia todos os 13 canais completamente
- PMKID = handshake sem deauth!
- Execute por 10-15 minutos para melhores resultados

### Clone & Deauth
- Selecione rede com sinal mais forte (maior dB)
- Segure A/C para rolar pela lista de redes
- Pressione A rapidamente para clonar
- Pressione B para ligar/desligar deauth

### Evil Portal
- Funciona melhor em áreas movimentadas
- Pressione C para trocar estilo do template
- Credenciais salvas em `/credentials.txt`
- Confira cartão SD para dados capturados

---

## 🐛 Resolução de Problemas

### Cartão SD não detectado
- Formatar como FAT32 (não exFAT)
- Máximo 32GB recomendado
- Verificar contatos do slot SD

### Botões não respondem
- Reiniciar M5Stack
- Verificar nível da bateria
- Atualizar biblioteca M5Unified

### Nenhum handshake capturado
- Use modo PRIORITÁRIO primeiro
- Aguarde pelo menos 10 minutos
- Redes devem ter clientes ativos

---

## ⚖️ Uso Legal e Ético

**APENAS PARA FINS EDUCACIONAIS**

✅ **Permitido:**
- Testar suas próprias redes
- Auditorias de segurança autorizadas
- Ambientes de laboratório
- Pesquisa acadêmica

❌ **Proibido:**
- Atacar redes de terceiros
- Capturar dados de outras pessoas
- Uso malicioso
- Violações de privacidade

**Uso não autorizado é CRIME**

---

## 🎯 Resumo de Botões

```
SELEÇÃO DE MODO (Menu Principal):
  [A] = Modo 1 (Handshake)
  [B] = Modo 2 (Clone)
  [C] = Modo 3 (Portal)

EM QUALQUER MODO:
  [B] = Voltar ao menu (botão central)
  
Siga os rótulos dos botões na parte inferior da tela!
```

---

**Versão:** 2.0.0  
**Última Atualização:** 2025-10-24

Para informações detalhadas, veja documentação completa na pasta `docs/`.
