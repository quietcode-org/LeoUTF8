# LeoUTF8 Version API Contract

LeoUTF8 exposes two distinct version concepts:

- the underlying UTF-8 engine version
- the LeoUTF8 brick version

These must not be confused.

## Engine Version

~~~c
const char *LeoUTF8Version(void);
~~~

Returns the version of the underlying UTF-8 engine used by LeoUTF8Core.

Current observed value:

~~~text
2.11.3
~~~

This currently reflects the vendored utf8proc version.

Use this when diagnosing the Unicode processing backend.

Do not use this as the LeoUTF8 project or release version.

## Brick Version

~~~c
const char *LeoUTF8BrickVersionString(void);
~~~

Returns the LeoUTF8 brick version.

Current value:

~~~text
0.7.0
~~~

This is the LeoUTF8 project-level version.

Use this when identifying the LeoUTF8 integration brick itself.

## Release Name

~~~c
const char *LeoUTF8BrickReleaseName(void);
~~~

Returns the release name for the current LeoUTF8 brick version.

Current value:

~~~text
Integration Brick Hardening
~~~

## Target Platform

~~~c
const char *LeoUTF8BrickTargetPlatform(void);
~~~

Returns the primary target platform for this LeoUTF8 brick build.

Current value:

~~~text
Mac OS X 10.5.8 Leopard PowerPC
~~~

## Public Constants

~~~c
#define LEO_UTF8_BRICK_VERSION_MAJOR 0
#define LEO_UTF8_BRICK_VERSION_MINOR 7
#define LEO_UTF8_BRICK_VERSION_PATCH 0

#define LEO_UTF8_BRICK_VERSION_STRING "0.7.0"
#define LEO_UTF8_BRICK_RELEASE_NAME "Integration Brick Hardening"
#define LEO_UTF8_BRICK_TARGET_PLATFORM "Mac OS X 10.5.8 Leopard PowerPC"
~~~

## Public Header

~~~c
#include "LeoUTF8Version.h"
~~~

`LeoUTF8.h` includes `LeoUTF8Version.h`, so most Core consumers can include only:

~~~c
#include "LeoUTF8.h"
~~~

## Version Policy

LeoUTF8 uses separate version identities intentionally:

~~~text
LeoUTF8Version()
  -> engine / utf8proc-adjacent version

LeoUTF8BrickVersionString()
  -> LeoUTF8 project brick version
~~~

This prevents confusion between upstream UTF-8 processing code and the
Leopard-Crew integration brick.

## Compatibility Rule

External consumers should use the brick version to decide whether a LeoUTF8
distribution provides the expected public brick surface.

External consumers should use the engine version only when they need to diagnose
Unicode backend behavior.

## Current V0.7 Meaning

Brick version `0.7.0` means:

- LeoUTF8 is in Integration Brick Hardening
- public version metadata exists
- static libraries remain the supported distribution form
- Core, CoreFoundation, and Foundation bridge layers are present
- HFS+ research exists but is not public API
