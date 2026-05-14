# LeoUTF8 Leopard/PPC Makefile.
# Builds LeoUTF8Core, LeoUTF8Foundation, probe tools, reusable artifacts,
# and distribution consumer checks.
#
# This Makefile is intentionally small and Leopard-friendly.
# It does not modify vendor/ sources.

CC = /usr/bin/gcc
AR = /usr/bin/ar

PREFIX = /usr/local
DESTDIR =

COMMON_FLAGS = -Os -arch ppc -mmacosx-version-min=10.5
BUILD_DIR = build-work
DIST_DIR = dist/LeoUTF8
DISTCHECK_DIR = $(BUILD_DIR)/distcheck

UTF8PROC_BUILD = $(BUILD_DIR)/utf8proc
UTF8PROC_LIB = $(UTF8PROC_BUILD)/libutf8proc.a
UTF8PROC_OBJ = $(UTF8PROC_BUILD)/utf8proc.o

CORE_INC = Sources/LeoUTF8Core
CORE_SRC = Sources/LeoUTF8Core/LeoUTF8.c
CORE_HDR = Sources/LeoUTF8Core/LeoUTF8.h
CORE_OBJ = $(BUILD_DIR)/LeoUTF8.o
CORE_LIB = $(BUILD_DIR)/libLeoUTF8Core.a

COREFOUNDATION_INC = Sources/LeoUTF8CoreFoundation
COREFOUNDATION_SRC = Sources/LeoUTF8CoreFoundation/LeoUTF8CoreFoundation.c
COREFOUNDATION_HDR = Sources/LeoUTF8CoreFoundation/LeoUTF8CoreFoundation.h
COREFOUNDATION_OBJ = $(BUILD_DIR)/LeoUTF8CoreFoundation.o
COREFOUNDATION_LIB = $(BUILD_DIR)/libLeoUTF8CoreFoundation.a

FOUNDATION_INC = Sources/LeoUTF8Foundation
FOUNDATION_SRC = Sources/LeoUTF8Foundation/LeoUTF8Foundation.m
FOUNDATION_HDR = Sources/LeoUTF8Foundation/LeoUTF8Foundation.h
FOUNDATION_OBJ = $(BUILD_DIR)/LeoUTF8Foundation.o
FOUNDATION_LIB = $(BUILD_DIR)/libLeoUTF8Foundation.a

C_PROBE_SRC = Sources/LeoUTF8CLI/leoutf8_probe.c
C_PROBE_OBJ = $(BUILD_DIR)/leoutf8_probe.o
C_PROBE_BIN = $(BUILD_DIR)/leoutf8_probe

COREFOUNDATION_PROBE_SRC = Sources/LeoUTF8CLI/leoutf8_corefoundation_probe.c
COREFOUNDATION_PROBE_OBJ = $(BUILD_DIR)/leoutf8_corefoundation_probe.o
COREFOUNDATION_PROBE_BIN = $(BUILD_DIR)/leoutf8_corefoundation_probe

FOUNDATION_PROBE_SRC = Sources/LeoUTF8CLI/leoutf8_foundation_probe.m
FOUNDATION_PROBE_OBJ = $(BUILD_DIR)/leoutf8_foundation_probe.o
FOUNDATION_PROBE_BIN = $(BUILD_DIR)/leoutf8_foundation_probe

CONSUMER_CORE_SRC = Tests/consumer_core_probe.c
CONSUMER_CORE_BIN = $(DISTCHECK_DIR)/consumer_core_probe

CONSUMER_COREFOUNDATION_SRC = Tests/consumer_corefoundation_probe.c
CONSUMER_COREFOUNDATION_BIN = $(DISTCHECK_DIR)/consumer_corefoundation_probe

CONSUMER_FOUNDATION_SRC = Tests/consumer_foundation_probe.m
CONSUMER_FOUNDATION_BIN = $(DISTCHECK_DIR)/consumer_foundation_probe

HFSPLUS_PROBE_SRC = Research/HFSPlus/hfsplus_filename_probe.c
HFSPLUS_PROBE_BIN = $(BUILD_DIR)/hfsplus_filename_probe

