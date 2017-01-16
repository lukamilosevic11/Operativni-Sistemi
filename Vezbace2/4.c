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

#define greska(a) {perror(a);exit(1);}

char path[1024];
int max = -1;

int main(int argc, char** argv) {

    if (argc != 2)
        greska("args failed");

    int numOfFifo = atoi(argv[1]), epollFd;
    int d = numOfFifo;
    char buffer[1024];
    int *ids = malloc(numOfFifo * sizeof (int));
    if (ids == NULL)
        greska("ids allocation failed");

    if (-1 == (epollFd = epoll_create(numOfFifo)))
        greska("epoll_create failed");

    struct epoll_event currentElement;
    for (int i = 0; i < numOfFifo; i++) {
        memset(buffer, 0, 1024);
        currentElement.events = EPOLLIN;

        sprintf(buffer, "/tmp/%d", i);
        int fd;
        if (-1 == (currentElement.data.fd = open(buffer, O_RDONLY | O_NONBLOCK)))
            greska("open failed");
        ids[i] = currentElement.data.fd;
        if (-1 == epoll_ctl(epollFd, EPOLL_CTL_ADD, currentElement.data.fd, &currentElement))
            greska("epoll_ctl failed");
    }

    struct epoll_event readyList[8];
    while (numOfFifo) {
        int numOfReady;
        if (-1 == (numOfReady = epoll_wait(epollFd, readyList, 8, 0)))
            greska("epoll wait failed");
        for (int i = 0; i < numOfReady; i++) {
            if (readyList[i].events & EPOLLIN) {
                char buf[1024];
                int bytesRead;
                if (-1 == (bytesRead = read(readyList[i].data.fd, buf, sizeof (buf))))
                    greska("read failed");
                for (int j = 0; j < d; j++) {
                    if (readyList[i].data.fd == ids[j])
                        if (bytesRead > max) {
                            max = bytesRead;
                            sprintf(path, "/tmp/%d", j);
                            break;
                        }
                }
            } else if (readyList[i].events & (EPOLLHUP | EPOLLERR)) {
                close(readyList[i].data.fd);
                numOfFifo--;
            }
        }
    }
    printf("%s %d\n", path, max);
    free(ids);
    close(epollFd);

    return 0;
}
