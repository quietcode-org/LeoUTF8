# Cupertino 2009 Compliance Rule

## Status

This document defines the local LeoUTF8 application of the Leopard-Crew
system-first doctrine.

It is part of the V0.7 Integration Brick Hardening phase.

## Core Rule

If Mac OS X 10.5.8, Apple documentation, Xcode 3.x, GCC, Cocoa,
CoreFoundation, Foundation, HFS+, or another native Leopard subsystem defines
a documented or system-typical rule for a software concern, LeoUTF8 follows
that rule before project-local style preferences.

Project style may refine implementation.

Project style must not override the target system's architecture, ownership
rules, documentation conventions, or boundary semantics.

## Meaning

Cupertino 2009 is not only a visual style.

Cupertino 2009 is a system contract.

For LeoUTF8, this means:

- public headers use Apple/HeaderDoc-style API comments
- public API documents ownership at the declaration site
- CoreFoundation ownership follows Create/Copy and CFRelease rules
- Foundation ownership assumes Leopard manual retain/release, not ARC
- filesystem behavior follows observed and documented HFS+ behavior
- UTF-8 behavior is explicit at byte boundaries
- vendored code remains vendored and is not edited for local style
- build behavior must remain realistic for Leopard/PPC and Xcode/GCC-era tools
- modern convenience abstractions must not hide Leopard-native semantics

## Public Header Rule

Public headers are API surface.

They are not merely compiler input.

Every header staged into:

~~~text
dist/LeoUTF8/include
release/LeoUTF8-*/include
~~~

must follow the HeaderDoc policy.

Current public headers:

~~~text
LeoUTF8.h
LeoUTF8Version.h
LeoUTF8CoreFoundation.h
LeoUTF8Foundation.h
~~~

## Ownership Rule

Ownership must follow the native layer.

LeoUTF8Core:

~~~text
Returned buffers are released with LeoUTF8Free().
~~~

CoreFoundation:

~~~text
Create/Copy results are released with CFRelease().
~~~

Foundation:

~~~text
Returned retained objects are released manually under Leopard retain/release.
No ARC assumptions.
~~~

## Filesystem Rule

HFS+ filename behavior is not generic Unicode behavior.

The following shortcuts are forbidden as HFS+ filename policy:

~~~text
NFD + Unicode casefold
CFStringCompare(..., kCFCompareCaseInsensitive)
[NSString compare:options:NSCaseInsensitiveSearch]
~~~

Observed Leopard/HFS+ behavior is the reference until a more authoritative
system-specific rule is established.

## API Rule

LeoUTF8 must not expose a public API merely because an implementation is easy.

A public API requires:

- documented ownership
- documented invalid input behavior
- documented target platform assumptions
- documented boundary semantics
- examples or tests proving intended use
- compatibility with the Leopard-Crew system-first doctrine

## Design Rule

Research findings may justify future APIs.

They do not automatically create APIs.

For example, LeoUTF8HFS is justified as a future layer, but remains locked until
its public behavior can be defined without violating observed HFS+ behavior.

## Doctrine

~~~text
Systemordnung vor Projektstil.
Apple-Konvention vor persönlicher Bequemlichkeit.
Cupertino 2009 vor modernem Fremdreflex.

If the system has a rule, follow the system.
If the rule is unclear, measure first.
If measurement contradicts convenience, trust measurement.
