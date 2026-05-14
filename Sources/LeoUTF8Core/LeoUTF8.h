#ifndef LEO_UTF8_H
#define LEO_UTF8_H

/*!
    @header LeoUTF8
    @abstract C-level UTF-8 boundary API for LeoUTF8.
    @discussion
        LeoUTF8Core provides the C-level UTF-8 boundary surface for the
        LeoUTF8 integration brick.

        The byte-length API is the preferred public API. It accepts explicit
        buffers and lengths and can therefore handle embedded NUL bytes.

        The NUL-terminated C-string API is convenience only and delegates to the
        byte-length API.

        LeoUTF8Core does not silently normalize input. Normalization and case
        folding are explicit operations.

        Buffers returned by LeoUTF8Core allocation functions are owned by the
        caller and must be released with LeoUTF8Free().
 */

#include "LeoUTF8Version.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
    @enum LeoUTF8Status
    @abstract Status values returned by LeoUTF8Core functions.
    @discussion
        LeoUTF8Core functions return a LeoUTF8Status value instead of relying on
        errno or exceptions.

        LEO_UTF8_OK indicates success.

        All non-OK values indicate failure. Use LeoUTF8StatusMessage() to obtain
        a stable human-readable status string for diagnostics.
 */
typedef enum LeoUTF8Status {
    /*!
        @constant LEO_UTF8_OK
        The operation completed successfully.
     */
    LEO_UTF8_OK = 0,

    /*!
        @constant LEO_UTF8_INVALID
        Input was NULL, malformed, incomplete, or otherwise invalid for the
        requested operation.
     */
    LEO_UTF8_INVALID = 1,

    /*!
        @constant LEO_UTF8_NO_MEMORY
        Memory allocation failed.
     */
    LEO_UTF8_NO_MEMORY = 2,

    /*!
        @constant LEO_UTF8_ERROR
        A non-specific internal error occurred.
     */
    LEO_UTF8_ERROR = 3
} LeoUTF8Status;

/*!
    @function LeoUTF8Version
    @abstract Returns the underlying UTF-8 engine version.
    @discussion
        Returns the version of the underlying UTF-8 processing engine used by
        LeoUTF8Core.

        This is not the LeoUTF8 brick version. Use
        LeoUTF8BrickVersionString() for the LeoUTF8 project-level brick version.

        The returned string is static and must not be freed.

    @result
        A pointer to a static NUL-terminated engine version string.
 */
const char *LeoUTF8Version(void);

/*!
    @function LeoUTF8StatusMessage
    @abstract Returns a human-readable message for a LeoUTF8Status value.
    @discussion
        Returns a stable diagnostic string for a LeoUTF8Status value.

        The returned string is static and must not be freed.

    @param status
        The LeoUTF8Status value to describe.

    @result
        A pointer to a static NUL-terminated diagnostic string.
 */
const char *LeoUTF8StatusMessage(LeoUTF8Status status);

/*!
    @function LeoUTF8ValidateBytes
    @abstract Validates an explicit UTF-8 byte buffer.
    @discussion
        Validates exactly length bytes.

        This function does not assume NUL-terminated input and does not
        normalize the input buffer.

        Embedded NUL bytes are allowed when the surrounding UTF-8 byte sequence
        is otherwise valid.

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to validate.

    @result
        LEO_UTF8_OK if the byte buffer is valid UTF-8, otherwise a LeoUTF8Status
        error value.
 */
LeoUTF8Status LeoUTF8ValidateBytes(const unsigned char *bytes,
                                   size_t length);

