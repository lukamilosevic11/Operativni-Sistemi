#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <pthread.h>

#define greska(a) {perror(a);exit(1);}

static atomic_int globalSum;

void* threadFunction(void* arg) {
    char* path = arg;
    FILE *f = fopen(path, "r");
    if (f == NULL)
        greska("fopen failed");

    // -45 +43
    int sum = 0;
    char c;
    while (1 == fscanf(f, "%c", &c)) {
        if (c == '+')
            sum += '+';
        else if (c == '-')
            sum -= '-';
    }
    atomic_fetch_add(&globalSum, sum);

    return NULL;
}

int main(int argc, char** argv) {
    int tnum;

    if (argc < 2)
        greska("args failed");

    tnum = argc - 1;
    pthread_t *tids = malloc(tnum * sizeof (pthread_t));
    if (tids == NULL)
        greska("tids allocation failed");

    atomic_init(&globalSum, 0);

    for (int i = 0; i < tnum; i++)
        if (pthread_create(tids + i, NULL, threadFunction, argv[i + 1]))
            greska("pthread_create failed");

    for (int i = 0; i < tnum; i++)
        if (pthread_join(tids[i], NULL))
            greska("pthread_create failed");

    printf("%d\n", atomic_load(&globalSum));
    free(tids);

    return 0;
}
