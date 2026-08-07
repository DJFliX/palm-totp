# Makefile for PalmTotp Palm OS Application

APP_NAME = totp
BUILD_DIR = build
PRC_NAME = $(BUILD_DIR)/$(APP_NAME).prc
CREATOR_ID = TOTP

# Source files
SOURCES = $(APP_NAME).c
RESOURCES = $(APP_NAME).rcp

# Get script directory for toolchain
TOOLCHAIN_DIR := $(shell cd .. && pwd)/toolchain

# Check if toolchain is installed
ifeq ($(wildcard $(TOOLCHAIN_DIR)/bin/m68k-palmos-gcc),)
  $(error Toolchain not found! Please run: cd .. && ./setup-toolchain.sh)
endif

# Set GCC environment (required for prc-tools to find libraries)
export GCC_EXEC_PREFIX = $(TOOLCHAIN_DIR)/lib/gcc-lib/

GCC_VERSION = $(shell $(TOOLCHAIN_DIR)/bin/m68k-palmos-gcc -dumpversion)
COMPILER_INCLUDE = $(TOOLCHAIN_DIR)/lib/gcc-lib/m68k-palmos/$(GCC_VERSION)/include

SDK     = $(TOOLCHAIN_DIR)/../sdks/sdk-4/include
PRC_INCLUDE = $(TOOLCHAIN_DIR)/share/prc-tools/include

# Compiler and tools
CC = $(TOOLCHAIN_DIR)/bin/m68k-palmos-gcc
COMMON = -Wno-multichar -m68000 -mno-align-int -mpcrel -fpic -fshort-enums -mshort
CFLAGS = $(COMMON) -palmos4 -nostdinc -O2 -Wall -I$(PRC_INCLUDE) \
	-I$(COMPILER_INCLUDE) \
	-I$(SDK)/Core -I$(SDK)/Core/System -I$(SDK)/Core/UI
PILRC = pilrc
BUILD_PRC = $(TOOLCHAIN_DIR)/bin/build-prc
PILOT_XFER = /opt/homebrew/opt/pilot-link/bin/pilot-xfer

# Build targets
OBJS = $(BUILD_DIR)/$(APP_NAME).o $(BUILD_DIR)/TOTPAlg.o $(BUILD_DIR)/sha1.o
APP_BINARY = $(BUILD_DIR)/$(APP_NAME)
RESOURCE_FILE = $(APP_NAME).ro

.PHONY: all clean install info

all: info

info:
	@echo "╔════════════════════════════════════════════════════╗"
	@echo "║        PalmTotp - Palm OS Application            ║"
	@echo "╚════════════════════════════════════════════════════╝"
	@echo ""
	@echo " A simple 'Hello World' demo for Palm OS"
	@echo ""
	@echo " Available targets:"
	@echo "  make build     - Build $(PRC_NAME)"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make install   - Install to Palm device via USB"
	@echo "  make rebuild   - Clean and build"
	@echo ""
	@echo " Status:"
	@if [ -f "$(TOOLCHAIN_DIR)/bin/m68k-palmos-gcc" ]; then \
		echo "  ✓ Toolchain ready"; \
	else \
		echo "  ✗ Toolchain not found - run: cd .. && ./setup-toolchain.sh"; \
	fi

build: $(PRC_NAME)
	@echo "✓ Built $(PRC_NAME)"
	@ls -lh $(PRC_NAME)

$(PRC_NAME): $(APP_BINARY) $(RESOURCE_FILE)
	@mkdir -p $(BUILD_DIR)
	$(BUILD_PRC) -o $@ \
		-t appl \
		-c $(CREATOR_ID) \
		-n "$(APP_NAME)" \
		$(APP_BINARY) $(BUILD_DIR)/*.bin

$(APP_BINARY): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RESOURCE_FILE): $(RESOURCES) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(PILRC) $(RESOURCES) $(BUILD_DIR)

clean:
	/bin/rm -rf $(BUILD_DIR)
	/bin/rm -f *.grc *~
	/bin/rm -f *.[oa] totp *.bin *.stamp *.grc

install: $(PRC_NAME)
	@echo "Installing $(PRC_NAME) to Palm device..."
	$(PILOT_XFER) -i $(PRC_NAME)

rebuild: clean build
