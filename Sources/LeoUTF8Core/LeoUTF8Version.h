#ifndef LEO_UTF8_VERSION_H
#define LEO_UTF8_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
    @header LeoUTF8Version
    @abstract LeoUTF8 brick version metadata.
    @discussion
        This header exposes the LeoUTF8 project-level brick version.

        LeoUTF8 deliberately separates the underlying UTF-8 engine version from
        the LeoUTF8 brick version.

        LeoUTF8Version() reports the underlying UTF-8 engine version.

        LeoUTF8BrickVersionString() reports the LeoUTF8 integration brick
        version.

        This distinction is part of the V0.7 Integration Brick Hardening phase.
 */

#define LEO_UTF8_BRICK_VERSION_MAJOR 0
#define LEO_UTF8_BRICK_VERSION_MINOR 7
#define LEO_UTF8_BRICK_VERSION_PATCH 2

#define LEO_UTF8_BRICK_VERSION_STRING "0.7.2"
#define LEO_UTF8_BRICK_RELEASE_NAME "Integration Brick Hardening"
#define LEO_UTF8_BRICK_TARGET_PLATFORM "Mac OS X 10.5.8 Leopard PowerPC"

/*!
    @function LeoUTF8BrickVersionString
    @abstract Returns the LeoUTF8 brick version string.
    @discussion
        Returns the project-level LeoUTF8 brick version.

        This is not the underlying UTF-8 engine version. Use this function when
        identifying the LeoUTF8 integration brick surface exposed to external
        consumers.

        The returned string is static and must not be freed.

    @result
        A pointer to a static NUL-terminated version string.
 */
const char *LeoUTF8BrickVersionString(void);

/*!
    @function LeoUTF8BrickReleaseName
    @abstract Returns the LeoUTF8 release name.
    @discussion
        Returns the human-readable release name for the current LeoUTF8 brick
        version.

        The returned string is static and must not be freed.

    @result
        A pointer to a static NUL-terminated release name string.
 */
const char *LeoUTF8BrickReleaseName(void);

/*!
    @function LeoUTF8BrickTargetPlatform
    @abstract Returns the primary LeoUTF8 target platform.
    @discussion
        Returns the primary target platform for this LeoUTF8 brick build.

        LeoUTF8 V0.7 targets Mac OS X 10.5.8 Leopard on PowerPC.

        The returned string is static and must not be freed.

    @result
        A pointer to a static NUL-terminated target platform string.
 */
const char *LeoUTF8BrickTargetPlatform(void);

#ifdef __cplusplus
}
#endif

#endif
