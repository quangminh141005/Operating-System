# Reader-Writer Synchronization in C

## Introduction
In concurrent programming, the **reader-writer problem** is a classic synchronization issue where multiple threads (readers and writers) access a shared resource. The challenge is to ensure data consistency while maximizing efficiency. This report presents two implementations of the **reader-writer synchronization** using:
1. **Semaphores**
2. **Monitors (Mutex & Condition Variables)**

## Implementation 1: Semaphore-Based Synchronization

### Code
```c
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
```

### Explanation
- **Semaphores** `mutex` is using to allow only one process to run at a time and `exclusive` is for allow only writer or reader to run at a sigle given moment
- **Readers** increment `nbReader`, ensuring only one writer at a time.
- **Writers** block all readers and other writers using `exclusive`.

### Execution Output (Example)
```
reader 1 is reading...
reader 2 is reading...
reader 2 finished reading.
reader 1 finished reading.
writer is writing...
writer finished writing.
```

## Implementation 2: Monitor-Based Synchronization

### Code
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t canRead = PTHREAD_COND_INITIALIZER;
pthread_cond_t canWrite = PTHREAD_COND_INITIALIZER;

int nbReaders = 0;
int nbWriters = 0;

void *writer(void *arg);
void *reader(void *arg);

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

void *reader(void *arg) {
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

void *writer(void *arg) {
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
```

### Explanation
- Uses **mutex** to protect shared variables.
- Uses **condition variables** `canRead` and `canWrite` to signal when readers or writers can proceed.
- **Readers** check if any writer is active before proceeding.
- **Writers** wait until no readers or writers are active.

### Execution Output (Example)
```
reader 1 is reading...
reader 2 is reading...
reader 2 finished reading.
reader 1 finished reading.
writer is writing...
writer finished writing.
```

## Conclusion
Both implementations ensure proper synchronization but differ in:
- **Semaphores:** Simpler, but harder to extend for complex conditions.
- **Monitors:** More flexible using `mutex` and `condition variables`.


