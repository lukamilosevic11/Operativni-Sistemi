#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define greska(a) {perror(a);exit(1);}

int main() {

    int pipeFds[2];

    if (-1 == pipe(pipeFds))
        greska("pipe failed");

    pid_t pid = fork();
    if (pid == -1)
        greska("fork failed");

    if (pid) {
        wait(NULL);
        close(pipeFds[1]);
        int bytesRead, i = 0, countSpace = 0, p;
        char buf[8192];
        bytesRead = read(pipeFds[0], buf, sizeof (buf));
        if (bytesRead == -1)
            greska("read failed");
        while (buf[i] != '\0') {
            if (buf[i] == '\n') {
                countSpace = 0;
                p = 1;
            } else if (buf[i] == ' ')
                countSpace++;
            else if (countSpace > 3 && isdigit(buf[i]) && p == 1) {
                printf("%c", buf[i]);
                if (buf[i + 1] == ' ') {
                    p = 0;
                    printf("\n");
                }
            }
            i++;
        }
        close(pipeFds[0]);
    } else {
        close(pipeFds[0]);
        if (-1 == dup2(pipeFds[1], STDOUT_FILENO))
            greska("dup2 failed");
        if (-1 == execlp("ls", "ls", "-l", NULL))
            greska("execl failed");
        close(pipeFds[1]);
        exit(0);
    }

    return 0;
}
