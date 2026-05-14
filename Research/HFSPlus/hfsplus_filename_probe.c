#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "LeoUTF8.h"

static void print_bytes(const char *label, const unsigned char *bytes, size_t length)
{
    size_t i;

    printf("%s:", label);
    for (i = 0; i < length; i++) {
        printf(" %02X", (unsigned int)bytes[i]);
    }
    printf("\n");
}

static int make_path(char *out, size_t outSize, const char *dir, const unsigned char *name)
{
    int written;

    written = snprintf(out, outSize, "%s/%s", dir, name);
    if (written < 0 || (size_t)written >= outSize) {
        fprintf(stderr, "path too long\n");
        return 1;
    }

    return 0;
}

static int create_file_exclusive(const char *path, const char *content)
{
    int fd;
    ssize_t ignored;

    fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        return -1;
    }

    ignored = write(fd, content, strlen(content));
    (void)ignored;

    close(fd);
    return 0;
}

int main(void)
{
    const char *probeDir = "build-work/hfsplus-filename-probe";

    /*
     * NFC: café.txt
     * NFD: cafe + combining acute + .txt
     */
    const unsigned char nfcName[] = "caf" "\xC3" "\xA9" ".txt";
    const unsigned char nfdName[] = "cafe" "\xCC" "\x81" ".txt";

    char nfcPath[1024];
    char nfdPath[1024];

    struct statfs fsinfo;
    DIR *dir;
    struct dirent *entry;

    unsigned char *expectedNFD;
    size_t expectedNFDLen;
    LeoUTF8Status status;

    int sawNFCBytes;
    int sawNFDBytes;
    int nfdExclusiveMatchedExisting;

    printf("LeoUTF8 HFS+ filename probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());

    system("rm -rf build-work/hfsplus-filename-probe");

    if (mkdir(probeDir, 0700) != 0) {
        perror("mkdir");
        return 1;
    }

    if (statfs(probeDir, &fsinfo) == 0) {
        printf("filesystem type: %s\n", fsinfo.f_fstypename);
    } else {
        perror("statfs");
    }

    print_bytes("NFC input bytes", nfcName, sizeof(nfcName) - 1);
    print_bytes("NFD input bytes", nfdName, sizeof(nfdName) - 1);

    expectedNFD = 0;
    expectedNFDLen = 0;

    status = LeoUTF8NormalizeNFDBytes(nfcName,
                                      sizeof(nfcName) - 1,
                                      &expectedNFD,
                                      &expectedNFDLen);

    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "LeoUTF8NormalizeNFDBytes failed: %s\n",
                LeoUTF8StatusMessage(status));
        return 1;
    }

    print_bytes("LeoUTF8 expected NFD bytes", expectedNFD, expectedNFDLen);

    if (make_path(nfcPath, sizeof(nfcPath), probeDir, nfcName) != 0) {
        LeoUTF8Free(expectedNFD);
        return 1;
    }

    if (make_path(nfdPath, sizeof(nfdPath), probeDir, nfdName) != 0) {
        LeoUTF8Free(expectedNFD);
        return 1;
    }

    if (create_file_exclusive(nfcPath, "created with NFC spelling\n") != 0) {
        perror("create NFC file");
        LeoUTF8Free(expectedNFD);
        return 1;
    }

    nfdExclusiveMatchedExisting = 0;

    if (create_file_exclusive(nfdPath, "created with NFD spelling\n") != 0) {
        if (errno == EEXIST) {
            nfdExclusiveMatchedExisting = 1;
            printf("NFD O_EXCL create result: EEXIST\n");
        } else {
            perror("create NFD file");
            LeoUTF8Free(expectedNFD);
            return 1;
        }
    } else {
        printf("NFD O_EXCL create result: separate file created\n");
    }

    dir = opendir(probeDir);
    if (dir == 0) {
        perror("opendir");
        LeoUTF8Free(expectedNFD);
        return 1;
    }

    sawNFCBytes = 0;
    sawNFDBytes = 0;

    printf("Directory entries:\n");

    while ((entry = readdir(dir)) != 0) {
        size_t nameLen;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        nameLen = strlen(entry->d_name);

        printf("entry: %s\n", entry->d_name);
        print_bytes("entry bytes", (const unsigned char *)entry->d_name, nameLen);

        if (nameLen == sizeof(nfcName) - 1 &&
            memcmp(entry->d_name, nfcName, nameLen) == 0) {
            sawNFCBytes = 1;
        }

        if (nameLen == expectedNFDLen &&
            memcmp(entry->d_name, expectedNFD, nameLen) == 0) {
            sawNFDBytes = 1;
        }
    }

    closedir(dir);

    printf("Summary:\n");
    printf("NFD O_EXCL matched existing file: %s\n",
           nfdExclusiveMatchedExisting ? "yes" : "no");
    printf("readdir returned NFC bytes: %s\n", sawNFCBytes ? "yes" : "no");
    printf("readdir returned LeoUTF8 NFD bytes: %s\n", sawNFDBytes ? "yes" : "no");

    LeoUTF8Free(expectedNFD);

    printf("LeoUTF8 HFS+ filename probe completed.\n");
    return 0;
}
