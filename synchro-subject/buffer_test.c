#define BUFFER_SIZE 10

typedef struct {
    char product;
    int amount;
} item;

item buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int nb = 0; // Number of items

void produce(item *i) {    
    while (nb == BUFFER_SIZE) {
        // DO NOTHING
    }
    memcpy(&buffer[in], i, sizeof(item));
    in = (i + 1) % BUFFER_SIZE;
    nb++;

}

void consume() {
    item *i = malloc(sizeof(item));
    while (nb == 0) {
        // Doing nothing
    }
    memcpy(i, &buffer[out], sizeof(item));
    out = (out+1) % BUFFER_SIZE;
    nb--;
    return i;
}