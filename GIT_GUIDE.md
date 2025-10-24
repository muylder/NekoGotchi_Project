# 🚀 M5Gotchi Pro - Guia Git & Compilação

## 📦 Projeto Completo Pronto para Git!

O projeto está **100% organizado** e pronto para:
- ✅ Push para GitHub
- ✅ Compilação automática (CI/CD)
- ✅ Geração de binários
- ✅ Distribuição via releases

---

## 📂 Estrutura Profissional

```
M5Gotchi_Pro_Project/
├── src/main.cpp              ⭐ Código principal
├── data/portal_templates/    🎨 Templates HTML
├── bin/                      📦 Binários compilados
├── docs/                     📖 Documentação
├── examples/                 💡 Exemplos
├── .github/workflows/        🤖 CI/CD automático
├── platformio.ini            ⚙️ Config PlatformIO
├── Makefile                  🔨 Build automation
└── build.sh                  🔧 Script compilação
```

---

## 🚀 Setup Inicial Git

### 1. Criar Repositório GitHub

```bash
# No GitHub, criar novo repositório:
# Nome: M5Gotchi_Pro
# Descrição: Ultimate WiFi Pentest Suite for M5Stack
# Público ou Privado
```

### 2. Inicializar Git Local

```bash
cd M5Gotchi_Pro_Project

# Inicializar git
git init

# Adicionar remote
git remote add origin https://github.com/SEU-USUARIO/M5Gotchi_Pro.git

# Adicionar todos os arquivos
git add .

# Commit inicial
git commit -m "Initial commit: M5Gotchi Pro v1.0.0"

# Push para GitHub
git branch -M main
git push -u origin main
```

---

## 🔨 Compilação

### Opção 1: Script Automático (Recomendado)

```bash
# Compilar todos os dispositivos
./build.sh all

# Ou apenas um:
./build.sh core    # M5Stack Core
./build.sh fire    # M5Stack Fire
./build.sh core2   # M5Stack Core2

# Binários gerados em: bin/
```

### Opção 2: Makefile

```bash
# Instalar deps
make install

# Compilar
make build

# Upload
make upload

# Monitor
make monitor

# Gerar binários
make bin
```

### Opção 3: PlatformIO Direto

```bash
# Instalar deps
pio pkg install

# Compilar todos
pio run

# Compilar específico
pio run -e m5stack-core-esp32

# Upload
pio run --target upload

# Monitor
pio device monitor
```

---

## 📦 Gerar Binários para Distribuição

### Método 1: Build Script

```bash
./build.sh all
```

**Gera:**
```
bin/
├── M5Gotchi_Pro_m5stack-core-esp32.bin
├── M5Gotchi_Pro_m5stack-fire.bin
├── M5Gotchi_Pro_m5stack-core2.bin
├── bootloader_*.bin
├── partitions_*.bin
├── flash_*.sh (scripts de flash)
└── README.md
```

### Método 2: GitHub Actions (Automático)

```bash
# 1. Push para GitHub
git push

# 2. GitHub Actions compila automaticamente
# 3. Binários disponíveis em Artifacts

# 4. Para release:
git tag v1.0.0
git push --tags

# 5. GitHub Actions cria release com binários!
```

---

## 🤖 CI/CD Automático

### GitHub Actions Configurado

**Triggers:**
- Push para `main` ou `develop`
- Pull Requests
- Tags/Releases

**Ações:**
1. Compila para 3 dispositivos
2. Gera binários
3. Upload como artifacts
4. Em releases: anexa binários automaticamente

**Ver progresso:**
```
GitHub → Actions → Build M5Gotchi Pro
```

---

## 📥 Distribuir Binários

### Via GitHub Releases

```bash
# 1. Criar tag
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# 2. GitHub Actions compila automaticamente
# 3. Binários anexados ao release
# 4. Usuários baixam arquivos .bin prontos!
```

### Via Pasta bin/

