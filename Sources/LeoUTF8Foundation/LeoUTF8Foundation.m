#import "LeoUTF8Foundation.h"

LeoUTF8Status LeoUTF8CreateNSStringFromBytes(const unsigned char *bytes,
                                             size_t length,
                                             NSString **outString)
{
    LeoUTF8Status status;
    NSString *string;

    if (outString == nil) {
        return LEO_UTF8_INVALID;
    }

    *outString = nil;

    if (bytes == nil) {
        return LEO_UTF8_INVALID;
    }

    status = LeoUTF8ValidateBytes(bytes, length);
    if (status != LEO_UTF8_OK) {
        return status;
    }

    string = [[NSString alloc] initWithBytes:bytes
                                      length:length
                                    encoding:NSUTF8StringEncoding];

    if (string == nil) {
        return LEO_UTF8_INVALID;
    }

    *outString = string;
    return LEO_UTF8_OK;
}

LeoUTF8Status LeoUTF8CopyUTF8DataFromNSString(NSString *string,
                                              NSData **outData)
{
    const char *utf8;
    NSUInteger length;
    NSData *data;

    if (outData == nil) {
        return LEO_UTF8_INVALID;
    }

    *outData = nil;

    if (string == nil) {
        return LEO_UTF8_INVALID;
    }

    if (![string canBeConvertedToEncoding:NSUTF8StringEncoding]) {
        return LEO_UTF8_INVALID;
    }

    utf8 = [string UTF8String];
    if (utf8 == nil) {
        return LEO_UTF8_ERROR;
    }

    length = [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding];

    data = [[NSData alloc] initWithBytes:utf8 length:length];
    if (data == nil) {
        return LEO_UTF8_NO_MEMORY;
    }

    *outData = data;
    return LEO_UTF8_OK;
}
