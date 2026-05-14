# LeoUTF8HFS Design Note

## Status

This is a design note.

It does not define a final public API yet.

LeoUTF8HFS is a proposed future layer for explicit HFS+ filename boundary
handling on Mac OS X 10.5.8 Leopard.

## Why LeoUTF8HFS Is Justified

LeoUTF8 research from V0.3 through V0.6 showed that HFS+ filename behavior is
not ordinary UTF-8 string behavior.

Measured findings show:

~~~text
Unicode normalization      != complete HFS+ policy
Unicode case folding       != HFS+ filename comparison
CFString comparison        != HFS+ filename comparison
NSString comparison        != HFS+ filename comparison
URL representation         != policy-free filename handling
~~~

Therefore HFS+ filename behavior deserves an explicit, scoped layer.

## Supporting Findings

LeoUTF8HFS is supported by the following findings:

~~~text
LEOUTF8-FIND-0002
  HFS+ filename normalization on Leopard/PPC

LEOUTF8-FIND-0003
  HFS+ CFString/NSString filesystem boundary

LEOUTF8-FIND-0004
  HFS+ case comparison behavior on Leopard/PPC

LEOUTF8-FIND-0005
  CFString/NSString comparison is not HFS+ filename comparison

LEOUTF8-FIND-0006
  CFURL / NSURL filesystem boundary on Leopard/PPC
~~~

## Design Principle

LeoUTF8HFS must be explicit.

It must never silently change the behavior of LeoUTF8Core,
LeoUTF8CoreFoundation, or LeoUTF8Foundation.

General UTF-8 operations remain general.

HFS+ behavior remains HFS+-scoped.

## Proposed Layering

~~~text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CoreFoundation
  -> LeoUTF8Foundation

  -> LeoUTF8HFS
       explicit HFS+ filename boundary helpers
~~~

LeoUTF8HFS depends on LeoUTF8Core.

It may also use CoreFoundation/Foundation helper paths where needed, but must
not hide those boundaries.

## What LeoUTF8HFS May Do

LeoUTF8HFS may provide explicit helpers for:

- validating UTF-8 filename input
- preparing filename bytes for HFS+ boundary use
- observing HFS+ filesystem byte representation
- comparing filenames according to measured HFS+ behavior
- converting filesystem bytes to display strings
- documenting URL/path boundary behavior
- safely handling Leopard-specific CFURL/NSURL differences

## What LeoUTF8HFS Must Not Do

LeoUTF8HFS must not:

- silently normalize all LeoUTF8 strings
- redefine LeoUTF8Core behavior
- pretend Unicode casefold is HFS+ comparison
- pretend CFStringCompare is HFS+ comparison
- pretend NSString comparison is HFS+ comparison
- pretend URL APIs erase HFS+ filename behavior
- implement Finder policy without measuring it
- implement APFS, SMB, AFP, or UFS policy without separate findings

## Explicitly Forbidden Shortcuts

Do not implement HFS+ filename comparison as:

~~~text
NFD + Unicode casefold
~~~

Do not implement it as:

~~~text
CFStringCompare(..., kCFCompareCaseInsensitive)
~~~

Do not implement it as:

~~~text
[NSString compare:options:NSCaseInsensitiveSearch]
~~~

The `straße.txt` vs `STRASSE.TXT` findings prove these shortcuts are wrong for
observed Leopard/HFS+ behavior.

## Candidate API Areas

The following names are only candidates.

They are not yet accepted public API.

### Validation

~~~c
LeoUTF8HFSValidateNameBytes(...)
~~~

Possible responsibility:

- validate UTF-8 filename bytes
- reject NULL input
- preserve byte-length semantics
- avoid C-string-only assumptions

### Normalization For HFS+ Observation

~~~c
LeoUTF8HFSNormalizeObservedNameBytes(...)
~~~

Possible responsibility:

- produce the observed HFS+ decomposed filename byte form
- remain explicit
- document that this is HFS+-scoped, not general string policy

### Filename Comparison

~~~c
LeoUTF8HFSCompareNameBytes(...)
LeoUTF8HFSEqualNameBytes(...)
~~~

Possible responsibility:

- compare two filename byte sequences according to measured HFS+ behavior
- avoid generic Unicode casefold shortcuts
- require further research before implementation

Status:

~~~text
locked pending more findings
~~~

### Display Bridge

~~~c
LeoUTF8HFSCreateDisplayString(...)
~~~

Possible responsibility:

- convert observed filesystem bytes into CFString/NSString display forms
- keep display separate from comparison
- avoid changing filesystem policy

### URL Boundary Helpers

~~~c
LeoUTF8HFSCopyFileSystemBytesFromCFURL(...)
LeoUTF8HFSCopyFileSystemBytesFromNSURLPath(...)
~~~

Possible responsibility:

- expose Leopard-safe CFURL/NSURL filesystem byte behavior
- document that NSURL on Leopard uses path -> NSString fileSystemRepresentation

## Open Research Before Public API

Before LeoUTF8HFS becomes public API, more research is needed.

Required areas:

- HFS+ case-sensitive volume behavior
- HFS+ Unicode comparison table behavior
- additional Latin-1 pairs
- ligatures
- Greek sigma variants
- Turkish dotted/dotless I
- CFURLCopyFileSystemPath
- NSURL path vs absoluteString
- Finder display behavior
- symlink behavior
- alias behavior
- mounted non-HFS volumes

## Proposed Initial API Policy

The first public LeoUTF8HFS API should be conservative.

Allowed early API:

- validation helpers
- observation helpers
- explicit normalization-to-observed-HFS-form helpers

Still locked:

- final HFS+ equality
- final HFS+ ordering
- Finder display policy
- URL escaping policy

## Design Doctrine

~~~text
Bytes remain bytes.
UTF-8 is explicit.
HFS+ filename behavior is a boundary.
Filesystem comparison is not general Unicode comparison.
Display strings are not filesystem policy.
URL paths are not policy-free.
LeoUTF8Core guards the boundary.
LeoUTF8HFS scopes the filesystem behavior.
~~~

## Current Decision

LeoUTF8HFS is justified as a future layer.

Implementation is not yet approved.

The next acceptable step is more research or a minimal private prototype,
not a public API.
