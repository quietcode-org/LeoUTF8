# LeoUTF8 Scope Lock

LeoUTF8 is a small Leopard/PPC UTF-8 capability brick.

## In Scope

- UTF-8 validation
- UTF-8 normalization helpers
- UTF-8 case folding helpers
- UTF-8 codepoint counting
- C API for UTF-8 byte strings
- CoreFoundation bridge
- Foundation / NSString bridge
- Small CLI test tool
- Reusable use by LeooRexx, LeoTerm, LeoBay, LeoMail, LeoLibrary and related projects

## Out of Scope

- Replacing NSString
- Replacing CFString
- Reimplementing ICU
- Unicode collation beyond minimal explicit helpers
- Regex
- Locale policy engine
- Automatic semantic changes to Rexx string functions
- MacPorts dependency requirement
- Hidden global encoding conversion

## Doctrine

Bytes remain bytes.
UTF-8 is explicit.
NSString and CFString remain the native Leopard text model.
