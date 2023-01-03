// Code implemented following Wikipedia pseudocode
// https://en.wikipedia.org/wiki/Peterson%27s_algorithm

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define LOCK_ENABLED
//#define ENABLE_THREAD_0
#define ENABLE_THREAD_1

bool flag[2] = {false, false};
int turn;

int resource = 0;

void *thread0(void *num);
void *thread1(void *num);

int main()
{
    printf("Testing Peterson mutual exclusion algorithm\n");

    pthread_t thread_id[2]; // each thread will simulate the behavior of one philosopher

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
}

void *thread0(void *num)
{
    int i = *((int *)num);
    while (1)
    {
        #ifdef LOCK_ENABLED
        flag[0] = true;
        while (flag[1] == true)
            sleep(0);
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
        flag[0] = false;
        #endif
    }
}

void *thread1(void *num)
{
    int i = *((int *)num);
    while (1)
    {
        #ifdef LOCK_ENABLED
        flag[1] = true;
        while (flag[0] == true)
            sleep(0);
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
        flag[1] = false;
        #endif
    }
}
