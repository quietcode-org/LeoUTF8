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

    LeoUTF8Status status;
    size_t count;

    printf("LeoUTF8 probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());
    printf("sample: %s\n", sample);
    print_hex(sample);

    status = LeoUTF8Validate(sample);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "validate failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    count = 0;
    status = LeoUTF8CodepointCount(sample, &count);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "count failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    printf("codepoints: %lu\n", (unsigned long)count);

    if (print_mapped("NFC", LeoUTF8NormalizeNFC, sample) != 0) {
        return 1;
    }

    if (print_mapped("NFD", LeoUTF8NormalizeNFD, sample) != 0) {
        return 1;
    }

    if (print_mapped("casefold", LeoUTF8CaseFold, sample) != 0) {
        return 1;
    }

    printf("LeoUTF8 probe passed.\n");
    return 0;
}