HFSBOUNDARY_PROBE_SRC = Research/HFSPlus/hfsplus_cfstring_boundary_probe.m
HFSBOUNDARY_PROBE_BIN = $(BUILD_DIR)/hfsplus_cfstring_boundary_probe

HFSCASE_PROBE_SRC = Research/HFSPlus/hfsplus_case_probe.c
HFSCASE_PROBE_BIN = $(BUILD_DIR)/hfsplus_case_probe

HFSCOMPARE_PROBE_SRC = Research/HFSPlus/hfsplus_cfcompare_probe.m
HFSCOMPARE_PROBE_BIN = $(BUILD_DIR)/hfsplus_cfcompare_probe

.PHONY: all libs probes check hfscheck hfsboundarycheck hfscasecheck hfscomparecheck clean dist distcheck distclean install utf8proc

all: libs probes

libs: $(CORE_LIB) $(COREFOUNDATION_LIB) $(FOUNDATION_LIB)

probes: $(C_PROBE_BIN) $(COREFOUNDATION_PROBE_BIN) $(FOUNDATION_PROBE_BIN)

check: probes
	$(C_PROBE_BIN)
	$(COREFOUNDATION_PROBE_BIN)
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

$(COREFOUNDATION_OBJ): $(COREFOUNDATION_SRC) $(COREFOUNDATION_HDR) $(CORE_HDR) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic -Werror \
		-I $(CORE_INC) \
		-I $(COREFOUNDATION_INC) \
		-c $(COREFOUNDATION_SRC) \
		-o $(COREFOUNDATION_OBJ)

$(COREFOUNDATION_LIB): $(COREFOUNDATION_OBJ)
	rm -f $(COREFOUNDATION_LIB)
	$(AR) crs $(COREFOUNDATION_LIB) $(COREFOUNDATION_OBJ)

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

$(COREFOUNDATION_PROBE_OBJ): $(COREFOUNDATION_PROBE_SRC) $(COREFOUNDATION_HDR) $(CORE_HDR) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic \
		-I $(CORE_INC) \
		-I $(COREFOUNDATION_INC) \
		-c $(COREFOUNDATION_PROBE_SRC) \
		-o $(COREFOUNDATION_PROBE_OBJ)

$(COREFOUNDATION_PROBE_BIN): $(COREFOUNDATION_PROBE_OBJ) $(COREFOUNDATION_LIB) $(CORE_LIB)
	$(CC) \
		-arch ppc -mmacosx-version-min=10.5 \
		$(COREFOUNDATION_PROBE_OBJ) \
		$(COREFOUNDATION_LIB) \
		$(CORE_LIB) \
		-framework CoreFoundation \
		-o $(COREFOUNDATION_PROBE_BIN)

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

dist: libs
	rm -rf $(DIST_DIR)
	mkdir -p $(DIST_DIR)/include
	mkdir -p $(DIST_DIR)/lib
	cp -p $(CORE_HDR) $(DIST_DIR)/include/
	cp -p $(COREFOUNDATION_HDR) $(DIST_DIR)/include/
	cp -p $(FOUNDATION_HDR) $(DIST_DIR)/include/
	cp -p $(CORE_LIB) $(DIST_DIR)/lib/
	cp -p $(COREFOUNDATION_LIB) $(DIST_DIR)/lib/
	cp -p $(FOUNDATION_LIB) $(DIST_DIR)/lib/
	@echo "LeoUTF8 distribution staged in $(DIST_DIR)"

$(DISTCHECK_DIR):
	mkdir -p $(DISTCHECK_DIR)

