#include <CoreFoundation/CoreFoundation.h>

#include <stdio.h>

#include "LeoUTF8CoreFoundation.h"

static void print_hex_bytes(const unsigned char *bytes, size_t length)
{
    size_t i;

    printf("bytes:");
    for (i = 0; i < length; i++) {
        printf(" %02X", (unsigned int)bytes[i]);
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

int main(void)
{
    CFStringRef string;
    CFStringRef invalidString;
    CFDataRef data;
    LeoUTF8Status status;
    size_t count;

    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    const unsigned char invalidUtf8[] = { 0xC3 };

    printf("LeoUTF8CoreFoundation probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());

    string = 0;
    status = LeoUTF8CreateCFStringFromBytes(sample, sizeof(sample) - 1, &string);
    if (expect_status("create CFString from UTF-8 bytes", status, LEO_UTF8_OK) != 0) {
        return 1;
    }

    data = 0;
    status = LeoUTF8CopyUTF8DataFromCFString(string, &data);
    if (expect_status("copy UTF-8 CFData from CFString", status, LEO_UTF8_OK) != 0) {
        CFRelease(string);
        return 1;
    }

    printf("CFData length bytes: %lu\n", (unsigned long)CFDataGetLength(data));
    print_hex_bytes((const unsigned char *)CFDataGetBytePtr(data),
                    (size_t)CFDataGetLength(data));

    count = 0;
    status = LeoUTF8CodepointCountBytes((const unsigned char *)CFDataGetBytePtr(data),
                                        (size_t)CFDataGetLength(data),
                                        &count);

    if (expect_status("count CFData UTF-8 bytes", status, LEO_UTF8_OK) != 0) {
        CFRelease(data);
        CFRelease(string);
        return 1;
    }

    if (count != 6) {
        fprintf(stderr, "unexpected CFData codepoint count: %lu\n",
                (unsigned long)count);
        CFRelease(data);
        CFRelease(string);
        return 1;
    }

    printf("CFData codepoints: %lu\n", (unsigned long)count);

    invalidString = 0;
    status = LeoUTF8CreateCFStringFromBytes(invalidUtf8,
                                            sizeof(invalidUtf8),
                                            &invalidString);

    if (expect_status("reject invalid UTF-8 CFString input",
                      status,
                      LEO_UTF8_INVALID) != 0) {
        CFRelease(data);
        CFRelease(string);
        if (invalidString != 0) {
            CFRelease(invalidString);
        }
        return 1;
    }

    if (invalidString != 0) {
        fprintf(stderr, "invalid CFString input left output dirty\n");
        CFRelease(invalidString);
        CFRelease(data);
        CFRelease(string);
        return 1;
    }

    printf("invalid CFString output cleanup: OK\n");

    CFRelease(data);
    CFRelease(string);

    printf("LeoUTF8CoreFoundation probe passed.\n");
    return 0;
}
