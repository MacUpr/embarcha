# Makefile for Sea State Classification Project
# Raspberry Pi Pico with MPU-6500 and Edge Impulse

# Project name
PROJECT = sea_state_classifier

# Build directory
BUILD_DIR = build

# Check if PICO_SDK_PATH is set
ifndef PICO_SDK_PATH
$(error PICO_SDK_PATH is not set. Please export PICO_SDK_PATH=/path/to/pico-sdk)
endif

# Default target
.PHONY: all
all: build

# Create build directory and configure CMake
.PHONY: configure
configure:
	@echo "Creating build directory..."
	@mkdir -p $(BUILD_DIR)
	@echo "Configuring CMake..."
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
.PHONY: build
build: configure
	@echo "Building project..."
	cd $(BUILD_DIR) && make -j4
	@echo "Build complete! Binary available at: $(BUILD_DIR)/app.uf2"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Deploy to Pico (requires picotool)
.PHONY: deploy
deploy: build
	@echo "Deploying to Raspberry Pi Pico..."
	@echo "Make sure your Pico is in BOOTSEL mode!"
	picotool load -f $(BUILD_DIR)/app.uf2
	@echo "Deployment complete!"

# Monitor serial output
.PHONY: monitor
monitor:
	@echo "Starting serial monitor (115200 baud)..."
	@echo "Press Ctrl+A then K to exit"
	screen /dev/ttyACM0 115200

# Alternative monitor using minicom
.PHONY: minicom
minicom:
	minicom -D /dev/ttyACM0 -b 115200

# Flash and monitor in one command
.PHONY: flash-monitor
flash-monitor: deploy
	@sleep 2
	@make monitor

# Show help
.PHONY: help
help:
	@echo "Sea State Classification - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make build         - Build the project (default)"
	@echo "  make clean         - Clean build artifacts"
	@echo "  make deploy        - Deploy to Pico (requires picotool)"
	@echo "  make monitor       - Start serial monitor"
	@echo "  make flash-monitor - Deploy and start monitor"
	@echo "  make help          - Show this help message"
	@echo ""
	@echo "Requirements:"
	@echo "  - PICO_SDK_PATH environment variable must be set"
	@echo "  - ARM GCC toolchain must be in PATH"
	@echo "  - picotool (for deploy target)"
	@echo "  - screen or minicom (for monitor target)"