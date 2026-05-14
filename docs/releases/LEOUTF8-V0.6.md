# LeoUTF8 V0.6 — CFURL / NSURL Filesystem Boundary Research Cut

## Status

LeoUTF8 V0.6 is a research cut.

It does not introduce a final URL or HFS+ filename API.

It documents measured CFURL / NSURL filesystem boundary behavior on real
Mac OS X 10.5.8 PowerPC hardware with HFS+.

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
- HFS+ normalization boundary research
- HFS+ CFString/NSString filesystem boundary research
- HFS+ case-comparison research
- CFString/NSString comparison boundary research

## New Research Target

V0.6 adds:

~~~sh
make hfsurlcheck
~~~

## CFURL / NSURL Filesystem Boundary Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_url_boundary_probe.m
~~~

Run with:

~~~sh
make hfsurlcheck
~~~

This probe checks the HFS+ filesystem boundary through:

- POSIX readdir
- CFURLGetFileSystemRepresentation
- NSURL path followed by NSString fileSystemRepresentation

## Test Input

Display form:

~~~text
café.txt
~~~

NFC UTF-8 input filename bytes:

~~~text
63 61 66 C3 A9 2E 74 78 74
~~~

LeoUTF8 expected NFD filename bytes:

~~~text
63 61 66 65 CC 81 2E 74 78 74
~~~

## Observed Results

Confirmed on Leopard/PPC:

~~~text
readdir filename bytes match LeoUTF8 NFD: yes
CFURL filesystem filename bytes match LeoUTF8 NFD: yes
NSURL filesystem filename bytes match LeoUTF8 NFD: yes
~~~

## Leopard NSURL Finding

Direct `-[NSURL fileSystemRepresentation]` is not available on Leopard.

The Leopard-safe route is:

~~~text
NSURL
  -> path
  -> NSString fileSystemRepresentation
~~~

This corrected route preserves the observed HFS+ decomposed filename boundary.

## Finding Document

~~~text
docs/findings/LEOUTF8-FIND-0006-cfurl-nsurl-filesystem-boundary.md
~~~

## Architectural Consequence

URL handling must be treated as another explicit filesystem boundary.

A future `LeoUTF8HFS` or URL helper must distinguish between:

~~~text
CFURL filesystem representation
NSURL path
NSString fileSystemRepresentation
HFS+ filename byte representation
display string representation
~~~

## Policy

Do not call:

~~~text
[url fileSystemRepresentation]
~~~

on Leopard `NSURL`.

Use:

~~~text
[[url path] fileSystemRepresentation]
~~~

when filesystem bytes are required from an NSURL on Leopard.

Do not assume URL APIs hide HFS+ decomposition behavior.

## Explicit Non-Goals For V0.6

LeoUTF8 V0.6 does not provide:

- final URL API
- final HFS+ filename API
- CFURL policy
- NSURL policy
- Finder policy
- alias handling
- symlink handling
- percent-escaping policy
- APFS behavior
- UFS behavior
- SMB behavior
- AFP behavior

## Confirmed Platform

- Mac OS X 10.5.8 Leopard
- PowerPC
- HFS+
- native /usr/bin/gcc
- POSIX file APIs
- CoreFoundation
- Foundation
- CFURL
- NSURL

## Doctrine

~~~text
Bytes remain bytes.
UTF-8 is explicit.
HFS+ filename behavior is a boundary.
URL behavior is a filesystem boundary.
CFURL and NSURL do not erase HFS+ filename normalization behavior.
NSURL on Leopard reaches filesystem bytes through path + NSString.
LeoUTF8Core guards the boundary.
~~~

## Next Possible Steps

- research CFURLCopyFileSystemPath
- research NSURL absoluteString vs path
- research percent escaping
- research aliases and symlinks
- research Finder display behavior
- design explicit LeoUTF8HFS / URL helper API
