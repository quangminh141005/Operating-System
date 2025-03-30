#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

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

int nbdisplay = 0;

void init() {
	int i;
	count = in = out = 0;
	for (i = 0; i < BUFFER_SIZE; i++) buffer[i] = false;
	srand(time(NULL));
}

void display() {
	int i;
	printf("-");
	for (i = 0; i < BUFFER_SIZE; i++) printf("--");
	printf("\n|");
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (buffer[i]) printf("x");
		else printf(" ");
		printf("|");
	}
	printf("\nDisplay count: %d\n", nbdisplay++);
	printf("-");
	for (i = 0; i < BUFFER_SIZE; i++) printf("--");
	printf("\n\n");
}

void sleep_random() {
	sleep(rand() % 2 + 1);
}

void produce() {
	pthread_mutex_lock(&mutex);

	while (count == BUFFER_SIZE) {
		pthread_cond_wait(&freeslots, &mutex);
	}

	printf("I am producing ...\n");
	buffer[in] = true;
	in = (in + 1) % BUFFER_SIZE;
	count++;

	display();

	pthread_cond_signal(&products);
	pthread_mutex_unlock(&mutex);
}

void consume() {
	pthread_mutex_lock(&mutex);

	while (count == 0) {
		pthread_cond_wait(&products, &mutex);
	}

	printf("I am consuming ...\n");
	buffer[out] = false;
	out = (out + 1) % BUFFER_SIZE;
	count--;

	display();

	pthread_cond_signal(&freeslots);
	pthread_mutex_unlock(&mutex);
}

void *consumer(void *threadid) {
	while (1) {
		consume();
		sleep_random();
	}
}

void *producer(void *threadid) {
	while (1) {
		produce();
		sleep_random();
	}
}

int main() {
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
