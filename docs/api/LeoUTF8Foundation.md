# LeoUTF8Foundation API Contract

LeoUTF8Foundation is the Leopard Foundation bridge layer of LeoUTF8.

It connects explicit UTF-8 byte buffers from LeoUTF8Core with Leopard-native
Foundation text objects such as `NSString` and `NSData`.

LeoUTF8Foundation is intentionally thin.

It does not replace `NSString`.
It does not implement Unicode logic independently.
It uses LeoUTF8Core as the UTF-8 validation boundary.

## Design Rules

- LeoUTF8Core remains the UTF-8 authority.
- Foundation remains the native Leopard text model.
- `NSString` is the native in-memory text object.
- `NSData` is the explicit UTF-8 byte transport object.
- No automatic normalization.
- No hidden global string semantics.
- No locale policy engine.

## Public Header

```objc
#import "LeoUTF8Foundation.h"
````

Consumers of the Foundation bridge should include `LeoUTF8Foundation.h`.

`LeoUTF8Foundation.h` includes `LeoUTF8.h`.

## NSString Creation From UTF-8 Bytes

```objc
LeoUTF8Status LeoUTF8CreateNSStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             NSString **outString);
```

Creates a retained `NSString` from an explicit UTF-8 byte buffer.

### Input

```c
const unsigned char *bytes, size_t length
```

The function validates exactly `length` bytes through LeoUTF8Core before asking  
Foundation to create an `NSString`.

This means malformed UTF-8 is rejected before it crosses into the native  
Foundation layer.

### Output

On success:

```objc
*outString
```

receives a retained `NSString`.

The caller owns the returned object and must release it.

### Failure

On failure:

```objc
*outString == nil
```

and the function returns a `LeoUTF8Status`.

Typical failures:

- `LEO_UTF8_INVALID` for `NULL` input, malformed UTF-8, or `NULL` output pointer
    
- `LEO_UTF8_ERROR` if Foundation unexpectedly fails to create the string
    

## UTF-8 NSData Creation From NSString

```objc
LeoUTF8Status LeoUTF8CopyUTF8DataFromNSString(NSString *string,
                                              NSData **outData);
```

Creates retained `NSData` containing the UTF-8 representation of an `NSString`.

### Input

```objc
NSString *string
```

The input must be a non-nil `NSString`.

### Output

On success:

```objc
*outData
```

receives retained `NSData`.

The caller owns the returned object and must release it.

The bytes in the returned `NSData` are explicit UTF-8 transport bytes.

### Failure

On failure:

```objc
*outData == nil
```

and the function returns a `LeoUTF8Status`.

Typical failures:

- `LEO_UTF8_INVALID` for `nil` input or `nil` output pointer
    
- `LEO_UTF8_INVALID` if the string cannot be converted to UTF-8
    
- `LEO_UTF8_ERROR` if Foundation unexpectedly fails to provide UTF-8 bytes
    
- `LEO_UTF8_NO_MEMORY` if `NSData` allocation fails
    

## Ownership Rules

LeoUTF8Foundation follows manual retain/release ownership rules suitable for  
Mac OS X 10.5 Leopard.

Returned Objective-C objects are retained.

Callers must release them:

```objc
NSString *string = nil;
NSData *data = nil;

if (LeoUTF8CreateNSStringFromBytes(bytes, length, &string) == LEO_UTF8_OK) {
    /* use string */
    [string release];
}

if (LeoUTF8CopyUTF8DataFromNSString(string, &data) == LEO_UTF8_OK) {
    /* use data */
    [data release];
}
```

LeoUTF8Foundation does not require ARC.

## Normalization Policy

LeoUTF8Foundation does not normalize automatically.

If a caller needs NFC, NFD, or case folding, it must explicitly use LeoUTF8Core:

```c
LeoUTF8NormalizeNFCBytes(...)
LeoUTF8NormalizeNFDBytes(...)
LeoUTF8CaseFoldBytes(...)
```

This preserves the LeoUTF8 doctrine:

```text
No hidden global string semantics.
UTF-8 transformations are explicit.
```

## Confirmed Probe Behavior

The current Leopard/PPC Foundation probe confirms:

- UTF-8 byte buffer to `NSString`
    
- `NSString` to UTF-8 `NSData`
    
- byte length preservation through `NSData`
    
- codepoint counting of `NSData` bytes through LeoUTF8Core
    
- invalid UTF-8 rejection before `NSString` creation
    
- execution on Mac OS X 10.5.8 PowerPC
    

Observed sample:

```text
Größe ÄÖÜ Straße
```

UTF-8 byte length:

```text
22 bytes
```

Codepoint count:

```text
16
```

Invalid UTF-8 input is rejected with:

```text
LEO_UTF8_INVALID
```

## Non-Goals

LeoUTF8Foundation does not currently provide:

- automatic normalization
    
- locale-specific casing
    
- Unicode collation
    
- grapheme-cluster segmentation
    
- file-system path normalization
    
- NSString category methods
    
- Objective-C classes
    
- AppKit integration
    
- replacement for Foundation or CoreFoundation
    

## Relationship To LeoUTF8Core

LeoUTF8Foundation depends on LeoUTF8Core.

LeoUTF8Core must remain usable without Foundation.

The layering is:

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8Foundation
```

Foundation is a bridge layer, not the source of UTF-8 policy.

## Current Foundation Doctrine

```text
NSString is native Leopard text.
NSData is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.