```bash
# Após ./build.sh all
cd bin/

# Usuários executam:
./flash_m5stack-core-esp32.sh /dev/ttyUSB0
```

---

## 🌳 Workflow Git Recomendado

### Branches

```
main          → Versão estável
develop       → Desenvolvimento
feature/*     → Novas funcionalidades
bugfix/*      → Correções
hotfix/*      → Correções urgentes
```

### Commits

```bash
# Formato
<tipo>: <descrição>

# Exemplos
Add: suporte WPA3
Fix: crash ao clonar rede
Update: otimizar deauth
Docs: atualizar README
```

### Pull Requests

```bash
# 1. Criar branch
git checkout -b feature/nova-funcionalidade

# 2. Desenvolver
# ... código ...

# 3. Commit
git add .
git commit -m "Add: nova funcionalidade"

# 4. Push
git push origin feature/nova-funcionalidade

# 5. Abrir PR no GitHub
```

---

## 📋 Checklist Pré-Push

Antes de fazer push:

- [ ] Código compila sem erros
- [ ] Testado em hardware real
- [ ] Documentação atualizada
- [ ] CHANGELOG.md atualizado
- [ ] Versão incrementada (se release)
- [ ] .gitignore correto
- [ ] Sem credenciais/secrets no código

---

## 🎯 Estrutura de Release

### Criar Release v1.0.0

```bash
# 1. Atualizar versão no código
vim src/main.cpp
# Mudar: #define VERSION "1.0.0"

# 2. Atualizar CHANGELOG
vim CHANGELOG.md

# 3. Commit
git add .
git commit -m "Release: v1.0.0"

# 4. Tag
git tag -a v1.0.0 -m "Version 1.0.0 - Initial Release"

# 5. Push
git push origin main
git push origin v1.0.0

# 6. GitHub Actions cria release automaticamente!
```

---

## 📊 Tamanhos

```
Projeto completo:
├── .tar.gz: 30 KB
├── .zip:    43 KB

Após compilação:
├── Binários: ~1.3 MB/device
├── Total:    ~4 MB (3 devices)
```

---

## 🔐 Segurança

### Arquivos no .gitignore

- Credenciais capturadas
- Arquivos PCAP
- Build artifacts (.pio/)
- IDE configs (.vscode/)
- Chaves privadas
- Tokens

### Nunca Commitar

- `credentials.txt`
- `*.pcap`
- Senhas reais
- Tokens de API
- Dados pessoais

---

## 🌐 URLs do Projeto

Após push para GitHub:

```
Repositório:
https://github.com/SEU-USUARIO/M5Gotchi_Pro

Issues:
https://github.com/SEU-USUARIO/M5Gotchi_Pro/issues

Actions (CI/CD):
https://github.com/SEU-USUARIO/M5Gotchi_Pro/actions

Releases:
https://github.com/SEU-USUARIO/M5Gotchi_Pro/releases

Wiki:
https://github.com/SEU-USUARIO/M5Gotchi_Pro/wiki
```

---

## 📝 Próximos Passos

### 1. Push Inicial

```bash
cd M5Gotchi_Pro_Project
git init
git add .
git commit -m "Initial commit: M5Gotchi Pro v1.0.0"
git remote add origin https://github.com/SEU-USUARIO/M5Gotchi_Pro.git
git push -u origin main
```

### 2. Compilar Binários

```bash
./build.sh all
```

### 3. Criar Release

```bash
git tag v1.0.0
git push --tags
```

### 4. Compartilhar!

- README.md no GitHub
- Releases com binários
- Documentação completa
- Issues para feedback

---

## 🎉 Projeto Pronto!

```
✅ Código organizado
✅ Estrutura profissional
✅ CI/CD configurado
✅ Documentação completa
✅ Build automático
✅ Pronto para distribuição

TUDO PRONTO PARA GIT! 🚀
```

---

**M5Gotchi Pro Project**
*Ready for GitHub & Compilation*

**Version:** 1.0.0  
**Date:** 2024-10-19
