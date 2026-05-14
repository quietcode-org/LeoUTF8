# utf8proc Adoption Policy

utf8proc may be used as the UTF-8 processing engine for LeoUTF8 if it can be
built cleanly on Mac OS X 10.5.8 PowerPC with the native Leopard toolchain.

## Rules

- utf8proc source lives under vendor/ as unchanged upstream reference.
- LeoUTF8 project code must live outside vendor/.
- No MacPorts runtime dependency is required for LeoUTF8.
- Any local compatibility glue belongs in Sources/, not vendor/.
- Only the needed utf8proc functionality is exposed.
- LeoUTF8 must remain small and explicit.

## Initial Target Functions

- validate
- normalize NFC
- normalize NFD
- casefold
- codepoint count
