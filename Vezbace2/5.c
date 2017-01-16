#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

#define greska(a) {perror(a);exit(1);}

int main(int argc, char** argv) {

    if (argc != 2)
        greska("args failed");
    char buf[127];
    int fd;
    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        greska("open failed");

    FILE *f = fdopen(fd, "r+");
    while (EOF != fscanf(f, "%s", buf))
        if (!strncasecmp("milivoje", buf, 8)) {
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = 0;
            lock.l_len = strlen(buf);
            if (-1 == fcntl(fd, F_GETLK, &lock))
                greska("fcntl fgetlk failed");
            if (lock.l_type == F_UNLCK) {
                lock.l_type = F_WRLCK;
                if (-1 == fcntl(fd, F_SETLK, &lock))
                    greska("fcntl fsetlk failed");
            } else {
                char c = lock.l_type == F_WRLCK ? 'w' : 'r';
                printf("%c %ld %jd\n", c, ftell(f) - strlen(buf), (intmax_t) lock.l_pid);
                fseek(f, lock.l_len, SEEK_CUR);
                continue;
            }
            if (-1 == fseek(f, -strlen(buf), SEEK_CUR))
                greska("fseek failed");
            fprintf(f, "dragutin");
            lock.l_type = F_UNLCK;
            if (-1 == fcntl(fd, F_SETLK, &lock))
                greska("fcntl failed");
        }

    close(fd);

    return 0;
}