/*!
    @function LeoUTF8CodepointCountBytes
    @abstract Counts Unicode codepoints in an explicit UTF-8 byte buffer.
    @discussion
        Counts the Unicode codepoints represented by exactly length bytes.

        This function does not assume NUL-terminated input and does not
        normalize the input buffer.

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to inspect.

    @param count
        On success, receives the number of Unicode codepoints found in the
        buffer. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CodepointCountBytes(const unsigned char *bytes,
                                         size_t length,
                                         size_t *count);

/*!
    @function LeoUTF8NormalizeNFCBytes
    @abstract Normalizes an explicit UTF-8 byte buffer to NFC.
    @discussion
        Validates and normalizes exactly length bytes to Unicode NFC.

        This function does not assume NUL-terminated input.

        On success, out receives a newly allocated NUL-terminated UTF-8 byte
        buffer and outLen receives its byte length excluding the terminating
        NUL.

        The caller owns the returned buffer and must release it with
        LeoUTF8Free().

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to normalize.

    @param out
        On success, receives a newly allocated UTF-8 buffer. This pointer must
        not be NULL.

    @param outLen
        On success, receives the byte length of the returned buffer excluding
        the terminating NUL. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8NormalizeNFCBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen);

/*!
    @function LeoUTF8NormalizeNFDBytes
    @abstract Normalizes an explicit UTF-8 byte buffer to NFD.
    @discussion
        Validates and normalizes exactly length bytes to Unicode NFD.

        This function does not assume NUL-terminated input.

        On success, out receives a newly allocated NUL-terminated UTF-8 byte
        buffer and outLen receives its byte length excluding the terminating
        NUL.

        The caller owns the returned buffer and must release it with
        LeoUTF8Free().

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to normalize.

    @param out
        On success, receives a newly allocated UTF-8 buffer. This pointer must
        not be NULL.

    @param outLen
        On success, receives the byte length of the returned buffer excluding
        the terminating NUL. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8NormalizeNFDBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen);

/*!
    @function LeoUTF8CaseFoldBytes
    @abstract Applies Unicode case folding to an explicit UTF-8 byte buffer.
    @discussion
        Validates and case-folds exactly length bytes.

        This is a Unicode case-folding operation. It is not HFS+ filename
        comparison policy and must not be used as a substitute for measured
        HFS+ filename comparison behavior.

        On success, out receives a newly allocated NUL-terminated UTF-8 byte
        buffer and outLen receives its byte length excluding the terminating
        NUL.

        The caller owns the returned buffer and must release it with
        LeoUTF8Free().

    @param bytes
        Pointer to the UTF-8 byte buffer.

    @param length
        Number of bytes to case-fold.

    @param out
        On success, receives a newly allocated UTF-8 buffer. This pointer must
        not be NULL.

    @param outLen
        On success, receives the byte length of the returned buffer excluding
        the terminating NUL. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CaseFoldBytes(const unsigned char *bytes,
                                   size_t length,
                                   unsigned char **out,
                                   size_t *outLen);

/*!
    @function LeoUTF8Validate
    @abstract Validates a NUL-terminated UTF-8 C string.
    @discussion
        Convenience wrapper around LeoUTF8ValidateBytes().

        This function treats text as a NUL-terminated C string and therefore
        cannot be used to validate buffers containing embedded NUL bytes.

        Prefer LeoUTF8ValidateBytes() when an explicit byte length is available.

    @param text
        Pointer to a NUL-terminated UTF-8 C string.

    @result
        LEO_UTF8_OK if text is valid UTF-8, otherwise a LeoUTF8Status error
        value.
 */
LeoUTF8Status LeoUTF8Validate(const unsigned char *text);

