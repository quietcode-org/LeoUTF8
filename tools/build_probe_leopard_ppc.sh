#!/bin/sh

# LeoUTF8 Leopard/PPC probe build.
# Builds vendored utf8proc outside vendor/ and links the initial LeoUTF8 probe.
# Must be run on Mac OS X 10.5.x PowerPC.

set -e

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
VENDOR_UTF8PROC="$ROOT_DIR/vendor/utf8proc"
BUILD_ROOT="$ROOT_DIR/build-work"
UTF8PROC_BUILD="$BUILD_ROOT/utf8proc-probe"
PROBE_SRC="$ROOT_DIR/Sources/LeoUTF8CLI/leoutf8_probe.c"
PROBE_BIN="$BUILD_ROOT/leoutf8_probe"

echo "LeoUTF8 Leopard/PPC probe build"
echo "Root:          $ROOT_DIR"
echo "Vendor source: $VENDOR_UTF8PROC"
echo "Build root:    $BUILD_ROOT"
echo "Probe source:  $PROBE_SRC"
echo "Probe binary:  $PROBE_BIN"
echo

SYSTEM_NAME="$(uname -s)"
MACHINE_NAME="$(uname -m)"

echo "Host system:  $SYSTEM_NAME"
echo "Host machine: $MACHINE_NAME"
echo

if [ "$SYSTEM_NAME" != "Darwin" ]; then
    echo "ERROR: This probe must run on Darwin / Mac OS X."
    exit 1
fi

case "$MACHINE_NAME" in
    ppc|powerpc|Power*)
        ;;
    *)
        echo "ERROR: This probe must run on PowerPC."
        exit 1
        ;;
esac

if [ ! -d "$VENDOR_UTF8PROC" ]; then
    echo "ERROR: utf8proc vendor source not found: $VENDOR_UTF8PROC"
    exit 1
fi

if [ ! -f "$PROBE_SRC" ]; then
    echo "ERROR: Probe source not found: $PROBE_SRC"
    exit 1
fi

echo "Preparing build-work..."
mkdir -p "$BUILD_ROOT"
rm -rf "$UTF8PROC_BUILD"
cp -R "$VENDOR_UTF8PROC" "$UTF8PROC_BUILD"

echo "Building libutf8proc.a..."
cd "$UTF8PROC_BUILD"

make clean

make libutf8proc.a \
    CC=/usr/bin/gcc \
    AR=/usr/bin/ar \
    CFLAGS="-Os -arch ppc -mmacosx-version-min=10.5" \
    PICFLAG="-fPIC" \
    WCFLAGS="-Wall -Wextra -pedantic"

echo
echo "Built:"
ls -lh "$UTF8PROC_BUILD/libutf8proc.a" "$UTF8PROC_BUILD/utf8proc.o"
file "$UTF8PROC_BUILD/libutf8proc.a" "$UTF8PROC_BUILD/utf8proc.o"

echo
echo "Building LeoUTF8 probe..."
cd "$ROOT_DIR"

/usr/bin/gcc \
    -Os -arch ppc -mmacosx-version-min=10.5 \
    -I "$UTF8PROC_BUILD" \
    "$PROBE_SRC" \
    "$UTF8PROC_BUILD/libutf8proc.a" \
    -o "$PROBE_BIN"

echo
echo "Running LeoUTF8 probe..."
"$PROBE_BIN"

echo
echo "LeoUTF8 Leopard/PPC probe completed successfully."
