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

#define ENABLE_THREAD_0
#define ENABLE_THREAD_1

#define ENABLE_MEM_FENCE // On modern CPUs and compilers, optimizations may lead to incorrect results.
                         // The compiler may reorder instructions, remove busy waits, and do other optimizations.
                         // In the case of the Lamport's Bakery, the compiler can optimize consecutive
                         // sets to the same variable with the last value.
                         // Also, the CPU memory model, also allows for instruction and read/writes reordering,
                         // and also have cache coherence problems.
                         // Using memory fences will tell the CPU not to reorder anything over the fence.
                         // Another way of guaranteing the order is using the -O3 option with gcc.
                         // When using -O3 optimization option, memory fences are not needed.
//#define YIELD_WAIT

volatile bool Entering[NUM_THREADS];
volatile int Number[NUM_THREADS];

int resource = 0;

void *thread(void *num);
void lock(int i);
void unlock(int i);
int max(volatile int* items, int count);
int cmp(int a0, int a1, int b0, int b1);
void mem_fence();
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
    // Problems that can happen here, without the memory fence:
    // - compiler removes the line that sets Entering to true, since it will be set to false anyway
    // - reordering, by compiler or CPU of the set instructions
    // - reordering of the memory reads/writes from/to the memory
    // - cache coherency problem between different processors
    // Another way of guaranteing the order is using the -O3 option with gcc
    Entering[i] = true;
    mem_fence();
    Number[i] = 1 + max(Number, NUM_THREADS);
    mem_fence();
    Entering[i] = false;
    for (int j = 0; j < NUM_THREADS; j++) {
        // Wait until thread j receives its number:
        while (Entering[j])
            wait();
        // Wait until all threads with smaller numbers or with the same
        // number, but with higher priority, finish their work:
        while ((Number[j] != 0) && (cmp(Number[j], j, Number[i], i) < 0))
            wait();
    }
}

void unlock(int i) {
    Number[i] = 0;
}

int max(volatile int* items, int count) {
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

void mem_fence() {
    #ifdef ENABLE_MEM_FENCE
    __sync_synchronize();
    #endif
}

void wait() {
    #ifdef YIELD_WAIT
    sched_yield();
    #endif
}
