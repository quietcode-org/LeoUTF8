#include <stdio.h>
#include <stdlib.h>

#include "utf8proc.h"

static void print_hex(const unsigned char *text)
{
    const unsigned char *p = text;

    printf("bytes:");
    while (*p != '\0') {
        printf(" %02X", (unsigned int)*p);
        p++;
    }
    printf("\n");
}

static int count_codepoints(const unsigned char *text)
{
    const unsigned char *p = text;
    utf8proc_int32_t codepoint;
    int count = 0;

    while (*p != '\0') {
        utf8proc_ssize_t step = utf8proc_iterate(p, -1, &codepoint);
        if (step < 0) {
            fprintf(stderr, "invalid UTF-8 at byte offset %d: %s\n",
                    (int)(p - text), utf8proc_errmsg(step));
            return -1;
        }
        p += step;
        count++;
    }

    return count;
}

static int print_mapped(const char *label, const unsigned char *text, utf8proc_option_t options)
{
    utf8proc_uint8_t *mapped = NULL;
    utf8proc_ssize_t rc;

    rc = utf8proc_map(text, 0, &mapped, options | UTF8PROC_NULLTERM);
    if (rc < 0) {
        fprintf(stderr, "%s failed: %s\n", label, utf8proc_errmsg(rc));
        return 1;
    }

    printf("%s: %s\n", label, mapped);
    print_hex(mapped);

    utf8proc_free(mapped);
    return 0;
}

int main(void)
{
    /*
     * UTF-8 bytes for:
     * Groesse with umlaut/eszett, uppercase umlauts, Strasse with eszett.
     *
     * Text: Gre  Strae
     */
    const unsigned char sample[] =
        "Gr" "\xC3" "\xB6" "\xC3" "\x9F" "e "
        "\xC3" "\x84" "\xC3" "\x96" "\xC3" "\x9C" " "
        "Stra" "\xC3" "\x9F" "e";

    int count;

    printf("LeoUTF8 probe\n");
    printf("utf8proc version: %s\n", utf8proc_version());
    printf("sample: %s\n", sample);
    print_hex(sample);

    count = count_codepoints(sample);
    if (count < 0) {
        fprintf(stderr, "ERROR: invalid UTF-8 input\n");
        return 1;
    }

    printf("codepoints: %d\n", count);

    if (print_mapped("NFC", sample, UTF8PROC_STABLE | UTF8PROC_COMPOSE) != 0) {
        return 1;
    }

    if (print_mapped("NFD", sample, UTF8PROC_STABLE | UTF8PROC_DECOMPOSE) != 0) {
        return 1;
    }

    if (print_mapped("casefold", sample, UTF8PROC_STABLE | UTF8PROC_CASEFOLD) != 0) {
        return 1;
    }

    printf("LeoUTF8 probe passed.\n");
    return 0;
}
