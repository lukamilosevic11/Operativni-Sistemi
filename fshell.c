#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_CH 10

int rec(char *line);
void exit_term();
void mkfile(const char *filePath, int *fd);
void cat(const char *filePath, int *fd);
void insert(const char *filePath, int *fd, char* txt, int pos);
void cp_func(const char *fileSrc, const char *fileDest, int *fdSrc, int *fdDest);

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
        sscanf(line, "%s", name);
        switch (rec(name)) {
            case 1:exit_term();
                break;
            case 2:
                filePath = (char*) malloc(len - 7);
                strncpy(filePath, line + 7, len - 7);
                mkfile(filePath, &fd);
                close(fd);
                free(filePath);
                free(name);
                break;
            case 3:
                filePath = (char*) malloc(len - 6);
                static mode_t mode = 0755;
                strncpy(filePath, line + 6, len - 6);
                mkdir(filePath, mode);
                free(filePath);
                free(name);
                break;
            case 4:
                filePath = (char*) malloc(len - 3);
                strncpy(filePath, line + 3, len - 3);
                unlink(filePath);
                free(filePath);
                free(name);
                break;
            case 5:
                filePath = (char*) malloc(len - 6);
                strncpy(filePath, line + 6, len - 6);
                rmdir(filePath);
                free(filePath);
                free(name);
                break;
            case 6:
                filePath = (char*) malloc(len - 4);
                strncpy(filePath, line + 4, len - 4);
                cat(filePath, &fd);
                close(fd);
                free(filePath);
                free(name);
                break;
            case 7:
                txt = (char*) malloc(len);
                filePath = (char*) malloc(len);
                if (txt == NULL || filePath == NULL)
                    printf("Error!\n");
                sscanf(line, "%s %s %d %s", name, txt, &pos, filePath);
                insert(filePath, &fd, txt, pos);
                close(fd);
                free(txt);
                free(filePath);
                free(name);
                break;
            case 8:
                fileSrc = (char*) malloc(len);
                fileDest = (char*) malloc(len);
                if (fileDest == NULL || fileSrc == NULL)
                    printf("Error!");
                sscanf(line, "%s %s %s", name, fileSrc, fileDest);
                cp_func(fileSrc, fileDest, &fd, &fdDest);
                free(fileSrc);
                free(fileDest);
                free(name);
                close(fd);
                close(fdDest);
                break;
            case 9:
                printf("Unknown command!\n");
                free(name);
                break;
        }
        printf("%% ");
    }
    free(line);

    return 0;
}

void cp_func(const char *fileSrc, const char *fileDest, int *fdSrc, int *fdDest) {
    static mode_t defaultMode = 0644;

    *fdSrc = open(fileSrc, O_RDONLY, defaultMode);
    *fdDest = open(fileDest, O_WRONLY | O_TRUNC | O_CREAT, defaultMode);

    static const uint32_t memBufSize = 1U << 13;
    char *memBuf = malloc(memBufSize);
    if (NULL == memBuf)
        printf("Error!\n");

    int32_t readBytes;
    while ((readBytes = read(*fdSrc, memBuf, memBufSize)) > 0)
        write(*fdDest, memBuf, readBytes);

    free(memBuf);
}

void insert(const char *filePath, int *fd, char* txt, int pos) {

    static mode_t defaultMode = 0644;
    int memBufSize;

    *fd = open(filePath, O_RDWR, defaultMode);

    lseek(*fd, pos, SEEK_SET);

    memBufSize = strlen(txt);

    write(*fd, txt, memBufSize);

    close(*fd);
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
    else
        return 9;
}

void exit_term() {
    exit(EXIT_SUCCESS);
}

void mkfile(const char *filePath, int *fd) {

    static mode_t defaultMode = 0644;

    *fd = open(filePath, O_TRUNC | O_CREAT, defaultMode);

}

void cat(const char *filePath, int *fd) {
    static mode_t defaultMode = 0644;
    *fd = open(filePath, O_RDONLY, defaultMode);

    static const uint32_t memBufSize = 1U << 13;
    char *memBuf = malloc(memBufSize);
    if (NULL == memBuf)
        printf("Error!\n");

    int32_t readBytes;
    while ((readBytes = read(*fd, memBuf, memBufSize)) > 0)
        write(STDOUT_FILENO, memBuf, readBytes);

    free(memBuf);
}
