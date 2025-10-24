#!/bin/bash
#
# M5Gotchi Pro - Build Script
# Compiles project and generates binary files
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Functions
print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  M5Gotchi Pro - Build System  ${NC}"
    echo -e "${BLUE}================================${NC}"
    echo ""
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_info() {
    echo -e "${YELLOW}ℹ${NC} $1"
}

check_platformio() {
    if ! command -v pio &> /dev/null; then
        print_error "PlatformIO not found!"
        echo "Install with: pip install platformio"
        exit 1
    fi
    print_success "PlatformIO found"
}

install_dependencies() {
    print_info "Installing dependencies..."
    pio pkg install
    print_success "Dependencies installed"
}

build_project() {
    local env=$1
    print_info "Building for $env..."
    pio run -e $env
    print_success "Build completed for $env"
}

copy_binaries() {
    local env=$1
    print_info "Copying binaries..."
    
    mkdir -p bin
    
    # Copy firmware.bin
    if [ -f ".pio/build/$env/firmware.bin" ]; then
        cp ".pio/build/$env/firmware.bin" "bin/M5Gotchi_Pro_${env}.bin"
        print_success "Copied firmware.bin"
    fi
    
    # Copy bootloader
    if [ -f ".pio/build/$env/bootloader.bin" ]; then
        cp ".pio/build/$env/bootloader.bin" "bin/bootloader_${env}.bin"
        print_success "Copied bootloader.bin"
    fi
    
    # Copy partitions
    if [ -f ".pio/build/$env/partitions.bin" ]; then
        cp ".pio/build/$env/partitions.bin" "bin/partitions_${env}.bin"
        print_success "Copied partitions.bin"
    fi
}

generate_flash_script() {
    local env=$1
    print_info "Generating flash script..."
    
    cat > bin/flash_${env}.sh << 'EOF'
#!/bin/bash
# Flash script for M5Gotchi Pro

PORT=${1:-/dev/ttyUSB0}

echo "Flashing M5Gotchi Pro to $PORT..."

esptool.py --chip esp32 \
    --port $PORT \
    --baud 921600 \
    --before default_reset \
    --after hard_reset \
    write_flash -z \
    --flash_mode dio \
    --flash_freq 40m \
    --flash_size detect \
    0x1000 bootloader_ENV.bin \
    0x8000 partitions_ENV.bin \
    0x10000 M5Gotchi_Pro_ENV.bin

echo "Done! Reset your device."
EOF

    sed -i "s/ENV/$env/g" "bin/flash_${env}.sh"
    chmod +x "bin/flash_${env}.sh"
    
    print_success "Flash script created: bin/flash_${env}.sh"
}

generate_readme() {
    print_info "Generating bin/README.md..."
    
    cat > bin/README.md << 'EOF'
# M5Gotchi Pro - Binary Files

## Quick Flash

### Option 1: Using esptool (Recommended)
```bash
# For M5Stack Core
./flash_m5stack-core-esp32.sh /dev/ttyUSB0

# For M5Stack Fire
./flash_m5stack-fire.sh /dev/ttyUSB0

# Windows users: replace /dev/ttyUSB0 with COM3 (your port)
```

### Option 2: Manual Flash
```bash
esptool.py --chip esp32 \
    --port /dev/ttyUSB0 \
    --baud 921600 \
    write_flash -z \
    0x1000 bootloader_m5stack-core-esp32.bin \
    0x8000 partitions_m5stack-core-esp32.bin \
    0x10000 M5Gotchi_Pro_m5stack-core-esp32.bin
```

### Option 3: Using PlatformIO
```bash
cd ..
pio run --target upload
```

## Files Description

| File | Address | Description |
|------|---------|-------------|
| `bootloader_*.bin` | 0x1000 | ESP32 bootloader |
| `partitions_*.bin` | 0x8000 | Partition table |
| `M5Gotchi_Pro_*.bin` | 0x10000 | Main firmware |

## Supported Devices

- M5Stack Core ESP32
- M5Stack Fire
- M5Stack Core2
- M5Stick-C

## Tools Required

Install esptool:
```bash
pip install esptool
```

## Troubleshooting

**Device not detected:**
- Install CH340 driver (for M5Stack)
- Try different USB cable
- Check port: `ls /dev/tty*` (Linux/Mac) or Device Manager (Windows)

**Flash failed:**
- Hold button A while resetting
- Try lower baud: `--baud 115200`
- Verify cable supports data transfer

**Permission denied (Linux):**
```bash
sudo usermod -a -G dialout $USER
# Logout and login again
```
EOF

    print_success "README.md created in bin/"
}

build_all() {
    print_header
    
    # Check requirements
    check_platformio
    
    # Install deps
    install_dependencies
    
    # Build for each environment
    ENVS=("m5stack-core-esp32" "m5stack-fire" "m5stack-core2" "m5stack-cardputer")
    
    for env in "${ENVS[@]}"; do
        echo ""
        print_info "Processing $env..."
        build_project $env
        copy_binaries $env
        generate_flash_script $env
        echo ""
    done
    
    # Generate README
    generate_readme
    
    # Summary
    echo ""
    print_header
    print_success "Build completed successfully!"
    echo ""
    print_info "Binary files are in: bin/"
    ls -lh bin/
    echo ""
    print_info "To flash your device:"
    echo "  cd bin"
    echo "  ./flash_m5stack-core-esp32.sh /dev/ttyUSB0"
    echo ""
}

# Parse arguments
case "${1:-all}" in
    all)
        build_all
        ;;
    core)
        print_header
        check_platformio
        build_project "m5stack-core-esp32"
        copy_binaries "m5stack-core-esp32"
        generate_flash_script "m5stack-core-esp32"
        ;;
    fire)
        print_header
        check_platformio
        build_project "m5stack-fire"
        copy_binaries "m5stack-fire"
        generate_flash_script "m5stack-fire"
        ;;
    core2)
        print_header
        check_platformio
        build_project "m5stack-core2"
        copy_binaries "m5stack-core2"
        generate_flash_script "m5stack-core2"
        ;;
    cardputer)
        print_header
        check_platformio
        build_project "m5stack-cardputer"
        copy_binaries "m5stack-cardputer"
        generate_flash_script "m5stack-cardputer"
        ;;
    clean)
        print_info "Cleaning..."
        rm -rf .pio bin/*
        print_success "Clean completed"
        ;;
    help|*)
        echo "M5Gotchi Pro - Build Script"
        echo ""
        echo "Usage: $0 [command]"
        echo ""
        echo "Commands:"
        echo "  all       - Build all environments (default)"
        echo "  core      - Build for M5Stack Core only"
        echo "  fire      - Build for M5Stack Fire only"
        echo "  core2     - Build for M5Stack Core2 only"
        echo "  cardputer - Build for M5Stack Cardputer only"
        echo "  clean     - Clean build files"
        echo "  help      - Show this help"
        echo ""
        ;;
esac
