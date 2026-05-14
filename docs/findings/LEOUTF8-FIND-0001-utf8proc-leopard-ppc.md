# LEOUTF8-FIND-0001: utf8proc builds and runs on Leopard/PPC

## Summary

utf8proc 2.11.3 can be built on Mac OS X 10.5.8 PowerPC using the native Leopard GCC toolchain and can be used through the initial LeoUTF8Core wrapper.

This confirms that LeoUTF8 can use utf8proc as a small C-based UTF-8 processing engine without requiring MacPorts, CMake, ICU, or a modern runtime dependency.

## Host

- System: Mac OS X 10.5.8 Leopard
- Architecture: PowerPC
- Compiler: `/usr/bin/gcc`
- Target flags:
  - `-Os`
  - `-arch ppc`
  - `-mmacosx-version-min=10.5`

## Source

- Vendor source: `vendor/utf8proc`
- utf8proc version reported by probe: `2.11.3`
- Vendor policy: source remains unchanged under `vendor/`

## Build Method

The probe build is reproducible through:

```sh
./tools/build_probe_leopard_ppc.sh
````

The script:

1. copies `vendor/utf8proc` into `build-work/utf8proc-probe`
    
2. builds `libutf8proc.a`
    
3. builds `Sources/LeoUTF8Core/LeoUTF8.c`
    
4. links `Sources/LeoUTF8CLI/leoutf8_probe.c`
    
5. runs the probe
    

## utf8proc Build Flags

```sh
make libutf8proc.a \
    CC=/usr/bin/gcc \
    AR=/usr/bin/ar \
    CFLAGS="-Os -arch ppc -mmacosx-version-min=10.5" \
    PICFLAG="-fPIC" \
    WCFLAGS="-Wall -Wextra -pedantic"
```

The upstream Makefile warning flags `-Wsign-conversion` and `-Wc++-compat` are not accepted by Leopard GCC 4 and are therefore omitted by overriding `WCFLAGS`.

## Artifacts

The build produced:

```text
libutf8proc.a: current ar archive random library
utf8proc.o:    Mach-O object ppc
```

## Probe Result

The initial LeoUTF8 probe passed on Leopard/PPC.

Test sample:

```text
Größe ÄÖÜ Straße
```

UTF-8 bytes:

```text
47 72 C3 B6 C3 9F 65 20 C3 84 C3 96 C3 9C 20 53 74 72 61 C3 9F 65
```

Codepoint count:

```text
16
```

NFC result:

```text
Größe ÄÖÜ Straße
```

NFC bytes:

```text
47 72 C3 B6 C3 9F 65 20 C3 84 C3 96 C3 9C 20 53 74 72 61 C3 9F 65
```

NFC length:

```text
22 bytes
```

NFD result:

```text
Größe ÄÖÜ Straße
```

NFD bytes:

```text
47 72 6F CC 88 C3 9F 65 20 41 CC 88 4F CC 88 55 CC 88 20 53 74 72 61 C3 9F 65
```

NFD length:

```text
26 bytes
```

Casefold result:

```text
grösse äöü strasse
```

Casefold bytes:

```text
67 72 C3 B6 73 73 65 20 C3 A4 C3 B6 C3 BC 20 73 74 72 61 73 73 65
```

Casefold length:

```text
22 bytes
```

## Confirmed Initial Operations

The current LeoUTF8Core proof confirms:

- UTF-8 validation
    
- UTF-8 codepoint counting
    
- NFC normalization
    
- NFD normalization
    
- Unicode case folding
    
- explicit byte-length reporting
    
- static linking against vendored utf8proc
    
- execution on real Leopard/PPC hardware
    

## Known Warnings

Leopard GCC emits warnings for utf8proc bit-fields:

```text
utf8proc.h:294: warning: type of bit-field 'comb_index' is a GCC extension
utf8proc.h:295: warning: type of bit-field 'comb_length' is a GCC extension
utf8proc.h:296: warning: type of bit-field 'comb_issecond' is a GCC extension
```

These warnings do not block the build.

## Interpretation

This finding confirms that LeoUTF8 can be implemented as a small, explicit UTF-8 boundary brick for Leopard-native tools.

The current architecture is:

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CLI probe
```

This supports the project doctrine:

```text
Bytes remain bytes.
UTF-8 is explicit.
NSString and CFString remain the native Leopard text model.
```

## Status

Accepted as first technical proof for LeoUTF8Core V0.  

