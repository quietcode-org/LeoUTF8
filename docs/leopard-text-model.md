# Leopard Text Model Notes For LeoUTF8

This document summarizes the current LeoUTF8 understanding of Leopard text and
filesystem boundaries.

## Core Principle

Leopard already has strong native Unicode text APIs.

LeoUTF8 exists because boundary handling must be explicit.

~~~text
Bytes are not strings.
Strings are not filesystem policy.
Filesystem names are not generic Unicode comparison.
~~~

## Native Leopard Text Objects

### CFStringRef

`CFStringRef` is native CoreFoundation text.

Use it for C/CoreFoundation-level Leopard code.

### NSString

`NSString` is native Foundation text.

Use it for Objective-C/Foundation/AppKit-level Leopard code.

### CFDataRef / NSData

`CFDataRef` and `NSData` are useful for explicit byte transport.

They are not text by themselves.

## LeoUTF8 Boundary Objects

LeoUTF8 focuses on explicit UTF-8 byte handling:

~~~text
const unsigned char *bytes + size_t length
~~~

This avoids hidden C-string assumptions and supports embedded NUL bytes.

## General UTF-8 Rules

LeoUTF8Core provides:

- validate
- count codepoints
- normalize NFC
- normalize NFD
- casefold
- free returned buffers

These operations are general Unicode/UTF-8 operations.

They do not define filesystem policy.

## Foundation Boundary

LeoUTF8Foundation converts:

~~~text
UTF-8 bytes -> NSString
NSString    -> UTF-8 NSData
~~~

Malformed UTF-8 is rejected before crossing into Foundation.

## CoreFoundation Boundary

LeoUTF8CoreFoundation converts:

~~~text
UTF-8 bytes -> CFStringRef
CFStringRef -> UTF-8 CFDataRef
~~~

Malformed UTF-8 is rejected before crossing into CoreFoundation.

## HFS+ Filename Boundary

HFS+ on the tested Leopard/PPC system returns decomposed UTF-8 filename bytes.

Observed:

~~~text
NFC input: café.txt
readdir:   café.txt
bytes:     63 61 66 65 CC 81 2E 74 78 74
~~~

The observed bytes match `LeoUTF8NormalizeNFDBytes()`.

## HFS+ Case Boundary

HFS+ filename comparison is not identical to Unicode case folding.

Important observed pair:

~~~text
straße.txt
STRASSE.TXT
~~~

LeoUTF8 Unicode casefold treats these as equivalent.

HFS+ treated them as separate files.

Therefore generic Unicode casefold must not be used as HFS+ filename comparison.

## CFString / NSString Comparison Boundary

Case-insensitive CFString and NSString comparison also treated:

~~~text
straße.txt
STRASSE.TXT
~~~

as equal.

HFS+ treated them as separate files.

Therefore Leopard string comparison APIs are not HFS+ filename comparison policy.

## URL Boundary

CFURL preserves filesystem representation behavior at the tested boundary.

NSURL on Leopard has no direct `-fileSystemRepresentation`.

Use:

~~~text
NSURL -> path -> NSString fileSystemRepresentation
~~~

## Practical Model

For external bytes:

~~~text
external bytes
  -> LeoUTF8Core validation
  -> explicit normalization only if needed
  -> CFString/NSString for native text
~~~

For HFS+ filenames:

~~~text
external/user input
  -> LeoUTF8Core validation
  -> HFS+-specific handling only in explicit HFS scope
  -> filesystem byte comparison must not use generic casefold blindly
  -> CFString/NSString for display/native text
~~~

For URLs:

~~~text
filesystem bytes
  -> CFURL file system representation when using CFURL
  -> NSURL path -> NSString fileSystemRepresentation when using NSURL
~~~

## Non-Policy Rule

LeoUTF8 must not silently normalize all strings.

HFS+ behavior belongs in explicit HFS+-scoped helpers, not in general UTF-8
helpers.
