# Consuming LeoUTF8 From Other Leopard Projects

This document describes how other Leopard-native projects can consume a staged
LeoUTF8 distribution.

The intended distribution layout is created with:

```sh
make dist
````

Result:

```text
dist/LeoUTF8/
├── include/
│   ├── LeoUTF8.h
│   └── LeoUTF8Foundation.h
└── lib/
    ├── libLeoUTF8Core.a
    └── libLeoUTF8Foundation.a
```

## Core Consumer

Use LeoUTF8Core when a C or C++ project only needs explicit UTF-8 byte handling:

- validation
    
- codepoint counting
    
- NFC normalization
    
- NFD normalization
    
- Unicode case folding
    

### Include

```c
#include "LeoUTF8.h"
```

### Compile And Link

```sh
/usr/bin/gcc \
  -Os -arch ppc -mmacosx-version-min=10.5 \
  -I dist/LeoUTF8/include \
  your_tool.c \
  dist/LeoUTF8/lib/libLeoUTF8Core.a \
  -o your_tool
```

### Link Order

For Core-only consumers:

```text
object files
libLeoUTF8Core.a
```

`libLeoUTF8Core.a` already contains the vendored utf8proc object code.

Consumers do not link against `vendor/utf8proc` directly.

## Foundation Consumer

Use LeoUTF8Foundation when an Objective-C / Foundation project needs to bridge  
between explicit UTF-8 bytes and Leopard-native Foundation objects.

LeoUTF8Foundation currently provides:

- UTF-8 bytes -> retained NSString
    
- NSString -> retained UTF-8 NSData
    

### Include

```objc
#import "LeoUTF8Foundation.h"
```

### Compile And Link

```sh
/usr/bin/gcc \
  -Os -arch ppc -mmacosx-version-min=10.5 \
  -I dist/LeoUTF8/include \
  your_foundation_tool.m \
  dist/LeoUTF8/lib/libLeoUTF8Foundation.a \
  dist/LeoUTF8/lib/libLeoUTF8Core.a \
  -framework Foundation \
  -o your_foundation_tool
```

### Link Order

For Foundation consumers:

```text
object files
libLeoUTF8Foundation.a
libLeoUTF8Core.a
-framework Foundation
```

The order matters.

`libLeoUTF8Foundation.a` depends on symbols from `libLeoUTF8Core.a`.

## Ownership Rules

LeoUTF8Core allocation ownership:

```c
unsigned char *out = 0;
size_t outLen = 0;

if (LeoUTF8NormalizeNFCBytes(bytes, length, &out, &outLen) == LEO_UTF8_OK) {
    /* use out */
    LeoUTF8Free(out);
}
```

Any memory returned by LeoUTF8Core must be released with:

```c
LeoUTF8Free(...)
```

LeoUTF8Foundation ownership follows manual Leopard retain/release rules:

```objc
NSString *string = nil;

if (LeoUTF8CreateNSStringFromBytes(bytes, length, &string) == LEO_UTF8_OK) {
    /* use string */
    [string release];
}
```

Returned `NSString` and `NSData` objects are retained.

Consumers must release them.

## Boundary Policy

LeoUTF8 is a boundary brick.

Use it when data crosses between:

- UTF-8 byte buffers
    
- C/POSIX code
    
- Rexx or scripting interfaces
    
- Foundation objects
    
- application-level text handling
    

Do not use LeoUTF8 as a replacement for `NSString`.

Do not bypass LeoUTF8Core validation when turning external UTF-8 bytes into  
Foundation objects.

## Recommended Consumer Pattern

For a Leopard-native application:

```text
external bytes / files / network / Rexx
  -> LeoUTF8Core validation / normalization if needed
  -> LeoUTF8Foundation
  -> NSString / NSData
  -> native app logic
```

For byte-oriented tools:

```text
external bytes
  -> LeoUTF8Core
  -> explicit UTF-8 result bytes
```

## Reproducible Consumer Check

LeoUTF8 includes a distribution consumer check:

```sh
make distcheck
```

This builds and runs:

```text
Tests/consumer_core_probe.c
Tests/consumer_foundation_probe.m
```

The consumer checks use only:

```text
dist/LeoUTF8/include
dist/LeoUTF8/lib
```

This proves that LeoUTF8 can be consumed as an external brick.  

