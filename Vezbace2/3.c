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
#include <stdint.h>

#define greska(a) {perror(a);exit(1);}

void handleSIGINT(int);
void handleSIGQUIT(int);
void handleSIGUSR1(int);
void handleSIGUSR2(int);

int number;

int main() {
    //printf("%jd\n", (intmax_t) getpid());
    do {
        scanf("%d", &number);
        if (SIG_ERR == signal(SIGUSR1, handleSIGUSR1))
            greska("signal failed");
        if (SIG_ERR == signal(SIGINT, handleSIGINT))
            greska("signal failed");
        if (SIG_ERR == signal(SIGUSR2, handleSIGUSR2))
            greska("signal failed");
        if (SIG_ERR == signal(SIGQUIT, handleSIGQUIT))
            greska("signal failed");
        pause();
    } while (true);


    return 0;
}

void handleSIGUSR1(int signum) {
    printf("%d\n", number * number);
}

void handleSIGUSR2(int signum) {
    printf("%d\n", number * number * number);
}

void handleSIGINT(int signum) {
    int cj, n = 0;
    while (number) {
        cj = number % 10;
        n = n * 10 + cj;
        number /= 10;
    }
    printf("%d\n", n);
}

void handleSIGQUIT(int signum) {
    exit(0);
}
