# LeoUTF8CoreFoundation API Contract

LeoUTF8CoreFoundation is the CoreFoundation bridge layer of LeoUTF8.

It connects explicit UTF-8 byte buffers from LeoUTF8Core with Leopard-native
CoreFoundation objects such as `CFStringRef` and `CFDataRef`.

This layer is intentionally thin.

LeoUTF8Core remains the UTF-8 authority.
CoreFoundation remains the native Leopard C-level text model.

## Design Rules

- LeoUTF8Core validates external UTF-8 bytes.
- CoreFoundation is used only after UTF-8 has crossed the LeoUTF8Core boundary.
- `CFStringRef` is native Leopard CoreFoundation text.
- `CFDataRef` is explicit UTF-8 byte transport.
- No automatic normalization.
- No hidden global string semantics.
- No locale policy engine.
- No replacement for CoreFoundation.

## Public Header

```c
#include "LeoUTF8CoreFoundation.h"
````

`LeoUTF8CoreFoundation.h` includes `LeoUTF8.h`.

## CFString Creation From UTF-8 Bytes

```c
LeoUTF8Status LeoUTF8CreateCFStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             CFStringRef *outString);
```

Creates a retained `CFStringRef` from an explicit UTF-8 byte buffer.

### Input

```c
const unsigned char *bytes, size_t length
```

The function validates exactly `length` bytes through LeoUTF8Core before asking  
CoreFoundation to create a `CFStringRef`.

Malformed UTF-8 is rejected before it crosses into CoreFoundation.

### Output

On success:

```c
*outString
```

receives a retained `CFStringRef`.

The caller owns the returned object and must release it with:

```c
CFRelease(*outString);
```

### Failure

On failure:

```c
*outString == NULL
```

and the function returns a `LeoUTF8Status`.

Typical failures:

- `LEO_UTF8_INVALID` for `NULL` input, malformed UTF-8, or `NULL` output pointer
    
- `LEO_UTF8_ERROR` if CoreFoundation unexpectedly fails to create the string
    

## UTF-8 CFData Creation From CFString

```c
LeoUTF8Status LeoUTF8CopyUTF8DataFromCFString(CFStringRef string,
                                              CFDataRef *outData);
```

Creates retained `CFDataRef` containing the UTF-8 representation of a  
`CFStringRef`.

### Input

```c
CFStringRef string
```

The input must be a non-NULL `CFStringRef`.

### Output

On success:

```c
*outData
```

receives retained `CFDataRef`.

The caller owns the returned object and must release it with:

```c
CFRelease(*outData);
```

The bytes in the returned `CFDataRef` are explicit UTF-8 transport bytes.

### Failure

On failure:

```c
*outData == NULL
```

and the function returns a `LeoUTF8Status`.

Typical failures:

- `LEO_UTF8_INVALID` for `NULL` input or `NULL` output pointer
    
- `LEO_UTF8_ERROR` if CoreFoundation fails to convert the string to UTF-8
    
- `LEO_UTF8_NO_MEMORY` if buffer or CFData allocation fails
    

## Ownership Rules

LeoUTF8CoreFoundation follows CoreFoundation Create/Copy ownership rules.

Returned objects are retained.

Callers must release them:

```c
CFStringRef string = NULL;
CFDataRef data = NULL;

if (LeoUTF8CreateCFStringFromBytes(bytes, length, &string) == LEO_UTF8_OK) {
    /* use string */
    CFRelease(string);
}

if (LeoUTF8CopyUTF8DataFromCFString(string, &data) == LEO_UTF8_OK) {
    /* use data */
    CFRelease(data);
}
```

## Normalization Policy

LeoUTF8CoreFoundation does not normalize automatically.

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

The current Leopard/PPC CoreFoundation probe confirms:

- UTF-8 byte buffer to `CFStringRef`
    
- `CFStringRef` to UTF-8 `CFDataRef`
    
- byte length preservation through `CFDataRef`
    
- codepoint counting of `CFDataRef` bytes through LeoUTF8Core
    
- invalid UTF-8 rejection before `CFStringRef` creation
    
- clean output state after invalid UTF-8 input
    
- execution on Mac OS X 10.5.8 PowerPC
    

Observed sample:

```text
Straße
```

UTF-8 byte length:

```text
7 bytes
```

Codepoint count:

```text
6
```

Invalid UTF-8 input is rejected with:

```text
LEO_UTF8_INVALID
```

and leaves the output pointer clean.

## Non-Goals

LeoUTF8CoreFoundation does not currently provide:

- automatic normalization
    
- locale-specific casing
    
- Unicode collation
    
- grapheme-cluster segmentation
    
- file-system path normalization
    
- CFString utility collection
    
- Objective-C APIs
    
- Foundation APIs
    
- AppKit integration
    
- replacement for CoreFoundation
    

## Relationship To Other Layers

LeoUTF8CoreFoundation depends on LeoUTF8Core.

LeoUTF8Core must remain usable without CoreFoundation.

The layering is:

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CoreFoundation
```

LeoUTF8Foundation is a separate Objective-C/Foundation bridge layer.

The full current stack is:

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> LeoUTF8CoreFoundation
  -> LeoUTF8Foundation
```

## Current CoreFoundation Doctrine

```text
CFStringRef is native Leopard CoreFoundation text.
CFDataRef is explicit UTF-8 transport.
LeoUTF8Core guards the boundary.

