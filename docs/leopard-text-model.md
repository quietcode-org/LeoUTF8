# Leopard Text Model

Leopard already has native Unicode-capable text facilities through CFString and
NSString.

LeoUTF8 does not replace these facilities.

Instead, LeoUTF8 defines the boundary between:

- UTF-8 byte strings
- C/POSIX code
- Rexx strings
- CFString
- NSString
- HFS+ filename behavior

Project policy:

- Build paths stay ASCII-safe.
- Source files use UTF-8 without BOM.
- User-facing text may use UTF-8.
- Native GUI layers use NSString / CFString.
- UTF-8 semantics are explicit, never hidden.
