#ifndef LEO_UTF8_H
#define LEO_UTF8_H

#include "LeoUTF8Version.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LeoUTF8Status {
    LEO_UTF8_OK = 0,
    LEO_UTF8_INVALID = 1,
    LEO_UTF8_NO_MEMORY = 2,
    LEO_UTF8_ERROR = 3
} LeoUTF8Status;

const char *LeoUTF8Version(void);
const char *LeoUTF8StatusMessage(LeoUTF8Status status);

/*
 * Byte-length based core API.
 *
 * These functions are the preferred LeoUTF8Core boundary API.
 * Input may contain embedded NUL bytes.
 */
LeoUTF8Status LeoUTF8ValidateBytes(const unsigned char *bytes,
                                   size_t length);

LeoUTF8Status LeoUTF8CodepointCountBytes(const unsigned char *bytes,
                                         size_t length,
                                         size_t *count);

LeoUTF8Status LeoUTF8NormalizeNFCBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen);

LeoUTF8Status LeoUTF8NormalizeNFDBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen);

LeoUTF8Status LeoUTF8CaseFoldBytes(const unsigned char *bytes,
                                   size_t length,
                                   unsigned char **out,
                                   size_t *outLen);

/*
 * Null-terminated C-string convenience API.
 *
 * These functions are wrappers around the byte-length based API.
 */
LeoUTF8Status LeoUTF8Validate(const unsigned char *text);
LeoUTF8Status LeoUTF8CodepointCount(const unsigned char *text, size_t *count);

LeoUTF8Status LeoUTF8NormalizeNFC(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen);

LeoUTF8Status LeoUTF8NormalizeNFD(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen);

LeoUTF8Status LeoUTF8CaseFold(const unsigned char *text,
                              unsigned char **out,
                              size_t *outLen);

void LeoUTF8Free(unsigned char *text);

#ifdef __cplusplus
}
#endif

#endif
