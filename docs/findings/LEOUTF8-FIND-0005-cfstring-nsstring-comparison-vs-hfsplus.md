# LEOUTF8-FIND-0005: CFString/NSString comparison is not HFS+ filename comparison

## Summary

On Mac OS X 10.5.8 PowerPC, `CFStringCompare` and `NSString compare:options:`
with case-insensitive options treat all tested filename pairs as equal,
including:

~~~text
straße.txt
STRASSE.TXT
~~~

However, the HFS+ case comparison probe showed that HFS+ treats these two names
as separate files.

This confirms that Leopard string comparison APIs must not be treated as a
drop-in replacement for HFS+ filename comparison policy.

## Host

- System: Mac OS X 10.5.8 Leopard
- Architecture: PowerPC
- Filesystem context: HFS+
- Probe target: `build-work/hfsplus_cfcompare_probe`

## Probe

Probe source:

~~~text
Research/HFSPlus/hfsplus_cfcompare_probe.m
~~~

Run with:

~~~sh
make hfscomparecheck
~~~

## Tested Pairs

~~~text
case.txt     vs CASE.txt
ä.txt        vs Ä.txt
café.txt     vs CAFÉ.TXT
straße.txt   vs STRASSE.TXT
~~~

## Observed CFString Results

For all tested pairs:

~~~text
CFString literal: different
CFString case-insensitive: equal
CFString case-insensitive + nonliteral: equal
CFString case-insensitive + diacritic-insensitive: equal
CFString case-insensitive + nonliteral + diacritic-insensitive: equal
~~~

This includes the sharp-S pair:

~~~text
straße.txt vs STRASSE.TXT
~~~

## Observed NSString Results

For all tested pairs:

~~~text
NSString literal: different
NSString case-insensitive: equal
NSString case-insensitive + diacritic-insensitive: equal
NSString case-insensitive + literal: equal
~~~

This also includes the sharp-S pair:

~~~text
straße.txt vs STRASSE.TXT
~~~

## Conflict With HFS+ Case Finding

`LEOUTF8-FIND-0004` observed that HFS+ treats:

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

## Architectural Consequence

LeoUTF8 must keep these concepts separate:

~~~text
Unicode case folding
CFString comparison
NSString comparison
HFS+ filename comparison
filesystem byte representation
display representation
~~~

A future `LeoUTF8HFS` layer must not be implemented as a thin wrapper around
generic `CFStringCompare` or `NSString compare:options:`.

## Policy Implication

Do not implement HFS+ filename comparison as:

~~~text
CFStringCompare(..., kCFCompareCaseInsensitive)
~~~

or:

~~~text
[NSString compare:options:NSCaseInsensitiveSearch]
~~~

without HFS+-specific validation.

The tested sharp-S pair proves this would produce a false equivalence relative
to observed HFS+ behavior on Leopard.

## Status

Accepted as the first CFString/NSString comparison boundary finding for
LeoUTF8 V0.5 research.
