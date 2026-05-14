#ifndef LEO_UTF8_CORE_FOUNDATION_H
#define LEO_UTF8_CORE_FOUNDATION_H

/*!
    @header LeoUTF8CoreFoundation
    @abstract CoreFoundation bridge for LeoUTF8Core.
    @discussion
        LeoUTF8CoreFoundation provides the CoreFoundation bridge for the
        LeoUTF8 integration brick.

        It converts explicit UTF-8 byte buffers to CFStringRef and CFStringRef
        values back to UTF-8 CFDataRef values.

        This layer follows CoreFoundation Create/Copy ownership rules.

        Returned CoreFoundation objects are owned by the caller and must be
        released with CFRelease().
 */

#include <CoreFoundation/CoreFoundation.h>

#include "LeoUTF8.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
    @function LeoUTF8CreateCFStringFromBytes
    @abstract Creates a retained CFStringRef from an explicit UTF-8 byte buffer.
    @discussion
        Validates exactly length bytes as UTF-8 and creates a CoreFoundation
        string from that byte buffer.

        This function does not assume NUL-terminated input.

        Embedded NUL bytes are accepted when the surrounding UTF-8 byte sequence
        is otherwise valid.

        On success, outString receives a retained CFStringRef. The caller owns
        the returned object and must release it with CFRelease().

        On failure, no retained CFStringRef is returned.

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to read from bytes.

    @param outString
        On success, receives a retained CFStringRef. This pointer must not be
        NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CreateCFStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             CFStringRef *outString);

/*!
    @function LeoUTF8CopyUTF8DataFromCFString
    @abstract Copies the UTF-8 representation of a CFStringRef into CFDataRef.
    @discussion
        Converts a CFStringRef to an explicit UTF-8 byte container.

        On success, outData receives a retained CFDataRef containing the UTF-8
        bytes for the supplied string.

        The caller owns the returned object and must release it with
        CFRelease().

        The returned CFDataRef is byte storage, not a string object.

    @param string
        Source CFStringRef. This value must not be NULL.

    @param outData
        On success, receives a retained CFDataRef containing UTF-8 bytes. This
        pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CopyUTF8DataFromCFString(CFStringRef string,
                                              CFDataRef *outData);

#ifdef __cplusplus
}
#endif

#endif
