#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#define greska(a) {perror(a);exit(1);}

typedef struct {
    int suma;
    pthread_mutex_t lock;
} mutex;

typedef struct {
    int id;
} ids;

mutex global;

int tnum, **prva_mat, **druga_mat;

void* thFunc(void* arg) {
    ids *x = arg;
    int sum = 0;
    for (int i = 0; i < tnum; i++)
        sum += prva_mat[x->id][i] * druga_mat[x->id][i];
    if (-1 == pthread_mutex_lock(&global.lock))
        greska("pthread_mutex_lock failed");
    global.suma += sum;
    if (-1 == pthread_mutex_unlock(&global.lock))
        greska("pthread_mutex_unlock failed");
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 2)
        greska("args failed");

    global.suma = 0;
    if (-1 == pthread_mutex_init(&global.lock, NULL))
        greska("failed pthread_mutex_init");
    FILE *f = fopen(argv[1], "r");
    fscanf(f, "%d", &tnum);

    prva_mat = malloc(tnum * sizeof (int*));
    if (prva_mat == NULL)
        greska("prva_mat failed");
    for (int i = 0; i < tnum; i++) {
        prva_mat[i] = malloc(tnum * sizeof (int));
        if (*prva_mat == NULL)
            greska("allocation");
    }

    druga_mat = malloc(tnum * sizeof (int*));
    if (druga_mat == NULL)
        greska("prva_mat failed");
    for (int i = 0; i < tnum; i++) {
        druga_mat[i] = malloc(tnum * sizeof (int));
        if (*druga_mat == NULL)
            greska("allocation");
    }

    ids *niz = malloc(tnum * sizeof (ids));
    if (niz == NULL)
        greska("niz allocation failed");
    for (int i = 0; i < tnum; i++)
        for (int j = 0; j < tnum; j++)
            fscanf(f, "%d", &prva_mat[i][j]);
    for (int i = 0; i < tnum; i++)
        for (int j = 0; j < tnum; j++)
            fscanf(f, "%d", &druga_mat[i][j]);
    // ispis
    /*for (int i = 0; i < tnum; i++) {
        for (int j = 0; j < tnum; j++)
            printf("%d ", prva_mat[i][j]);
        printf("\n");
    }
    for (int i = 0; i < tnum; i++) {
        for (int j = 0; j < tnum; j++)
            printf("%d ", druga_mat[i][j]);
        printf("\n");
    }*/

    pthread_t *tids = malloc(tnum * sizeof (pthread_t));
    if (tids == NULL)
        greska("tids allocation failed");
    for (int i = 0; i < tnum; i++) {
        niz[i].id = i;
        if (-1 == pthread_create(tids + i, NULL, thFunc, niz + i))
            greska("pthread_create failed");
    }

    for (int i = 0; i < tnum; i++) {
        if (-1 == pthread_join(tids[i], NULL))
            greska("pthread_join failed");
    }
    printf("%d\n", global.suma);
    if (-1 == pthread_mutex_destroy(&global.lock))
        greska("pthread_mutex_destroy failed");

    free(tids);
    return 0;
}
