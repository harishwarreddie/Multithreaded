#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#define NUMFLAVORS 4
#define NUMSLOTS 2000
#define NUMCONSUMERS 50
#define NUMPRODUCERS 30
#define SELECTED_CONSUMERS 5

typedef struct {
    int flavor[NUMFLAVORS][NUMSLOTS];
    int outptr[NUMFLAVORS];
    int in_ptr[NUMFLAVORS];
    int serial[NUMFLAVORS];
    int spaces[NUMFLAVORS];
    int donuts[NUMFLAVORS];
} DONUT_SHOP;

DONUT_SHOP shared_ring;
pthread_mutex_t prod[NUMFLAVORS];
pthread_mutex_t cons[NUMFLAVORS];
pthread_cond_t prod_cond[NUMFLAVORS];
pthread_cond_t cons_cond[NUMFLAVORS];
pthread_t thread_id[NUMCONSUMERS + NUMPRODUCERS];

void *producer(void *arg) {
    unsigned short xsub[3];
    struct timeval randtime;
    gettimeofday(&randtime, NULL);
    xsub[0] = (unsigned short)randtime.tv_usec;
    xsub[1] = (unsigned short)(randtime.tv_usec >> 16);
    xsub[2] = (unsigned short)(pthread_self());

    while (1) {
        int j = nrand48(xsub) % NUMFLAVORS;
        pthread_mutex_lock(&prod[j]);
        while (shared_ring.spaces[j] == 0)
            pthread_cond_wait(&prod_cond[j], &prod[j]);
        
        shared_ring.flavor[j][shared_ring.in_ptr[j]] = shared_ring.serial[j]++;
        shared_ring.in_ptr[j] = (shared_ring.in_ptr[j] + 1) % NUMSLOTS;
        shared_ring.spaces[j]--;
        shared_ring.donuts[j]++;
        
        pthread_mutex_unlock(&prod[j]);
        pthread_cond_signal(&cons_cond[j]);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    FILE *output_file = NULL;
    char filename[20];
    if (id < SELECTED_CONSUMERS) {
        sprintf(filename, "consumer_%d.txt", id);
        output_file = fopen(filename, "w");
    }

    for (int i = 0; i < 2000; i++) {
        int dozen[NUMFLAVORS] = {0};
        for (int m = 0; m < 12; m++) {
            int j = rand() % NUMFLAVORS;
            pthread_mutex_lock(&cons[j]);
            while (shared_ring.donuts[j] == 0)
                pthread_cond_wait(&cons_cond[j], &cons[j]);
            
            shared_ring.outptr[j] = (shared_ring.outptr[j] + 1) % NUMSLOTS;
            shared_ring.donuts[j]--;
            shared_ring.spaces[j]++;
            dozen[j]++;
            
            pthread_mutex_unlock(&cons[j]);
            pthread_cond_signal(&prod_cond[j]);
        }
        if (output_file && i < 10) {
            fprintf(output_file, "Consumer thread #: %d dozen #: %d\n", id, i + 1);
            fprintf(output_file, "Plain: %d Jelly: %d Coconut: %d Honey-dip: %d\n\n",
                    dozen[0], dozen[1], dozen[2], dozen[3]);
        }
        usleep(100);
    }
    if (output_file) {
        fclose(output_file);
    }
    return NULL;
}

int main() {
    int i;
    for (i = 0; i < NUMFLAVORS; i++) {
        pthread_mutex_init(&prod[i], NULL);
        pthread_mutex_init(&cons[i], NULL);
        pthread_cond_init(&prod_cond[i], NULL);
        pthread_cond_init(&cons_cond[i], NULL);
        shared_ring.outptr[i] = shared_ring.in_ptr[i] = shared_ring.serial[i] = 0;
        shared_ring.spaces[i] = NUMSLOTS;
        shared_ring.donuts[i] = 0;
    }
    
    int arg_array[NUMCONSUMERS];
    for (i = 0; i < NUMCONSUMERS; i++) {
        arg_array[i] = i;
        pthread_create(&thread_id[i], NULL, consumer, &arg_array[i]);
    }
    
    for (; i < NUMPRODUCERS + NUMCONSUMERS; i++) {
        pthread_create(&thread_id[i], NULL, producer, NULL);
    }
    
    for (i = 0; i < NUMCONSUMERS; i++) {
        pthread_join(thread_id[i], NULL);
    }
    return 0;
}