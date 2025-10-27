# 📖 Documentation Generation Guide

This guide explains how to generate HTML documentation from the M5Gotchi Pro codebase using Doxygen.

## 🎯 What You'll Get

- **HTML documentation** with searchable API reference
- **Class diagrams** showing relationships
- **Call graphs** for function dependencies
- **Source code browser** with syntax highlighting
- **Architecture diagrams** (from ARCHITECTURE.md)
- **Examples** integrated from code comments

## 📋 Prerequisites

### Windows

1. **Install Doxygen:**
   - Download from: https://www.doxygen.nl/download.html
   - Run installer: `doxygen-X.X.X-setup.exe`
   - Add to PATH during installation

2. **Verify installation:**
   ```powershell
   doxygen --version
   ```

### Linux/macOS

```bash
# Ubuntu/Debian
sudo apt-get install doxygen graphviz

# macOS (Homebrew)
brew install doxygen graphviz

# Verify
doxygen --version
```

## 🚀 Generate Documentation

### Method 1: Command Line (Recommended)

```powershell
# Navigate to project root
cd c:\Users\andre\Gotchi_mod\M5Gotchi_Pro_Project

# Generate documentation
doxygen Doxyfile

# Output will be in: docs/doxygen/html/
```

### Method 2: Doxywizard (GUI)

1. Open Doxywizard application
2. Load `Doxyfile` from project root
3. Click **Run** tab
4. Click **Run doxygen**
5. Wait for completion
6. Click **Show HTML output**

## 📂 Output Structure

```
docs/doxygen/
└── html/
    ├── index.html          # Main entry point
    ├── annotated.html      # Class list
    ├── files.html          # File list
    ├── modules.html        # Modules/groups
    ├── namespaces.html     # Namespaces
    ├── classes.html        # Alphabetical class index
    ├── functions.html      # Function index
    ├── globals.html        # Global symbols
    └── search/             # Search index
```

## 🌐 View Documentation

### Open in Browser

**Windows:**
```powershell
Start-Process docs/doxygen/html/index.html
```

**Linux/macOS:**
```bash
open docs/doxygen/html/index.html  # macOS
xdg-open docs/doxygen/html/index.html  # Linux
```

### Local Server (Optional)

For better search functionality:

```bash
cd docs/doxygen/html
python -m http.server 8080
# Open http://localhost:8080 in browser
```

## 📝 Documentation Coverage

### Fully Documented Modules ✅

- ✅ `m5gotchi_wifi_analyzer.h` - WiFi network analyzer
- ✅ `bluetooth_attacks.h` - BLE spam attacks
- ✅ `m5gotchi_sushi_sql_game.h` - SQL injection game
- ✅ `m5gotchi_achievement_manager.h` - Achievement system
- ✅ `m5gotchi_tutorial_system.h` - Tutorial system
- ✅ `m5gotchi_memory_utils.h` - Memory optimization tools
- ✅ `m5gotchi_progmem_strings.h` - PROGMEM string library
- ✅ `m5gotchi_rfid_nfc.h` - RFID/NFC tools
- ✅ `m5gotchi_chameleon_ultra.h` - Chameleon Ultra interface

### Partially Documented

- ⚠️ `m5gotchi_neko_virtual_pet.h` - Basic comments only
- ⚠️ `m5gotchi_kawaii_social.h` - Basic comments only
- ⚠️ Other game modules

### To Be Documented

- ❌ Remaining WiFi modules (deauth, handshake, portal)
- ❌ GPS wardriving module
- ❌ RF433/IR modules
- ❌ Web dashboard

## 🔍 What Gets Documented

Doxygen extracts documentation from:

1. **File headers** (`@file`, `@brief`, `@details`)
2. **Class/struct documentation** (`@class`, `@struct`)
3. **Function comments** (`@brief`, `@param`, `@return`)
4. **Member variables** (`///< inline comments`)
5. **Enums** and constants
6. **Code examples** (`@example`)
7. **Markdown files** (README, ARCHITECTURE, etc.)

## 📖 Documentation Style Guide

### File Header Example

```cpp
/**
 * @file my_module.h
 * @brief Short one-line description
 * @version 1.0
 * @date 2025-10-26
 * 
 * @details Longer description explaining what this module does,
 * its purpose, main features, and how it fits into the project.
 * 
 * @copyright (c) 2025 M5Gotchi Pro Project
 * @license MIT License
 */
```

### Class Documentation Example

```cpp
/**
 * @class MyClass
 * @brief Brief description of the class
 * 
 * @details Detailed explanation of the class purpose, architecture,
 * usage patterns, and any important notes.
 * 
 * @example
 * ```cpp
 * MyClass obj;
 * obj.init();
 * obj.doSomething();
 * ```
 */
class MyClass {
```

### Function Documentation Example

```cpp
/**
 * @brief Short description of what function does
 * @param input Description of input parameter
 * @param flags Optional flags (default: 0)
 * @return Description of return value
 * 
 * @details Longer explanation if needed, including algorithms,
 * performance characteristics, side effects, etc.
 */
bool myFunction(int input, uint8_t flags = 0);
```

### Member Variable Documentation

```cpp
class MyClass {
private:
    int counter;           ///< Current counter value
    bool isActive;         ///< Active state flag
    String name;           ///< Object name (max 32 chars)
};
```

## 🛠️ Customization

### Edit Doxyfile

Key settings you might want to change:

```ini
# Project info
PROJECT_NAME = "M5Gotchi Pro"
PROJECT_NUMBER = 2.0
PROJECT_BRIEF = "Your custom tagline"

# Output location
OUTPUT_DIRECTORY = docs/doxygen

# Extract private members (default: NO)
EXTRACT_PRIVATE = YES

# Generate class diagrams (requires Graphviz)
HAVE_DOT = YES
CLASS_GRAPH = YES

# Include source code browser
SOURCE_BROWSER = YES

# Color scheme (0-359 for hue)
HTML_COLORSTYLE_HUE = 220
```

