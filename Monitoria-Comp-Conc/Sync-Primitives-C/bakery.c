// Code implemented following Wikipedia pseudocode
// https://en.wikipedia.org/wiki/Lamport%27s_bakery_algorithm

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>

#define LOCK_ENABLED

#define NUM_THREADS 5

bool Entering[NUM_THREADS];
int Number[NUM_THREADS];

int resource = 0;

void *thread(void *num);
void lock(int i);
void unlock(int i);
int max(int* items, int count);
int cmp(int a0, int a1, int b0, int b1);

int main()
{
    printf("Testing Lamport's bakery mutual exclusion algorithm\n");

    pthread_t thread_id[NUM_THREADS]; // each thread will simulate the behavior of one philosopher

    int num[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
    {
        num[i] = i;
        pthread_create(&thread_id[i], NULL, thread, &num[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(thread_id[i], NULL);
}

void *thread(void *num)
{
    int i = *((int *)num);
    while (true) {
        #ifdef LOCK_ENABLED
        lock(i);
        #endif

        // critical section
        for (int it = 0; it < 10; it++)
        {
            printf("%i", i);
            sleep(0);
        }
        printf("\n");
        // end of critical section

        #ifdef LOCK_ENABLED
        unlock(i);
        #endif
    }
}

void lock(int i) {
    Entering[i] = true;
    Number[i] = 1 + max(Number, NUM_THREADS);
    Entering[i] = false;
    for (int j = 0; j < NUM_THREADS; j++) {
        // Wait until thread j receives its number:
        while (Entering[j]) { /* nothing */ }
        // Wait until all threads with smaller numbers or with the same
        // number, but with higher priority, finish their work:
        while ((Number[j] != 0) && (cmp(Number[j], j, Number[i], i) < 0)) { /* nothing */ }
    }
}
  
void unlock(int i) {
    Number[i] = 0;
}

int max(int* items, int count) {
    int max_value = items[0];
    for (int it = 1; it < count; it++) {
        if (items[it] > max_value)
            max_value = items[it];
    }
    return max_value;
}

int cmp(int a0, int a1, int b0, int b1) {
    if (a0 < b0) return -1;
    if (a0 > b0) return +1;
    if (a1 < b1) return -1;
    if (a1 > b1) return +1;
    return 0;
}