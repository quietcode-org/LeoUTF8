#include <stdio.h>

#include "LeoUTF8.h"

static void print_hex(const unsigned char *text)
{
    const unsigned char *p;

    p = text;

    printf("bytes:");
    while (*p != '\0') {
        printf(" %02X", (unsigned int)*p);
        p++;
    }
    printf("\n");
}

static int expect_status(const char *label,
                         LeoUTF8Status actual,
                         LeoUTF8Status expected)
{
    if (actual != expected) {
        fprintf(stderr,
                "%s failed: expected %s, got %s\n",
                label,
                LeoUTF8StatusMessage(expected),
                LeoUTF8StatusMessage(actual));
        return 1;
    }

    printf("%s: %s\n", label, LeoUTF8StatusMessage(actual));
    return 0;
}

static int print_mapped(const char *label,
                        LeoUTF8Status (*fn)(const unsigned char *,
                                             unsigned char **,
                                             size_t *),
                        const unsigned char *text)
{
    unsigned char *mapped;
    size_t mappedLen;
    LeoUTF8Status status;

    mapped = 0;
    mappedLen = 0;

    status = fn(text, &mapped, &mappedLen);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "%s failed: %s\n", label, LeoUTF8StatusMessage(status));
        return 1;
    }

    printf("%s: %s\n", label, mapped);
    printf("%s length bytes: %lu\n", label, (unsigned long)mappedLen);
    print_hex(mapped);

    LeoUTF8Free(mapped);
    return 0;
}

static int run_valid_checks(const unsigned char *sample)
{
    const unsigned char embeddedNul[] = { 'A', 0x00, 'B' };
    LeoUTF8Status status;
    size_t count;

    status = LeoUTF8Validate(sample);
    if (expect_status("valid UTF-8", status, LEO_UTF8_OK) != 0) {
        return 1;
    }

    count = 0;
    status = LeoUTF8CodepointCount(sample, &count);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "count failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    printf("codepoints: %lu\n", (unsigned long)count);

    status = LeoUTF8ValidateBytes(embeddedNul, sizeof(embeddedNul));
    if (expect_status("embedded NUL byte validation", status, LEO_UTF8_OK) != 0) {
        return 1;
    }

    count = 0;
    status = LeoUTF8CodepointCountBytes(embeddedNul, sizeof(embeddedNul), &count);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "embedded NUL count failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    printf("embedded NUL codepoints: %lu\n", (unsigned long)count);

    if (count != 3) {
        fprintf(stderr, "embedded NUL count failed: expected 3, got %lu\n",
                (unsigned long)count);
        return 1;
    }

    if (print_mapped("NFC", LeoUTF8NormalizeNFC, sample) != 0) {
        return 1;
    }

    if (print_mapped("NFD", LeoUTF8NormalizeNFD, sample) != 0) {
        return 1;
    }

    if (print_mapped("casefold", LeoUTF8CaseFold, sample) != 0) {
        return 1;
    }

    return 0;
}

static int run_invalid_checks(void)
{
    const unsigned char isolatedContinuation[] = { 0x80, 0x00 };
    const unsigned char truncatedTwoByte[] = { 0xC3, 0x00 };
    const unsigned char truncatedThreeByte[] = { 0xE2, 0x82, 0x00 };
    const unsigned char overlongSlash[] = { 0xC0, 0xAF, 0x00 };

    LeoUTF8Status status;
    unsigned char *mapped;
    size_t mappedLen;
    size_t count;

    printf("Running invalid UTF-8 checks...\n");

    if (expect_status("NULL validate",
                      LeoUTF8Validate(0),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    count = 0;
    if (expect_status("NULL count input",
                      LeoUTF8CodepointCount(0, &count),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    if (expect_status("NULL count output",
                      LeoUTF8CodepointCount((const unsigned char *)"ok", 0),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    if (expect_status("isolated continuation byte",
                      LeoUTF8Validate(isolatedContinuation),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    if (expect_status("truncated two-byte sequence",
                      LeoUTF8Validate(truncatedTwoByte),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    if (expect_status("truncated three-byte sequence",
                      LeoUTF8Validate(truncatedThreeByte),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    if (expect_status("overlong slash sequence",
                      LeoUTF8Validate(overlongSlash),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    count = 0;
    if (expect_status("invalid count input",
                      LeoUTF8CodepointCount(overlongSlash, &count),
                      LEO_UTF8_INVALID) != 0) {
        return 1;
    }

    mapped = 0;
    mappedLen = 12345;
    status = LeoUTF8NormalizeNFC(overlongSlash, &mapped, &mappedLen);
    if (expect_status("invalid normalize input",
                      status,
                      LEO_UTF8_INVALID) != 0) {
        if (mapped != 0) {
            LeoUTF8Free(mapped);
        }
        return 1;
    }

    if (mapped != 0 || mappedLen != 0) {
        fprintf(stderr, "invalid normalize input left output dirty\n");
        if (mapped != 0) {
            LeoUTF8Free(mapped);
        }
        return 1;
    }

    printf("invalid output cleanup: OK\n");
    return 0;
}

int main(void)
{
    /*
     * UTF-8 bytes for:
     * Text: Groesse with umlaut/eszett, uppercase umlauts, Strasse with eszett.
     *
     * Display text: Größe ÄÖÜ Straße
     */
    const unsigned char sample[] =
        "Gr" "\xC3" "\xB6" "\xC3" "\x9F" "e "
        "\xC3" "\x84" "\xC3" "\x96" "\xC3" "\x9C" " "
        "Stra" "\xC3" "\x9F" "e";

    printf("LeoUTF8 probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());
    printf("sample: %s\n", sample);
    print_hex(sample);

    if (run_valid_checks(sample) != 0) {
        return 1;
    }

    if (run_invalid_checks() != 0) {
        return 1;
    }

    printf("LeoUTF8 probe passed.\n");
    return 0;
}
