#ifndef LEO_UTF8_FOUNDATION_H
#define LEO_UTF8_FOUNDATION_H

/*!
    @header LeoUTF8Foundation
    @abstract Foundation bridge for LeoUTF8Core.
    @discussion
        LeoUTF8Foundation provides the Foundation bridge for the LeoUTF8
        integration brick.

        It converts explicit UTF-8 byte buffers to NSString objects and
        NSString values back to UTF-8 NSData values.

        This layer follows Mac OS X 10.5.8 Leopard manual retain/release
        ownership rules.

        Returned Objective-C objects are owned by the caller and must be
        released by the caller. No ARC assumptions are made.
 */

#import <Foundation/Foundation.h>

#include "LeoUTF8.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
    @function LeoUTF8CreateNSStringFromBytes
    @abstract Creates a retained NSString from an explicit UTF-8 byte buffer.
    @discussion
        Validates exactly length bytes as UTF-8 and creates a Foundation string
        from that byte buffer.

        This function does not assume NUL-terminated input.

        Embedded NUL bytes are accepted when the surrounding UTF-8 byte sequence
        is otherwise valid.

        On success, outString receives a retained NSString. The caller owns the
        returned object and must release it under Leopard manual retain/release
        rules.

        On failure, no retained NSString is returned.

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to read from bytes.

    @param outString
        On success, receives a retained NSString. This pointer must not be
        NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CreateNSStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             NSString **outString);

/*!
    @function LeoUTF8CopyUTF8DataFromNSString
    @abstract Copies the UTF-8 representation of an NSString into NSData.
    @discussion
        Converts an NSString to an explicit UTF-8 byte container.

        On success, outData receives a retained NSData object containing the
        UTF-8 bytes for the supplied string.

        The caller owns the returned object and must release it under Leopard
        manual retain/release rules.

        The returned NSData is byte storage, not a string object.

    @param string
        Source NSString. This value must not be nil.

    @param outData
        On success, receives a retained NSData object containing UTF-8 bytes.
        This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CopyUTF8DataFromNSString(NSString *string,
                                              NSData **outData);

#ifdef __cplusplus
}
#endif

#endif
