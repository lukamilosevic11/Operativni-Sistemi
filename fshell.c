#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <pwd.h>
#include<errno.h>
#include <grp.h>

int rec(char *line);
void exit_term();
bool mkfile(const char *filePath, int *fd);
bool cat(const char *filePath, int *fd);
bool insert(const char *filePath, int *fd, char* txt, int pos);
bool cp_func(const char *fileSrc, const char *fileDest, int *fdSrc, int *fdDest);
void ErrorFatalImpl(const char *userMsg, const char *fileName,
        const char *functionName, const int lineNum);
bool FInfo(const char* filePath);

#define MAX_CH 10
#define ErrorFatal(userMsg) ErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define Assert(expr, userMsg) \
    do { \
        if (!(expr)) \
            ErrorFatal(userMsg); \
    } while(0)

int main(int argc, char **argv) {
    char *line = NULL,
            *filePath = NULL,
            *txt = NULL,
            *fileSrc = NULL,
            *fileDest = NULL,
            *name = NULL;
    size_t line_len = 0;
    int len, fd, pos, fdDest;

    printf("%% ");
    while ((len = getline(&line, &line_len, stdin)) != -1) {
        line[len - 1] = '\0';
        name = (char*) malloc(MAX_CH);
        if (name == NULL)
            ErrorFatal("Allocate.");
        sscanf(line, "%s", name);
        switch (rec(name)) {
            case 1:exit_term();
                break;
            case 2:
                filePath = (char*) malloc(len - 7);
                if (filePath == NULL)
                    ErrorFatal("Allocate.");
                strncpy(filePath, line + 7, len - 7);
                Assert(mkfile(filePath, &fd), "mkfile");
                close(fd);
                free(filePath);
                free(name);
                break;
            case 3:
                filePath = (char*) malloc(len - 6);
                if (filePath == NULL)
                    ErrorFatal("Allocate.");
                static mode_t mode = 0755;
                strncpy(filePath, line + 6, len - 6);
                if (mkdir(filePath, mode) == -1)
                    printf("%s\n", strerror(errno));
                free(filePath);
                free(name);
                break;
            case 4:
                filePath = (char*) malloc(len - 3);
                if (filePath == NULL)
                    ErrorFatal("Allocate.");
                strncpy(filePath, line + 3, len - 3);
                if (unlink(filePath) == -1)
                    printf("%s\n", strerror(errno));
                free(filePath);
                free(name);
                break;
            case 5:
                filePath = (char*) malloc(len - 6);
                if (filePath == NULL)
                    ErrorFatal("Allocate.");
                strncpy(filePath, line + 6, len - 6);
                if (rmdir(filePath) == -1)
                    printf("%s\n", strerror(errno));
                free(filePath);
                free(name);
                break;
            case 6:
                filePath = (char*) malloc(len - 4);
                if (filePath == NULL)
                    ErrorFatal("Allocate.");
                strncpy(filePath, line + 4, len - 4);
                Assert(cat(filePath, &fd), "cat");
                close(fd);
                free(filePath);
                free(name);
                break;
            case 7:
                txt = (char*) malloc(len);
                filePath = (char*) malloc(len);
                if (txt == NULL || filePath == NULL)
                    ErrorFatal("Allocate.");
                sscanf(line, "%s %s %d %s", name, txt, &pos, filePath);
                Assert(insert(filePath, &fd, txt, pos), "insert");
                close(fd);
                free(txt);
                free(filePath);
                free(name);
                break;
            case 8:
                fileSrc = (char*) malloc(len);
                fileDest = (char*) malloc(len);
                if (fileDest == NULL || fileSrc == NULL)
                    ErrorFatal("Allocate.");
                sscanf(line, "%s %s %s", name, fileSrc, fileDest);
                Assert(cp_func(fileSrc, fileDest, &fd, &fdDest), "cp");
                free(fileSrc);
                free(fileDest);
                free(name);
                close(fd);
                close(fdDest);
                break;
            case 9:
                filePath = (char*) malloc(len - 5);
                if (filePath == NULL)
                    ErrorFatal("Allocate.");
                strncpy(filePath, line + 5, len - 5);
                if (!FInfo(filePath))
                    printf("%s\n", strerror(errno));
                free(filePath);
                break;
            case 10:
                printf("Unknown command!\n");
                free(name);
                break;
        }
        printf("%% ");
    }
    free(line);

    return 0;
}

void ErrorFatalImpl(const char *userMsg, const char *fileName,
        const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}

