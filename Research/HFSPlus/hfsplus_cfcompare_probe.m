#include <stdio.h>
#include <string.h>

#import <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

#include "LeoUTF8.h"

static void print_result(const char *label, int equal)
{
    printf("%s: %s\n", label, equal ? "equal" : "different");
}

static CFStringRef create_cfstring(const unsigned char *bytes, size_t length)
{
    return CFStringCreateWithBytes(kCFAllocatorDefault,
                                   bytes,
                                   (CFIndex)length,
                                   kCFStringEncodingUTF8,
                                   false);
}

static NSString *create_nsstring(const unsigned char *bytes, size_t length)
{
    return [[NSString alloc] initWithBytes:bytes
                                    length:length
                                  encoding:NSUTF8StringEncoding];
}

static int cf_equal(CFStringRef a, CFStringRef b, CFStringCompareFlags flags)
{
    return CFStringCompare(a, b, flags) == kCFCompareEqualTo;
}

static int ns_equal(NSString *a, NSString *b, NSStringCompareOptions options)
{
    return [a compare:b options:options] == NSOrderedSame;
}

static int run_pair(const char *label,
                    const unsigned char *first,
                    size_t firstLen,
                    const unsigned char *second,
                    size_t secondLen)
{
    CFStringRef cfFirst;
    CFStringRef cfSecond;
    NSString *nsFirst;
    NSString *nsSecond;

    printf("\n=== %s ===\n", label);
    printf("first: %s\n", first);
    printf("second: %s\n", second);

    cfFirst = create_cfstring(first, firstLen);
    cfSecond = create_cfstring(second, secondLen);

    if (cfFirst == 0 || cfSecond == 0) {
        fprintf(stderr, "CFString creation failed\n");
        if (cfFirst != 0) {
            CFRelease(cfFirst);
        }
        if (cfSecond != 0) {
            CFRelease(cfSecond);
        }
        return 1;
    }

    nsFirst = create_nsstring(first, firstLen);
    nsSecond = create_nsstring(second, secondLen);

    if (nsFirst == nil || nsSecond == nil) {
        fprintf(stderr, "NSString creation failed\n");
        if (nsFirst != nil) {
            [nsFirst release];
        }
        if (nsSecond != nil) {
            [nsSecond release];
        }
        CFRelease(cfSecond);
        CFRelease(cfFirst);
        return 1;
    }

    print_result("CFString literal",
                 cf_equal(cfFirst, cfSecond, 0));

    print_result("CFString case-insensitive",
                 cf_equal(cfFirst, cfSecond, kCFCompareCaseInsensitive));

    print_result("CFString case-insensitive + nonliteral",
                 cf_equal(cfFirst,
                          cfSecond,
                          kCFCompareCaseInsensitive | kCFCompareNonliteral));

    print_result("CFString case-insensitive + diacritic-insensitive",
                 cf_equal(cfFirst,
                          cfSecond,
                          kCFCompareCaseInsensitive | kCFCompareDiacriticInsensitive));

    print_result("CFString case-insensitive + nonliteral + diacritic-insensitive",
                 cf_equal(cfFirst,
                          cfSecond,
                          kCFCompareCaseInsensitive |
                          kCFCompareNonliteral |
                          kCFCompareDiacriticInsensitive));

    print_result("NSString literal",
                 ns_equal(nsFirst, nsSecond, NSLiteralSearch));

    print_result("NSString case-insensitive",
                 ns_equal(nsFirst, nsSecond, NSCaseInsensitiveSearch));

    print_result("NSString case-insensitive + diacritic-insensitive",
                 ns_equal(nsFirst,
                          nsSecond,
                          NSCaseInsensitiveSearch | NSDiacriticInsensitiveSearch));

    print_result("NSString case-insensitive + literal",
                 ns_equal(nsFirst,
                          nsSecond,
                          NSCaseInsensitiveSearch | NSLiteralSearch));

    [nsSecond release];
    [nsFirst release];
    CFRelease(cfSecond);
    CFRelease(cfFirst);

    return 0;
}

int main(void)
{
    NSAutoreleasePool *pool;

    const unsigned char asciiLower[] = "case.txt";
    const unsigned char asciiUpper[] = "CASE.txt";

    const unsigned char umlautLower[] = "\xC3" "\xA4" ".txt";
    const unsigned char umlautUpper[] = "\xC3" "\x84" ".txt";

    const unsigned char cafeLower[] = "caf" "\xC3" "\xA9" ".txt";
    const unsigned char cafeUpper[] = "CAF" "\xC3" "\x89" ".TXT";

    const unsigned char sharpLower[] =
        "stra" "\xC3" "\x9F" "e.txt";
    const unsigned char sharpUpperSS[] =
        "STRASSE.TXT";

    pool = [[NSAutoreleasePool alloc] init];

    printf("LeoUTF8 HFS+ CFString/NSString comparison probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());

    if (run_pair("ASCII case: case.txt vs CASE.txt",
                 asciiLower,
                 sizeof(asciiLower) - 1,
                 asciiUpper,
                 sizeof(asciiUpper) - 1) != 0) {
        [pool release];
        return 1;
    }

    if (run_pair("Umlaut case: ä.txt vs Ä.txt",
                 umlautLower,
                 sizeof(umlautLower) - 1,
                 umlautUpper,
                 sizeof(umlautUpper) - 1) != 0) {
        [pool release];
        return 1;
    }

    if (run_pair("Accent case: café.txt vs CAFÉ.TXT",
                 cafeLower,
                 sizeof(cafeLower) - 1,
                 cafeUpper,
                 sizeof(cafeUpper) - 1) != 0) {
        [pool release];
        return 1;
    }

    if (run_pair("Sharp S: straße.txt vs STRASSE.TXT",
                 sharpLower,
                 sizeof(sharpLower) - 1,
                 sharpUpperSS,
                 sizeof(sharpUpperSS) - 1) != 0) {
        [pool release];
        return 1;
    }

    [pool release];

    printf("\nLeoUTF8 HFS+ CFString/NSString comparison probe completed.\n");
    return 0;
}
