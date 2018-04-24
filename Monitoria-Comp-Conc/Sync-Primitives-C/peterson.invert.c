#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define LOCK_ENABLED

bool flag[2] = {true, false};
int turn;

int resource = 0;

void *thread0(void *num);
void *thread1(void *num);

int main()
{
    printf("Testing Peterson mutual exclusion algorithm\n");

    pthread_t thread_id[2]; // each thread will simulate the behavior of one philosopher

    int num0 = 0;
    pthread_create(&thread_id[0], NULL, thread0, &num0);

    int num1 = 1;
    pthread_create(&thread_id[1], NULL, thread1, &num1);

    for (int i = 0; i < 2; i++)
        pthread_join(thread_id[i], NULL);
}

void *thread0(void *num)
{
    int i = *((int *)num);
    while (1)
    {
        #ifdef LOCK_ENABLED
        sleep(0);
        turn = 1;
        sleep(0);
        flag[0] = true;
        sleep(0);
        while (flag[1] == true && turn == 1)
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
        sleep(0);
        turn = 0;
        sleep(0);
        flag[1] = true;
        sleep(0);
        while (flag[0] == true && turn == 0)
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
