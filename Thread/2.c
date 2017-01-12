#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

#define greska(a) {perror(a);exit(1);}
#define DATA_LEN 127

typedef struct {
    int id;
} thids;

static int g_Num;
static char g_Data[DATA_LEN];
static pthread_mutex_t g_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_Signaler = PTHREAD_COND_INITIALIZER;

void* threadFunction(void* arg) {
    int numLL = 0;
    thids *idx = arg;
    while (1) {
        if (pthread_mutex_lock(&g_Lock))
            greska("pthread_cond_lock failed");

            if (pthread_cond_wait(&g_Signaler, &g_Lock))
                greska("pthread_cond_wait failed");

        if (g_Num == idx->id) {
            int d = strlen(g_Data);
            for (int i = 0; i < d; i++)
                if (islower(g_Data[i]))
                    numLL++;
            g_Num = -2;
            printf("%d %d\n", idx->id, numLL);
            numLL = 0;
        }

        if (-1 == g_Num) {
            if (pthread_mutex_unlock(&g_Lock))
                greska("pthread_mutex_unlock failed");
            break;
        }
        if (pthread_mutex_unlock(&g_Lock))
            greska("pthread_mutex_unlock failed");
    }

    return NULL;
}

int main(int argc, char** argv) {
    int tnum;

    if (argc != 2)
        greska("args failed");

    tnum = atoi(argv[1]);
    pthread_t *tids = malloc(tnum * sizeof (pthread_t));
    if (tids == NULL)
        greska("tids allocation failed");

    thids *ids = malloc(tnum * sizeof (thids));
    for (int i = 0; i < tnum; i++) {
        ids[i].id = i;
        if (pthread_create(tids + i, NULL, threadFunction, ids + i))
            greska("pthread_create failed");
    }

    char *line = NULL;
    size_t lineLen = 0;
    while (-1 != getline(&line, &lineLen, stdin)) {
        if (pthread_mutex_lock(&g_Lock))
            greska("pthread_mutex_lock failed");

        sscanf(line, "%d", &g_Num);
        strcpy(g_Data, line + 2);

        if (-1 == g_Num) {
            if (pthread_mutex_unlock(&g_Lock))
                greska("pthread_mutex_unlock failed");
            if (pthread_cond_broadcast(&g_Signaler))
                greska("pthread_cond_broadcast failed");
            break;
        }

        if (pthread_mutex_unlock(&g_Lock))
            greska("pthread_mutex_unlock failed");
        if (pthread_cond_broadcast(&g_Signaler))
            greska("pthread_cond_signal failed");
    }

    for (int i = 0; i < tnum; ++i)
        if (pthread_join(tids[i], NULL))
            greska("pthread_join failed");

    free(line);
    if (pthread_mutex_destroy(&g_Lock))
        greska("pthread_mutex_destroy failed");
    if (pthread_cond_destroy(&g_Signaler))
        greska("pthread_cond_destroy failed");

    return 0;
}
