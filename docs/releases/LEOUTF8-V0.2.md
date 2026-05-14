# LeoUTF8 V0.2 — CoreFoundation Bridge Proof

## Status

LeoUTF8 V0.2 extends the V0.1 Leopard/PPC proof brick with a CoreFoundation bridge layer.

It has been built and tested on real Mac OS X 10.5.8 PowerPC hardware.

## Included Layers

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CoreFoundation
  -> LeoUTF8Foundation
  -> dist consumers
````

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
    

### LeoUTF8CoreFoundation

- UTF-8 bytes to retained `CFStringRef`
    
- `CFStringRef` to retained UTF-8 `CFDataRef`
    
- invalid UTF-8 rejection before crossing into CoreFoundation
    
- clean output state on invalid UTF-8 input
    
- CoreFoundation Create/Copy ownership rules
    

### LeoUTF8Foundation

- UTF-8 bytes to retained `NSString`
    
- `NSString` to retained UTF-8 `NSData`
    
- invalid UTF-8 rejection before crossing into Foundation
    
- manual retain/release ownership suitable for Mac OS X 10.5 Leopard
    

## Build Targets

```sh
make
make check
make dist
make distcheck
make install DESTDIR=... PREFIX=...
```

## Confirmed Checks

`make check` confirms:

- LeoUTF8Core probe
    
- LeoUTF8CoreFoundation probe
    
- LeoUTF8Foundation probe
    

`make distcheck` confirms external consumers for:

- `libLeoUTF8Core.a`
    
- `libLeoUTF8CoreFoundation.a`
    
- `libLeoUTF8Foundation.a`
    

## Distribution Layout

`make dist` creates:

```text
dist/LeoUTF8/
├── include/
│   ├── LeoUTF8.h
│   ├── LeoUTF8CoreFoundation.h
│   └── LeoUTF8Foundation.h
└── lib/
    ├── libLeoUTF8Core.a
    ├── libLeoUTF8CoreFoundation.a
    └── libLeoUTF8Foundation.a
```

## Proven Consumer Use

External consumers can use LeoUTF8 through only:

```text
dist/LeoUTF8/include
dist/LeoUTF8/lib
```

Confirmed consumers:

- C consumer using `libLeoUTF8Core.a`
    
- C/CoreFoundation consumer using `libLeoUTF8CoreFoundation.a` + `libLeoUTF8Core.a`
    
- Objective-C/Foundation consumer using `libLeoUTF8Foundation.a` + `libLeoUTF8Core.a`
    

## Link Order

### Core

```text
object files
libLeoUTF8Core.a
```

### CoreFoundation

```text
object files
libLeoUTF8CoreFoundation.a
libLeoUTF8Core.a
-framework CoreFoundation
```

### Foundation

```text
object files
libLeoUTF8Foundation.a
libLeoUTF8Core.a
-framework Foundation
```

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
    
- CoreFoundation bridge
    
- Foundation bridge
    

## Explicit Non-Goals For V0.2

LeoUTF8 V0.2 does not provide:

- ICU replacement
    
- NSString replacement
    
- CFString replacement
    
- automatic normalization
    
- locale-specific casing
    
- Unicode collation
    
- regex
    
- grapheme-cluster segmentation
    
- HFS+ filename policy
    
- framework packaging
    
- dynamic library packaging
    
- LeooRexx integration
    

## Doctrine

```text
Bytes remain bytes.
UTF-8 is explicit.
Byte-length API is the core.
C-string API is convenience.
CFStringRef is native Leopard CoreFoundation text.
CFDataRef is explicit UTF-8 transport.
NSString is native Leopard Foundation text.
NSData is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.
```

## Next Possible Steps

- add HFS+ filename normalization research
    
- add CoreFoundation/Foundation documentation examples
    
- package as a Leopard-friendly framework
    
- add release archive packaging
    
- integrate with LeooRexx as an optional UTF-8 capability  
    

