# LeoUTF8 Architecture

LeoUTF8 has three layers:

1. LeoUTF8Core
   - C API
   - UTF-8 validation
   - normalization helpers
   - case folding helpers
   - codepoint counting

2. LeoUTF8Foundation
   - CFStringRef bridge
   - NSString bridge
   - explicit UTF-8 import/export

3. LeoUTF8CLI
   - command-line validation and test tool
   - useful for scripts, LeooRexx, and diagnostics

LeoUTF8 is not a text framework replacement.
It is a small capability brick that connects UTF-8 byte-oriented worlds with
Leopard's native Unicode string facilities.
