#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#define NBC 2
#define NBP 2

#define BUFFER_SIZE 20
#define boolean int
#define true 1
#define false 0

int count, in, out;
boolean buffer[BUFFER_SIZE];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t products = PTHREAD_COND_INITIALIZER;
pthread_cond_t freeslots = PTHREAD_COND_INITIALIZER;

pthread_cond_t sleepcond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sleepmutex = PTHREAD_MUTEX_INITIALIZER;

int nbdisplay = 0;
void init() {
	int i;
	count = in = out = 0;
	for (i=0;i<BUFFER_SIZE;i++) buffer[i] = false;
	srand(time(NULL));
}

void display() {

	int i;
	printf("-");
	for (i=0;i<BUFFER_SIZE;i++) printf("--");
	printf("\n");
	printf("|");
	for (i=0;i<BUFFER_SIZE;i++) {
		if (buffer[i]) printf("x");
		else printf(" ");
		printf("|");
	}
	printf("%d", nbdisplay++);
	printf("\n");
	printf("-");
	for (i=0;i<BUFFER_SIZE;i++) printf("--");
	printf("\n");
	printf("\n");
}


void sleep() {
	struct timespec t;
	t.tv_sec = time(0) + (rand() % 2) + 1;
	t.tv_nsec = 0;
	pthread_mutex_lock(&sleepmutex);
	pthread_cond_timedwait(&sleepcond, &sleepmutex, &t);
	pthread_mutex_unlock(&sleepmutex);
}
         
	
void produce () {
	pthread_mutex_lock(&mutex);
	// you must re-implement this function
	// it should synchronize access to the shared buffer
	while (count == BUFFER_SIZE) {
		printf("IM FULLLLLLLLLL\n");
		pthread_cond_wait(&freeslots, &mutex);
	}

	printf("I am producing ...\n");
	buffer [in] = true;
	in = (in + 1) % BUFFER_SIZE;
	count++;
	display();
	pthread_cond_signal(&products);
	pthread_mutex_unlock(&mutex);
}

void consume () {
	pthread_mutex_lock(&mutex);
	// you must re-implement this function
	// it should synchronize access to the shared buffer
	while (count == 0) {
		pthread_cond_wait(&products, & mutex);
	}
	printf("I am consuming ...\n");
	buffer[out] = false;
	out = (out + 1) % BUFFER_SIZE;
	count--;
	display();
	pthread_cond_signal(&freeslots);
	pthread_mutex_unlock(&mutex);
}


void *consumer(void *threadid)
{
   for(;;) {
	consume();
	sleep();
   }
}

void *producer(void *threadid)
{
   for(;;) {
	produce();
	sleep();
   }
}

int main() {
	// you must re-implement this function
	// it should create a number of threads that execute the comsumer and producer routines
	pthread_t producers[NBP], consumers[NBC];

	init();

	for (int i = 0; i < NBP; i++) {
		pthread_create(&producers[i], NULL, producer, NULL);
	}

	for (int i = 0; i < NBC; i++) {
		pthread_create(&consumers[i], NULL, consumer, NULL);
	}

	for (int i = 0; i < NBP; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NBC; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}



