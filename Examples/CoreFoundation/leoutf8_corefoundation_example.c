#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

#include "LeoUTF8CoreFoundation.h"

int main(void)
{
    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    CFStringRef string;
    CFDataRef data;
    LeoUTF8Status status;

    string = 0;
    data = 0;

    printf("LeoUTF8 CoreFoundation example\n");
    printf("engine version: %s\n", LeoUTF8Version());
    printf("brick version: %s\n", LeoUTF8BrickVersionString());

    status = LeoUTF8CreateCFStringFromBytes(sample, sizeof(sample) - 1, &string);
    if (status != LEO_UTF8_OK) {
        printf("create CFString failed: %s\n", LeoUTF8StatusMessage(status));
        return 1;
    }

    status = LeoUTF8CopyUTF8DataFromCFString(string, &data);
    if (status != LEO_UTF8_OK) {
        printf("copy CFData failed: %s\n", LeoUTF8StatusMessage(status));
        CFRelease(string);
        return 1;
    }

    printf("UTF-8 byte length: %lu\n", (unsigned long)CFDataGetLength(data));

    CFRelease(data);
    CFRelease(string);

    printf("LeoUTF8 CoreFoundation example passed.\n");
    return 0;
}
