# LeoUTF8Core API Contract

LeoUTF8Core is the small C-level UTF-8 boundary layer of LeoUTF8.

It wraps the vendored utf8proc engine and exposes a stable, explicit API for
Leopard-native tools and future bridges to CoreFoundation, Foundation, Rexx,
CLI tools, and other Leopard-Crew projects.

## Design Rules

- Bytes remain bytes.
- UTF-8 is explicit.
- NSString and CFString remain the native Leopard text model.
- utf8proc is an implementation detail.
- No hidden global string semantics.
- No automatic normalization.
- No locale policy engine.

## Public Header

```c
#include "LeoUTF8.h"
````

Consumers should include `LeoUTF8.h`, not `utf8proc.h`.

## API Layers

LeoUTF8Core exposes two C API layers:

```text
1. Byte-length based core API
2. Null-terminated C-string convenience API
```

The byte-length API is the preferred system boundary API.

The null-terminated API is provided for convenience and is implemented as a  
wrapper around the byte-length API.

## Byte-Length Core API

The preferred input model is:

```c
const unsigned char *bytes, size_t length
```

These functions do not require null-terminated input and may process buffers  
that contain embedded NUL bytes.

### Validation

```c
LeoUTF8Status LeoUTF8ValidateBytes(const unsigned char *bytes,
                                   size_t length);
```

Validates exactly `length` bytes.

Returns:

- `LEO_UTF8_OK` for valid UTF-8
    
- `LEO_UTF8_INVALID` for malformed UTF-8 or `NULL`
    
- `LEO_UTF8_ERROR` for implementation-level range errors
    

### Codepoint Count

```c
LeoUTF8Status LeoUTF8CodepointCountBytes(const unsigned char *bytes,
                                         size_t length,
                                         size_t *count);
```

Counts Unicode codepoints in exactly `length` bytes.

Returns:

- `LEO_UTF8_OK` on success
    
- `LEO_UTF8_INVALID` if `bytes` or `count` is `NULL`
    
- `LEO_UTF8_INVALID` if the input is malformed UTF-8
    

On success, `*count` receives the number of Unicode codepoints.

Important:

```text
Codepoint count is not grapheme-cluster count.
```

For example, a decomposed character such as `o` + combining diaeresis counts as  
two codepoints, not one user-perceived character.

### NFC Normalization

```c
LeoUTF8Status LeoUTF8NormalizeNFCBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen);
```

Produces a newly allocated NFC-normalized UTF-8 byte string.

### NFD Normalization

```c
LeoUTF8Status LeoUTF8NormalizeNFDBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen);
```

Produces a newly allocated NFD-normalized UTF-8 byte string.

### Case Folding

```c
LeoUTF8Status LeoUTF8CaseFoldBytes(const unsigned char *bytes,
                                   size_t length,
                                   unsigned char **out,
                                   size_t *outLen);
```

Produces a newly allocated Unicode-casefolded UTF-8 byte string.

Case folding is not locale-specific lowercase conversion.

Example:

```text
Straße -> strasse
```

LeoUTF8Core does not currently provide locale-specific casing.

## Null-Terminated Convenience API

The convenience API accepts null-terminated UTF-8 byte strings:

```c
const unsigned char *text
```

These functions are wrappers around the byte-length core API and determine the  
input length with `strlen`.

```c
LeoUTF8Status LeoUTF8Validate(const unsigned char *text);

LeoUTF8Status LeoUTF8CodepointCount(const unsigned char *text,
                                    size_t *count);

LeoUTF8Status LeoUTF8NormalizeNFC(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen);

LeoUTF8Status LeoUTF8NormalizeNFD(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen);

LeoUTF8Status LeoUTF8CaseFold(const unsigned char *text,
                              unsigned char **out,
                              size_t *outLen);
```

Use these only when the input is known to be a normal C string.

For system boundaries, file buffers, network data, Rexx buffers, or other  
byte sequences, prefer the byte-length API.

## Status Codes

```c
typedef enum LeoUTF8Status {
    LEO_UTF8_OK = 0,
    LEO_UTF8_INVALID = 1,
    LEO_UTF8_NO_MEMORY = 2,
    LEO_UTF8_ERROR = 3
} LeoUTF8Status;
```

### Meaning

- `LEO_UTF8_OK`  
    Operation completed successfully.
    
- `LEO_UTF8_INVALID`  
    Input was `NULL`, malformed UTF-8, or otherwise invalid for the requested operation.
    
- `LEO_UTF8_NO_MEMORY`  
    Allocation failed inside the UTF-8 engine.
    
- `LEO_UTF8_ERROR`  
    Generic fallback for unexpected implementation-level errors.
    

## Status Messages

```c
const char *LeoUTF8StatusMessage(LeoUTF8Status status);
```

Returns a stable English diagnostic string for a LeoUTF8 status code.

These strings are meant for diagnostics and logs. They are not localized.

## Version

```c
const char *LeoUTF8Version(void);
```

Returns the UTF-8 engine version currently used by LeoUTF8Core.

At the initial proof stage this returns the vendored utf8proc version.

## Output Ownership

The following functions allocate output:

```c
LeoUTF8NormalizeNFCBytes(...)
LeoUTF8NormalizeNFDBytes(...)
LeoUTF8CaseFoldBytes(...)

LeoUTF8NormalizeNFC(...)
LeoUTF8NormalizeNFD(...)
LeoUTF8CaseFold(...)
```

On success:

```c
*out
```

points to a newly allocated null-terminated UTF-8 byte string.

The caller owns this memory and must release it with:

```c
LeoUTF8Free(*out);
```

## Output Length

If `outLen` is not `NULL`, it receives the byte length of the output string,  
excluding the terminating NUL byte.

## Failure Behavior

On failure:

```c
*out == NULL
```

and, if `outLen` is provided:

```c
*outLen == 0
```

This is part of the API contract.

## Memory Release

```c
void LeoUTF8Free(unsigned char *text);
```

Releases memory returned by LeoUTF8Core output functions.

It is safe to call with `NULL`.

Any string returned by LeoUTF8Core must be released with `LeoUTF8Free`, not  
with `free`.

## Confirmed Operations

The current Leopard/PPC probe confirms:

- valid UTF-8 validation
    
- invalid UTF-8 rejection
    
- NULL input rejection
    
- embedded NUL byte validation through byte-length API
    
- embedded NUL codepoint counting through byte-length API
    
- codepoint counting
    
- NFC normalization
    
- NFD normalization
    
- Unicode case folding
    
- output cleanup on invalid input
    
- static linking against vendored utf8proc
    
- execution on Mac OS X 10.5.8 PowerPC
    

## Non-Goals

LeoUTF8Core does not currently provide:

- NSString APIs
    
- CFString APIs
    
- grapheme-cluster segmentation
    
- Unicode collation
    
- regex
    
- locale-specific case conversion
    
- implicit conversion of all strings
    
- automatic normalization at API boundaries
    
- replacement for Foundation, CoreFoundation, or NSString
    

## Future Layers

LeoUTF8Core is intended to sit below:

```text
LeoUTF8Foundation
LeoUTF8CLI
LeooRexx UTF-8 helpers
LeoTerm / LeoBay / LeoFeed / LeoGPT text bridges
```

The intended architecture remains:

```text
vendor/utf8proc
  -> LeoUTF8Core
  -> Foundation / CLI / Rexx / app-specific bridges
```

## Current Core Doctrine

```text
Byte-length API is the core.
C-string API is convenience.
Foundation/NSString comes later.
```
