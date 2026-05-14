#include "LeoUTF8.h"

#include <limits.h>
#include <string.h>

#include "utf8proc.h"

static LeoUTF8Status LeoUTF8StatusFromUtf8proc(utf8proc_ssize_t rc)
{
    if (rc >= 0) {
        return LEO_UTF8_OK;
    }

    if (rc == UTF8PROC_ERROR_NOMEM) {
        return LEO_UTF8_NO_MEMORY;
    }

    if (rc == UTF8PROC_ERROR_INVALIDUTF8) {
        return LEO_UTF8_INVALID;
    }

    return LEO_UTF8_ERROR;
}

static LeoUTF8Status LeoUTF8ToUtf8procLength(size_t length,
                                             utf8proc_ssize_t *utf8Length)
{
    if (utf8Length == 0) {
        return LEO_UTF8_INVALID;
    }

    if (length > (size_t)LONG_MAX) {
        return LEO_UTF8_ERROR;
    }

    *utf8Length = (utf8proc_ssize_t)length;
    return LEO_UTF8_OK;
}

static LeoUTF8Status LeoUTF8MapBytes(const unsigned char *bytes,
                                     size_t length,
                                     utf8proc_option_t options,
                                     unsigned char **out,
                                     size_t *outLen)
{
    utf8proc_uint8_t *mapped;
    utf8proc_ssize_t utf8Length;
    utf8proc_ssize_t rc;
    LeoUTF8Status status;

    if (out == 0) {
        return LEO_UTF8_INVALID;
    }

    *out = 0;

    if (outLen != 0) {
        *outLen = 0;
    }

    if (bytes == 0) {
        return LEO_UTF8_INVALID;
    }

    status = LeoUTF8ToUtf8procLength(length, &utf8Length);
    if (status != LEO_UTF8_OK) {
        return status;
    }

    mapped = 0;
    rc = utf8proc_map(bytes, utf8Length, &mapped, options);

    if (rc < 0) {
        return LeoUTF8StatusFromUtf8proc(rc);
    }

    *out = mapped;

    if (outLen != 0) {
        *outLen = (size_t)rc;
    }

    return LEO_UTF8_OK;
}

const char *LeoUTF8Version(void)
{
    return utf8proc_version();
}

const char *LeoUTF8StatusMessage(LeoUTF8Status status)
{
    switch (status) {
        case LEO_UTF8_OK:
            return "OK";
        case LEO_UTF8_INVALID:
            return "Invalid UTF-8";
        case LEO_UTF8_NO_MEMORY:
            return "Out of memory";
        case LEO_UTF8_ERROR:
        default:
            return "LeoUTF8 error";
    }
}

LeoUTF8Status LeoUTF8ValidateBytes(const unsigned char *bytes, size_t length)
{
    const unsigned char *p;
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t remaining;
    utf8proc_ssize_t step;
    LeoUTF8Status status;

    if (bytes == 0) {
        return LEO_UTF8_INVALID;
    }

    status = LeoUTF8ToUtf8procLength(length, &remaining);
    if (status != LEO_UTF8_OK) {
        return status;
    }

    p = bytes;

    while (remaining > 0) {
        step = utf8proc_iterate(p, remaining, &codepoint);
        if (step < 0) {
            return LeoUTF8StatusFromUtf8proc(step);
        }

        if (step == 0) {
            return LEO_UTF8_ERROR;
        }

        p += step;
        remaining -= step;
    }

    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8CodepointCountBytes(const unsigned char *bytes,
                                         size_t length,
                                         size_t *count)
{
    const unsigned char *p;
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t remaining;
    utf8proc_ssize_t step;
    LeoUTF8Status status;
    size_t localCount;

    if (bytes == 0 || count == 0) {
        return LEO_UTF8_INVALID;
    }

    status = LeoUTF8ToUtf8procLength(length, &remaining);
    if (status != LEO_UTF8_OK) {
        return status;
    }

    p = bytes;
    localCount = 0;

    while (remaining > 0) {
        step = utf8proc_iterate(p, remaining, &codepoint);
        if (step < 0) {
            return LeoUTF8StatusFromUtf8proc(step);
        }

        if (step == 0) {
            return LEO_UTF8_ERROR;
        }

        p += step;
        remaining -= step;
        localCount++;
    }

    *count = localCount;
    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8NormalizeNFCBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen)
{
    return LeoUTF8MapBytes(bytes,
                           length,
                           UTF8PROC_STABLE | UTF8PROC_COMPOSE,
                           out,
                           outLen);
}

LeoUTF8Status LeoUTF8NormalizeNFDBytes(const unsigned char *bytes,
                                       size_t length,
                                       unsigned char **out,
                                       size_t *outLen)
{
    return LeoUTF8MapBytes(bytes,
                           length,
                           UTF8PROC_STABLE | UTF8PROC_DECOMPOSE,
                           out,
                           outLen);
}

LeoUTF8Status LeoUTF8CaseFoldBytes(const unsigned char *bytes,
                                   size_t length,
                                   unsigned char **out,
                                   size_t *outLen)
{
    return LeoUTF8MapBytes(bytes,
                           length,
                           UTF8PROC_STABLE | UTF8PROC_CASEFOLD,
                           out,
                           outLen);
}

LeoUTF8Status LeoUTF8Validate(const unsigned char *text)
{
    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    return LeoUTF8ValidateBytes(text, strlen((const char *)text));
}

LeoUTF8Status LeoUTF8CodepointCount(const unsigned char *text, size_t *count)
{
    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    return LeoUTF8CodepointCountBytes(text, strlen((const char *)text), count);
}

LeoUTF8Status LeoUTF8NormalizeNFC(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen)
{
    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    return LeoUTF8NormalizeNFCBytes(text, strlen((const char *)text), out, outLen);
}

LeoUTF8Status LeoUTF8NormalizeNFD(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen)
{
    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    return LeoUTF8NormalizeNFDBytes(text, strlen((const char *)text), out, outLen);
}

LeoUTF8Status LeoUTF8CaseFold(const unsigned char *text,
                              unsigned char **out,
                              size_t *outLen)
{
    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    return LeoUTF8CaseFoldBytes(text, strlen((const char *)text), out, outLen);
}

void LeoUTF8Free(unsigned char *text)
{
    if (text != 0) {
        utf8proc_free(text);
    }
}
