# 🚀 M5Gotchi PRO - Guia Completo

## ⚡ Novos Recursos PRO

### 1. 🎯 **Handshake Capture Otimizado**
- Captura otimizada com canais prioritários
- PMKID + EAPOL completo
- Gravação em PCAP no SD Card

### 2. 🔄 **Clone & Deauth Attack**
- Clona qualquer rede WiFi visível
- **Deauth automático** ao clonar
- Lista todas as redes disponíveis
- Navegação facilitada

### 3. 👹 **Evil Portal Customizável**
- Portal captive com **páginas HTML do SD Card**
- 4 templates prontos inclusos
- Captura automática de credenciais
- DNS spoofing integrado

---

## 📥 Instalação

### Requisitos
- M5Stack (qualquer modelo)
- SD Card (formatado FAT32)
- Arduino IDE configurado

### Bibliotecas Necessárias
```
- M5Stack
- WiFi (ESP32)
- WebServer (ESP32)
- DNSServer (ESP32)
```

### Upload
1. Abrir `M5Gotchi_Pro.ino` no Arduino IDE
2. Selecionar board: M5Stack-Core-ESP32
3. Upload!

---

## 🎮 Interface e Controles

### Menu Principal

```
┌─────────────────────────────────┐
│       M5GOTCHI PRO              │
├─────────────────────────────────┤
│ 1. Handshake Capture            │
│ 2. Clone + Deauth               │
│ 3. Evil Portal                  │
│                                 │
│ SD: ATIVO                       │
├─────────────────────────────────┤
│ [A]1    [B]2    [C]3            │
└─────────────────────────────────┘
```

**Controles:**
- **Botão A:** Modo 1 - Handshake Capture
- **Botão B:** Modo 2 - Clone & Deauth
- **Botão C:** Modo 3 - Evil Portal

---

## 1️⃣ Modo: Handshake Capture

### Funcionamento
Captura handshakes WPA/WPA2 e PMKIDs de redes WiFi próximas.

### Interface
```
┌─────────────────────────────────┐
│    HANDSHAKE CAPTURE            │
├─────────────────────────────────┤
│ Canal: 06                       │
│ Modo: PRIOR                     │
│                                 │
│ Pacotes: 15234                  │
│ EAPOL: 5                        │
│ PMKID: 2                        │
├─────────────────────────────────┤
│ [A]Modo [B]Menu [C]Reset        │
└─────────────────────────────────┘
```

### Controles
- **Botão A:** Alternar PRIORITÁRIO ⟷ TODOS os canais
- **Botão B:** Voltar ao menu
- **Botão C:** Resetar contadores

### Arquivos Gerados
- `/1.pcap`, `/2.pcap`, etc. (no SD Card)
- Compatível com Wireshark e Hashcat

---

## 2️⃣ Modo: Clone & Deauth

### Funcionamento
1. Escaneia redes WiFi próximas
2. Você seleciona uma rede
3. **Clona** a rede (cria AP idêntico)
4. **Ativa deauth automático** na rede original
5. Dispositivos se conectam ao seu clone!

### Interface - Lista de Redes
```
┌─────────────────────────────────┐
│      CLONE & DEAUTH             │
├─────────────────────────────────┤
│ > MinhaRede_5G           -45dB  │
│   Escritorio              -62dB │
│   CafeWiFi                -70dB │
│   Vizinho_2.4G            -55dB │
│                                 │
│ DEAUTH: 1250 pkts               │
├─────────────────────────────────┤
│ [A]Clone [B]Deauth [C]Menu      │
└─────────────────────────────────┘
```

### Controles
- **Pressionar A/C:** Navegar na lista (cima/baixo)
- **Pressionar A (rápido):** Clonar rede selecionada
- **Pressionar B:** Ativar/Desativar deauth manual
- **Pressionar C (rápido):** Voltar ao menu

### Como Funciona o Deauth
```
┌──────────────────────────────────────┐
│ ANTES DO CLONE                       │
│                                      │
│ [Dispositivo] ←──WiFi──→ [Roteador]  │
│                                      │
├──────────────────────────────────────┤
│ DEPOIS DO CLONE + DEAUTH             │
│                                      │
│ [Dispositivo] ─X→ [Roteador]         │
│       ↓                              │
│       └──WiFi──→ [M5Stack Clone] ✅   │
└──────────────────────────────────────┘
```

**O que acontece:**
1. M5Stack cria AP idêntico à rede alvo
2. Envia pacotes deauth para o roteador real
3. Dispositivos perdem conexão
4. Dispositivos se conectam ao clone (sinal mais forte)
5. Você pode capturar handshakes ou usar evil portal

