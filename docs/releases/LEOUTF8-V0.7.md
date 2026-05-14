# LeoUTF8 V0.7 — Integration Brick Hardening Release

## Status

LeoUTF8 V0.7 is an integration-brick hardening release.

It does not introduce a new Unicode feature layer.

It hardens LeoUTF8 as a standalone Leopard/PPC brick that can be built, tested,
staged, consumed, documented, and locally packaged.

## Confirmed Platform

- Mac OS X 10.5.8 Leopard
- PowerPC
- native /usr/bin/gcc
- static libraries
- CoreFoundation
- Foundation
- HFS+ research probes

## Version Identity

LeoUTF8 V0.7 introduces explicit brick version metadata.

Current values:

~~~text
Engine version: 2.11.3
Brick version:  0.7.0
Release name:   Integration Brick Hardening
Target:         Mac OS X 10.5.8 Leopard PowerPC
~~~

Public version header:

~~~text
include/LeoUTF8Version.h
~~~

## Public Distribution Surface

The staged distribution contains:

~~~text
dist/LeoUTF8/
  include/
    LeoUTF8.h
    LeoUTF8Version.h
    LeoUTF8CoreFoundation.h
    LeoUTF8Foundation.h
  lib/
    libLeoUTF8Core.a
    libLeoUTF8CoreFoundation.a
    libLeoUTF8Foundation.a
~~~

## Release Archive

V0.7 adds local release archive creation:

~~~sh
make release
~~~

Generated archive:

~~~text
release/LeoUTF8-0.7.0-leopard-ppc.tar.gz
~~~

Archive contents include:

~~~text
README.md
include/
lib/
docs/
Examples/
vendor-licenses/
~~~

## Examples

V0.7 adds external consumer examples:

~~~sh
make examples
~~~

Examples cover:

- Core C usage
- CoreFoundation usage
- Foundation usage

They build against the staged `dist/LeoUTF8` output, not private source paths.

## Confirmed Gate Run

The V0.7 final gate run completed successfully on Leopard/PPC:

~~~text
make
make check
make distcheck
make examples
make hfscheck
make hfsboundarycheck
make hfscasecheck
make hfscomparecheck
make hfsurlcheck
make release
~~~

## HFS+ Research Status

HFS+ research remains documented but is not public API.

Still locked:

- final HFS+ filename comparison API
- automatic filename normalization
- Finder policy
- CFURL policy
- NSURL policy
- APFS / SMB / AFP behavior

## Explicit Non-Goals For V0.7

LeoUTF8 V0.7 does not provide:

- dynamic library packaging
- framework packaging
- LeooRexx interpreter-core integration
- Rexx string semantics
- public LeoUTF8HFS API
- HFS+ comparison implementation
- hidden global normalization

## Doctrine

~~~text
Bytes remain bytes.
UTF-8 is explicit.
Engine version and brick version are separate.
Foundation and CoreFoundation are native Leopard text layers.
LeoUTF8Core guards the byte boundary.
HFS+ behavior stays HFS+-scoped.
Examples show usage.
Tests prove behavior.
Release archives package the brick.
