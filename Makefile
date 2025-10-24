# M5Gotchi Pro - Makefile
# Build automation for PlatformIO

.PHONY: all build upload clean monitor test bin install help

# Default target
all: build

# Build the project
build:
	@echo "🔨 Building M5Gotchi Pro..."
	pio run

# Build specific environment
build-core:
	@echo "🔨 Building for M5Stack Core..."
	pio run -e m5stack-core-esp32

build-fire:
	@echo "🔨 Building for M5Stack Fire..."
	pio run -e m5stack-fire

build-core2:
	@echo "🔨 Building for M5Stack Core2..."
	pio run -e m5stack-core2

build-cardputer:
	@echo "🔨 Building for M5Stack Cardputer..."
	pio run -e m5stack-cardputer

# Upload to device
upload:
	@echo "📤 Uploading to device..."
	pio run --target upload

upload-core:
	@echo "📤 Uploading to M5Stack Core..."
	pio run -e m5stack-core-esp32 --target upload

upload-fire:
	@echo "📤 Uploading to M5Stack Fire..."
	pio run -e m5stack-fire --target upload

upload-cardputer:
	@echo "📤 Uploading to M5Stack Cardputer..."
	pio run -e m5stack-cardputer --target upload

# Clean build files
clean:
	@echo "🧹 Cleaning build files..."
	pio run --target clean
	rm -rf .pio
	rm -rf bin/*.bin bin/*.elf

# Open serial monitor
monitor:
	@echo "📟 Opening serial monitor..."
	pio device monitor

# Run tests
test:
	@echo "🧪 Running tests..."
	pio test

# Copy binaries to bin folder
bin:
	@echo "📦 Copying binaries to bin/..."
	@mkdir -p bin
	@find .pio/build -name "*.bin" -exec cp {} bin/ \;
	@find .pio/build -name "*.elf" -exec cp {} bin/ \;
	@echo "✅ Binaries copied to bin/"
	@ls -lh bin/

# Install dependencies
install:
	@echo "📥 Installing dependencies..."
	pio pkg install

# Update dependencies
update:
	@echo "🔄 Updating dependencies..."
	pio pkg update

# Check code
check:
	@echo "🔍 Checking code..."
	pio check

# Format code
format:
	@echo "✨ Formatting code..."
	find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Show project info
info:
	@echo "ℹ️  Project Information:"
	@echo "========================"
	pio project config

# List devices
devices:
	@echo "🔌 Available devices:"
	pio device list

# Generate compile_commands.json for IDEs
compile-commands:
	@echo "📝 Generating compile_commands.json..."
	pio run --target compiledb

# Full clean (including dependencies)
distclean: clean
	@echo "🗑️  Deep cleaning..."
	rm -rf .pio
	rm -rf bin/*

# Release build
release: clean
	@echo "🚀 Building release..."
	pio run
	@$(MAKE) bin
	@echo "✅ Release build complete!"
	@echo "📦 Binaries available in bin/"

# Help
help:
	@echo "M5Gotchi Pro - Build System"
	@echo "============================"
	@echo ""
	@echo "Available targets:"
	@echo "  make build          - Build the project"
	@echo "  make build-core     - Build for M5Stack Core"
	@echo "  make build-fire     - Build for M5Stack Fire"
	@echo "  make build-core2    - Build for M5Stack Core2"
	@echo "  make build-cardputer - Build for M5Stack Cardputer"
	@echo "  make upload         - Upload to device"
	@echo "  make upload-core    - Upload to M5Stack Core"
	@echo "  make upload-cardputer - Upload to M5Stack Cardputer"
	@echo "  make monitor        - Open serial monitor"
	@echo "  make clean          - Clean build files"
	@echo "  make bin            - Copy binaries to bin/"
	@echo "  make install        - Install dependencies"
	@echo "  make update         - Update dependencies"
	@echo "  make check          - Check code quality"
	@echo "  make devices        - List connected devices"
	@echo "  make release        - Build release version"
	@echo "  make help           - Show this help"
	@echo ""
	@echo "Quick start:"
	@echo "  1. make install     - Install dependencies"
	@echo "  2. make build       - Build the project"
	@echo "  3. make upload      - Upload to device"
	@echo "  4. make monitor     - View serial output"
