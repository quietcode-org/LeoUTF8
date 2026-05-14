#ifndef LEO_UTF8_VERSION_H
#define LEO_UTF8_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define LEO_UTF8_BRICK_VERSION_MAJOR 0
#define LEO_UTF8_BRICK_VERSION_MINOR 7
#define LEO_UTF8_BRICK_VERSION_PATCH 0

#define LEO_UTF8_BRICK_VERSION_STRING "0.7.0"
#define LEO_UTF8_BRICK_RELEASE_NAME "Integration Brick Hardening"
#define LEO_UTF8_BRICK_TARGET_PLATFORM "Mac OS X 10.5.8 Leopard PowerPC"

/*
 * Returns the LeoUTF8 brick version.
 *
 * This is the LeoUTF8 project version, not the underlying utf8proc engine version.
 */
const char *LeoUTF8BrickVersionString(void);

/*
 * Returns the LeoUTF8 release name for the current brick version.
 */
const char *LeoUTF8BrickReleaseName(void);

/*
 * Returns the primary target platform for this LeoUTF8 brick build.
 */
const char *LeoUTF8BrickTargetPlatform(void);

#ifdef __cplusplus
}
#endif

#endif
