/*
 * Author: Jaleel Rogers
 * Title: CretaceousWorkloadBalancer
 * Date: 12/06/24
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_THREADS 5
#define NUM_SEMAPHORES 3

typedef struct {
    int work;
    char ID;
} ThreadData;

sem_t semaphores[NUM_SEMAPHORES]; // Binary semaphores

// Function to sleep for a random amount of time (0-10ms)
void random_sleep() {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (rand() % 11) * 1000000; // 0-10ms in nanoseconds
    nanosleep(&ts, NULL); //Comment out to fullfil objective for Phase 3
}

void* Thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Seed random number generator for each thread
    unsigned int seed = time(NULL) ^ (pthread_self() & 0xFFFFFF);

    while (data->work > 0) {
        // Randomly decide the number of semaphores needed (1 to 3)
        int sem_needed = (rand_r(&seed) % NUM_SEMAPHORES) + 1;

        // Randomly select `sem_needed` semaphores
        int selected[NUM_SEMAPHORES] = {0};
        for (int i = 0; i < sem_needed; i++) {
            int sem;
            do {
                sem = rand_r(&seed) % NUM_SEMAPHORES;
            } while (selected[sem]); // Ensure uniqueness
            selected[sem] = 1;
        }

        // Try to lock the selected semaphores
        int acquired_all = 1;
        for (int i = 0; i < NUM_SEMAPHORES; i++) {
            if (selected[i] && sem_trywait(&semaphores[i]) != 0) {
                acquired_all = 0; // Could not acquire one of the semaphores
                break;
            }
        }

        if (acquired_all) {
            // Successfully acquired all required semaphores
            for (int i = 0; i < NUM_SEMAPHORES; i++) {
                if (selected[i]) {
                    printf("%c>%d\n", data->ID, i); // Print acquisition
                }
            }
            printf("%c has %d work left\n", data->ID, data->work - 1);
            data->work--; // Reduce work

            // Release semaphores
            for (int i = 0; i < NUM_SEMAPHORES; i++) {
                if (selected[i]) {
                    printf("%c<%d\n", data->ID, i); // Print release
                    sem_post(&semaphores[i]);
                }
            }

            // Sleep for a random amount of time
            random_sleep();
        } else {
            // Could not acquire all semaphores, release any that were held
            for (int i = 0; i < NUM_SEMAPHORES; i++) {
                if (selected[i]) {
                    printf("%c<%d\n", data->ID, i); // Print release
                    sem_post(&semaphores[i]);
                }
            }

            // Sleep for a random amount of time
            random_sleep();
        }
    }

    printf("Thread %c: Finished all work, exiting.\n", data->ID);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int result;

    // Initialize semaphores
    for (int i = 0; i < NUM_SEMAPHORES; i++) {
        sem_init(&semaphores[i], 0, 1); // Binary semaphore
    }

    // Seed random number generator
    srand(time(NULL));

    // Create threads with unique IDs and initial work values
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].work = 10;
        threadData[i].ID = 'A' + i;

        result = pthread_create(&threads[i], NULL, Thread, &threadData[i]);
        if (result) {
            printf("ERROR: pthread_create failed for thread %c\n", threadData[i].ID);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy semaphores
    for (int i = 0; i < NUM_SEMAPHORES; i++) {
        sem_destroy(&semaphores[i]);
    }

    printf("Main thread: All threads have finished.\n");
    return 0;
}
