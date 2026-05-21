#include "LeoUTF8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HexBuffer {
    unsigned char *bytes;
    size_t length;
} HexBuffer;

static int HexValue(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }

    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }

    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }

    return -1;
}

static int IsIgnoredHexChar(int ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

static int ParseHexBytes(const char *text, HexBuffer *buffer)
{
    size_t digits;
    size_t index;
    int high;
    int low;
    const char *cursor;

    if (text == 0 || buffer == 0) {
        return -1;
    }

    buffer->bytes = 0;
    buffer->length = 0;

    digits = 0;
    cursor = text;

    while (*cursor != '\0') {
        if (IsIgnoredHexChar((unsigned char)*cursor)) {
            cursor++;
            continue;
        }

        if (HexValue((unsigned char)*cursor) < 0) {
            return -1;
        }

        digits++;
        cursor++;
    }

    if ((digits % 2) != 0) {
        return -1;
    }

    buffer->length = digits / 2;

    if (buffer->length == 0) {
        buffer->bytes = (unsigned char *)malloc(1);
    } else {
        buffer->bytes = (unsigned char *)malloc(buffer->length);
    }

    if (buffer->bytes == 0) {
        return -1;
    }

    index = 0;
    cursor = text;

    while (*cursor != '\0') {
        while (*cursor != '\0' && IsIgnoredHexChar((unsigned char)*cursor)) {
            cursor++;
        }

        if (*cursor == '\0') {
            break;
        }

        high = HexValue((unsigned char)*cursor);
        cursor++;

        while (*cursor != '\0' && IsIgnoredHexChar((unsigned char)*cursor)) {
            cursor++;
        }

        if (*cursor == '\0') {
            free(buffer->bytes);
            buffer->bytes = 0;
            buffer->length = 0;
            return -1;
        }

        low = HexValue((unsigned char)*cursor);
        cursor++;

        if (high < 0 || low < 0 || index >= buffer->length) {
            free(buffer->bytes);
            buffer->bytes = 0;
            buffer->length = 0;
            return -1;
        }

        buffer->bytes[index] = (unsigned char)((high << 4) | low);
        index++;
    }

    return 0;
}

static void FreeHexBuffer(HexBuffer *buffer)
{
    if (buffer == 0) {
        return;
    }

    free(buffer->bytes);
    buffer->bytes = 0;
    buffer->length = 0;
}

static int StartsWith(const char *line, const char *prefix)
{
    size_t prefixLength;

    if (line == 0 || prefix == 0) {
        return 0;
    }

    prefixLength = strlen(prefix);

    return strncmp(line, prefix, prefixLength) == 0;
}

static int LoadCaseFile(const char *path, HexBuffer *input, HexBuffer *expected)
{
    FILE *file;
    char line[4096];
    int haveInput;
    int haveExpected;

    if (path == 0 || input == 0 || expected == 0) {
        return -1;
    }

    haveInput = 0;
    haveExpected = 0;

    file = fopen(path, "r");

    if (file == 0) {
        return -1;
    }

    while (fgets(line, sizeof(line), file) != 0) {
        if (StartsWith(line, "input_hex=")) {
            FreeHexBuffer(input);

            if (ParseHexBytes(line + strlen("input_hex="), input) != 0) {
                fclose(file);
                return -1;
            }

            haveInput = 1;
            continue;
        }

        if (StartsWith(line, "expected_hex=")) {
            FreeHexBuffer(expected);

            if (ParseHexBytes(line + strlen("expected_hex="), expected) != 0) {
                fclose(file);
                return -1;
            }

            haveExpected = 1;
            continue;
        }
    }

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    fclose(file);

    if (!haveInput || !haveExpected) {
        return -1;
    }

    return 0;
}

static const char *ProbeModeName(void)
{
#if defined(LEOFUZZ_TRANSFORM_NFC)
    return "NFC";
#elif defined(LEOFUZZ_TRANSFORM_NFD)
    return "NFD";
#elif defined(LEOFUZZ_TRANSFORM_CASEFOLD)
    return "CASEFOLD";
#else
    return "UNKNOWN";
#endif
}

static LeoUTF8Status RunTransform(
    const unsigned char *bytes,
    size_t length,
    unsigned char **out,
    size_t *outLength
)
{
#if defined(LEOFUZZ_TRANSFORM_NFC)
    return LeoUTF8NormalizeNFCBytes(bytes, length, out, outLength);
#elif defined(LEOFUZZ_TRANSFORM_NFD)
    return LeoUTF8NormalizeNFDBytes(bytes, length, out, outLength);
#elif defined(LEOFUZZ_TRANSFORM_CASEFOLD)
    return LeoUTF8CaseFoldBytes(bytes, length, out, outLength);
#else
    (void)bytes;
    (void)length;
    (void)out;
    (void)outLength;
    return LEO_UTF8_ERROR;
#endif
}

int main(int argc, char **argv)
{
    HexBuffer input;
    HexBuffer expected;
    unsigned char *out;
    size_t outLength;
    LeoUTF8Status status;
    int mismatch;

    if (argc != 2) {
        fprintf(stderr, "LEOFUZZ:ERROR usage: leoutf8_transform_expect_probe case-file\n");
        return 2;
    }

    input.bytes = 0;
    input.length = 0;
    expected.bytes = 0;
    expected.length = 0;
    out = 0;
    outLength = 0;
    mismatch = 0;

    if (LoadCaseFile(argv[1], &input, &expected) != 0) {
        fprintf(stderr, "LEOFUZZ:ERROR invalid-case-file\n");
        FreeHexBuffer(&input);
        FreeHexBuffer(&expected);
        return 2;
    }

    status = RunTransform(input.bytes, input.length, &out, &outLength);

    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "LEOFUZZ:ERROR transform=%s status=%s\n",
            ProbeModeName(),
            LeoUTF8StatusMessage(status)
        );

        FreeHexBuffer(&input);
        FreeHexBuffer(&expected);
        LeoUTF8Free(out);
        return 3;
    }

    if (outLength != expected.length) {
        mismatch = 1;
    } else if (expected.length > 0 && memcmp(out, expected.bytes, expected.length) != 0) {
        mismatch = 1;
    }

    if (mismatch) {
        fprintf(stderr, "LEOFUZZ:ERROR transform=%s output-mismatch expected_bytes=%lu actual_bytes=%lu\n",
            ProbeModeName(),
            (unsigned long)expected.length,
            (unsigned long)outLength
        );

        FreeHexBuffer(&input);
        FreeHexBuffer(&expected);
        LeoUTF8Free(out);
        return 3;
    }

    printf("LEOFUZZ:OK transform=%s exact-output bytes=%lu\n",
        ProbeModeName(),
        (unsigned long)outLength
    );

    FreeHexBuffer(&input);
    FreeHexBuffer(&expected);
    LeoUTF8Free(out);

    return 0;
}