$(CONSUMER_CORE_BIN): dist $(CONSUMER_CORE_SRC) | $(DISTCHECK_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic \
		-I $(DIST_DIR)/include \
		$(CONSUMER_CORE_SRC) \
		$(DIST_DIR)/lib/libLeoUTF8Core.a \
		-o $(CONSUMER_CORE_BIN)

$(CONSUMER_COREFOUNDATION_BIN): dist $(CONSUMER_COREFOUNDATION_SRC) | $(DISTCHECK_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic \
		-I $(DIST_DIR)/include \
		$(CONSUMER_COREFOUNDATION_SRC) \
		$(DIST_DIR)/lib/libLeoUTF8CoreFoundation.a \
		$(DIST_DIR)/lib/libLeoUTF8Core.a \
		-framework CoreFoundation \
		-o $(CONSUMER_COREFOUNDATION_BIN)

$(CONSUMER_FOUNDATION_BIN): dist $(CONSUMER_FOUNDATION_SRC) | $(DISTCHECK_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-Wall -Wextra \
		-I $(DIST_DIR)/include \
		$(CONSUMER_FOUNDATION_SRC) \
		$(DIST_DIR)/lib/libLeoUTF8Foundation.a \
		$(DIST_DIR)/lib/libLeoUTF8Core.a \
		-framework Foundation \
		-o $(CONSUMER_FOUNDATION_BIN)

distcheck: $(CONSUMER_CORE_BIN) $(CONSUMER_COREFOUNDATION_BIN) $(CONSUMER_FOUNDATION_BIN)
	$(CONSUMER_CORE_BIN)
	$(CONSUMER_COREFOUNDATION_BIN)
	$(CONSUMER_FOUNDATION_BIN)
	@echo "LeoUTF8 distcheck completed successfully."

$(HFSPLUS_PROBE_BIN): $(HFSPLUS_PROBE_SRC) $(CORE_LIB) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic \
		-I $(CORE_INC) \
		$(HFSPLUS_PROBE_SRC) \
		$(CORE_LIB) \
		-o $(HFSPLUS_PROBE_BIN)

hfscheck: $(HFSPLUS_PROBE_BIN)
	$(HFSPLUS_PROBE_BIN)

$(HFSBOUNDARY_PROBE_BIN): $(HFSBOUNDARY_PROBE_SRC) $(CORE_LIB) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-Wall -Wextra \
		-I $(CORE_INC) \
		$(HFSBOUNDARY_PROBE_SRC) \
		$(CORE_LIB) \
		-framework Foundation \
		-framework CoreFoundation \
		-o $(HFSBOUNDARY_PROBE_BIN)

hfsboundarycheck: $(HFSBOUNDARY_PROBE_BIN)
	$(HFSBOUNDARY_PROBE_BIN)

$(HFSCASE_PROBE_BIN): $(HFSCASE_PROBE_SRC) $(CORE_LIB) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-std=c99 \
		-Wall -Wextra -pedantic \
		-I $(CORE_INC) \
		$(HFSCASE_PROBE_SRC) \
		$(CORE_LIB) \
		-o $(HFSCASE_PROBE_BIN)

hfscasecheck: $(HFSCASE_PROBE_BIN)
	$(HFSCASE_PROBE_BIN)

$(HFSCOMPARE_PROBE_BIN): $(HFSCOMPARE_PROBE_SRC) $(CORE_LIB) | $(BUILD_DIR)
	$(CC) \
		$(COMMON_FLAGS) \
		-Wall -Wextra \
		-I $(CORE_INC) \
		$(HFSCOMPARE_PROBE_SRC) \
		$(CORE_LIB) \
		-framework Foundation \
		-framework CoreFoundation \
		-o $(HFSCOMPARE_PROBE_BIN)

hfscomparecheck: $(HFSCOMPARE_PROBE_BIN)
	$(HFSCOMPARE_PROBE_BIN)

install: libs
	mkdir -p $(DESTDIR)$(PREFIX)/include
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp -p $(CORE_HDR) $(DESTDIR)$(PREFIX)/include/
	cp -p $(COREFOUNDATION_HDR) $(DESTDIR)$(PREFIX)/include/
	cp -p $(FOUNDATION_HDR) $(DESTDIR)$(PREFIX)/include/
	cp -p $(CORE_LIB) $(DESTDIR)$(PREFIX)/lib/
	cp -p $(COREFOUNDATION_LIB) $(DESTDIR)$(PREFIX)/lib/
	cp -p $(FOUNDATION_LIB) $(DESTDIR)$(PREFIX)/lib/
	@echo "LeoUTF8 installed into $(DESTDIR)$(PREFIX)"

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf dist
