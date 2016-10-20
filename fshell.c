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
void name(char *line, char line_n[MAX_CH]);
void exit_term();
void mkfile(const char *filePath, int *fd);
void cat(const char *filePath, int *fd);
int name_insert(char *line, char *txt, char* filePath, int len);
void insert(const char *filePath, int *fd, char* txt, int pos);
void name_cp(char* line, char *fileSrc, char* fileDest, int len);
void cp_func(const char *fileSrc, const char *fileDest, int *fdSrc, int *fdDest);

int main(int argc, char **argv) {
    char  *line = NULL,
            *filePath = NULL,
            *txt = NULL,
            *fileSrc = NULL,
            *fileDest = NULL;
    size_t line_len = 0;
    int len, fd, pos, fdDest;

    printf("%% ");
    while ((len = getline(&line, &line_len, stdin)) != -1) {
        line[len - 1] = '\0';
        switch (rec(line)) {
            case 1:exit_term();
                break;
            case 2:
                filePath = (char*) malloc(len - 7);
                strncpy(filePath, line + 7, len - 7);
                mkfile(filePath, &fd);
                close(fd);
                free(filePath);
                break;
            case 3:
                filePath = (char*) malloc(len - 6);
                static mode_t mode = 0755;
                strncpy(filePath, line + 6, len - 6);
                mkdir(filePath, mode);
                free(filePath);
                break;
            case 4:
                filePath = (char*) malloc(len - 3);
                strncpy(filePath, line + 3, len - 3);
                unlink(filePath);
                free(filePath);
                break;
            case 5:
                filePath = (char*) malloc(len - 6);
                strncpy(filePath, line + 6, len - 6);
                rmdir(filePath);
                free(filePath);
                break;
            case 6:
                filePath = (char*) malloc(len - 4);
                strncpy(filePath, line + 4, len - 4);
                cat(filePath, &fd);
                close(fd);
                free(filePath);
                break;
            case 7:
                txt = (char*) malloc(len);
                filePath = (char*) malloc(len);
                if (txt == NULL || filePath == NULL)
                    printf("Error!\n");
                pos = name_insert(line, txt, filePath, len);
                insert(filePath, &fd, txt, pos);
                close(fd);
                free(txt);
                free(filePath);
                break;
            case 8:
                fileSrc = (char*) malloc(len);
                fileDest = (char*) malloc(len);
                if (fileDest == NULL || fileSrc == NULL)
                    printf("Error!");
                name_cp(line, fileSrc, fileDest, len);
                cp_func(fileSrc, fileDest, &fd, &fdDest);
                free(fileSrc);
                free(fileDest);
                close(fd);
                close(fdDest);
                break;
            case 9:
                printf("Unknown command!\n");
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

    static const uint32_t memBufSize = 1U << 13; // 8KB
    char *memBuf = malloc(memBufSize);
    if (NULL == memBuf)
        printf("Error!\n");

    int32_t readBytes;
    while ((readBytes = read(*fdSrc, memBuf, memBufSize)) > 0)
        write(*fdDest, memBuf, readBytes);

    free(memBuf);
}

void name_cp(char* line, char *fileSrc, char* fileDest, int len) {
    int i = 3, j = 0, p = 0;
    while (line[i] != '\0') {
        if (line[i] == ' ' && p == 0) {
            fileSrc[j] = '\0';
            p = 1;
            j = 0;
            i++;
        } else if (p == 0) {
            fileSrc[j] = line[i];
            i++;
            j++;
        } else if (p == 1) {
            fileDest[j] = line[i];
            i++;
            j++;
        }
    }
    fileDest[j] = '\0';
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

int name_insert(char* line, char *txt, char* filePath, int len) {
    int i = 7, j = 0, p = 0, pos = 0;
    char *tmp;
    while (line[i] != '\0') {
        if (line[i] == ' ' && p == 0) {
            txt[j] = '\0';
            i++;
            do {
                pos = pos + line[i] - '0';
                pos *= 10;
                i++;
            } while (isdigit(line[i]));
            pos /= 10;
            p = 1;
            j = 0;
            i++;
        } else if (p == 0) {
            txt[j] = line[i];
            i++;
            j++;
        } else if (p == 1) {
            filePath[j] = line[i];
            i++;
            j++;
        }
    }
    filePath[j] = '\0';
    return pos;
}

int rec(char *line) {
    char line_n[MAX_CH];
    name(line, line_n);
    if (strcmp(line_n, "exit") == 0)
        return 1;
    else if (strcmp(line_n, "mkfile") == 0)
        return 2;
    else if (strcmp(line_n, "mkdir") == 0)
        return 3;
    else if (strcmp(line_n, "rm") == 0)
        return 4;
    else if (strcmp(line_n, "rmdir") == 0)
        return 5;
    else if (strcmp(line_n, "cat") == 0)
        return 6;
    else if (strcmp(line_n, "insert") == 0)
        return 7;
    else if (strcmp(line_n, "cp") == 0)
        return 8;
    else
        return 9;
}

void name(char *line, char line_n[MAX_CH]) {
    int i = 0;
    while (line[i] != ' ' && line[i] != '\0') {
        line_n[i] = line[i];
        i++;
    }
    line_n[i] = '\0';
}

void exit_term() {
    _exit(EXIT_SUCCESS);
}

void mkfile(const char *filePath, int *fd) {

    static mode_t defaultMode = 0644;
    int flags = 0;

    flags |= O_TRUNC;
    flags |= O_CREAT;

    *fd = open(filePath, flags, defaultMode);

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
