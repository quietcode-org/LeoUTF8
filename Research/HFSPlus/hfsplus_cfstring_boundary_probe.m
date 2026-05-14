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

#import <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

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

static int read_single_entry(const char *dirPath, char *outName, size_t outSize)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirPath);
    if (dir == 0) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != 0) {
        size_t nameLen;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        nameLen = strlen(entry->d_name);
        if (nameLen + 1 > outSize) {
            fprintf(stderr, "directory entry name too long\n");
            closedir(dir);
            return 1;
        }

        memcpy(outName, entry->d_name, nameLen + 1);
        closedir(dir);
        return 0;
    }

    closedir(dir);
    fprintf(stderr, "no directory entry found\n");
    return 1;
}

static int bytes_equal(const unsigned char *a, size_t aLen,
                       const unsigned char *b, size_t bLen)
{
    if (aLen != bLen) {
        return 0;
    }

    return memcmp(a, b, aLen) == 0;
}

int main(void)
{
    NSAutoreleasePool *pool;

    const char *probeDir = "build-work/hfsplus-cfstring-boundary-probe";

    /*
     * NFC: café.txt
     * NFD: cafe + combining acute + .txt
     */
    const unsigned char nfcName[] = "caf" "\xC3" "\xA9" ".txt";

    char nfcPath[1024];
    char entryName[1024];
    size_t entryLen;

    struct statfs fsinfo;

    unsigned char *expectedNFD;
    size_t expectedNFDLen;
    LeoUTF8Status status;

    CFStringRef cfString;
    CFIndex cfMaxLen;
    char *cfFS;
    size_t cfFSLen;
    Boolean cfOK;

    NSString *nsString;
    const char *nsFS;
    size_t nsFSLen;

    int readdirMatchesNFD;
    int cfMatchesNFD;
    int nsMatchesNFD;

    pool = [[NSAutoreleasePool alloc] init];

    printf("LeoUTF8 HFS+ CFString/NSString boundary probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());

    system("rm -rf build-work/hfsplus-cfstring-boundary-probe");

    if (mkdir(probeDir, 0700) != 0) {
        perror("mkdir");
        [pool release];
        return 1;
    }

    if (statfs(probeDir, &fsinfo) == 0) {
        printf("filesystem type: %s\n", fsinfo.f_fstypename);
    } else {
        perror("statfs");
    }

    print_bytes("NFC input bytes", nfcName, sizeof(nfcName) - 1);

    expectedNFD = 0;
    expectedNFDLen = 0;

    status = LeoUTF8NormalizeNFDBytes(nfcName,
                                      sizeof(nfcName) - 1,
                                      &expectedNFD,
                                      &expectedNFDLen);

    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "LeoUTF8NormalizeNFDBytes failed: %s\n",
                LeoUTF8StatusMessage(status));
        [pool release];
        return 1;
    }

    print_bytes("LeoUTF8 expected NFD bytes", expectedNFD, expectedNFDLen);

    if (make_path(nfcPath, sizeof(nfcPath), probeDir, nfcName) != 0) {
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    if (create_file_exclusive(nfcPath, "created with NFC spelling\n") != 0) {
        perror("create NFC file");
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    if (read_single_entry(probeDir, entryName, sizeof(entryName)) != 0) {
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    entryLen = strlen(entryName);

    printf("readdir entry: %s\n", entryName);
    print_bytes("readdir bytes", (const unsigned char *)entryName, entryLen);

    readdirMatchesNFD = bytes_equal((const unsigned char *)entryName,
                                    entryLen,
                                    expectedNFD,
                                    expectedNFDLen);

    cfString = CFStringCreateWithFileSystemRepresentation(kCFAllocatorDefault,
                                                          entryName);

    if (cfString == 0) {
        fprintf(stderr, "CFStringCreateWithFileSystemRepresentation failed\n");
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    cfMaxLen = CFStringGetMaximumSizeOfFileSystemRepresentation(cfString);
    if (cfMaxLen <= 0) {
        fprintf(stderr, "CFStringGetMaximumSizeOfFileSystemRepresentation failed\n");
        CFRelease(cfString);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    cfFS = (char *)malloc((size_t)cfMaxLen);
    if (cfFS == 0) {
        fprintf(stderr, "malloc failed for CF filesystem buffer\n");
        CFRelease(cfString);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    cfOK = CFStringGetFileSystemRepresentation(cfString, cfFS, cfMaxLen);
    if (!cfOK) {
        fprintf(stderr, "CFStringGetFileSystemRepresentation failed\n");
        free(cfFS);
        CFRelease(cfString);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    cfFSLen = strlen(cfFS);

    printf("CFString filesystem representation: %s\n", cfFS);
    print_bytes("CFString filesystem bytes",
                (const unsigned char *)cfFS,
                cfFSLen);

    cfMatchesNFD = bytes_equal((const unsigned char *)cfFS,
                               cfFSLen,
                               expectedNFD,
                               expectedNFDLen);

    nsString = [[NSString alloc] initWithBytes:entryName
                                        length:entryLen
                                      encoding:NSUTF8StringEncoding];

    if (nsString == nil) {
        fprintf(stderr, "NSString initWithBytes failed\n");
        free(cfFS);
        CFRelease(cfString);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    nsFS = [nsString fileSystemRepresentation];
    if (nsFS == 0) {
        fprintf(stderr, "NSString fileSystemRepresentation failed\n");
        [nsString release];
        free(cfFS);
        CFRelease(cfString);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    nsFSLen = strlen(nsFS);

    printf("NSString filesystem representation: %s\n", nsFS);
    print_bytes("NSString filesystem bytes",
                (const unsigned char *)nsFS,
                nsFSLen);

    nsMatchesNFD = bytes_equal((const unsigned char *)nsFS,
                               nsFSLen,
                               expectedNFD,
                               expectedNFDLen);

    printf("Summary:\n");
    printf("readdir bytes match LeoUTF8 NFD: %s\n",
           readdirMatchesNFD ? "yes" : "no");
    printf("CFString filesystem bytes match LeoUTF8 NFD: %s\n",
           cfMatchesNFD ? "yes" : "no");
    printf("NSString filesystem bytes match LeoUTF8 NFD: %s\n",
           nsMatchesNFD ? "yes" : "no");

    [nsString release];
    free(cfFS);
    CFRelease(cfString);
    LeoUTF8Free(expectedNFD);
    [pool release];

    printf("LeoUTF8 HFS+ CFString/NSString boundary probe completed.\n");
    return 0;
}
