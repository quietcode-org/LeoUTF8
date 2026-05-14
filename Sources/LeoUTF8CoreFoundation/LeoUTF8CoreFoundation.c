#include "LeoUTF8CoreFoundation.h"

#include <limits.h>
#include <stdlib.h>

static LeoUTF8Status LeoUTF8SizeToCFIndex(size_t length, CFIndex *outLength)
{
    if (outLength == 0) {
        return LEO_UTF8_INVALID;
    }

    if (length > (size_t)LONG_MAX) {
        return LEO_UTF8_ERROR;
    }

    *outLength = (CFIndex)length;
    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8CreateCFStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             CFStringRef *outString)
{
    LeoUTF8Status status;
    CFIndex cfLength;
    CFStringRef string;

    if (outString == 0) {
        return LEO_UTF8_INVALID;
    }

    *outString = 0;

    if (bytes == 0) {
        return LEO_UTF8_INVALID;
    }

    status = LeoUTF8ValidateBytes(bytes, length);
    if (status != LEO_UTF8_OK) {
        return status;
    }

    status = LeoUTF8SizeToCFIndex(length, &cfLength);
    if (status != LEO_UTF8_OK) {
        return status;
    }

    string = CFStringCreateWithBytes(kCFAllocatorDefault,
                                     bytes,
                                     cfLength,
                                     kCFStringEncodingUTF8,
                                     false);

    if (string == 0) {
        return LEO_UTF8_ERROR;
    }

    *outString = string;
    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8CopyUTF8DataFromCFString(CFStringRef string,
                                              CFDataRef *outData)
{
    CFIndex charCount;
    CFIndex usedBytes;
    CFIndex convertedChars;
    UInt8 *buffer;
    CFDataRef data;

    if (outData == 0) {
        return LEO_UTF8_INVALID;
    }

    *outData = 0;

    if (string == 0) {
        return LEO_UTF8_INVALID;
    }

    charCount = CFStringGetLength(string);
    usedBytes = 0;

    convertedChars = CFStringGetBytes(string,
                                      CFRangeMake(0, charCount),
                                      kCFStringEncodingUTF8,
                                      0,
                                      false,
                                      0,
                                      0,
                                      &usedBytes);

    if (convertedChars != charCount || usedBytes < 0) {
        return LEO_UTF8_ERROR;
    }

    buffer = 0;

    if (usedBytes > 0) {
        buffer = (UInt8 *)malloc((size_t)usedBytes);
        if (buffer == 0) {
            return LEO_UTF8_NO_MEMORY;
        }

        convertedChars = CFStringGetBytes(string,
                                          CFRangeMake(0, charCount),
                                          kCFStringEncodingUTF8,
                                          0,
                                          false,
                                          buffer,
                                          usedBytes,
                                          &usedBytes);

        if (convertedChars != charCount) {
            free(buffer);
            return LEO_UTF8_ERROR;
        }
    }

    data = CFDataCreate(kCFAllocatorDefault, buffer, usedBytes);

    if (buffer != 0) {
        free(buffer);
    }

    if (data == 0) {
        return LEO_UTF8_NO_MEMORY;
    }

    *outData = data;
    return LEO_UTF8_OK;
}
