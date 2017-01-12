#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define greska(a) {perror(a);exit(1);}
#define MAX_EVENTS (8)
#define MSG_LEN (127)

int palindrom(char *msg);
void ocisti_beline(char *s);

int main(int argc, char** argv) {
    int i, epollFd, numOfFifos, j = 0;

    if (argc < 2)
        greska("arg failed");

    numOfFifos = argc - 1;
    epollFd = epoll_create(numOfFifos);
    if (epollFd == -1)
        greska("epoll_create failed");

    struct epoll_event element;

    for (i = 0; i < numOfFifos; i++) {
        element.data.fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        if (element.data.fd == -1)
            greska("open failed");
        element.events = EPOLLIN;
        if (-1 == epoll_ctl(epollFd, EPOLL_CTL_ADD, element.data.fd, &element))
            greska("epoll_ctl failed");
    }

    struct epoll_event readyList[MAX_EVENTS];
    while (numOfFifos) {
        int readyFifoNum;
        readyFifoNum = epoll_wait(epollFd, readyList, MAX_EVENTS, -1);
        if (readyFifoNum == -1)
            greska("epoll_wait failed");

        char msg[MSG_LEN];
        char pom[MSG_LEN];
        for (i = 0; i < readyFifoNum; i++) {
            if (readyList[i].events & EPOLLIN) {
                if (-1 == read(readyList[i].data.fd, msg, MSG_LEN))
                    greska("read failed");

                while (msg[j] == ' ' || msg[j] == '\t' || msg[j] == ',' || msg[j] == '.' || isalpha(msg[j]))
                    j++;
                msg[j] = 0;
                strcpy(pom, msg);
                if (palindrom(msg))
                    printf("%s 1\n", pom);
                else
                    printf("%s 0\n", pom);
            } else if (readyList[i].events & (EPOLLERR | EPOLLHUP)) {
                close(readyList[i].data.fd);
                numOfFifos--;
            }
        }
    }
    close(epollFd);
    return 0;
}

int palindrom(char *msg) {
    int d, i, n;
    ocisti_beline(msg);
    d = strlen(msg);
    n = d - 1;
    for (i = 0; i < d / 2; i++)
        if (msg[i] != msg[n--])
            return 0;

    return 1;
}

void ocisti_beline(char *s) {
    int d = strlen(s), i, j;
    for (i = 0; i < d; i++) {
        if (isupper(s[i]))
            s[i] = tolower(s[i]);
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || !isalpha(s[i])) {
            if (isupper(s[i + 1]))
                s[i + 1] = tolower(s[i + 1]);
            for (j = i; j < d - 1; j++)
                s[j] = s[j + 1];
            d--;
            s[j] = '\0';
            i--;
        }
    }
}
