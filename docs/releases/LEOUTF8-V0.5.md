# LeoUTF8 V0.5 — CFString/NSString Comparison Boundary Research Cut

## Status

LeoUTF8 V0.5 is a research cut.

It does not introduce a final HFS+ filename comparison API.

It documents measured differences between Leopard string comparison APIs and
observed HFS+ filename comparison behavior on real Mac OS X 10.5.8 PowerPC
hardware.

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

## New Research Target

V0.5 adds:

~~~sh
make hfscomparecheck
~~~

## CFString / NSString Comparison Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_cfcompare_probe.m
~~~

Run with:

~~~sh
make hfscomparecheck
~~~

This probe compares selected filename pairs through:

- CFStringCompare
- NSString compare:options:

## Tested Pairs

~~~text
case.txt     vs CASE.txt
ä.txt        vs Ä.txt
café.txt     vs CAFÉ.TXT
straße.txt   vs STRASSE.TXT
~~~

## Observed String Comparison Results

Confirmed on Leopard/PPC:

~~~text
CFString case-insensitive comparison treats all tested pairs as equal.
NSString case-insensitive comparison treats all tested pairs as equal.
~~~

This includes:

~~~text
straße.txt
STRASSE.TXT
~~~

## Conflict With Observed HFS+ Behavior

V0.4 observed that HFS+ treats:

~~~text
straße.txt
STRASSE.TXT
~~~

as separate files.

Therefore:

~~~text
CFString/NSString case-insensitive equality
  does not imply
HFS+ filename equality
~~~

## Key Finding

Leopard string comparison APIs are not a drop-in replacement for HFS+ filename
comparison policy.

## Finding Document

~~~text
docs/findings/LEOUTF8-FIND-0005-cfstring-nsstring-comparison-vs-hfsplus.md
~~~

## Architectural Consequence

A future `LeoUTF8HFS` layer must keep these concepts separate:

~~~text
Unicode case folding
CFString comparison
NSString comparison
HFS+ filename comparison
filesystem byte representation
display representation
~~~

## Policy

Do not implement HFS+ filename comparison as:

~~~text
CFStringCompare(..., kCFCompareCaseInsensitive)
~~~

or:

~~~text
[NSString compare:options:NSCaseInsensitiveSearch]
~~~

without HFS+-specific validation.

The tested sharp-S pair proves that this would create a false equivalence
relative to observed HFS+ behavior on Leopard.

## Explicit Non-Goals For V0.5

LeoUTF8 V0.5 does not provide:

- final HFS+ filename comparison API
- automatic filename normalization
- Finder comparison policy
- CFURL comparison policy
- full HFS+ Unicode comparison table
- case-sensitive HFS+ behavior
- APFS behavior
- UFS behavior
- SMB behavior
- AFP behavior

## Confirmed Platform

- Mac OS X 10.5.8 Leopard
- PowerPC
- HFS+
- native /usr/bin/gcc
- CoreFoundation
- Foundation
- LeoUTF8Core

## Doctrine

~~~text
Bytes remain bytes.
UTF-8 is explicit.
HFS+ filename behavior is a boundary.
Unicode case folding is not HFS+ filename comparison.
CFString comparison is not HFS+ filename comparison.
NSString comparison is not HFS+ filename comparison.
Filesystem comparison policy is not general string policy.
LeoUTF8Core guards the boundary.
~~~

## Next Possible Steps

- research CFURL behavior
- research Finder display behavior
- research HFS+ decomposition exceptions
- research more case pairs
- research case-sensitive HFS+
- design explicit LeoUTF8HFS comparison API
