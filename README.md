# LeoUTF8

LeoUTF8 is a Leopard-native UTF-8 boundary brick for Mac OS X 10.5.8 PowerPC.

It provides a small, explicit bridge between UTF-8 byte buffers, C APIs,
Foundation objects, and Leopard-native applications.

LeoUTF8 does not replace `NSString`, `CFString`, Foundation, or the Leopard text
system. It complements them by making UTF-8 validation, normalization,
case folding, and byte/string boundary handling available as a reusable system
component.

## Status

LeoUTF8 is currently at an early but working V0 proof stage.

Confirmed on real Mac OS X 10.5.8 PowerPC hardware:

- vendored `utf8proc` builds with the native Leopard GCC toolchain
- `LeoUTF8Core` wraps `utf8proc`
- byte-length UTF-8 API works with embedded NUL bytes
- C-string convenience API works
- invalid UTF-8 is rejected
- NFC normalization works
- NFD normalization works
- Unicode case folding works
- `NSString` creation from UTF-8 bytes works
- UTF-8 `NSData` creation from `NSString` works
- probes build and run on Leopard/PPC

## Architecture

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8Foundation
  -> CLI probes / future Leopard-Crew tools
````

### LeoUTF8Core

`LeoUTF8Core` is the C-level UTF-8 boundary layer.

It provides:

- UTF-8 validation
    
- Unicode codepoint counting
    
- NFC normalization
    
- NFD normalization
    
- Unicode case folding
    
- byte-length APIs
    
- null-terminated C-string convenience APIs
    
- explicit ownership rules
    

Core doctrine:

```text
Byte-length API is the core.
C-string API is convenience.
Bytes remain bytes.
UTF-8 is explicit.
```

### LeoUTF8Foundation

`LeoUTF8Foundation` is the Leopard Foundation bridge layer.

It provides:

- UTF-8 byte buffer -> retained `NSString`
    
- `NSString` -> retained UTF-8 `NSData`
    
- validation through `LeoUTF8Core` before malformed UTF-8 crosses into Foundation
    

Foundation doctrine:

```text
NSString is native Leopard text.
NSData is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.
```

## What LeoUTF8 Is Not

LeoUTF8 is not:

- an ICU clone
    
- a replacement for `NSString`
    
- a replacement for `CFString`
    
- a replacement for Foundation
    
- a locale policy engine
    
- a regex engine
    
- a Unicode collation framework
    
- a hidden global string semantics layer
    

UTF-8 transformations are explicit. Nothing is normalized silently.

## Why LeoUTF8 Exists

Leopard already has strong native Unicode facilities through Foundation and  
CoreFoundation.

The missing piece is a small, reusable boundary brick for Leopard-native tools  
that need to move deliberately between:

- byte-oriented C/POSIX data
    
- UTF-8 transport data
    
- Rexx strings
    
- CLI tools
    
- file/network buffers
    
- `NSString`
    
- `NSData`
    
- future Leopard-Crew application layers
    

LeoUTF8 defines that boundary explicitly.

## Build Probe On Leopard/PPC

Run on Mac OS X 10.5.8 PowerPC:

```sh
cd /Users/admin/Desktop/Projekte/LeoUTF8
./tools/build_probe_leopard_ppc.sh
```

The script:

1. copies vendored `utf8proc` into `build-work`
    
2. builds `libutf8proc.a`
    
3. builds `LeoUTF8Core`
    
4. runs the C probe
    
5. builds `LeoUTF8Foundation`
    
6. runs the Foundation probe
    

Successful output includes:

```text
LeoUTF8 probe passed.
LeoUTF8Foundation probe passed.
LeoUTF8 Leopard/PPC probes completed successfully.
```

## Repository Layout

```text
docs/
  api/
    LeoUTF8Core.md
    LeoUTF8Foundation.md
  findings/
    LEOUTF8-FIND-0001-utf8proc-leopard-ppc.md
  architecture.md
  leopard-text-model.md
  scope-lock.md
  utf8proc-adoption-policy.md

Sources/
  LeoUTF8Core/
    LeoUTF8.h
    LeoUTF8.c
  LeoUTF8Foundation/
    LeoUTF8Foundation.h
    LeoUTF8Foundation.m
  LeoUTF8CLI/
    leoutf8_probe.c
    leoutf8_foundation_probe.m

tools/
  build_probe_leopard_ppc.sh

vendor/
  utf8proc/
```

## Vendor Policy

`vendor/` contains unchanged upstream reference material.

Do not edit vendored sources directly.

Leopard-specific build rules, wrappers, compatibility code, and project logic  
belong outside `vendor/`.

## Design Rule

Use Leopard-native text facilities where they already exist.

Use `utf8proc` only for the narrowly defined UTF-8 operations Leopard does not  
expose as a small C-level capability.

No ICU clone. No framework sprawl. No hidden global string semantics.  

