#import <Foundation/Foundation.h>
#include <stdio.h>

#import "LeoUTF8Foundation.h"

int main(void)
{
    NSAutoreleasePool *pool;
    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    NSString *string;
    NSData *data;
    LeoUTF8Status status;

    pool = [[NSAutoreleasePool alloc] init];

    string = nil;
    data = nil;

    printf("LeoUTF8 Foundation example\n");
    printf("engine version: %s\n", LeoUTF8Version());
    printf("brick version: %s\n", LeoUTF8BrickVersionString());

    status = LeoUTF8CreateNSStringFromBytes(sample, sizeof(sample) - 1, &string);
    if (status != LEO_UTF8_OK) {
        printf("create NSString failed: %s\n", LeoUTF8StatusMessage(status));
        [pool release];
        return 1;
    }

    status = LeoUTF8CopyUTF8DataFromNSString(string, &data);
    if (status != LEO_UTF8_OK) {
        printf("copy NSData failed: %s\n", LeoUTF8StatusMessage(status));
        [string release];
        [pool release];
        return 1;
    }

    printf("NSString UTF8: %s\n", [string UTF8String]);
    printf("NSData length: %lu\n", (unsigned long)[data length]);

    [data release];
    [string release];
    [pool release];

    printf("LeoUTF8 Foundation example passed.\n");
    return 0;
}
