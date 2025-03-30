#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t canRead = PTHREAD_COND_INITIALIZER;
pthread_cond_t canWrite = PTHREAD_COND_INITIALIZER;

int nbReaders = 0;
int nbWriters = 0;

void* writer(void* arg);
void* reader(void* arg);

int main(void) {
    pthread_t r1, r2, w1;
    pthread_create(&r1, NULL, reader, "reader 1");
    pthread_create(&r2, NULL, reader, "reader 2");
    pthread_create(&w1, NULL, writer, "writer");
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w1, NULL);
    return 0;
}

void* reader(void* arg) {
    pthread_mutex_lock(&mutex);
    while (nbWriters > 0) {
        pthread_cond_wait(&canRead, &mutex);
    }
    nbReaders++;
    pthread_mutex_unlock(&mutex);
    printf("%s is reading...\n", (char*)arg);
    sleep(1);
    printf("%s finished reading.\n", (char*)arg);
    pthread_mutex_lock(&mutex);
    nbReaders--;
    if (nbReaders == 0) {
        pthread_cond_signal(&canWrite);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* writer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (nbReaders > 0 || nbWriters > 0) {
        pthread_cond_wait(&canWrite, &mutex);
    }
    nbWriters++;
    pthread_mutex_unlock(&mutex);
    printf("%s is writing...\n", (char*)arg);
    sleep(2);
    printf("%s finished writing.\n", (char*)arg);
    pthread_mutex_lock(&mutex);
    nbWriters--;
    pthread_cond_signal(&canWrite);
    pthread_cond_broadcast(&canRead);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
