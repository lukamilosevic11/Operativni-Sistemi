#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <ctype.h>

#define greska(a) {perror(a);exit(1);}

int main(int argc, char** argv) {

    if (argc != 2)
        greska("args failed");

    int pipeFds[2];

    if (-1 == pipe(pipeFds))
        greska("pipe failed");

    pid_t childPid = fork();

    if (childPid) {
        //roditelj
        close(pipeFds[1]);
        wait(&childPid);
        if (WEXITSTATUS(childPid))
            printf("Neuspeh\n");
        char buf[127];
        FILE *f = fdopen(pipeFds[0], "r");
        fgets(buf, 127, f);
        fprintf(stdout, "%s", buf);
        close(pipeFds[0]);
    } else {
        //dete
        close(pipeFds[0]);
        FILE *f = fopen(argv[1], "r");
        char buf[125];
        char p[127] = "echo \"";
        fgets(buf, 125, f);
        buf[strlen(buf) - 1] = '\0';
        strcat(p, buf);
        strcat(p, "\" | tr a A");

        if (-1 == dup2(pipeFds[1], STDOUT_FILENO))
            greska("dup2 failed");
        if (-1 == system(p))
            greska("system failed");
        close(pipeFds[1]);
    }
    return 0;
}
