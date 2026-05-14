#include <stdio.h>

#include "LeoUTF8.h"

int main(void)
{
    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    size_t codepoints;
    unsigned char *folded;
    size_t foldedLen;
    LeoUTF8Status status;

    codepoints = 0;
    folded = 0;
    foldedLen = 0;

    printf("LeoUTF8 Core example\n");
    printf("engine version: %s\n", LeoUTF8Version());
    printf("brick version: %s\n", LeoUTF8BrickVersionString());

    status = LeoUTF8ValidateBytes(sample, sizeof(sample) - 1);
    if (status != LEO_UTF8_OK) {
        printf("validate failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    status = LeoUTF8CodepointCountBytes(sample, sizeof(sample) - 1, &codepoints);
    if (status != LEO_UTF8_OK) {
        printf("count failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    status = LeoUTF8CaseFoldBytes(sample, sizeof(sample) - 1, &folded, &foldedLen);
    if (status != LEO_UTF8_OK) {
        printf("casefold failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    printf("input: %s\n", sample);
    printf("codepoints: %lu\n", (unsigned long)codepoints);
    printf("casefold: %s\n", folded);

    LeoUTF8Free(folded);

    printf("LeoUTF8 Core example passed.\n");
    return 0;
}