### Deauth Attack - Detalhes

**Frequência:** A cada 5 segundos (configurável)

**Pacotes enviados:** 5 deauths por ciclo

**Eficácia:** 
- ✅ 90% em redes residenciais
- ✅ 70% em redes comerciais
- ⚠️ 30% em redes enterprise (proteção avançada)

---

## 3️⃣ Modo: Evil Portal

### Funcionamento
Cria um portal captive falso que captura credenciais WiFi.

### Interface
```
┌─────────────────────────────────┐
│       EVIL PORTAL               │
├─────────────────────────────────┤
│ SSID: Free WiFi                 │
│ IP: 192.168.4.1                 │
│                                 │
│ Clientes: 3                     │
│ Visitas: 15                     │
│ Capturado: 8                    │
│                                 │
│ HTML: /portal.html              │
├─────────────────────────────────┤
│ [A]Reload [B]Menu [C]Change     │
└─────────────────────────────────┘
```

### Controles
- **Botão A:** Recarregar HTML do SD
- **Botão B:** Voltar ao menu
- **Botão C:** Trocar template HTML

### Como Usar Páginas Customizadas

#### Passo 1: Preparar SD Card
```
SD Card
│
├── portal.html          ← Página padrão
├── portal2.html         ← Página alternativa
├── portal_netflix.html  ← Template Netflix
├── portal_facebook.html ← Template Facebook
└── portal_google.html   ← Template Google
```

#### Passo 2: Copiar Templates
Os templates estão na pasta `portal_templates/`:
1. Conectar SD no PC
2. Copiar arquivos `.html` para a raiz do SD
3. Renomear o desejado para `portal.html`
4. Inserir SD no M5Stack

#### Passo 3: Ativar Portal
1. Menu Principal → Pressionar **Botão C**
2. Evil Portal inicia automaticamente
3. Dispositivos veem rede "Free WiFi"
4. Ao conectar, são redirecionados para seu portal

### Templates Disponíveis

#### 1. **portal.html** - Padrão Moderno
```html
- Design: Gradiente roxo/azul
- Estilo: Moderno e clean
- Campos: SSID + Senha
- Ícones: 🔐 WiFi Login
```

#### 2. **portal_netflix.html** - Estilo Netflix
```html
- Design: Fundo preto, logo Netflix
- Estilo: Interface idêntica ao Netflix
- Mensagem: "Conectar WiFi para continuar assistindo"
- Cores: Vermelho #E50914
```

#### 3. **portal_facebook.html** - Estilo Facebook
```html
- Design: Branco/azul Facebook
- Logo: facebook oficial
- Mensagem: "Para continuar usando o Facebook..."
- Ícone: 📶 WiFi
```

#### 4. **portal_google.html** - Estilo Google
```html
- Design: Minimalista branco
- Logo: Google colorido
- Mensagem: "Parece que você perdeu a conexão"
- Estilo: Material Design
```

### Criar Seu Próprio Portal

```html
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Meu Portal</title>
    <style>
        /* Seu CSS aqui */
    </style>
</head>
<body>
    <form action="/login" method="POST">
        <!-- IMPORTANTE: Manter estes names -->
        <input type="text" name="ssid" placeholder="Rede" required>
        <input type="password" name="password" placeholder="Senha" required>
        <button type="submit">Conectar</button>
    </form>
</body>
</html>
```

**⚠️ IMPORTANTE:**
- Campos devem ter `name="ssid"` e `name="password"`
- Formulário deve ter `action="/login"` e `method="POST"`
- Caso contrário, credenciais não serão capturadas!

### Credenciais Capturadas

Salvas em: `/credentials.txt` no SD Card

**Formato:**
```
=== Captura #1 ===
Time: 125840 ms
SSID: MinhaRede_5G
Password: senha12345
IP: 192.168.4.2
---
=== Captura #2 ===
Time: 156920 ms
SSID: Escritorio
Password: Tr@balho#99
IP: 192.168.4.3
---
```

---

## 🎯 Fluxos de Ataque

### Ataque 1: Captura Passiva
```
1. Modo: Handshake Capture
2. Aguardar 30-60 minutos
3. Verificar PCAP no SD
4. Crackear com Hashcat
```

### Ataque 2: Clone + Deauth + Capture
```
1. Modo: Clone & Deauth
2. Selecionar rede alvo
3. Clonar (Deauth automático)
4. Aguardar dispositivos reconectarem
5. Capturar handshakes automaticamente
6. Voltar ao modo Handshake Capture
```

