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
#include <stdbool.h>
#include <pwd.h>
#include <errno.h>
#include <utime.h>
#include <grp.h>
#include <stdint.h>

bool IsRegGreater500(const char *fpath);
void osCatFile(const char* destPath, const char* srcPath);
void AllFiles(const char* filePath, const char* catFile);
int osNumberOfUsersInGroup(const char* filePath);

int main(int argc, char** argv) {
    char *path = malloc(strlen(argv[1]) + 1 + strlen(argv[2]) + 1);
    if (argc != 3) {
        perror("Arguments failed\n");
        exit(1);
    }
    strcpy(path, argv[1]);
    strcat(path, "/");
    strcat(path, argv[2]);


    AllFiles(argv[1], path);

    return 0;
}

void AllFiles(const char* filePath, const char* catFile) {
    struct stat finfo;
    if (stat(filePath, &finfo) == -1) {
        perror("stat() failed");
        exit(1);
    }
    if (IsRegGreater500(filePath)) {
        if (osNumberOfUsersInGroup(filePath) > 3) {
            static mode_t defaultMode = 0733;
            if (open(catFile, O_CREAT | O_RDWR | O_TRUNC, defaultMode) == -1) {
                perror("open failed");
                exit(1);
            }
            osCatFile(catFile, filePath);
        } else {
            struct utimbuf newTime;
            time_t ftime;

            ftime = finfo.st_atime;
            newTime.actime = ftime - 60 * 60 * 24 * 3;

            if (utime(filePath, &newTime) == -1) {
                perror("utime() failed");
                exit(1);
            }
        }
    } else if (S_ISDIR(finfo.st_mode)) {
        DIR *dir = opendir(filePath);
        struct dirent *entry = NULL;

        if (dir == NULL) {
            perror("opendir() failed\n");
            exit(1);
        }

        while (NULL != (entry = readdir(dir))) {
            char *newPath = malloc(strlen(filePath) + strlen(entry->d_name) + 2);
            if (newPath == NULL) {
                perror("newPath allocation failed");
                exit(1);
            }

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            strcpy(newPath, filePath);
            strcat(newPath, "/");
            strcat(newPath, entry->d_name);

            AllFiles(newPath, catFile);
            free(newPath);
        }
        if (closedir(dir) == -1) {
            perror("closedir() failed");
            exit(1);
        }
    }
}

bool IsRegGreater500(const char *fpath) {
    struct stat finfo;
    if (stat(fpath, &finfo) == -1) {
        perror("Stat failed");
        exit(1);
    }

    if (S_ISREG(finfo.st_mode) && (finfo.st_size > 500))
        return true;

    return false;
}

void osCatFile(const char* destPath, const char* srcPath) {

    static mode_t defaultMode = 0644;
    int fd, d;
    size_t len = 0;
    char *line = NULL;

    FILE *f = fopen(srcPath, "r");

    fd = open(destPath, O_RDWR | O_APPEND, defaultMode);

    if (fd == -1) {
        perror("Open failed");
        exit(1);
    }

    while ((d = getline(&line, &len, f)) != -1) {
        if (line[d - 1] == '\n')
            line[d - 1] = ' ';
        if (write(fd, line, strlen(line)) == -1) {
            perror("write() failed");
            exit(1);
        }
    }

    close(fd);
    fclose(f);
}

int osNumberOfUsersInGroup(const char* filePath) {
    int count = 0;
    struct stat finfo;
    struct passwd *user;
    stat(filePath, &finfo);
    setpwent();
    while (user = getpwent())
        if (user->pw_gid == finfo.st_gid)
            count++;
    endpwent();
    return count;
}
