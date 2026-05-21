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

int main(int argc, char **argv)
{
    unsigned char *bytes;
    size_t length;
    LeoUTF8Status status;

    if (argc != 2) {
        fprintf(stderr, "LEOFUZZ:ERROR usage: leoutf8_validate_probe input-file\n");
        return 2;
    }

    bytes = 0;
    length = 0;

    if (ReadFile(argv[1], &bytes, &length) != 0) {
        fprintf(stderr, "LEOFUZZ:REJECT could-not-read-input\n");
        return 1;
    }

    status = LeoUTF8ValidateBytes(bytes, length);

    free(bytes);

    if (status == LEO_UTF8_OK) {
        printf("LEOFUZZ:OK valid-utf8 bytes=%lu\n", (unsigned long)length);
        return 0;
    }

    printf("LEOFUZZ:REJECT invalid-utf8 status=%s bytes=%lu\n",
        LeoUTF8StatusMessage(status),
        (unsigned long)length
    );

    return 1;
}
