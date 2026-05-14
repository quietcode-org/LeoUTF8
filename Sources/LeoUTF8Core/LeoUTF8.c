#include "LeoUTF8.h"

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

static LeoUTF8Status LeoUTF8Map(const unsigned char *text,
                                utf8proc_option_t options,
                                unsigned char **out,
                                size_t *outLen)
{
    utf8proc_uint8_t *mapped;
    utf8proc_ssize_t rc;

    if (out == 0) {
        return LEO_UTF8_ERROR;
    }

    *out = 0;

    if (outLen != 0) {
        *outLen = 0;
    }

    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    mapped = 0;
    rc = utf8proc_map(text, 0, &mapped, options | UTF8PROC_NULLTERM);

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

LeoUTF8Status LeoUTF8Validate(const unsigned char *text)
{
    const unsigned char *p;
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t step;

    if (text == 0) {
        return LEO_UTF8_INVALID;
    }

    p = text;

    while (*p != '\0') {
        step = utf8proc_iterate(p, -1, &codepoint);
        if (step < 0) {
            return LeoUTF8StatusFromUtf8proc(step);
        }

        p += step;
    }

    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8CodepointCount(const unsigned char *text, size_t *count)
{
    const unsigned char *p;
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t step;
    size_t localCount;

    if (text == 0 || count == 0) {
        return LEO_UTF8_INVALID;
    }

    p = text;
    localCount = 0;

    while (*p != '\0') {
        step = utf8proc_iterate(p, -1, &codepoint);
        if (step < 0) {
            return LeoUTF8StatusFromUtf8proc(step);
        }

        p += step;
        localCount++;
    }

    *count = localCount;
    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8NormalizeNFC(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen)
{
    return LeoUTF8Map(text,
                      UTF8PROC_STABLE | UTF8PROC_COMPOSE,
                      out,
                      outLen);
}

LeoUTF8Status LeoUTF8NormalizeNFD(const unsigned char *text,
                                  unsigned char **out,
                                  size_t *outLen)
{
    return LeoUTF8Map(text,
                      UTF8PROC_STABLE | UTF8PROC_DECOMPOSE,
                      out,
                      outLen);
}

LeoUTF8Status LeoUTF8CaseFold(const unsigned char *text,
                              unsigned char **out,
                              size_t *outLen)
{
    return LeoUTF8Map(text,
                      UTF8PROC_STABLE | UTF8PROC_CASEFOLD,
                      out,
                      outLen);
}

void LeoUTF8Free(unsigned char *text)
{
    if (text != 0) {
        utf8proc_free(text);
    }
}
