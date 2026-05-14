# LeoUTF8 V0.1 — Leopard/PPC Proof Brick

## Status

LeoUTF8 V0.1 is the first proven Leopard/PPC UTF-8 boundary brick.

It has been built and tested on real Mac OS X 10.5.8 PowerPC hardware.

## What Is Included

### LeoUTF8Core

- byte-length UTF-8 validation
- byte-length Unicode codepoint counting
- NFC normalization
- NFD normalization
- Unicode case folding
- embedded NUL byte support through byte-length APIs
- null-terminated C-string convenience APIs
- invalid UTF-8 rejection
- explicit output ownership through `LeoUTF8Free`

### LeoUTF8Foundation

- UTF-8 bytes to retained `NSString`
- `NSString` to retained UTF-8 `NSData`
- invalid UTF-8 rejection before crossing into Foundation
- manual retain/release ownership suitable for Mac OS X 10.5 Leopard

### Build Targets

```sh
make
make check
make dist
make distcheck
make install DESTDIR=... PREFIX=...
````

## Distribution Layout

`make dist` creates:

```text
dist/LeoUTF8/
├── include/
│   ├── LeoUTF8.h
│   └── LeoUTF8Foundation.h
└── lib/
    ├── libLeoUTF8Core.a
    └── libLeoUTF8Foundation.a
```

## Proven Consumer Use

`make distcheck` proves that external consumers can use LeoUTF8 through only:

```text
dist/LeoUTF8/include
dist/LeoUTF8/lib
```

Confirmed consumers:

- C consumer using `libLeoUTF8Core.a`
    
- Objective-C/Foundation consumer using `libLeoUTF8Foundation.a` + `libLeoUTF8Core.a`
    

## Vendor Policy

- `vendor/utf8proc` is unchanged upstream reference material.
    
- LeoUTF8 project code lives outside `vendor/`.
    
- Leopard-specific build rules live in the project Makefile and scripts.
    
- No MacPorts dependency is required.
    

## Confirmed Platform

- Mac OS X 10.5.8 Leopard
    
- PowerPC
    
- native `/usr/bin/gcc`
    
- static libraries
    
- Foundation bridge
    

## Explicit Non-Goals For V0.1

LeoUTF8 V0.1 does not provide:

- ICU replacement
    
- NSString replacement
    
- CFString replacement
    
- automatic normalization
    
- locale-specific casing
    
- Unicode collation
    
- regex
    
- grapheme-cluster segmentation
    
- framework packaging
    
- dynamic library packaging
    
- LeooRexx integration
    

## Architecture

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8Foundation
  -> dist/LeoUTF8
  -> external consumers
```

## Doctrine

```text
Bytes remain bytes.
UTF-8 is explicit.
Byte-length API is the core.
C-string API is convenience.
NSString is native Leopard text.
NSData is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.
```

## Next Possible Steps

- package as a Leopard-friendly framework
    
- add CoreFoundation bridge
    
- add real install location policy
    
- integrate with LeooRexx as an optional UTF-8 capability
    
- add HFS+ filename normalization research
    
- add release archive packaging  
