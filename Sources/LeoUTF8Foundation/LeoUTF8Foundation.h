#ifndef LEO_UTF8_FOUNDATION_H
#define LEO_UTF8_FOUNDATION_H

#import <Foundation/Foundation.h>

#include "LeoUTF8.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Creates a retained NSString from an explicit UTF-8 byte buffer.
 *
 * On success, *outString receives a retained NSString.
 * The caller owns it and must release it.
 */
LeoUTF8Status LeoUTF8CreateNSStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             NSString **outString);

/*
 * Creates retained NSData containing the UTF-8 representation of an NSString.
 *
 * On success, *outData receives retained NSData.
 * The caller owns it and must release it.
 */
LeoUTF8Status LeoUTF8CopyUTF8DataFromNSString(NSString *string,
                                              NSData **outData);

#ifdef __cplusplus
}
#endif

#endif
