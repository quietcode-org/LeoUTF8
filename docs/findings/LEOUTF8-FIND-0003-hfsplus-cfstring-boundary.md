# LEOUTF8-FIND-0003: HFS+ CFString/NSString filesystem boundary

## Summary

On Mac OS X 10.5.8 PowerPC, a filename created with an NFC UTF-8 spelling on an HFS+ volume is observed as decomposed UTF-8 bytes through:

- `readdir()`
- `CFStringGetFileSystemRepresentation`
- `NSString fileSystemRepresentation`

The observed byte sequences match `LeoUTF8NormalizeNFDBytes()` for the original NFC input.

## Host

- System: Mac OS X 10.5.8 Leopard
- Architecture: PowerPC
- Filesystem reported by `statfs`: `hfs`
- Probe target: `build-work/hfsplus_cfstring_boundary_probe`

## Probe

Probe source:

```text
Research/HFSPlus/hfsplus_cfstring_boundary_probe.m
````

Run with:

```sh
make hfsboundarycheck
```

## Test Input

Display form:

```text
café.txt
```

NFC UTF-8 input bytes:

```text
63 61 66 C3 A9 2E 74 78 74
```

LeoUTF8 expected NFD bytes:

```text
63 61 66 65 CC 81 2E 74 78 74
```

## Observed POSIX Boundary

`readdir()` returned:

```text
entry: café.txt
readdir bytes: 63 61 66 65 CC 81 2E 74 78 74
```

Summary result:

```text
readdir bytes match LeoUTF8 NFD: yes
```

## Observed CoreFoundation Boundary

`CFStringCreateWithFileSystemRepresentation()` was used to create a `CFStringRef`  
from the `readdir()` entry.

`CFStringGetFileSystemRepresentation()` then returned:

```text
CFString filesystem representation: café.txt
CFString filesystem bytes: 63 61 66 65 CC 81 2E 74 78 74
```

Summary result:

```text
CFString filesystem bytes match LeoUTF8 NFD: yes
```

## Observed Foundation Boundary

`NSString` was created from the `readdir()` UTF-8 bytes.

`NSString fileSystemRepresentation` then returned:

```text
NSString filesystem representation: café.txt
NSString filesystem bytes: 63 61 66 65 CC 81 2E 74 78 74
```

Summary result:

```text
NSString filesystem bytes match LeoUTF8 NFD: yes
```

## Interpretation

On this Leopard/PPC HFS+ volume, the filesystem boundary is consistently observed  
as decomposed UTF-8 bytes.

This is true through:

- POSIX directory enumeration
    
- CoreFoundation filesystem representation APIs
    
- Foundation filesystem representation APIs
    

This confirms that HFS+ filename handling must be treated as its own boundary,  
not as ordinary byte-stable UTF-8.

## Architectural Consequence

LeoUTF8 should keep general UTF-8 string operations separate from HFS+-specific  
filename policy.

Future HFS+-specific helpers are justified, but they should be explicit and  
scoped.

Possible future layer:

```text
LeoUTF8HFS
  -> explicit HFS+ filename normalization
  -> explicit HFS+ filename comparison
  -> explicit filesystem/display boundary helpers
```

## Policy Implication

Do not silently normalize all LeoUTF8 strings.

Only HFS+-scoped APIs should apply HFS+-specific filename normalization.

Recommended model:

```text
external/user UTF-8 input
  -> LeoUTF8Core validation
  -> explicit HFS+ filename normalization when comparing filesystem names
  -> CFString/NSString for display and native Leopard text handling
```

## Non-Goals

This finding does not define a final HFS+ filename API.

It does not yet cover:

- HFS+ decomposition exceptions
    
- case-sensitive HFS+
    
- case-insensitive comparison behavior
    
- APFS
    
- UFS
    
- SMB
    
- AFP
    
- mounted non-HFS volumes
    
- CFURL behavior
    
- Finder display behavior
    

## Status

Accepted as the second HFS+ filename boundary finding for LeoUTF8 V0.3 research.  

