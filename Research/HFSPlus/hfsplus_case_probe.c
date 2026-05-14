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

static int make_path_bytes(char *out,
                           size_t outSize,
                           const char *dir,
                           const unsigned char *name,
                           size_t nameLen)
{
    size_t dirLen;

    dirLen = strlen(dir);

    if (dirLen + 1 + nameLen + 1 > outSize) {
        fprintf(stderr, "path too long\n");
        return 1;
    }

    memcpy(out, dir, dirLen);
    out[dirLen] = '/';
    memcpy(out + dirLen + 1, name, nameLen);
    out[dirLen + 1 + nameLen] = '\0';

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

static int print_directory_entries(const char *dirPath)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirPath);
    if (dir == 0) {
        perror("opendir");
        return 1;
    }

    printf("Directory entries:\n");

    while ((entry = readdir(dir)) != 0) {
        size_t nameLen;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        nameLen = strlen(entry->d_name);

        printf("entry: %s\n", entry->d_name);
        print_bytes("entry bytes", (const unsigned char *)entry->d_name, nameLen);
    }

    closedir(dir);
    return 0;
}

static int print_leoutf8_forms(const char *label,
                               const unsigned char *name,
                               size_t nameLen)
{
    unsigned char *nfd;
    unsigned char *casefold;
    size_t nfdLen;
    size_t casefoldLen;
    LeoUTF8Status status;

    nfd = 0;
    casefold = 0;
    nfdLen = 0;
    casefoldLen = 0;

    status = LeoUTF8NormalizeNFDBytes(name, nameLen, &nfd, &nfdLen);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "%s NFD failed: %s\n", label, LeoUTF8StatusMessage(status));
        return 1;
    }

    status = LeoUTF8CaseFoldBytes(name, nameLen, &casefold, &casefoldLen);
    if (status != LEO_UTF8_OK) {
        fprintf(stderr, "%s casefold failed: %s\n", label, LeoUTF8StatusMessage(status));
        LeoUTF8Free(nfd);
        return 1;
    }

    print_bytes("LeoUTF8 NFD bytes", nfd, nfdLen);
    print_bytes("LeoUTF8 casefold bytes", casefold, casefoldLen);

    LeoUTF8Free(casefold);
    LeoUTF8Free(nfd);

    return 0;
}

static int run_case_test(const char *baseDir,
                         const char *testId,
                         const char *label,
                         const unsigned char *firstName,
                         size_t firstLen,
                         const unsigned char *secondName,
                         size_t secondLen)
{
    char testDir[1024];
    char firstPath[1024];
    char secondPath[1024];
    int secondMatchedExisting;

    printf("\n=== %s ===\n", label);

    if (snprintf(testDir, sizeof(testDir), "%s/%s", baseDir, testId) >= (int)sizeof(testDir)) {
        fprintf(stderr, "test dir path too long\n");
        return 1;
    }

    if (mkdir(testDir, 0700) != 0) {
        perror("mkdir test dir");
        return 1;
    }

    printf("first name: %s\n", firstName);
    print_bytes("first bytes", firstName, firstLen);
    if (print_leoutf8_forms("first name", firstName, firstLen) != 0) {
        return 1;
    }

    printf("second name: %s\n", secondName);
    print_bytes("second bytes", secondName, secondLen);
    if (print_leoutf8_forms("second name", secondName, secondLen) != 0) {
        return 1;
    }

    if (make_path_bytes(firstPath, sizeof(firstPath), testDir, firstName, firstLen) != 0) {
        return 1;
    }

    if (make_path_bytes(secondPath, sizeof(secondPath), testDir, secondName, secondLen) != 0) {
        return 1;
    }

    if (create_file_exclusive(firstPath, "first spelling\n") != 0) {
        perror("create first file");
        return 1;
    }

    secondMatchedExisting = 0;

    if (create_file_exclusive(secondPath, "second spelling\n") != 0) {
        if (errno == EEXIST) {
            secondMatchedExisting = 1;
            printf("second O_EXCL create result: EEXIST\n");
        } else {
            perror("create second file");
            return 1;
        }
    } else {
        printf("second O_EXCL create result: separate file created\n");
    }

    if (print_directory_entries(testDir) != 0) {
        return 1;
    }

    printf("Summary for %s:\n", label);
    printf("second spelling matched existing file: %s\n",
           secondMatchedExisting ? "yes" : "no");

    return 0;
}

int main(void)
{
    const char *probeDir = "build-work/hfsplus-case-probe";
    struct statfs fsinfo;

    const unsigned char asciiLower[] = "case.txt";
    const unsigned char asciiUpper[] = "CASE.txt";

    const unsigned char umlautLower[] = "\xC3" "\xA4" ".txt";
    const unsigned char umlautUpper[] = "\xC3" "\x84" ".txt";

    const unsigned char cafeLower[] = "caf" "\xC3" "\xA9" ".txt";
    const unsigned char cafeUpper[] = "CAF" "\xC3" "\x89" ".TXT";

    const unsigned char sharpLower[] =
        "stra" "\xC3" "\x9F" "e.txt";
    const unsigned char sharpUpperSS[] =
        "STRASSE.TXT";

    printf("LeoUTF8 HFS+ case comparison probe\n");
    printf("LeoUTF8 version: %s\n", LeoUTF8Version());

    system("rm -rf build-work/hfsplus-case-probe");

    if (mkdir(probeDir, 0700) != 0) {
        perror("mkdir probe dir");
        return 1;
    }

    if (statfs(probeDir, &fsinfo) == 0) {
        printf("filesystem type: %s\n", fsinfo.f_fstypename);
    } else {
        perror("statfs");
    }

    if (run_case_test(probeDir,
                      "t1-ascii",
                      "ASCII case: case.txt vs CASE.txt",
                      asciiLower,
                      sizeof(asciiLower) - 1,
                      asciiUpper,
                      sizeof(asciiUpper) - 1) != 0) {
        return 1;
    }

    if (run_case_test(probeDir,
                      "t2-umlaut",
                      "Umlaut case: ä.txt vs Ä.txt",
                      umlautLower,
                      sizeof(umlautLower) - 1,
                      umlautUpper,
                      sizeof(umlautUpper) - 1) != 0) {
        return 1;
    }

    if (run_case_test(probeDir,
                      "t3-cafe",
                      "Accent case: café.txt vs CAFÉ.TXT",
                      cafeLower,
                      sizeof(cafeLower) - 1,
                      cafeUpper,
                      sizeof(cafeUpper) - 1) != 0) {
        return 1;
    }

    if (run_case_test(probeDir,
                      "t4-sharp-s",
                      "Sharp S: straße.txt vs STRASSE.TXT",
                      sharpLower,
                      sizeof(sharpLower) - 1,
                      sharpUpperSS,
                      sizeof(sharpUpperSS) - 1) != 0) {
        return 1;
    }

    printf("\nLeoUTF8 HFS+ case comparison probe completed.\n");
    return 0;
}
