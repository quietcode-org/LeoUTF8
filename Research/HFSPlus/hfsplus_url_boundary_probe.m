#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <dirent.h>
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

    const char *probeDir = "build-work/hfsplus-url-boundary-probe";

    const unsigned char nfcName[] = "caf" "\xC3" "\xA9" ".txt";

    char nfcPath[1024];
    char entryName[1024];
    char entryPath[1024];

    size_t entryNameLen;
    size_t entryPathLen;

    struct statfs fsinfo;

    unsigned char *expectedNFD;
    size_t expectedNFDLen;
    LeoUTF8Status status;

    CFURLRef cfURL;
    UInt8 cfFS[2048];
    Boolean cfOK;
    size_t cfFSLen;

    NSString *pathString;
    NSURL *nsURL;
    NSString *nsURLPath;
    const char *nsFS;
    size_t nsFSLen;

    int entryMatchesNFD;
    int cfFilenameMatchesNFD;
    int nsFilenameMatchesNFD;

    pool = [[NSAutoreleasePool alloc] init];

    printf("LeoUTF8 HFS+ CFURL/NSURL boundary probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());

    system("rm -rf build-work/hfsplus-url-boundary-probe");

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

    print_bytes("NFC input filename bytes", nfcName, sizeof(nfcName) - 1);

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

    print_bytes("LeoUTF8 expected NFD filename bytes", expectedNFD, expectedNFDLen);

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

    entryNameLen = strlen(entryName);

    printf("readdir entry: %s\n", entryName);
    print_bytes("readdir filename bytes",
                (const unsigned char *)entryName,
                entryNameLen);

    entryMatchesNFD = bytes_equal((const unsigned char *)entryName,
                                  entryNameLen,
                                  expectedNFD,
                                  expectedNFDLen);

    if (make_path(entryPath, sizeof(entryPath), probeDir, (const unsigned char *)entryName) != 0) {
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    entryPathLen = strlen(entryPath);

    printf("filesystem path: %s\n", entryPath);
    print_bytes("filesystem path bytes",
                (const unsigned char *)entryPath,
                entryPathLen);

    cfURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                    (const UInt8 *)entryPath,
                                                    (CFIndex)entryPathLen,
                                                    false);

    if (cfURL == 0) {
        fprintf(stderr, "CFURLCreateFromFileSystemRepresentation failed\n");
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    cfOK = CFURLGetFileSystemRepresentation(cfURL,
                                            true,
                                            cfFS,
                                            (CFIndex)sizeof(cfFS));

    if (!cfOK) {
        fprintf(stderr, "CFURLGetFileSystemRepresentation failed\n");
        CFRelease(cfURL);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    cfFSLen = strlen((const char *)cfFS);

    printf("CFURL filesystem representation: %s\n", cfFS);
    print_bytes("CFURL filesystem bytes", cfFS, cfFSLen);

    if (cfFSLen < expectedNFDLen) {
        cfFilenameMatchesNFD = 0;
    } else {
        cfFilenameMatchesNFD =
            bytes_equal(cfFS + (cfFSLen - expectedNFDLen),
                        expectedNFDLen,
                        expectedNFD,
                        expectedNFDLen);
    }

    pathString = [[NSString alloc] initWithBytes:entryPath
                                          length:entryPathLen
                                        encoding:NSUTF8StringEncoding];

    if (pathString == nil) {
        fprintf(stderr, "NSString path creation failed\n");
        CFRelease(cfURL);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    nsURL = [NSURL fileURLWithPath:pathString];

    if (nsURL == nil) {
        fprintf(stderr, "NSURL fileURLWithPath failed\n");
        [pathString release];
        CFRelease(cfURL);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    nsURLPath = [nsURL path];

    if (nsURLPath == nil) {
        fprintf(stderr, "NSURL path failed\n");
        [pathString release];
        CFRelease(cfURL);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    nsFS = [nsURLPath fileSystemRepresentation];

    if (nsFS == 0) {
        fprintf(stderr, "NSURL path filesystem representation failed\n");
        [pathString release];
        CFRelease(cfURL);
        LeoUTF8Free(expectedNFD);
        [pool release];
        return 1;
    }

    nsFSLen = strlen(nsFS);

    printf("NSURL path filesystem representation: %s\n", nsFS);
    print_bytes("NSURL path filesystem bytes",
                (const unsigned char *)nsFS,
                nsFSLen);

    if (nsFSLen < expectedNFDLen) {
        nsFilenameMatchesNFD = 0;
    } else {
        nsFilenameMatchesNFD =
            bytes_equal((const unsigned char *)nsFS + (nsFSLen - expectedNFDLen),
                        expectedNFDLen,
                        expectedNFD,
                        expectedNFDLen);
    }

    printf("Summary:\n");
    printf("readdir filename bytes match LeoUTF8 NFD: %s\n",
           entryMatchesNFD ? "yes" : "no");
    printf("CFURL filesystem filename bytes match LeoUTF8 NFD: %s\n",
           cfFilenameMatchesNFD ? "yes" : "no");
    printf("NSURL filesystem filename bytes match LeoUTF8 NFD: %s\n",
           nsFilenameMatchesNFD ? "yes" : "no");

    [pathString release];
    CFRelease(cfURL);
    LeoUTF8Free(expectedNFD);
    [pool release];

    printf("LeoUTF8 HFS+ CFURL/NSURL boundary probe completed.\n");
    return 0;
}
