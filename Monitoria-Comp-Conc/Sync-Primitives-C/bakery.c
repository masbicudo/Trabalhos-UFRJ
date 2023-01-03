// Code implemented following Wikipedia pseudocode
// https://en.wikipedia.org/wiki/Lamport%27s_bakery_algorithm

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>

#define NUM_THREADS 5

#define AMOUNT 5000000

#define LOCK_ENABLED

bool Entering[NUM_THREADS];
int Number[NUM_THREADS];

int resource = 0;

void *thread(void *num);
void lock(int i);
void unlock(int i);
int max(int* items, int count);
int cmp(int a0, int a1, int b0, int b1);
void wait();

int main()
{
    printf("Testing Lamport's bakery mutual exclusion algorithm\n");

    pthread_t thread_id[NUM_THREADS];

    int num[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
    {
        num[i] = i;
        pthread_create(&thread_id[i], NULL, thread, &num[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(thread_id[i], NULL);

    printf("Contagem final: %d\n", resource);
}

void *thread(void *num)
{
    int i = *((int *)num);
    for (int it = 0; it < AMOUNT; it++)
    {
        #ifdef LOCK_ENABLED
        lock(i);
        #endif

        // critical section
        resource++;
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
        while (Entering[j]) ;
        // Wait until all threads with smaller numbers or with the same
        // number, but with higher priority, finish their work:
        while ((Number[j] != 0) && (cmp(Number[j], j, Number[i], i) < 0)) ;
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
