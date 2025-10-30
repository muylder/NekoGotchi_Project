# 🔒 GUIA DE LIMPEZA DE SEGURANÇA - M5GOTCHI

## ⚠️ ATENÇÃO: Dados Sensíveis Encontrados

Se você viu sua senha WiFi em algum arquivo, siga estes passos **IMEDIATAMENTE**:

## 📋 Passos de Limpeza:

### 1. **Deletar Arquivos do Cartão SD**
Conecte o Cardputer e delete estes arquivos (se existirem):

```
/config.json
/credentials.txt
/session.json
/stats.json
/logs/system.log
/ai_memory.json
/wardrive.csv
```

### 2. **Comandos de Limpeza (Execute no M5Gotchi)**

Se o dispositivo tiver File Manager, navegue até cada arquivo e delete.

### 3. **Script de Limpeza Rápida**

Crie um arquivo `cleanup.ino` e execute:

```cpp
#include <M5Cardputer.h>
#include <SD.h>

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    Serial.begin(115200);
    Serial.println("🔒 SECURITY CLEANUP STARTING...");
    
    if (!SD.begin(12, SPI, 25000000)) {
        Serial.println("❌ SD Card not found");
        return;
    }
    
    // Lista de arquivos para deletar
    const char* sensitiveFiles[] = {
        "/config.json",
        "/credentials.txt",
        "/session.json",
        "/stats.json",
        "/logs/system.log",
        "/ai_memory.json",
        "/wardrive.csv",
        "/vuln_db.json"
    };
    
    for (int i = 0; i < 8; i++) {
        if (SD.exists(sensitiveFiles[i])) {
            SD.remove(sensitiveFiles[i]);
            Serial.printf("✅ Deleted: %s\n", sensitiveFiles[i]);
        } else {
            Serial.printf("⚪ Not found: %s\n", sensitiveFiles[i]);
        }
    }
    
    Serial.println("\n🔒 CLEANUP COMPLETE!");
    Serial.println("⚠️ IMPORTANTE: Mude a senha do seu WiFi!");
}

void loop() {
    delay(1000);
}
```

## 🚨 AÇÕES CRÍTICAS:

### ✅ **MUDE SUA SENHA WIFI IMEDIATAMENTE**

1. Acesse o roteador (geralmente 192.168.1.1 ou 192.168.0.1)
2. Login com admin/senha do roteador
3. Vá em Wireless/WiFi Settings
4. Mude a senha WiFi
5. Reconecte todos os dispositivos

### ✅ **Verifique Dispositivos Conectados**

- Liste todos dispositivos conectados ao seu roteador
- Remova qualquer dispositivo desconhecido
- Ative filtro MAC se possível

## 🛡️ Prevenção Futura:

O novo código `main_cardputer_clean.cpp` **NÃO salva senhas**. Ele apenas:
- Escaneia redes WiFi (modo passivo)
- Salva estatísticas do pet (hunger, happiness, XP)
- Não conecta em redes
- Não captura credenciais

## 📝 O Que o Código Antigo Fazia:

O código `main_cardputer.cpp` tinha funcionalidades de pentesting que podiam:
- Capturar handshakes WiFi
- Salvar em `/credentials.txt`
- Criar evil portals
- Logar atividades em `/config.json`

## ✅ Código Novo (Seguro):

O `main_cardputer_clean.cpp` é **100% seguro**:
- Apenas pet virtual
- Scanner WiFi passivo (só lê, não ataca)
- Sem capturas de senha
- Sem conexões automáticas

## 🔍 Como Verificar:

Execute no terminal do dispositivo:

```bash
# No M5Cardputer, vá para Tools > File Manager
# Ou conecte o SD no PC e verifique:

dir /SD/
```

Delete qualquer arquivo suspeito manualmente.

---

## 💡 LEMBRETE IMPORTANTE:

**NUNCA use ferramentas de pentesting em redes que não são suas!**
**É ILEGAL e pode resultar em prisão.**

Este projeto é apenas para:
- Aprendizado educacional
- Testes em SUA PRÓPRIA rede
- Pet virtual kawaii 🐱

---

Se você tiver dúvidas ou precisar de ajuda, me avise!
