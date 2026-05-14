# LeoUTF8 V0.3 — HFS+ Filename Boundary Research Cut

## Status

LeoUTF8 V0.3 is a research cut.

It does not introduce a final HFS+ filename API.

It documents measured Leopard/HFS+ filename boundary behavior on real
Mac OS X 10.5.8 PowerPC hardware.

## Included From Earlier Cuts

LeoUTF8 already includes:

- LeoUTF8Core
- LeoUTF8CoreFoundation
- LeoUTF8Foundation
- static libraries
- make
- make check
- make dist
- make distcheck
- external consumer checks

The active V0.3 work adds HFS+ filename boundary research.

## New Research Targets

V0.3 adds:

~~~sh
make hfscheck
make hfsboundarycheck
~~~

## HFS+ POSIX Filename Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_filename_probe.c
~~~

Run with:

~~~sh
make hfscheck
~~~

This probe checks how an HFS+ volume handles canonically equivalent NFC and NFD
UTF-8 filenames through POSIX file creation and readdir.

Confirmed on Leopard/PPC:

~~~text
NFD O_EXCL matched existing file: yes
readdir returned NFC bytes: no
readdir returned LeoUTF8 NFD bytes: yes
~~~

Finding:

~~~text
docs/findings/LEOUTF8-FIND-0002-hfsplus-filename-normalization.md
~~~

## HFS+ CFString / NSString Boundary Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_cfstring_boundary_probe.m
~~~

Run with:

~~~sh
make hfsboundarycheck
~~~

This probe checks the same HFS+ filename boundary through:

- POSIX readdir
- CFStringGetFileSystemRepresentation
- NSString fileSystemRepresentation

Confirmed on Leopard/PPC:

~~~text
readdir bytes match LeoUTF8 NFD: yes
CFString filesystem bytes match LeoUTF8 NFD: yes
NSString filesystem bytes match LeoUTF8 NFD: yes
~~~

Finding:

~~~text
docs/findings/LEOUTF8-FIND-0003-hfsplus-cfstring-boundary.md
~~~

## Interpretation

On the tested Leopard/PPC HFS+ volume:

- a filename created with NFC UTF-8 input is observed through readdir as decomposed UTF-8 bytes
- canonically equivalent NFD input hits the same existing file with O_EXCL
- CFString filesystem representation returns the same decomposed byte form
- NSString fileSystemRepresentation returns the same decomposed byte form
- the observed decomposed byte form matches LeoUTF8NormalizeNFDBytes

## Architectural Consequence

HFS+ filename handling must be treated as a distinct boundary.

General LeoUTF8 string operations should remain separate from HFS+-specific
filename behavior.

Future HFS+-specific helpers are justified, but must be explicit and scoped.

Possible future layer:

~~~text
LeoUTF8HFS
  -> explicit HFS+ filename normalization
  -> explicit HFS+ filename comparison
  -> explicit filesystem/display boundary helpers
~~~

## Policy

Do not silently normalize all LeoUTF8 strings.

Only HFS+-scoped APIs should apply HFS+-specific filename normalization.

Recommended model:

~~~text
external/user UTF-8 input
  -> LeoUTF8Core validation
  -> explicit HFS+ filename normalization when comparing filesystem names
  -> CFString/NSString for display and native Leopard text handling
~~~

## Explicit Non-Goals For V0.3

LeoUTF8 V0.3 does not provide:

- final HFS+ filename API
- automatic filename normalization
- case-insensitive HFS+ comparison policy
- Finder display policy
- CFURL policy
- APFS behavior
- UFS behavior
- SMB behavior
- AFP behavior
- dynamic library packaging
- framework packaging

## Confirmed Platform

- Mac OS X 10.5.8 Leopard
- PowerPC
- HFS+
- native /usr/bin/gcc
- POSIX file APIs
- CoreFoundation
- Foundation

## Doctrine

~~~text
Bytes remain bytes.
UTF-8 is explicit.
HFS+ filename behavior is a boundary.
Filesystem comparison form is not general string policy.
CFString/NSString are display/native text bridges, not byte-stability guarantees.
LeoUTF8Core guards the boundary.
~~~

## Next Possible Steps

- research HFS+ case-insensitive filename comparison
- research CFURL behavior
- research Finder display behavior
- research HFS+ decomposition exceptions
- design explicit LeoUTF8HFS API
- keep HFS+ policy separate from general LeoUTF8Core
