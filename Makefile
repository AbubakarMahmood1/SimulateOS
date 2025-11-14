# Makefile for MiniOS - Operating System Simulator
# Compiles kernel, libraries, applications, and main OS

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -I./include
LDFLAGS = -pthread

# Directories
KERNEL_DIR = kernel
LIB_DIR = lib
APPS_DIR = apps
BUILD_DIR = build
BIN_DIR = bin

# Source files
KERNEL_SRCS = $(wildcard $(KERNEL_DIR)/*.c)
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
APP_SRCS = $(wildcard $(APPS_DIR)/*.c)

# Object files
KERNEL_OBJS = $(KERNEL_SRCS:$(KERNEL_DIR)/%.c=$(BUILD_DIR)/kernel_%.o)
LIB_OBJS = $(LIB_SRCS:$(LIB_DIR)/%.c=$(BUILD_DIR)/lib_%.o)

# Application executables
APPS = $(APP_SRCS:$(APPS_DIR)/%.c=$(BIN_DIR)/apps/%)

# Main executable
TARGET = $(BIN_DIR)/minios

# Default target
all: directories $(TARGET) $(APPS)
	@echo ""
	@echo "════════════════════════════════════════════════════════════"
	@echo "           MiniOS Build Complete!                           "
	@echo "════════════════════════════════════════════════════════════"
	@echo ""
	@echo "To run MiniOS, execute: ./bin/minios"
	@echo ""

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)/apps

# Build main OS executable
$(TARGET): main.c $(KERNEL_OBJS) $(LIB_OBJS)
	@echo "Linking MiniOS..."
	$(CC) $(CFLAGS) -o $@ main.c $(KERNEL_OBJS) $(LIB_OBJS) $(LDFLAGS)

# Compile kernel object files
$(BUILD_DIR)/kernel_%.o: $(KERNEL_DIR)/%.c
	@echo "Compiling kernel/$*.c..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compile library object files
$(BUILD_DIR)/lib_%.o: $(LIB_DIR)/%.c
	@echo "Compiling lib/$*.c..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compile application executables
$(BIN_DIR)/apps/%: $(APPS_DIR)/%.c
	@echo "Compiling application $*..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	@echo "Clean complete!"

# Run the OS
run: all
	@echo "Starting MiniOS..."
	@echo ""
	./$(TARGET)

# Show help
help:
	@echo "MiniOS Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build MiniOS and all applications (default)"
	@echo "  clean   - Remove all build files"
	@echo "  run     - Build and run MiniOS"
	@echo "  help    - Show this help message"
	@echo ""

.PHONY: all clean run help directories
