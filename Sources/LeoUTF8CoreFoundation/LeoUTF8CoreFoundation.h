#ifndef LEO_UTF8_CORE_FOUNDATION_H
#define LEO_UTF8_CORE_FOUNDATION_H

#include <CoreFoundation/CoreFoundation.h>

#include "LeoUTF8.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Creates a retained CFStringRef from an explicit UTF-8 byte buffer.
 *
 * On success, *outString receives a retained CFStringRef.
 * The caller owns it and must release it with CFRelease.
 */
LeoUTF8Status LeoUTF8CreateCFStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             CFStringRef *outString);

/*
 * Creates retained CFDataRef containing the UTF-8 representation of a CFStringRef.
 *
 * On success, *outData receives retained CFDataRef.
 * The caller owns it and must release it with CFRelease.
 */
LeoUTF8Status LeoUTF8CopyUTF8DataFromCFString(CFStringRef string,
                                              CFDataRef *outData);

#ifdef __cplusplus
}
#endif

#endif
