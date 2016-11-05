#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void osCatFileFromPos(const char* destPath,
        const char* srcPath,
        const off_t pos);

int main(int argc, char** argv) {

    if (argc != 4) {
        perror("Arguments failed");
        exit(1);
    }

    osCatFileFromPos(argv[1], argv[2], (off_t) atoi(argv[3]));

    return 0;
}

void osCatFileFromPos(const char* destPath,
        const char* srcPath,
        const off_t pos) {
    static mode_t defaultMode = 0644;
    int fd, d;
    size_t len = 0;
    char *line = NULL;

    FILE *f = fopen(srcPath, "r");

    fd = open(destPath, O_RDWR, defaultMode);

    if (fd == -1) {
        perror("Open failed");
        exit(1);
    }

    while ((getline(&line, &len, f)) != -1);

    if (lseek(fd, pos, SEEK_SET) == -1) {
        perror("lseek failed");
        exit(1);
    }

    if (write(fd, line, strlen(line)) == -1) {
        perror("write failed");
        exit(1);
    }

    close(fd);
    fclose(f);
}
