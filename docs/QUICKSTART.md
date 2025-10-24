# ⚡ Guia Rápido - M5Gotchi Optimizado

## 🚀 Setup Rápido (5 minutos)

### Passo 1: Preparar Arduino IDE
```
1. Abrir Arduino IDE
2. File > Preferences
3. Adicionar URL: https://dl.espressif.com/dl/package_esp32_index.json
4. Tools > Board > Boards Manager
5. Instalar: "esp32" by Espressif Systems
6. Tools > Manage Libraries
7. Instalar: "M5Stack" by M5Stack
```

### Passo 2: Configurar Board
```
Tools > Board > M5Stack-Core-ESP32
Tools > Upload Speed > 115200
Tools > Port > [Selecionar sua porta COM/USB]
```

### Passo 3: Upload
```
1. Conectar M5Stack via USB
2. Abrir arquivo: M5Gotchi_Optimized.ino
3. Clicar no botão Upload (→)
4. Aguardar compilação e upload
```

## 🎯 Primeiro Uso

### 1. Inserir SD Card (Opcional)
- Formatar como **FAT32**
- Inserir no slot do M5Stack

### 2. Ligar
- Botão de power (lateral)
- Aguardar inicialização

### 3. Começar Captura
- **Automaticamente inicia capturando!**
- Display mostra estatísticas em tempo real

## 🎮 Comandos Básicos

| Botão | Ação |
|-------|------|
| **A** | Modo: PRIORITÁRIO ⟷ TODOS |
| **B** | Ativar/Desativar SD Card |
| **C** | Resetar contadores |

## 📊 O que Observar

### Sucesso ✅
```
Canal: 06
Modo: PRIORITARIO
Pacotes: 15234
EAPOL: 5      ← Isso é bom! 🎉
PMKID: 2      ← Isso também! 🎉
```

### Aguardando ⏳
```
Canal: 11
Modo: PRIORITARIO
Pacotes: 8523
EAPOL: 0      ← Normal no início
PMKID: 0      ← Seja paciente
```

## ⚠️ Problemas Comuns

### "SD CARD FALHOU!"
**Solução**: 
1. Remover e reinserir SD
2. Formatar como FAT32
3. Tentar outro cartão

### Não captura nada após 30min
**Soluções**:
1. Pressionar botão A (mudar para modo TODOS)
2. Ir para local com mais dispositivos WiFi
3. Deixar rodando por 1-2 horas

### Display apagado
**Solução**: Pressionar qualquer botão

## 📁 Recuperar Arquivos

### Windows
```
1. Remover SD do M5Stack
2. Inserir no PC
3. Copiar arquivos *.pcap
```

### Linux/Mac
```bash
# Montar SD Card
mount /dev/sdb1 /mnt/sd

# Copiar arquivos
cp /mnt/sd/*.pcap ~/captures/

# Desmontar
umount /mnt/sd
```

## 🔬 Analisar Capturas

### Wireshark (Visual)
```
1. Abrir Wireshark
2. File > Open > 1.pcap
3. Filtro: "eapol"
4. Verificar handshakes
```

### Hashcat (Cracking)
```bash
# Converter PCAP
hcxpcapngtool -o hash.22000 1.pcap

# Crackear
hashcat -m 22000 hash.22000 rockyou.txt
```

## 📈 Métricas de Sucesso

| Tempo | EAPOL Esperado | PMKID Esperado |
|-------|----------------|----------------|
| 15 min | 0-3 | 0-1 |
| 30 min | 3-10 | 1-5 |
| 1 hora | 10-30 | 5-15 |
| 2 horas | 30-60 | 15-30 |

## 🎓 Dicas Profissionais

### ✨ Para Mais Capturas
```cpp
// No código, mudar:
#define CHANNEL_HOP_INTERVAL_MS 5000  // 5 segundos
```

### ✨ Canal Fixo (máxima captura)
```cpp
// No setup(), adicionar:
currentChannel = 6;  // Seu canal alvo
usePriorityChannelsOnly = false;
```

### ✨ Monitor Serial
```
Tools > Serial Monitor
Baud Rate: 115200

Verá:
"EAPOL capturado! Total: 5"
"*** PMKID CAPTURADO! ***"
```

## 🏆 Checklist de Sucesso

- [ ] Arduino IDE configurado
- [ ] M5Stack reconhecido na porta COM
- [ ] Upload concluído sem erros
- [ ] Display mostra "M5GOTCHI OPTIMIZED"
- [ ] Contadores aumentando (Pacotes, Beacons)
- [ ] SD Card ativo (se usando)
- [ ] Aguardou pelo menos 30 minutos
- [ ] Testou ambos os modos (A e TODOS)

## 🆘 Precisa de Ajuda?

### Serial Monitor não mostra nada
```
Tools > Serial Monitor
Baud Rate: 115200
```

### Erro de compilação
```
Verificar:
- Biblioteca M5Stack instalada?
- Board ESP32 selecionado?
- Porta correta selecionada?
```

### M5Stack não liga
```
- Carregar bateria via USB-C
- Pressionar botão power (lateral esquerda)
- Segurar por 2-3 segundos
```

## 📝 Notas Importantes

1. **Primeira vez pode levar 1h** para capturar primeiro handshake
2. **Paciência é fundamental** - handshakes dependem de atividade da rede
3. **Local faz diferença** - mais dispositivos = mais capturas
4. **Modo PRIORITÁRIO é recomendado** - 80% das redes usam canais 1, 6, 11

## ⚖️ Lembrete Legal

```
🚨 USE APENAS EM SUAS PRÓPRIAS REDES! 🚨

Capturar handshakes sem autorização é ILEGAL.
Esta ferramenta é para fins educacionais.
Você é responsável pelo uso que fizer.
```

---

**Boas capturas! 🎯**
*Lembre-se: Com grandes poderes vêm grandes responsabilidades!*
