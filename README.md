# LeoUTF8

LeoUTF8 is a Leopard-native UTF-8 boundary brick for Mac OS X 10.5.8 PowerPC.

It provides a small, explicit bridge between UTF-8 byte buffers, C APIs,
CoreFoundation objects, Foundation objects, and Leopard-native applications.

LeoUTF8 does not replace `NSString`, `CFString`, Foundation, CoreFoundation, or
the Leopard text system. It complements them by making UTF-8 validation,
normalization, case folding, and byte/string boundary handling available as a
small reusable system component.

## Status

LeoUTF8 is currently at a working Leopard/PPC proof stage.

Confirmed on real Mac OS X 10.5.8 PowerPC hardware:

- vendored `utf8proc` builds with the native Leopard GCC toolchain
- `LeoUTF8Core` wraps `utf8proc`
- byte-length UTF-8 API works with embedded NUL bytes
- C-string convenience API works
- invalid UTF-8 is rejected
- NFC normalization works
- NFD normalization works
- Unicode case folding works
- `CFStringRef` creation from UTF-8 bytes works
- UTF-8 `CFDataRef` creation from `CFStringRef` works
- `NSString` creation from UTF-8 bytes works
- UTF-8 `NSData` creation from `NSString` works
- internal probes build and run on Leopard/PPC
- external consumer probes build and run against `dist/LeoUTF8`
- external examples build and run against `dist/LeoUTF8`
- public LeoUTF8 brick version metadata is available
- engine version and brick version are reported separately

## Architecture

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CoreFoundation
  -> LeoUTF8Foundation
  -> CLI probes / dist consumers / future Leopard-Crew tools
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

### LeoUTF8CoreFoundation

`LeoUTF8CoreFoundation` is the CoreFoundation bridge layer.

It provides:

- UTF-8 byte buffer -> retained `CFStringRef`
    
- `CFStringRef` -> retained UTF-8 `CFDataRef`
    
- validation through `LeoUTF8Core` before malformed UTF-8 crosses into CoreFoundation
    

CoreFoundation doctrine:

```text
CFStringRef is native Leopard CoreFoundation text.
CFDataRef is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.
```

### LeoUTF8Foundation

`LeoUTF8Foundation` is the Foundation bridge layer.

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

## Version Identity

LeoUTF8 exposes two separate version concepts:

~~~text
LeoUTF8Version()
  -> UTF-8 engine / utf8proc-adjacent version

LeoUTF8BrickVersionString()
  -> LeoUTF8 project brick version
~~~

Current V0.7 values:

~~~text
Engine version: 2.11.3
Brick version:  0.7.0
Release name:   Integration Brick Hardening
Target:         Mac OS X 10.5.8 Leopard PowerPC
~~~

See:

~~~text
docs/api/LeoUTF8Version.md
~~~

## What LeoUTF8 Is Not

LeoUTF8 is not:

- an ICU clone
    
- a replacement for `NSString`
    
- a replacement for `CFString`
    
- a replacement for Foundation
    
- a replacement for CoreFoundation
    
- a locale policy engine
    
- a regex engine
    
- a Unicode collation framework
    
- a hidden global string semantics layer
    

UTF-8 transformations are explicit. Nothing is normalized silently.

## Why LeoUTF8 Exists

Leopard already has strong native Unicode facilities through CoreFoundation and  
Foundation.

The missing piece is a small, reusable boundary brick for Leopard-native tools  
that need to move deliberately between:

- byte-oriented C/POSIX data
    
- UTF-8 transport data
    
- Rexx strings
    
- CLI tools
    
- file/network buffers
    
- `CFStringRef`
    
- `CFDataRef`
    
- `NSString`
    
- `NSData`
    
- future Leopard-Crew application layers
    

LeoUTF8 defines that boundary explicitly.

## Build And Test On Leopard/PPC

Run on Mac OS X 10.5.8 PowerPC:

```sh
cd /Users/admin/Desktop/Projekte/LeoUTF8

make clean
make
make check
make dist
make distcheck
```

Successful output includes:

```text
LeoUTF8 probe passed.
LeoUTF8CoreFoundation probe passed.
LeoUTF8Foundation probe passed.
consumer core probe passed
consumer corefoundation probe passed
consumer foundation probe passed
LeoUTF8 distcheck completed successfully.
```

## Examples

LeoUTF8 includes external consumer examples:

~~~sh
make examples
~~~

The examples build against the staged `dist/LeoUTF8` output, not private source paths.

They demonstrate:

- Core C usage
- CoreFoundation usage
- Foundation usage

See:

~~~text
docs/examples/LeoUTF8Examples.md
~~~

## Distribution Layout

`make dist` creates:

```text
dist/LeoUTF8/
├── include/
│   ├── LeoUTF8.h
│   ├── LeoUTF8Version.h
│   ├── LeoUTF8CoreFoundation.h
│   └── LeoUTF8Foundation.h
└── lib/
    ├── libLeoUTF8Core.a
    ├── libLeoUTF8CoreFoundation.a
    └── libLeoUTF8Foundation.a
```

## Install Staging

LeoUTF8 supports staged install testing:

```sh
make install DESTDIR="$PWD/build-work/install-test" PREFIX=/LeoUTF8
```

This does not write into the live system when `DESTDIR` is used.

## HFS+ Filename Boundary Research

