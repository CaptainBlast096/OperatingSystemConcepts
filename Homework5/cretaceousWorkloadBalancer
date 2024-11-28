#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 5
typedef struct {
    int work;
    char ID;
} ThreadData;

void* Thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    unsigned int seed = time(NULL) ^ (pthread_self() & 0xFFFFFF);

    while (data -> work > 0) {
        printf("Thread %c: Work remaining = %d\n", data -> ID, data-> work);
        sleep(1);
        data -> work--;
    }
    printf("Thread %c: Finished all work, exiting.\n", data -> ID);
    pthread_exit(NULL);
}
/*
 * Need to find a way to create four version of the thread
 * Assign a work value to each integer
 * Create an exit once work is 0
 */
int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int result;

    srand(time(NULL));

    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].work = 10;
        threadData[i].ID = 'A' + i;

        result = pthread_create(&threads[i], NULL, Thread, &threadData[i]);
        if (result) {
            printf("ERROR: pthread_create failed for thread %c\n", threadData[i].ID);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i],NULL);
    }

    printf("Main thread: All threads have finished.\n");


    return 0;
}
