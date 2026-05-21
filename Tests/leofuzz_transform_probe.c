#include "LeoUTF8.h"

#include <stdio.h>
#include <stdlib.h>

static int ReadFile(const char *path, unsigned char **outBytes, size_t *outLength)
{
    FILE *file;
    long fileSize;
    unsigned char *buffer;
    size_t bytesRead;

    if (path == 0 || outBytes == 0 || outLength == 0) {
        return -1;
    }

    *outBytes = 0;
    *outLength = 0;

    file = fopen(path, "rb");

    if (file == 0) {
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }

    fileSize = ftell(file);

    if (fileSize < 0) {
        fclose(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return -1;
    }

    if (fileSize == 0) {
        buffer = (unsigned char *)malloc(1);

        if (buffer == 0) {
            fclose(file);
            return -1;
        }

        fclose(file);
        *outBytes = buffer;
        *outLength = 0;
        return 0;
    }

    buffer = (unsigned char *)malloc((size_t)fileSize);

    if (buffer == 0) {
        fclose(file);
        return -1;
    }

    bytesRead = fread(buffer, 1, (size_t)fileSize, file);

    if (bytesRead != (size_t)fileSize) {
        free(buffer);
        fclose(file);
        return -1;
    }

    fclose(file);

    *outBytes = buffer;
    *outLength = (size_t)fileSize;

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
    unsigned char *bytes;
    unsigned char *out;
    size_t length;
    size_t outLength;
    LeoUTF8Status status;

    if (argc != 2) {
        fprintf(stderr, "LEOFUZZ:ERROR usage: leoutf8_transform_probe input-file\n");
        return 2;
    }

    bytes = 0;
    out = 0;
    length = 0;
    outLength = 0;

    if (ReadFile(argv[1], &bytes, &length) != 0) {
        fprintf(stderr, "LEOFUZZ:REJECT could-not-read-input\n");
        return 1;
    }

    status = RunTransform(bytes, length, &out, &outLength);

    free(bytes);

    if (status == LEO_UTF8_OK) {
        printf("LEOFUZZ:OK transform=%s input_bytes=%lu output_bytes=%lu\n",
            ProbeModeName(),
            (unsigned long)length,
            (unsigned long)outLength
        );

        LeoUTF8Free(out);
        return 0;
    }

    printf("LEOFUZZ:REJECT transform=%s status=%s input_bytes=%lu\n",
        ProbeModeName(),
        LeoUTF8StatusMessage(status),
        (unsigned long)length
    );

    LeoUTF8Free(out);
    return 1;
}