/*!
    @function LeoUTF8CodepointCount
    @abstract Counts Unicode codepoints in a NUL-terminated UTF-8 C string.
    @discussion
        Convenience wrapper around LeoUTF8CodepointCountBytes().

        This function treats text as a NUL-terminated C string and therefore
        cannot be used to count buffers containing embedded NUL bytes.

        Prefer LeoUTF8CodepointCountBytes() when an explicit byte length is
        available.

    @param text
        Pointer to a NUL-terminated UTF-8 C string.

    @param count
        On success, receives the number of Unicode codepoints in text. This
        pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CodepointCount(const unsigned char *text, size_t *count);

/*!
    @function LeoUTF8NormalizeNFC
    @abstract Normalizes a NUL-terminated UTF-8 C string to NFC.
    @discussion
        Convenience wrapper around LeoUTF8NormalizeNFCBytes().

        This function treats text as a NUL-terminated C string and therefore
        cannot be used to normalize buffers containing embedded NUL bytes.

        On success, out receives a newly allocated NUL-terminated UTF-8 byte
        buffer and outLen receives its byte length excluding the terminating
        NUL.

        The caller owns the returned buffer and must release it with
        LeoUTF8Free().

    @param text
        Pointer to a NUL-terminated UTF-8 C string.

    @param out
        On success, receives a newly allocated UTF-8 buffer. This pointer must
        not be NULL.

    @param outLen
        On success, receives the byte length of the returned buffer excluding
        the terminating NUL. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8NormalizeNFC(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen);

/*!
    @function LeoUTF8NormalizeNFD
    @abstract Normalizes a NUL-terminated UTF-8 C string to NFD.
    @discussion
        Convenience wrapper around LeoUTF8NormalizeNFDBytes().

        This function treats text as a NUL-terminated C string and therefore
        cannot be used to normalize buffers containing embedded NUL bytes.

        On success, out receives a newly allocated NUL-terminated UTF-8 byte
        buffer and outLen receives its byte length excluding the terminating
        NUL.

        The caller owns the returned buffer and must release it with
        LeoUTF8Free().

    @param text
        Pointer to a NUL-terminated UTF-8 C string.

    @param out
        On success, receives a newly allocated UTF-8 buffer. This pointer must
        not be NULL.

    @param outLen
        On success, receives the byte length of the returned buffer excluding
        the terminating NUL. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8NormalizeNFD(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen);

/*!
    @function LeoUTF8CaseFold
    @abstract Applies Unicode case folding to a NUL-terminated UTF-8 C string.
    @discussion
        Convenience wrapper around LeoUTF8CaseFoldBytes().

        This function treats text as a NUL-terminated C string and therefore
        cannot be used to case-fold buffers containing embedded NUL bytes.

        This is a Unicode case-folding operation. It is not HFS+ filename
        comparison policy and must not be used as a substitute for measured
        HFS+ filename comparison behavior.

        On success, out receives a newly allocated NUL-terminated UTF-8 byte
        buffer and outLen receives its byte length excluding the terminating
        NUL.

        The caller owns the returned buffer and must release it with
        LeoUTF8Free().

    @param text
        Pointer to a NUL-terminated UTF-8 C string.

    @param out
        On success, receives a newly allocated UTF-8 buffer. This pointer must
        not be NULL.

    @param outLen
        On success, receives the byte length of the returned buffer excluding
        the terminating NUL. This pointer must not be NULL.

    @result
        LEO_UTF8_OK on success, otherwise a LeoUTF8Status error value.
 */
LeoUTF8Status LeoUTF8CaseFold(const unsigned char *text,
                              unsigned char **out,
                              size_t *outLen);

/*!
    @function LeoUTF8Free
    @abstract Releases a buffer allocated by LeoUTF8Core.
    @discussion
        Releases a buffer returned by LeoUTF8Core allocation functions such as
        LeoUTF8NormalizeNFCBytes(), LeoUTF8NormalizeNFDBytes(),
        LeoUTF8CaseFoldBytes(), and their C-string convenience wrappers.

        Passing NULL is allowed.

        Do not use free() directly on LeoUTF8-allocated buffers. Use
        LeoUTF8Free() so ownership stays within the LeoUTF8Core contract.

    @param text
        Buffer returned by a LeoUTF8Core allocation function, or NULL.
 */
void LeoUTF8Free(unsigned char *text);

#ifdef __cplusplus
}
#endif

#endif
