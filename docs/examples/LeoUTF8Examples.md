# LeoUTF8 External Consumer Examples

## Status

LeoUTF8 V0.7 includes external consumer examples.

These examples are not internal tests.

They demonstrate how another Leopard-native project can consume LeoUTF8 through the staged `dist/LeoUTF8` brick.

## Purpose

Tests verify behavior.

`distcheck` verifies external consumability.

Examples show intended usage.

## Example Layout

~~~text
Examples/
  Core/
    leoutf8_core_example.c
  CoreFoundation/
    leoutf8_corefoundation_example.c
  Foundation/
    leoutf8_foundation_example.m
~~~

## Build Target

Run:

~~~sh
make examples
~~~

This target first stages:

~~~text
dist/LeoUTF8
~~~

and then builds the examples against only:

~~~text
dist/LeoUTF8/include
dist/LeoUTF8/lib
~~~

## Core Example

Source:

~~~text
Examples/Core/leoutf8_core_example.c
~~~

Uses:

~~~text
LeoUTF8.h
libLeoUTF8Core.a
~~~

Demonstrates:

- engine version reporting
- brick version reporting
- UTF-8 validation
- codepoint counting
- Unicode case folding
- `LeoUTF8Free`

## CoreFoundation Example

Source:

~~~text
Examples/CoreFoundation/leoutf8_corefoundation_example.c
~~~

Uses:

~~~text
LeoUTF8CoreFoundation.h
libLeoUTF8CoreFoundation.a
libLeoUTF8Core.a
-framework CoreFoundation
~~~

Demonstrates:

- UTF-8 bytes to `CFStringRef`
- `CFStringRef` to UTF-8 `CFDataRef`
- CoreFoundation ownership through `CFRelease`

## Foundation Example

Source:

~~~text
Examples/Foundation/leoutf8_foundation_example.m
~~~

Uses:

~~~text
LeoUTF8Foundation.h
libLeoUTF8Foundation.a
libLeoUTF8Core.a
-framework Foundation
~~~

Demonstrates:

- UTF-8 bytes to `NSString`
- `NSString` to UTF-8 `NSData`
- manual Leopard retain/release ownership

## Expected Output

Successful output includes:

~~~text
LeoUTF8 Core example passed.
LeoUTF8 CoreFoundation example passed.
LeoUTF8 Foundation example passed.
LeoUTF8 examples completed successfully.
~~~

## Design Meaning

The examples prove the V0.7 integration-brick rule:

~~~text
LeoUTF8 must be usable through its public dist surface.
~~~

Consumers should not include private source paths.

Consumers should not link against `vendor/utf8proc`.

Consumers should treat LeoUTF8 as a staged brick:

~~~text
dist/LeoUTF8/include
dist/LeoUTF8/lib
~~~

## Current Boundary

The examples cover:

- Core
- CoreFoundation
- Foundation

They do not cover:

- HFS+ filename policy
- CFURL / NSURL behavior
- LeooRexx integration
- framework packaging
- dynamic library packaging

Those remain separate concerns.