LeoUTF8 V0.3 research currently investigates Leopard/HFS+ filename boundaries.

Two research probes are available:

~~~sh
make hfscheck
make hfsboundarycheck
~~~

### `make hfscheck`

Checks how an HFS+ volume handles canonically equivalent NFC and NFD UTF-8
filenames through POSIX file creation and `readdir()`.

Confirmed on Leopard/PPC:

~~~text
NFD O_EXCL matched existing file: yes
readdir returned NFC bytes: no
readdir returned LeoUTF8 NFD bytes: yes
~~~

### `make hfsboundarycheck`

Checks the same HFS+ filename boundary through:

- POSIX `readdir()`
- `CFStringGetFileSystemRepresentation`
- `NSString fileSystemRepresentation`

Confirmed on Leopard/PPC:

~~~text
readdir bytes match LeoUTF8 NFD: yes
CFString filesystem bytes match LeoUTF8 NFD: yes
NSString filesystem bytes match LeoUTF8 NFD: yes
~~~

### `make hfscasecheck`

Checks observed HFS+ case-comparison behavior for selected filename pairs.

Current tested pairs:

~~~text
case.txt     vs CASE.txt
ä.txt        vs Ä.txt
café.txt     vs CAFÉ.TXT
straße.txt   vs STRASSE.TXT
~~~

Confirmed on Leopard/PPC:

~~~text
ASCII case.txt vs CASE.txt        -> same file
ä.txt vs Ä.txt                    -> same file
café.txt vs CAFÉ.TXT              -> same file
straße.txt vs STRASSE.TXT         -> separate files
~~~

Important finding:

~~~text
Unicode case folding is not equivalent to HFS+ filename comparison.
~~~

In particular, LeoUTF8 case folding maps `straße.txt` and `STRASSE.TXT` to the
same folded form, but observed HFS+ behavior treats them as separate filenames.

### `make hfscomparecheck`

Checks Leopard string-comparison behavior for the same filename pairs used by
`make hfscasecheck`.

The probe compares the pairs through:

- `CFStringCompare`
- `NSString compare:options:`

Current tested pairs:

~~~text
case.txt     vs CASE.txt
ä.txt        vs Ä.txt
café.txt     vs CAFÉ.TXT
straße.txt   vs STRASSE.TXT
~~~

Confirmed on Leopard/PPC:

~~~text
CFString case-insensitive comparison treats all tested pairs as equal.
NSString case-insensitive comparison treats all tested pairs as equal.
~~~

Important finding:

~~~text
CFString/NSString comparison is not HFS+ filename comparison.
~~~

In particular, HFS+ treats `straße.txt` and `STRASSE.TXT` as separate files,
while both CFString and NSString case-insensitive comparison treat them as equal.

Therefore a future `LeoUTF8HFS` layer must not blindly use generic
CFString/NSString comparison as HFS+ filename comparison policy.

### `make hfsurlcheck`

Checks the HFS+ filesystem boundary through CFURL and NSURL.

The probe creates a filename with NFC UTF-8 input and observes the resulting
filesystem representation through:

- POSIX `readdir()`
- `CFURLGetFileSystemRepresentation`
- `NSURL path` followed by `NSString fileSystemRepresentation`

Confirmed on Leopard/PPC:

~~~text
readdir filename bytes match LeoUTF8 NFD: yes
CFURL filesystem filename bytes match LeoUTF8 NFD: yes
NSURL filesystem filename bytes match LeoUTF8 NFD: yes
~~~

Important Leopard-specific finding:

~~~text
NSURL itself does not provide -fileSystemRepresentation on Leopard.
Use NSURL -> path -> NSString fileSystemRepresentation.
~~~

Therefore a future `LeoUTF8HFS` or URL helper must treat CFURL, NSURL, NSString,
and filesystem bytes as separate boundary layers.

These probes do not define a final HFS+ filename API yet.

They establish the measured boundary behavior that a future explicit
`LeoUTF8HFS` layer may use.

## Repository Layout

```text
Examples/
  Core/
    leoutf8_core_example.c
  CoreFoundation/
    leoutf8_corefoundation_example.c
  Foundation/
    leoutf8_foundation_example.m

docs/
  api/
    LeoUTF8Core.md
    LeoUTF8Version.md
    LeoUTF8CoreFoundation.md
    LeoUTF8Foundation.md
    consuming-leoutf8.md
  design/
    LeoUTF8HFS.md
  examples/
    LeoUTF8Examples.md
  findings/
    LEOUTF8-FIND-0001-utf8proc-leopard-ppc.md
  releases/
    LEOUTF8-V0.1.md
  architecture.md
  leopard-text-model.md
  scope-lock.md
  utf8proc-adoption-policy.md

Sources/
  LeoUTF8Core/
    LeoUTF8.h
    LeoUTF8.c
  LeoUTF8CoreFoundation/
    LeoUTF8CoreFoundation.h
    LeoUTF8CoreFoundation.c
  LeoUTF8Foundation/
    LeoUTF8Foundation.h
    LeoUTF8Foundation.m
  LeoUTF8CLI/
    leoutf8_probe.c
    leoutf8_corefoundation_probe.c
    leoutf8_foundation_probe.m

Tests/
  consumer_core_probe.c
  consumer_corefoundation_probe.c
  consumer_foundation_probe.m

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

