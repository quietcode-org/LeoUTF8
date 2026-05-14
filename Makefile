# LeoUTF8 Leopard/PPC Makefile.
# Builds LeoUTF8Core, LeoUTF8Foundation, and probe tools.
#
# This Makefile is intentionally small and Leopard-friendly.
# It does not modify vendor/ sources.

CC = /usr/bin/gcc
AR = /usr/bin/ar

COMMON_FLAGS = -Os -arch ppc -mmacosx-version-min=10.5
BUILD_DIR = build-work

UTF8PROC_BUILD = $(BUILD_DIR)/utf8proc
UTF8PROC_LIB = $(UTF8PROC_BUILD)/libutf8proc.a
UTF8PROC_OBJ = $(UTF8PROC_BUILD)/utf8proc.o

CORE_INC = Sources/LeoUTF8Core
CORE_SRC = Sources/LeoUTF8Core/LeoUTF8.c
CORE_HDR = Sources/LeoUTF8Core/LeoUTF8.h
CORE_OBJ = $(BUILD_DIR)/LeoUTF8.o
CORE_LIB = $(BUILD_DIR)/libLeoUTF8Core.a

FOUNDATION_INC = Sources/LeoUTF8Foundation
FOUNDATION_SRC = Sources/LeoUTF8Foundation/LeoUTF8Foundation.m
FOUNDATION_HDR = Sources/LeoUTF8Foundation/LeoUTF8Foundation.h
FOUNDATION_OBJ = $(BUILD_DIR)/LeoUTF8Foundation.o
FOUNDATION_LIB = $(BUILD_DIR)/libLeoUTF8Foundation.a

C_PROBE_SRC = Sources/LeoUTF8CLI/leoutf8_probe.c
C_PROBE_OBJ = $(BUILD_DIR)/leoutf8_probe.o
C_PROBE_BIN = $(BUILD_DIR)/leoutf8_probe

FOUNDATION_PROBE_SRC = Sources/LeoUTF8CLI/leoutf8_foundation_probe.m
FOUNDATION_PROBE_OBJ = $(BUILD_DIR)/leoutf8_foundation_probe.o
FOUNDATION_PROBE_BIN = $(BUILD_DIR)/leoutf8_foundation_probe

.PHONY: all libs probes check clean utf8proc

all: libs probes

libs: $(CORE_LIB) $(FOUNDATION_LIB)

probes: $(C_PROBE_BIN) $(FOUNDATION_PROBE_BIN)

check: probes
	$(C_PROBE_BIN)
	$(FOUNDATION_PROBE_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(UTF8PROC_LIB): | $(BUILD_DIR)
	rm -rf $(UTF8PROC_BUILD)
	cp -R vendor/utf8proc $(UTF8PROC_BUILD)
	cd $(UTF8PROC_BUILD) && make clean
	cd $(UTF8PROC_BUILD) && make libutf8proc.a \
		CC=$(CC) \
		AR=$(AR) \
		CFLAGS="$(COMMON_FLAGS)" \
		PICFLAG="-fPIC" \
		WCFLAGS="-Wall -Wextra"

utf8proc: $(UTF8PROC_LIB)

$(CORE_OBJ): $(CORE_SRC) $(CORE_HDR) $(UTF8PROC_LIB) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic -Werror \
		-I $(CORE_INC) \
		-isystem $(UTF8PROC_BUILD) \
		-c $(CORE_SRC) \
		-o $(CORE_OBJ)

$(CORE_LIB): $(CORE_OBJ) $(UTF8PROC_LIB)
	rm -f $(CORE_LIB)
	$(AR) crs $(CORE_LIB) $(CORE_OBJ) $(UTF8PROC_OBJ)

$(FOUNDATION_OBJ): $(FOUNDATION_SRC) $(FOUNDATION_HDR) $(CORE_HDR) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-Wall -Wextra -Werror \
		-I $(CORE_INC) \
		-I $(FOUNDATION_INC) \
		-c $(FOUNDATION_SRC) \
		-o $(FOUNDATION_OBJ)

$(FOUNDATION_LIB): $(FOUNDATION_OBJ)
	rm -f $(FOUNDATION_LIB)
	$(AR) crs $(FOUNDATION_LIB) $(FOUNDATION_OBJ)

$(C_PROBE_OBJ): $(C_PROBE_SRC) $(CORE_HDR) $(UTF8PROC_LIB) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic \
		-I $(CORE_INC) \
		-isystem $(UTF8PROC_BUILD) \
		-c $(C_PROBE_SRC) \
		-o $(C_PROBE_OBJ)

$(C_PROBE_BIN): $(C_PROBE_OBJ) $(CORE_LIB)
	$(CC) \
		-arch ppc -mmacosx-version-min=10.5 \
		$(C_PROBE_OBJ) \
		$(CORE_LIB) \
		-o $(C_PROBE_BIN)

$(FOUNDATION_PROBE_OBJ): $(FOUNDATION_PROBE_SRC) $(FOUNDATION_HDR) $(CORE_HDR) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-Wall -Wextra \
		-I $(CORE_INC) \
		-I $(FOUNDATION_INC) \
		-c $(FOUNDATION_PROBE_SRC) \
		-o $(FOUNDATION_PROBE_OBJ)

$(FOUNDATION_PROBE_BIN): $(FOUNDATION_PROBE_OBJ) $(FOUNDATION_LIB) $(CORE_LIB)
	$(CC) \
		-arch ppc -mmacosx-version-min=10.5 \
		$(FOUNDATION_PROBE_OBJ) \
		$(FOUNDATION_LIB) \
		$(CORE_LIB) \
		-framework Foundation \
		-o $(FOUNDATION_PROBE_BIN)

clean:
	rm -rf $(BUILD_DIR)
