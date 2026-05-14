#import <Foundation/Foundation.h>

#include <stdio.h>

#import "LeoUTF8Foundation.h"

int main(void)
{
    NSAutoreleasePool *pool;
    NSString *string;
    NSData *data;
    LeoUTF8Status status;

    const unsigned char sample[] =
        "Stra" "\xC3" "\x9F" "e";

    pool = [[NSAutoreleasePool alloc] init];

    string = nil;
    status = LeoUTF8CreateNSStringFromBytes(sample, sizeof(sample) - 1, &string);
    if (status != LEO_UTF8_OK) {
        printf("create NSString failed: %s\n", LeoUTF8StatusMessage(status));
        [pool release];
        return 1;
    }

    data = nil;
    status = LeoUTF8CopyUTF8DataFromNSString(string, &data);
    if (status != LEO_UTF8_OK) {
        printf("copy UTF-8 NSData failed: %s\n", LeoUTF8StatusMessage(status));
        [string release];
        [pool release];
        return 1;
    }

    if ([data length] != 7) {
        printf("unexpected NSData length: %lu\n", (unsigned long)[data length]);
        [data release];
        [string release];
        [pool release];
        return 1;
    }

    printf("consumer foundation probe passed\n");
    printf("NSString UTF8: %s\n", [string UTF8String]);
    printf("NSData length: %lu\n", (unsigned long)[data length]);

    [data release];
    [string release];
    [pool release];

    return 0;
}