bool FInfo(const char* filePath) {
    struct stat finfo;
    if (stat(filePath, &finfo) == -1)
        return false;

    if (S_ISREG(finfo.st_mode))
        printf("Type: regular file\n");
    else if (S_ISDIR(finfo.st_mode))
        printf("Type: directory\n");
    else if (S_ISCHR(finfo.st_mode))
        printf("Type: character device\n");
    else if (S_ISBLK(finfo.st_mode))
        printf("Type: block device\n");
    else if (S_ISFIFO(finfo.st_mode))
        printf("Type: FIFO (pipe)\n");
    else if (S_ISLNK(finfo.st_mode))
        printf("Type: symbolic link\n");
    else if (S_ISSOCK(finfo.st_mode))
        printf("Type: socket\n");

    struct passwd *uI = getpwuid(finfo.st_uid);
    printf("Owner: %s\n", uI->pw_name);

    struct group *groupI = getgrgid(finfo.st_gid);
    printf("Group: %s\n", groupI->gr_name);

    printf("Size: %ju\n", (intmax_t) finfo.st_size);
        
        if ((S_IRUSR & finfo.st_mode)) u += 4; //KEFI AKO SE NEKO PITA!!!!1
	if ((S_IWUSR & finfo.st_mode)) u += 2;
	if ((S_IXUSR & finfo.st_mode)) u++;
	if ((S_IRGRP & finfo.st_mode)) g += 4;
	if ((S_IWGRP & finfo.st_mode)) g += 2;
	if ((S_IXGRP & finfo.st_mode)) g++;
	if ((S_IROTH & finfo.st_mode)) o += 4;
	if ((S_IWOTH & finfo.st_mode)) o += 2;
    if ((S_IXOTH & finfo.st_mode)) o++;
	printf("Access: 0%d%d%d\n", u, g, o);

    return true;
}

bool cp_func(const char *fileSrc, const char *fileDest, int *fdSrc, int *fdDest) {
    static mode_t defaultMode = 0644;

    *fdSrc = open(fileSrc, O_RDONLY, defaultMode);
    *fdDest = open(fileDest, O_WRONLY | O_TRUNC | O_CREAT, defaultMode);
    if (*fdSrc == -1 || *fdDest == -1) {
        printf("%s\n", strerror(errno));
        return true;
    }

    static const uint32_t memBufSize = 1U << 13;
    char *memBuf = malloc(memBufSize);
    if (NULL == memBuf)
        return false;

    int32_t readBytes;
    while ((readBytes = read(*fdSrc, memBuf, memBufSize)) > 0)
        if (write(*fdDest, memBuf, readBytes) == -1) {
            free(memBuf);
            return false;
        }

    free(memBuf);

    return true;
}

bool insert(const char *filePath, int *fd, char* txt, int pos) {

    static mode_t defaultMode = 0644;
    int memBufSize;

    *fd = open(filePath, O_RDWR, defaultMode);
    if (*fd == -1) {
        printf("%s\n", strerror(errno));
        return true;
    }

    if (lseek(*fd, pos, SEEK_SET) == -1)
        return false;

    memBufSize = strlen(txt);

    if (write(*fd, txt, memBufSize) == -1)
        return false;

    close(*fd);

    return true;
}

int rec(char *name) {
    if (strcmp(name, "exit") == 0)
        return 1;
    else if (strcmp(name, "mkfile") == 0)
        return 2;
    else if (strcmp(name, "mkdir") == 0)
        return 3;
    else if (strcmp(name, "rm") == 0)
        return 4;
    else if (strcmp(name, "rmdir") == 0)
        return 5;
    else if (strcmp(name, "cat") == 0)
        return 6;
    else if (strcmp(name, "insert") == 0)
        return 7;
    else if (strcmp(name, "cp") == 0)
        return 8;
    else if (strcmp(name, "info") == 0)
        return 9;
    else
        return 10;
}

void exit_term() {
    exit(EXIT_SUCCESS);
}

bool mkfile(const char *filePath, int *fd) {

    static mode_t defaultMode = 0644;

    *fd = open(filePath, O_TRUNC | O_CREAT, defaultMode);
    if (*fd == -1) {
        printf("%s\n", strerror(errno));
        return true;
    }

    return true;
}

bool cat(const char *filePath, int *fd) {
    static mode_t defaultMode = 0644;
    *fd = open(filePath, O_RDONLY, defaultMode);
    if (*fd == -1) {
        printf("%s\n", strerror(errno));
        return true;
    }

    static const uint32_t memBufSize = 1U << 13;
    char *memBuf = malloc(memBufSize);
    if (NULL == memBuf)
        return false;

    int32_t readBytes;
    while ((readBytes = read(*fd, memBuf, memBufSize)) > 0)
        if (write(STDOUT_FILENO, memBuf, readBytes) == -1) {
            free(memBuf);
            return false;
        }
    free(memBuf);

    return true;
}
