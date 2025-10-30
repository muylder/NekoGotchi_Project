# 🔒 GUIA RÁPIDO - LIMPEZA DE SEGURANÇA

## ⚠️ URGENTE: Use isto se viu sua senha WiFi nos arquivos!

### Opção 1: Flashear Ferramenta de Limpeza (RECOMENDADO)

1. **Adicione ao platformio.ini:**

```ini
[env:cleanup-cardputer]
platform = espressif32@6.4.0
board = m5stack-stamps3
lib_deps = 
    m5stack/M5Unified@^0.2.0
    m5stack/M5Cardputer@^1.0.0
build_src_filter = 
    -<*>
    +<security_cleanup.cpp>
build_flags = 
    -DCARDPUTER
```

2. **Compile e flasheie:**

```bash
pio run -e cleanup-cardputer -t upload
```

3. **No dispositivo:**
   - Pressione ENTER para confirmar
   - Aguarde a limpeza
   - MUDE SUA SENHA WIFI!

---

### Opção 2: Limpeza Manual (Mais Rápido)

**No próprio Cardputer:**

1. Acesse o File Manager (se tiver)
2. Delete estes arquivos:
   - `/config.json` ⚠️
   - `/credentials.txt` ⚠️⚠️⚠️
   - `/session.json`
   - Pasta `/handshakes/`
   - Pasta `/portals/`

**OU conecte o SD card no PC:**

1. Remova o SD do Cardputer
2. Conecte no PC
3. Delete todos os arquivos `.json` e `.txt`
4. Delete pastas `handshakes`, `portals`, `captures`

---

### Opção 3: Formatar SD (Mais Seguro)

```bash
# No PC com SD conectado:
# Windows:
format X: /FS:FAT32 /Q

# Linux/Mac:
sudo mkfs.vfat -F 32 /dev/sdX1
```

---

## 🚨 AÇÕES CRÍTICAS DEPOIS:

### 1. MUDE SUA SENHA WIFI AGORA!

Acesse seu roteador:
- URL: http://192.168.1.1 ou http://192.168.0.1
- Login: admin / senha do roteador
- Wireless Settings > Security
- Mude a senha WPA2
- Salve e reinicie o roteador

### 2. Verifique Dispositivos Conectados

- Liste todos os dispositivos
- Remova desconhecidos
- Mude a senha do admin do roteador também!

### 3. Monitore por 24h

- Veja se aparecem dispositivos estranhos
- Verifique logs do roteador
- Considere resetar o roteador (factory reset)

---

## ✅ Depois da Limpeza:

Flasheie o firmware seguro:

```bash
# Voltar para o pet kawaii (seguro)
pio run -e m5stack-cardputer -t upload
```

O `main_cardputer_clean.cpp` é 100% seguro e não salva senhas!

---

## 📞 Precisa de Ajuda?

Se tiver dúvidas ou problemas, me avise!

**LEMBRE-SE: NUNCA use ferramentas de hacking em redes que não são suas!**
