#import <Foundation/Foundation.h>

#include <stdio.h>

#import "LeoUTF8Foundation.h"

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
    NSAutoreleasePool *pool;
    NSString *string;
    NSData *data;
    LeoUTF8Status status;
    size_t count;

    const unsigned char sample[] =
        "Gr" "\xC3" "\xB6" "\xC3" "\x9F" "e "
        "\xC3" "\x84" "\xC3" "\x96" "\xC3" "\x9C" " "
        "Stra" "\xC3" "\x9F" "e";

    const unsigned char invalidUtf8[] = { 0xC3 };

    pool = [[NSAutoreleasePool alloc] init];

    printf("LeoUTF8Foundation probe\n");
    printf("LeoUTF8 engine version: %s\n", LeoUTF8Version());
    printf("LeoUTF8 brick version: %s\n", LeoUTF8BrickVersionString());
    printf("LeoUTF8 brick release: %s\n", LeoUTF8BrickReleaseName());
    printf("LeoUTF8 brick target: %s\n", LeoUTF8BrickTargetPlatform());

    string = nil;
    status = LeoUTF8CreateNSStringFromBytes(sample, sizeof(sample) - 1, &string);
    if (expect_status("create NSString from UTF-8 bytes", status, LEO_UTF8_OK) != 0) {
        [pool release];
        return 1;
    }

    printf("NSString UTF8: %s\n", [string UTF8String]);

    data = nil;
    status = LeoUTF8CopyUTF8DataFromNSString(string, &data);
    if (expect_status("copy UTF-8 NSData from NSString", status, LEO_UTF8_OK) != 0) {
        [string release];
        [pool release];
        return 1;
    }

    printf("NSData length bytes: %lu\n", (unsigned long)[data length]);
    print_hex_bytes((const unsigned char *)[data bytes], (size_t)[data length]);

    count = 0;
    status = LeoUTF8CodepointCountBytes((const unsigned char *)[data bytes],
                                        (size_t)[data length],
                                        &count);

    if (expect_status("count NSData UTF-8 bytes", status, LEO_UTF8_OK) != 0) {
        [data release];
        [string release];
        [pool release];
        return 1;
    }

    printf("NSData codepoints: %lu\n", (unsigned long)count);

    status = LeoUTF8CreateNSStringFromBytes(invalidUtf8,
                                            sizeof(invalidUtf8),
                                            &string);

    if (expect_status("reject invalid UTF-8 NSString input",
                      status,
                      LEO_UTF8_INVALID) != 0) {
        [data release];
        [string release];
        [pool release];
        return 1;
    }

    [data release];
    [string release];
    [pool release];

    printf("LeoUTF8Foundation probe passed.\n");
    return 0;
}
