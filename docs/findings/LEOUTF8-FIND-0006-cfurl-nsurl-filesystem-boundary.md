# LEOUTF8-FIND-0006: CFURL / NSURL filesystem boundary on Leopard/PPC

## Summary

On Mac OS X 10.5.8 PowerPC with HFS+, a filename created with NFC UTF-8 input
is observed as decomposed UTF-8 bytes through:

- POSIX `readdir()`
- `CFURLGetFileSystemRepresentation`
- `NSURL path` followed by `NSString fileSystemRepresentation`

The observed filename byte sequence matches `LeoUTF8NormalizeNFDBytes()`.

A direct `-[NSURL fileSystemRepresentation]` call is not available on Leopard
and raises an unrecognized selector exception. The Leopard-safe route is:

~~~text
NSURL
  -> path
  -> NSString fileSystemRepresentation
~~~

## Host

- System: Mac OS X 10.5.8 Leopard
- Architecture: PowerPC
- Filesystem reported by `statfs`: `hfs`
- Probe target: `build-work/hfsplus_url_boundary_probe`

## Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_url_boundary_probe.m
~~~

Run with:

~~~sh
make hfsurlcheck
~~~

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

## Observed POSIX Boundary

The file was created using the NFC spelling.

`readdir()` returned:

~~~text
readdir entry: café.txt
readdir filename bytes: 63 61 66 65 CC 81 2E 74 78 74
~~~

Summary result:

~~~text
readdir filename bytes match LeoUTF8 NFD: yes
~~~

## Observed CFURL Boundary

`CFURLCreateFromFileSystemRepresentation()` was used to create a `CFURLRef`
from the filesystem path.

`CFURLGetFileSystemRepresentation()` returned a full path whose final filename
component matched the LeoUTF8 NFD byte sequence:

~~~text
CFURL filesystem filename bytes match LeoUTF8 NFD: yes
~~~

## Observed NSURL Boundary

Direct `-[NSURL fileSystemRepresentation]` is not available on Leopard.

The initial probe produced:

~~~text
warning: 'NSURL' may not respond to '-fileSystemRepresentation'
*** -[NSURL fileSystemRepresentation]: unrecognized selector
~~~

The corrected Leopard-safe route is:

~~~text
NSURL fileURLWithPath:
  -> [url path]
  -> [path fileSystemRepresentation]
~~~

That corrected route returned a full path whose final filename component matched
the LeoUTF8 NFD byte sequence:

~~~text
NSURL filesystem filename bytes match LeoUTF8 NFD: yes
~~~

## Interpretation

On this Leopard/PPC HFS+ volume:

- POSIX `readdir()` observes the filename as decomposed UTF-8 bytes.
- CFURL filesystem representation preserves the observed HFS+ decomposed
  filename boundary.
- NSURL can participate in the boundary only through its `path` as an NSString.
- NSURL itself must not be assumed to expose `fileSystemRepresentation` on
  Leopard.

## Architectural Consequence

LeoUTF8 should treat URL handling as another explicit filesystem boundary.

A future HFS+/URL helper must distinguish between:

~~~text
CFURL filesystem representation
NSURL path
NSString fileSystemRepresentation
HFS+ filename byte representation
display string representation
~~~

## Policy Implication

Do not call:

~~~text
[url fileSystemRepresentation]
~~~

on Leopard `NSURL`.

Use:

~~~text
[[url path] fileSystemRepresentation]
~~~

when a filesystem byte representation is required from an NSURL on Leopard.

Do not assume URL APIs hide HFS+ decomposition behavior.

## Non-Goals

This finding does not define a final URL API.

It does not yet cover:

- relative CFURL behavior
- CFURLCopyFileSystemPath
- NSURL absoluteString/path encoding differences
- percent escaping
- symlinks
- aliases
- Finder behavior
- non-HFS filesystems
- AFP / SMB mounted volumes

## Status

Accepted as the first CFURL / NSURL filesystem boundary finding for LeoUTF8 V0.6 research.