### Add Custom CSS

Create `docs/custom.css`:

```css
/* Custom styling for M5Gotchi docs */
body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
}

.title {
    color: #07FF;  /* Cyan like M5Stack */
}
```

Update `Doxyfile`:

```ini
HTML_EXTRA_STYLESHEET = docs/custom.css
```

## 🐛 Troubleshooting

### Issue: "doxygen: command not found"

**Solution:** Install Doxygen and add to PATH

```powershell
# Windows: Check PATH
$env:PATH -split ';' | Select-String doxygen

# Add manually if needed
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\Program Files\doxygen\bin", "User")
```

### Issue: Graphs not generating

**Solution:** Install Graphviz

```powershell
# Windows (Chocolatey)
choco install graphviz

# Or download from: https://graphviz.org/download/
```

Update `Doxyfile`:

```ini
HAVE_DOT = YES
DOT_PATH = "C:/Program Files/Graphviz/bin"
```

### Issue: Warnings about undocumented items

**Solution:** This is normal for incomplete documentation. To suppress:

```ini
WARN_IF_UNDOCUMENTED = NO
```

### Issue: UTF-8 encoding errors

**Solution:** Ensure files are UTF-8 encoded

```ini
INPUT_ENCODING = UTF-8
DOXYFILE_ENCODING = UTF-8
```

## 📊 Documentation Statistics

After generation, check console output:

```
Searching for include files...
Searching for documented files...
Reading and parsing tag files
Parsing files
Preprocessing c:\...\src\m5gotchi_wifi_analyzer.h...
Building group list...
Building directory list...
Building namespace list...
Building file list...
Building class list...
Computing nesting relations for classes...
Associating documentation with classes...
Building example list...
Searching for enumerations...
Searching for documented typedefs...
Searching for members imported via using declarations...
Searching for included using directives...
Searching for documented variables...
Building interface member list...
Building member list...
Searching for friends...
Searching for documented defines...
Computing class inheritance relations...
Computing class usage relations...
Flushing cached template relations that have become invalid...
Computing class relations...
Add enum values to enums...
Searching for member function documentation...
Creating members for template instances...
Building page list...
Search for main page...
Computing page relations...
Determining the scope of groups...
Sorting lists...
Determining which enums are documented
Computing member relations...
Building full member lists recursively...
Adding members to member groups.
Computing member references...
Inheriting documentation...
Generating disk names...
Adding source references...
Adding xrefitems...
Sorting member lists...
Setting anonymous enum type...
Computing dependencies between directories...
Generating citations page...
Counting members...
Counting data structures...
Resolving user defined references...
Finding anchors and sections in the documentation...
Transferring function references...
Combining using relations...
Adding members to index pages...
Correcting members for VHDL...
Computing tooltip texts...
Generating style sheet...
Generating search indices...
Generating example documentation...
Generating file sources...
Generating code for file src/m5gotchi_wifi_analyzer.h...
...
Generating file documentation...
Generating docs for file src/m5gotchi_wifi_analyzer.h...
...
Generating page documentation...
Generating group documentation...
Generating class documentation...
Generating namespace documentation...
Generating graph info page...
Generating directory documentation...
Generating index page...
Generating page index...
Generating module index...
Generating namespace index...
Generating namespace member index...
Generating annotated compound index...
Generating alphabetical compound index...
Generating hierarchical class index...
Generating member index...
Generating file index...
Generating file member index...
Generating example index...
finalizing index lists...
writing tag file...
Running plantuml with JAVA...
lookup cache used 1234/65536 hits=5678 misses=1234
finished...
```

Look for:
- Number of classes documented
- Number of files processed
- Warnings about undocumented items

## 🎓 Learning Doxygen

### Official Resources

- **Manual:** https://www.doxygen.nl/manual/
- **Commands:** https://www.doxygen.nl/manual/commands.html
- **Examples:** https://www.doxygen.nl/manual/examples.html

### Quick Command Reference

| Command | Purpose | Example |
|---------|---------|---------|
| `@file` | Document file | `@file myfile.h` |
| `@brief` | Short description | `@brief Does XYZ` |
| `@details` | Long description | `@details This function...` |
| `@param` | Parameter | `@param x Input value` |
| `@return` | Return value | `@return True on success` |
| `@class` | Class doc | `@class MyClass` |
| `@struct` | Struct doc | `@struct MyStruct` |
| `@enum` | Enum doc | `@enum MyEnum` |
| `@var` | Variable doc | `@var counter` |
| `@example` | Code example | `@example example.cpp` |
| `@note` | Important note | `@note This is critical` |
| `@warning` | Warning | `@warning Don't do X` |
| `@see` | Cross-reference | `@see OtherClass` |
| `@todo` | TODO item | `@todo Implement Y` |
| `@deprecated` | Deprecated | `@deprecated Use Z instead` |

## 📦 Publishing Documentation

### GitHub Pages

1. Generate docs: `doxygen Doxyfile`
2. Copy `docs/doxygen/html/*` to `docs/` in repo
3. Enable GitHub Pages in repo settings
4. Set source to `/docs` folder
5. Access at: `https://yourusername.github.io/repo-name/`

### Self-Hosted

1. Generate docs locally
2. Upload `docs/doxygen/html/` to web server
3. Configure web server to serve static files
4. Set `index.html` as default page

---

**Last Updated:** 2025-10-26  
**Doxygen Version:** 1.9.8+  
**Documentation Version:** 2.0
