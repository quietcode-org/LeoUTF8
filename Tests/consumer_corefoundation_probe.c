#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

#include "LeoUTF8CoreFoundation.h"

int main(void)
{
    CFStringRef string;
    CFDataRef data;
    LeoUTF8Status status;

    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    string = 0;
    status = LeoUTF8CreateCFStringFromBytes(sample, sizeof(sample) - 1, &string);
    if (status != LEO_UTF8_OK) {
        printf("create CFString failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    data = 0;
    status = LeoUTF8CopyUTF8DataFromCFString(string, &data);
    if (status != LEO_UTF8_OK) {
        printf("copy UTF-8 CFData failed: %s\n", LeoUTF8StatusMessage(status));
        CFRelease(string);
        return 1;
    }

    if (CFDataGetLength(data) != 7) {
        printf("unexpected CFData length: %lu\n", (unsigned long)CFDataGetLength(data));
        CFRelease(data);
        CFRelease(string);
        return 1;
    }

    printf("consumer corefoundation probe passed\n");
    printf("CFData length: %lu\n", (unsigned long)CFDataGetLength(data));

    CFRelease(data);
    CFRelease(string);

    return 0;
}