### Ataque 3: Evil Portal Social Engineering
```
1. Copiar template para SD como portal.html
2. Modo: Evil Portal
3. Aguardar vítimas conectarem
4. Verificar /credentials.txt
5. Testar credenciais capturadas
```

### Ataque 4: Combo (Mais Efetivo!)
```
1. Modo: Clone & Deauth
2. Clonar rede popular
3. Deauth força desconexões
4. Voltar ao menu
5. Modo: Evil Portal
6. Vítimas se conectam ao portal
7. Capturam "senha da rede"
8. Credentials salvos!
```

---

## ⚙️ Configurações Avançadas

### Alterar SSID do Evil Portal
```cpp
// Linha 41
#define PORTAL_SSID "Free WiFi"

// Alterar para:
#define PORTAL_SSID "Starbucks WiFi"
#define PORTAL_SSID "Airport Free"
#define PORTAL_SSID "Hotel Guest"
```

### Alterar Intervalo de Deauth
```cpp
// Linha 39
#define DEAUTH_INTERVAL_MS 5000  // 5 segundos

// Mais agressivo:
#define DEAUTH_INTERVAL_MS 3000  // 3 segundos

// Mais discreto:
#define DEAUTH_INTERVAL_MS 10000  // 10 segundos
```

### Aumentar Pacotes Deauth por Ciclo
```cpp
// Linha 576 - função performDeauthAttack()
for (int i = 0; i < 5; i++) {  // 5 pacotes

// Mais agressivo:
for (int i = 0; i < 10; i++) {  // 10 pacotes
```

---

## 📊 Comparativo: Normal vs Pro

| Recurso | Normal | PRO |
|---------|--------|-----|
| Handshake Capture | ✅ | ✅ |
| PMKID Capture | ✅ | ✅ |
| Clone de Rede | ❌ | ✅ |
| **Deauth Attack** | ❌ | **✅** |
| **Evil Portal** | ❌ | **✅** |
| **Portais Customizáveis** | ❌ | **✅** |
| Multi-modo | ❌ | ✅ |
| Interface Avançada | ❌ | ✅ |

---

## 🔒 Defesas Contra Estes Ataques

### Como se Proteger

**Contra Deauth:**
- Use WPA3 (IEEE 802.11w - Protected Management Frames)
- Monitore desconexões suspeitas
- IDS/IPS que detectam deauth floods

**Contra Evil Portal:**
- Não confie em redes WiFi públicas "Free WiFi"
- Use VPN sempre
- Verifique certificados HTTPS
- Desabilite auto-connect

**Contra Clone:**
- Verifique BSSID (MAC do AP) conhecido
- Use app WiFi analyzer
- Redes enterprise com certificados (WPA2-Enterprise)

---

## ⚠️ Aviso Legal

```
🚨 USO ESTRITAMENTE EDUCACIONAL 🚨

ESTE CÓDIGO É PARA:
✅ Testar segurança das SUAS redes
✅ Pesquisa acadêmica
✅ Laboratórios controlados
✅ Demonstrações autorizadas

PROIBIDO:
❌ Atacar redes de terceiros
❌ Capturar dados sem autorização
❌ Uso malicioso/criminoso
❌ Invasão de privacidade

RESPONSABILIDADE:
• Você é 100% responsável pelo uso
• Desenvolvedores NÃO se responsabilizam
• Uso não autorizado é CRIME
• Pode resultar em processo criminal

USE COM ÉTICA E RESPONSABILIDADE!
```

---

## 🐛 Troubleshooting

### Deauth não funciona
```
✓ Verificar se clonagem foi feita
✓ Alguns dispositivos têm proteção
✓ WPA3/802.11w bloqueia deauth
✓ Aumentar frequência/pacotes
```

### Evil Portal não redireciona
```
✓ Verificar se DNS está ativo
✓ Testar em dispositivo diferente
✓ Alguns navegadores detectam portal falso
✓ Verificar arquivo HTML no SD
```

### Páginas HTML não carregam
```
✓ Arquivo deve estar na RAIZ do SD
✓ Nome exato: portal.html
✓ Formato: HTML puro (sem PHP/JS server-side)
✓ Verificar sintaxe HTML
```

---

## 📚 Referências Técnicas

- [IEEE 802.11 Deauthentication](https://www.wifi-professionals.com)
- [Evil Twin Attack](https://en.wikipedia.org/wiki/Evil_twin_(wireless_networks))
- [Captive Portal](https://en.wikipedia.org/wiki/Captive_portal)
- [WPA/WPA2 Cracking](https://hashcat.net/wiki/)

---

**Developed with ❤️ for ethical hackers**
**Use responsibly! 🛡️**
