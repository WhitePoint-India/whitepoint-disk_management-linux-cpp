# Makefile for Disk-Management Example Application
# Supports building, running, and debugging

# Variables
BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/example
CMAKE := cmake
MAKE := make
DEBUGGER := lldb
BUILD_TYPE ?= Debug

# Colors for output
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
RESET := \033[0m

# Default target
.PHONY: all
all: build

# Help target - displays available commands
.PHONY: help
help:
	@echo "$(BLUE)Available targets:$(RESET)"
	@echo "  $(GREEN)make build$(RESET)    - Build the application (default)"
	@echo "  $(GREEN)make run$(RESET)      - Build and run the application"
	@echo "  $(GREEN)make debug$(RESET)    - Build and debug the application with lldb"
	@echo "  $(GREEN)make clean$(RESET)    - Clean build artifacts"
	@echo "  $(GREEN)make rebuild$(RESET)  - Clean and rebuild the application"
	@echo "  $(GREEN)make test$(RESET)     - Run the application with test output"
	@echo ""
	@echo "$(BLUE)Build types:$(RESET)"
	@echo "  $(YELLOW)BUILD_TYPE=Debug$(RESET)   (default) - Build with debug symbols"
	@echo "  $(YELLOW)BUILD_TYPE=Release$(RESET) - Build with optimizations"
	@echo ""
	@echo "$(BLUE)Examples:$(RESET)"
	@echo "  make build BUILD_TYPE=Release"
	@echo "  make debug"

# Build target - configures and builds the project
.PHONY: build
build: $(BUILD_DIR)/Makefile
	@echo "$(BLUE)Building application...$(RESET)"
	@$(MAKE) -C $(BUILD_DIR)
	@echo "$(GREEN)Build complete!$(RESET)"

# Configure CMake if needed
$(BUILD_DIR)/Makefile:
	@echo "$(BLUE)Configuring CMake...$(RESET)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

# Run target - builds and runs the application
.PHONY: run
run: build
	@echo "$(BLUE)Running application...$(RESET)"
	@echo "$(YELLOW)----------------------------------------$(RESET)"
	@$(EXECUTABLE)
	@echo "$(YELLOW)----------------------------------------$(RESET)"
	@echo "$(GREEN)Application finished!$(RESET)"

# Debug target - builds with debug symbols and launches debugger
.PHONY: debug
debug: BUILD_TYPE := Debug
debug: build
	@echo "$(BLUE)Starting debugger ($(DEBUGGER))...$(RESET)"
	@echo "$(YELLOW)Debug commands:$(RESET)"
	@echo "  r/run         - Start the program"
	@echo "  b/breakpoint  - Set a breakpoint (e.g., 'b main')"
	@echo "  c/continue    - Continue execution"
	@echo "  n/next        - Step over"
	@echo "  s/step        - Step into"
	@echo "  p/print       - Print variable (e.g., 'p welcomeMsg')"
	@echo "  bt            - Show backtrace"
	@echo "  q/quit        - Exit debugger"
	@echo "$(YELLOW)----------------------------------------$(RESET)"
	@$(DEBUGGER) $(EXECUTABLE)

# Test target - runs the application with verbose output
.PHONY: test
test: build
	@echo "$(BLUE)Running tests...$(RESET)"
	@echo "$(YELLOW)Test output:$(RESET)"
	@if $(EXECUTABLE); then \
		echo "$(GREEN)✓ Test passed: Application runs successfully$(RESET)"; \
	else \
		echo "$(RED)✗ Test failed: Application crashed$(RESET)"; \
		exit 1; \
	fi

# Clean target - removes build artifacts
.PHONY: clean
clean:
	@echo "$(BLUE)Cleaning build artifacts...$(RESET)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Clean complete!$(RESET)"

# Rebuild target - clean and build
.PHONY: rebuild
rebuild: clean build

# Verbose build - shows all compiler commands
.PHONY: verbose
verbose: $(BUILD_DIR)/Makefile
	@echo "$(BLUE)Building with verbose output...$(RESET)"
	@$(MAKE) -C $(BUILD_DIR) VERBOSE=1

.DEFAULT_GOAL := all