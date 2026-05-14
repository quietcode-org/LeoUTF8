# LeoUTF8

LeoUTF8 is a Leopard-native UTF-8 capability brick for Mac OS X 10.5.8 PowerPC.

It provides a small, explicit bridge between UTF-8 byte strings, C APIs,
CoreFoundation, Foundation, and Leopard-native applications.

LeoUTF8 does not replace NSString, CFString, or the Leopard text system.
It complements them by making UTF-8 validation, normalization, and conversion
available in a small reusable system component.

## Design Rule

Use Leopard-native text facilities where they already exist.
Use utf8proc only for the narrowly defined UTF-8 operations Leopard does not
expose as a small C-level capability.

No ICU clone. No framework sprawl. No hidden global string semantics.
