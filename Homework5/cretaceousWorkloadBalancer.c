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

sem_t semaphores[NUM_SEMAPHORES];

void random_sleep() {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (rand() % 11) * 1000000;
    nanosleep(&ts, NULL);
}
void* Thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Seeding the random number generator
    unsigned int seed = time(NULL) ^ (pthread_self() & 0xFFFFFF);

    while (data -> work > 0) {
        // Randomly decide the number of semaphores needed (1 to 3)
        int pick_sem = (rand_r(&seed) % NUM_SEMAPHORES) + 1;

        int selected[NUM_SEMAPHORES] = {0};
        for (int i = 0; i < pick_sem; i++) {
            int sem;
            do {
                sem = rand_r(&seed) % NUM_SEMAPHORES;
            } while (selected[sem]); // Helps with randomness
            selected[sem] = 1;
        }
        int acquired_flag = 1;
        for (int i = 0; i < NUM_SEMAPHORES; i++) {
            if (selected[i] && sem_trywait(&semaphores[i]) != 0) {
                acquired_all = 0;
                break;
            }
        }
        if (acquired_flag) {
            for (int i = 0; i < NUM_SEMAPHORES; i++) {
                if (selected[i]) {
                    printf("%c>%d\n,", data->ID, i);
                }
            }
            printf(" has %d work left\n")
        }
       // printf("Thread %c: Work remaining = %d\n", data -> ID, data-> work);
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
    sem_t sem1, sem2, sem3;
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
