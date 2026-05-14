#include <stdio.h>

#include "LeoUTF8.h"

int main(void)
{
    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    size_t count;
    LeoUTF8Status status;

    count = 0;

    status = LeoUTF8ValidateBytes(sample, sizeof(sample) - 1);
    if (status != LEO_UTF8_OK) {
        printf("validate failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    status = LeoUTF8CodepointCountBytes(sample, sizeof(sample) - 1, &count);
    if (status != LEO_UTF8_OK) {
        printf("count failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    if (count != 6) {
        printf("unexpected codepoint count: %lu\n", (unsigned long)count);
        return 1;
    }

    printf("consumer core probe passed\n");
    printf("codepoints: %lu\n", (unsigned long)count);
    return 0;
}
