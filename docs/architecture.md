# LeoUTF8 Architecture

LeoUTF8 is a Leopard-native UTF-8 boundary brick for Mac OS X 10.5.8 PowerPC.

It does not replace Leopard text APIs.

It makes UTF-8 boundary handling explicit and reusable for Leopard-native C,
CoreFoundation, Foundation, and future Leopard-Crew projects.

## Current Architecture

~~~text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CoreFoundation
  -> LeoUTF8Foundation
  -> dist consumers
~~~

## Layer Responsibilities

### vendor/utf8proc

`vendor/utf8proc` contains unchanged upstream reference material.

Rules:

- Do not edit vendored sources directly.
- Leopard-specific policy belongs outside `vendor/`.
- Build adaptation belongs in Makefile/scripts.
- LeoUTF8 owns the boundary policy, not utf8proc.

### LeoUTF8Core

`LeoUTF8Core` is the C-level UTF-8 boundary layer.

It provides:

- UTF-8 validation
- codepoint counting
- NFC normalization
- NFD normalization
- Unicode case folding
- byte-length APIs
- C-string convenience APIs
- explicit allocation ownership through `LeoUTF8Free`

Doctrine:

~~~text
Byte-length API is the core.
C-string API is convenience.
Bytes remain bytes.
UTF-8 is explicit.
~~~

### LeoUTF8CoreFoundation

`LeoUTF8CoreFoundation` bridges LeoUTF8Core to CoreFoundation.

It provides:

- UTF-8 bytes -> retained `CFStringRef`
- `CFStringRef` -> retained UTF-8 `CFDataRef`

Doctrine:

~~~text
CFStringRef is native Leopard CoreFoundation text.
CFDataRef is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.
~~~

### LeoUTF8Foundation

`LeoUTF8Foundation` bridges LeoUTF8Core to Foundation.

It provides:

- UTF-8 bytes -> retained `NSString`
- `NSString` -> retained UTF-8 `NSData`

Doctrine:

~~~text
NSString is native Leopard Foundation text.
NSData is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.
~~~

## Build Model

LeoUTF8 currently builds static libraries:

~~~text
libLeoUTF8Core.a
libLeoUTF8CoreFoundation.a
libLeoUTF8Foundation.a
~~~

`libLeoUTF8Core.a` contains the LeoUTF8Core object and the vendored utf8proc
object code.

Consumers do not link against `vendor/utf8proc` directly.

## Distribution Layout

`make dist` creates:

~~~text
dist/LeoUTF8/
├── include/
│   ├── LeoUTF8.h
│   ├── LeoUTF8CoreFoundation.h
│   └── LeoUTF8Foundation.h
└── lib/
    ├── libLeoUTF8Core.a
    ├── libLeoUTF8CoreFoundation.a
    └── libLeoUTF8Foundation.a
~~~

## Verification Targets

Current verification targets:

~~~sh
make
make check
make dist
make distcheck
make hfscheck
make hfsboundarycheck
make hfscasecheck
make hfscomparecheck
make hfsurlcheck
~~~

## External Consumer Proof

`make distcheck` confirms that external consumers can use LeoUTF8 through only:

~~~text
dist/LeoUTF8/include
dist/LeoUTF8/lib
~~~

Confirmed consumers:

- C consumer using `libLeoUTF8Core.a`
- CoreFoundation consumer using `libLeoUTF8CoreFoundation.a`
- Foundation consumer using `libLeoUTF8Foundation.a`

## HFS+ Research Findings

LeoUTF8 V0.3 through V0.6 established several important Leopard/HFS+ boundary
facts.

### HFS+ Normalization Boundary

A filename created with NFC UTF-8 input is observed through `readdir()` as
decomposed UTF-8 bytes matching `LeoUTF8NormalizeNFDBytes()`.

### CFString / NSString Filesystem Boundary

`CFStringGetFileSystemRepresentation` and `NSString fileSystemRepresentation`
also return the HFS+ decomposed filesystem byte form.

### HFS+ Case Comparison Boundary

HFS+ filename comparison is not generic Unicode case folding.

Observed:

~~~text
case.txt     vs CASE.txt      -> same file
ä.txt        vs Ä.txt         -> same file
café.txt     vs CAFÉ.TXT      -> same file
straße.txt   vs STRASSE.TXT   -> separate files
~~~

### CFString / NSString Comparison Boundary

Case-insensitive `CFStringCompare` and `NSString compare:options:` treat
`straße.txt` and `STRASSE.TXT` as equal, while HFS+ treats them as separate
files.

Therefore:

~~~text
Unicode casefold      != HFS+ filename comparison
CFString comparison   != HFS+ filename comparison
NSString comparison   != HFS+ filename comparison
~~~

### CFURL / NSURL Boundary

`CFURLGetFileSystemRepresentation` preserves the observed HFS+ decomposed
filename boundary.

On Leopard, `NSURL` does not provide `-fileSystemRepresentation`.

Use:

~~~text
NSURL
  -> path
  -> NSString fileSystemRepresentation
~~~

## Architectural Rule

LeoUTF8 must keep these concepts separate:

~~~text
UTF-8 validation
Unicode normalization
Unicode case folding
CFString comparison
NSString comparison
HFS+ filename comparison
filesystem byte representation
display representation
URL representation
~~~

No single generic Unicode operation should be treated as Leopard/HFS+ filename
policy.

## Release Milestones

~~~text
v0.1-leopard-ppc-proof
  Core + Foundation proof

v0.2-corefoundation-bridge
  Core + CoreFoundation + Foundation

v0.3-hfsplus-boundary-research
  HFS+ POSIX / CFString / NSString boundary research

v0.4-hfsplus-case-research
  HFS+ case comparison research

v0.5-string-compare-boundary
  CFString/NSString comparison vs HFS+ filename comparison

v0.6-url-boundary-research
  CFURL / NSURL filesystem boundary research
~~~

## Current Direction

LeoUTF8 is now ready for a design note for a future explicit `LeoUTF8HFS`
layer.

That layer should be scoped, measured, and conservative.

It must not be a generic string policy engine.
