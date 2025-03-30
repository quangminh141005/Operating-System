#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex, exclusive;
int nbReader = 0;

void *reader(void *args);
void *writer(void *args);

int main(void ) {
    pthread_t r1, r2, w1;
    sem_init(&mutex, 0, 1);
    sem_init(&exclusive, 0, 1);

    // Creating processes
    pthread_create(&r1, NULL, reader, "reader 1");
    pthread_create(&r2, NULL, reader, "reader 2");
    pthread_create(&w1, NULL, writer, "writer");
    
    // Start the process
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w1, NULL);
    
    // Destroy the process
    sem_destroy(&mutex);
    sem_destroy(&exclusive);
}

void *reader(void *args) {
    sem_wait(&mutex);
    nbReader++;
    if (nbReader == 1) sem_wait(&exclusive);
    sem_post(&mutex);

    printf("%s is reading...\n", (char*)args);
    sleep(3);

    sem_wait(&mutex);
    nbReader--;
    if (nbReader == 0) sem_post(&exclusive);
    sem_post(&mutex);

    printf("%s finished!\n", (char*)args);
    return NULL;
}

void *writer(void *args) {
    sem_wait(&exclusive);
    printf("%s is writing...\n", (char*)args);
    sleep(3);
    sem_post(&exclusive);
    printf("%s finished!\n", (char*)args);
    return NULL;
}

/*
OUTPUT: 
reader 1 is reading...
reader 2 is reading...
reader 1 finished!
reader 2 finished!
writer is writing...
writer finished!
*/