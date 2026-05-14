# LeoUTF8 V0.4 — HFS+ Case Comparison Research Cut

## Status

LeoUTF8 V0.4 is a research cut.

It does not introduce a final HFS+ filename comparison API.

It documents measured Leopard/HFS+ case-comparison behavior on real
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
- HFS+ normalization boundary research
- HFS+ CFString/NSString filesystem boundary research

## New Research Target

V0.4 adds:

~~~sh
make hfscasecheck
~~~

## HFS+ Case Comparison Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_case_probe.c
~~~

Run with:

~~~sh
make hfscasecheck
~~~

This probe checks observed HFS+ filename comparison behavior for selected
case-related filename pairs.

## Tested Pairs

~~~text
case.txt     vs CASE.txt
ä.txt        vs Ä.txt
café.txt     vs CAFÉ.TXT
straße.txt   vs STRASSE.TXT
~~~

## Observed Results

Confirmed on Leopard/PPC:

~~~text
case.txt vs CASE.txt          -> same file
ä.txt vs Ä.txt                -> same file
café.txt vs CAFÉ.TXT          -> same file
straße.txt vs STRASSE.TXT     -> separate files
~~~

## Key Finding

Unicode case folding is not equivalent to HFS+ filename comparison.

LeoUTF8 case folding maps:

~~~text
straße.txt
STRASSE.TXT
~~~

to the same folded form:

~~~text
strasse.txt
~~~

However, observed HFS+ behavior treats these as separate filenames.

## Finding Document

~~~text
docs/findings/LEOUTF8-FIND-0004-hfsplus-case-comparison.md
~~~

## Architectural Consequence

LeoUTF8 must not implement HFS+ filename comparison as generic Unicode
casefolding.

Future HFS+-specific comparison helpers must be explicit and scoped.

They should distinguish between:

~~~text
Unicode case folding
HFS+ filename comparison
display normalization
filesystem byte representation
~~~

## Policy

Do not implement HFS+ filename comparison as:

~~~text
NFD + Unicode casefold
~~~

without further research.

The tested sharp-S pair proves that this would create a false equivalence
relative to observed HFS+ behavior on Leopard.

## Explicit Non-Goals For V0.4

LeoUTF8 V0.4 does not provide:

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
- POSIX file APIs
- LeoUTF8Core

## Doctrine

~~~text
Bytes remain bytes.
UTF-8 is explicit.
HFS+ filename behavior is a boundary.
Unicode case folding is not HFS+ filename comparison.
Filesystem comparison policy is not general string policy.
LeoUTF8Core guards the boundary.
~~~

## Next Possible Steps

- research additional Latin-1 case pairs
- research ligatures
- research Greek sigma variants
- research Turkish dotted/dotless I
- research HFS+ case-sensitive volumes
- research CFStringCompare options
- design explicit LeoUTF8HFS comparison API
