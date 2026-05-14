# LEOUTF8-FIND-0004: HFS+ case comparison behavior on Leopard/PPC

## Summary

On Mac OS X 10.5.8 PowerPC with HFS+, filename comparison is case-insensitive
for the tested ASCII, umlaut, and accented Latin examples.

However, HFS+ does not treat `straße.txt` and `STRASSE.TXT` as the same filename,
even though Unicode case folding through LeoUTF8 maps both spellings to the same
casefolded byte sequence.

This confirms that Unicode case folding is not equivalent to HFS+ filename
comparison policy.

## Host

- System: Mac OS X 10.5.8 Leopard
- Architecture: PowerPC
- Filesystem reported by `statfs`: `hfs`
- Probe target: `build-work/hfsplus_case_probe`

## Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_case_probe.c
~~~

Run with:

~~~sh
make hfscasecheck
~~~

## Confirmed Pre-Checks

Before the case probe, the following targets were confirmed successful:

~~~text
make
make check
make distcheck
make hfscheck
make hfsboundarycheck
~~~

## Tested Pairs

### ASCII case

~~~text
case.txt
CASE.txt
~~~

Observed result:

~~~text
second O_EXCL create result: EEXIST
second spelling matched existing file: yes
~~~

Interpretation:

HFS+ treated the ASCII case variant as the same filename.

## Umlaut case

~~~text
ä.txt
Ä.txt
~~~

Observed result:

~~~text
second O_EXCL create result: EEXIST
second spelling matched existing file: yes
~~~

Directory entry returned:

~~~text
entry: ä.txt
entry bytes: 61 CC 88 2E 74 78 74
~~~

Interpretation:

HFS+ treated the lowercase and uppercase umlaut forms as the same filename and
returned a decomposed filesystem representation.

## Accent case

~~~text
café.txt
CAFÉ.TXT
~~~

Observed result:

~~~text
second O_EXCL create result: EEXIST
second spelling matched existing file: yes
~~~

Directory entry returned:

~~~text
entry: café.txt
entry bytes: 63 61 66 65 CC 81 2E 74 78 74
~~~

Interpretation:

HFS+ treated the accented lowercase and uppercase forms as the same filename and
returned a decomposed filesystem representation.

## Sharp S case

~~~text
straße.txt
STRASSE.TXT
~~~

LeoUTF8 casefold result:

~~~text
straße.txt   -> strasse.txt
STRASSE.TXT  -> strasse.txt
~~~

Observed HFS+ result:

~~~text
second O_EXCL create result: separate file created
second spelling matched existing file: no
~~~

Directory entries:

~~~text
entry: STRASSE.TXT
entry bytes: 53 54 52 41 53 53 45 2E 54 58 54

entry: straße.txt
entry bytes: 73 74 72 61 C3 9F 65 2E 74 78 74
~~~

Interpretation:

HFS+ did not treat `straße.txt` and `STRASSE.TXT` as the same filename.

This is the key finding: Unicode case folding and HFS+ filename comparison are
not equivalent.

## Architectural Consequence

LeoUTF8 must not use generic Unicode case folding as a substitute for HFS+
filename comparison.

Future HFS+-specific helpers should distinguish between:

~~~text
Unicode case folding
HFS+ filename comparison
display normalization
filesystem byte representation
~~~

## Policy Implication

Do not implement HFS+ filename comparison as:

~~~text
NFD + Unicode casefold
~~~

without further research.

The tested `ß` / `SS` pair proves that this would produce a false equivalence
relative to observed HFS+ behavior on Leopard.

## Future Research

Further research is needed for:

- additional Latin-1 characters
- ligatures
- Greek sigma variants
- Turkish dotted/dotless I
- HFS+ case-sensitive volumes
- HFS+ decomposition exceptions
- CFStringCompare behavior with filesystem options
- Finder behavior
- CFURL behavior

## Status

Accepted as the first HFS+ case comparison finding for LeoUTF8 V0.4 research.
