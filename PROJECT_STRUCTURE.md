# 📁 M5Gotchi Pro - Estrutura do Projeto

## 🌳 Árvore de Diretórios

```
M5Gotchi_Pro_Project/
│
├── 📄 README.md                      # Documentação principal
├── 📄 LICENSE                        # Licença MIT
├── 📄 CONTRIBUTING.md                # Guia de contribuição
├── 📄 platformio.ini                 # Configuração PlatformIO
├── 📄 Makefile                       # Build automation
├── 🔧 build.sh                       # Script de compilação
│
├── 📂 src/                           # Código fonte
│   ├── README.md                     # Explicação das versões
│   ├── main.cpp                      # Código M5Stack Core/Fire/Core2
│   └── main_cardputer.cpp            # Código Cardputer (otimizado)
│
├── 📂 data/                          # Dados para SD Card
│   └── portal_templates/             # Templates HTML para Evil Portal
│       ├── portal.html               # Template padrão moderno
│       ├── portal_netflix.html       # Estilo Netflix
│       ├── portal_facebook.html      # Estilo Facebook
│       └── portal_google.html        # Estilo Google
│
├── 📂 bin/                           # Binários compilados
│   └── README.md                     # Instruções de flash
│
├── 📂 docs/                          # Documentação
│   ├── PRO_GUIDE.md                  # Guia completo M5Stack
│   ├── CARDPUTER_GUIDE.md            # Guia completo Cardputer ⌨️
│   ├── CARDPUTER_QUICKSTART.md       # Setup rápido Cardputer
│   ├── QUICKSTART.md                 # Setup rápido geral
│   └── COMPARISON.md                 # Antes vs Depois
│
├── 📂 examples/                      # Exemplos
│   └── M5Gotchi_Basic.ino           # Versão básica otimizada
│
├── 📂 .github/                       # GitHub Configuration
│   └── workflows/
│       └── build.yml                 # CI/CD com GitHub Actions
│
├── 📂 .vscode/                       # VSCode Configuration
│   └── settings.json                 # Configurações do editor
│
└── 📂 platformio/                    # PlatformIO cache (auto-gerado)
```

## 📦 Arquivos de Configuração

### Build & Compilação
- `platformio.ini` - Configuração de ambientes e bibliotecas
- `Makefile` - Targets de build simplificados
- `build.sh` - Script automatizado de compilação

### Git & Versionamento
- `.gitignore` - Arquivos ignorados pelo Git
- `.gitattributes` - Normalização de line endings
- `.editorconfig` - Configuração de editor universal

### CI/CD
- `.github/workflows/build.yml` - GitHub Actions para build automático

## 🎯 Arquivos Principais

### src/main.cpp
**Tamanho:** ~20KB  
**Descrição:** Código para M5Stack Core/Fire/Core2  
**Recursos:**
- Tela 320x240 pixels
- 3 botões físicos (A, B, C)
- 3 modos de operação
- Interface espaçada

### src/main_cardputer.cpp ⌨️
**Tamanho:** ~25KB  
**Descrição:** Código otimizado para M5Stack Cardputer  
**Recursos:**
- Tela 240x135 pixels (compacta)
- Teclado QWERTY completo
- 10+ atalhos de teclado
- Interface condensada
- Mesma lógica WiFi

### data/portal_templates/
**4 Templates HTML prontos para uso:**
1. `portal.html` - Design moderno gradiente
2. `portal_netflix.html` - Interface Netflix
3. `portal_facebook.html` - Interface Facebook
4. `portal_google.html` - Interface Google

## 🚀 Workflow de Desenvolvimento

### 1. Desenvolvimento Local
```bash
# Editar código
vim src/main.cpp

# Compilar e testar
make build
make upload
make monitor
```

### 2. Compilação de Binários
```bash
# Gerar binários para todos os dispositivos
./build.sh all

# Binários em: bin/
```

### 3. Git Workflow
```bash
# Nova feature
git checkout -b feature/nova-funcionalidade

# Desenvolver e commitar
git add .
git commit -m "Add: nova funcionalidade"

# Push e PR
git push origin feature/nova-funcionalidade
```

### 4. CI/CD Automático
- Push → GitHub Actions
- Build automático para 3 devices
- Binários como artifacts
- Release automático em tags

## 📊 Tamanho dos Arquivos

```
Código fonte (src/):        ~45 KB (2 versões)
├── main.cpp:               ~20 KB (M5Stack)
└── main_cardputer.cpp:     ~25 KB (Cardputer)

Templates (data/):          ~40 KB
Documentação (docs/):       ~180 KB
├── PRO_GUIDE.md:           ~50 KB
├── CARDPUTER_GUIDE.md:     ~80 KB
└── Outros:                 ~50 KB

Exemplos (examples/):       ~15 KB

Binários compilados (bin/):
├── Bootloader:             ~27 KB
├── Partitions:             ~3 KB
└── Firmware:               ~1.2 MB

Total projeto (sem .pio):   ~1.5 MB
```

## 🔄 Fluxo de Build

```
┌─────────────────────┐
│   Código Fonte      │
│   (src/main.cpp)    │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   PlatformIO        │
│   Compilador        │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   Binários          │
│   (bin/*.bin)       │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   Flash Device      │
│   (M5Stack)         │
└─────────────────────┘
```

## 🎯 Ambientes Suportados

| Ambiente | Board | Flash | PSRAM | Arquivo |
|----------|-------|-------|-------|---------|
| m5stack-core-esp32 | M5Stack Core | 16MB | Não | main.cpp |
| m5stack-fire | M5Stack Fire | 16MB | 4MB | main.cpp |
| m5stack-core2 | M5Stack Core2 | 16MB | 8MB | main.cpp |
| m5stack-cardputer | M5Stack Cardputer | 8MB | 8MB | main_cardputer.cpp ⌨️ |
| m5stick-c | M5Stick-C | 4MB | Não | main.cpp |

## 📝 Convenções de Código

### Arquivos
- C++ Header: `.h`
- C++ Source: `.cpp`
- Arduino: `.ino`
- Config: `.ini`, `.json`, `.yml`

### Nomenclatura
- Classes: `PascalCase`
- Funções: `camelCase`
- Variáveis: `snake_case`
- Constantes: `UPPER_CASE`
- Macros: `UPPER_CASE`

### Comentários
```cpp
// Comentários inline em inglês

/**
 * Comentários de função em formato Doxygen
 * @param parameter Descrição
 * @return Descrição do retorno
 */
```

## 🔐 Segurança no Repositório

### Arquivos Sensíveis (.gitignore)
- Credenciais capturadas
- Arquivos PCAP
- Chaves privadas
- Tokens de API
- Configurações pessoais

### Dados de Teste
- Usar apenas redes próprias
- Não commitar dados reais
- Exemplos devem ser fictícios

## 🌐 URLs do Projeto

- **GitHub:** `github.com/seu-usuario/M5Gotchi_Pro`
- **Issues:** `github.com/seu-usuario/M5Gotchi_Pro/issues`
- **Wiki:** `github.com/seu-usuario/M5Gotchi_Pro/wiki`
- **Releases:** `github.com/seu-usuario/M5Gotchi_Pro/releases`

## 📈 Métricas do Projeto

- **Linhas de código:** ~1600 linhas (2 versões)
- **Arquivos fonte:** 2 mains + exemplos
- **Documentação:** 6 guias completos
- **Templates:** 4 portais HTML
- **Dispositivos suportados:** 5 modelos M5Stack
- **Versões:** 2 (M5Stack + Cardputer)

---

**Última atualização:** 2025-10-24  
**Versão:** 2.0.0 (Cardputer Edition)
