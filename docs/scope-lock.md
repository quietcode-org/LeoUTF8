# LeoUTF8 Scope Lock

This document defines the current scope of LeoUTF8 after the V0.6 research cut.

## Project Identity

LeoUTF8 is a Leopard-native UTF-8 boundary brick.

It is not a general Unicode framework.

It is not an ICU clone.

It is not a replacement for CoreFoundation or Foundation.

## In Scope

### Core UTF-8 Boundary

- UTF-8 validation
- codepoint counting
- NFC normalization
- NFD normalization
- Unicode case folding
- byte-length APIs
- C-string convenience APIs
- explicit ownership rules

### CoreFoundation Bridge

- UTF-8 bytes -> `CFStringRef`
- `CFStringRef` -> UTF-8 `CFDataRef`
- malformed UTF-8 rejection before CoreFoundation boundary

### Foundation Bridge

- UTF-8 bytes -> `NSString`
- `NSString` -> UTF-8 `NSData`
- malformed UTF-8 rejection before Foundation boundary

### Build And Distribution

- native Leopard/PPC build
- static libraries
- `make`
- `make check`
- `make dist`
- `make distcheck`
- external consumer checks

### Research Scope

HFS+ and Leopard text boundary research is in scope when it is observational and
documented.

Current research targets:

- `make hfscheck`
- `make hfsboundarycheck`
- `make hfscasecheck`
- `make hfscomparecheck`
- `make hfsurlcheck`

## Out Of Scope

LeoUTF8 currently does not provide:

- ICU replacement
- locale policy engine
- regex
- collation framework
- grapheme-cluster engine
- automatic global normalization
- automatic filename normalization
- final HFS+ filename comparison API
- Finder policy
- CFURL policy
- NSURL policy
- APFS behavior
- SMB behavior
- AFP behavior
- dynamic library packaging
- framework packaging

## Explicitly Forbidden Shortcuts

Do not implement HFS+ filename comparison as plain:

~~~text
NFD + Unicode casefold
~~~

Do not implement HFS+ filename comparison as plain:

~~~text
CFStringCompare(..., kCFCompareCaseInsensitive)
~~~

Do not implement HFS+ filename comparison as plain:

~~~text
[NSString compare:options:NSCaseInsensitiveSearch]
~~~

The `straße.txt` vs `STRASSE.TXT` findings prove that these shortcuts would be
wrong for observed Leopard/HFS+ behavior.

## Future Candidate: LeoUTF8HFS

A future `LeoUTF8HFS` layer is justified, but not yet implemented.

It should be explicit and scoped.

Possible responsibilities:

- HFS+ filename validation
- HFS+ observed filesystem representation helpers
- HFS+ comparison research wrapper
- filesystem/display boundary helpers
- URL/path boundary helpers

It must not leak HFS+ policy into general LeoUTF8Core behavior.

## Current Rule

General Unicode operations stay general.

HFS+ behavior stays HFS+-scoped.

URL behavior stays URL-scoped.

No hidden global string semantics.
