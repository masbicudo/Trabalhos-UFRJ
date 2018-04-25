// Code implemented following Wikipedia pseudocode
// https://en.wikipedia.org/wiki/Peterson%27s_algorithm
// With hints about memory fences from:
// https://www.geeksforgeeks.org/petersons-algorithm-for-mutual-exclusion-set-2-cpu-cycles-and-memory-fence/

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define AMOUNT 5000000

#define LOCK_ENABLED

#define ENABLE_THREAD_0
#define ENABLE_THREAD_1

#define ENABLE_MEM_FENCE // On modern CPUs and compilers, optimizations may lead to incorrect results.
                         // The compiler may reorder instructions, remove busy waits, and do other optimizations.
                         // Also, the CPU memory model, also allows for instruction and read/writes reordering,
                         // and also have cache coherence problems.
                         // Using memory fences will tell the CPU not to reorder anything over the fence.
//#define YIELD_WAIT

// In my tests, the fastest setup was using busy wait (by undefining YIELD_WAIT)
// and by calling __sync_synchronize() inside another method instead of directly.

int flag0 = 0;
int flag1 = 0;
int turn;

int resource = 0;

void *thread0(void *num);
void *thread1(void *num);

void mem_fence();
void wait();

int main()
{
    printf("Testing Peterson mutual exclusion algorithm\n");

    pthread_t thread_id[2];

    #ifdef ENABLE_THREAD_0
    int num0 = 0;
    pthread_create(&thread_id[0], NULL, thread0, &num0);
    #endif

    #ifdef ENABLE_THREAD_1
    int num1 = 1;
    pthread_create(&thread_id[1], NULL, thread1, &num1);
    #endif

    #ifdef ENABLE_THREAD_0
    pthread_join(thread_id[0], NULL);
    #endif
    #ifdef ENABLE_THREAD_1
    pthread_join(thread_id[1], NULL);
    #endif

    printf("Contagem final: %d\n", resource);
}

void *thread0(void *num)
{
    int i = *((int *)num);
    for (int it = 0; it < AMOUNT; it++)
    {
        #ifdef LOCK_ENABLED
        flag0 = 1;
        turn = 1;
        mem_fence();
        while (flag1 == 1 && turn == 1)
            wait();
        #endif

        // critical section
        resource++;
        // end of critical section

        #ifdef LOCK_ENABLED
        flag0 = 0;
        #endif
    }
    printf("Contagem thread0: %d\n", resource);
}

void *thread1(void *num)
{
    int i = *((int *)num);
    for (int it = 0; it < AMOUNT; it++)
    {
        #ifdef LOCK_ENABLED
        flag1 = 1;
        turn = 0;
        mem_fence();
        while (flag0 == 1 && turn == 0)
            wait();
        #endif

        // critical section
        resource++;
        // end of critical section

        #ifdef LOCK_ENABLED
        flag1 = 0;
        #endif
    }
    printf("Contagem thread1: %d\n", resource);
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
