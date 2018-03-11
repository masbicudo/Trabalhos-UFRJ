/***********************************************************
* You can use all the programs on  www.c-program-example.com
* for personal and learning purposes. For permissions to use the
* programs for commercial purposes,
* contact info@c-program-example.com
* To find more C programs, do visit www.c-program-example.com
* and browse!
* 
*                      Happy Coding
***********************************************************/

// PROBLEM:
// The philosophers will always get the left chopstick first
// and then get the right one. To avoid the deadlock problem,
// if the right chopstick is not available, the philosopher
// drops the one that he got already.
//
// This works most of the time, but there is a problem.
// If the philosophers somehow sincronize the access to
// the chopsticks, so that one gets them and uses them
// for 6 seconds, and the other one tries to get the
// chopsticks in the middle of this 6 seconds, every 6
// seconds, then the first will always be able to feast,
// and the other one will always try to feast unsuccessfully
// while the first one is feasting.

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 // number of philosophers

#define LEFT ((ph_num + 0) % N)
#define RIGHT ((ph_num + 1) % N)

sem_t chopsticks[N]; // binary semaphores controling the access to each chopstick

void *philospher(void *num);
void take_fork(int);
void put_fork(int);

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main()
{
    int phil_num[N];
    int i;
    pthread_t thread_id[N]; // each thread will simulate the behavior of one philosopher

    for (i = 0; i < N; i++)
        sem_init(&chopsticks[i], 0, 1);

    for (i = 0; i < N; i++)
    {
        phil_num[i] = i;
        pthread_create(&thread_id[i], NULL, philospher, &phil_num[i]);
    }

    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);
}

void *philospher(void *num)
{
    int i = *((int *)num);
    while (1)
    {
        printf("Philosopher %d is thinking\n", i + 1);
        sleep(1);

        printf("Philosopher %d is hungry\n", i + 1);
        take_fork(i);

        printf(ANSI_COLOR_GREEN "Philosopher %d is eating\n" ANSI_COLOR_RESET, i + 1);
        sleep(1);

        put_fork(i);
    }
}

void take_fork(int ph_num)
{
    while (1)
    {
        // waiting for left chopstick
        printf("Philosopher %d is Hungry and waits for chopstick %d\n", ph_num + 1, LEFT + 1);
        sem_wait(&chopsticks[LEFT]);

        // got left chopstick
        printf("Philosopher %d got chopstick %d\n", ph_num + 1, LEFT + 1);
        sleep(1);

        // waiting for right chopstick
        printf("Philosopher %d now try taking chopstick %d\n", ph_num + 1, RIGHT + 1);
        if (sem_trywait(&chopsticks[RIGHT]) == 0)
        {
            // got right chopstick
            printf("Philosopher %d took chopstick %d and %d, and now is Eating\n", ph_num + 1, LEFT + 1, RIGHT + 1);
            sleep(1);

            break;
        }
        else
        {
            // could not get right chopstick, releasing the left chopstick
            printf(ANSI_COLOR_RED "Philosopher %d unable to get both chopsticks %d and %d, releasing left one\n" ANSI_COLOR_RESET, ph_num + 1, LEFT + 1, RIGHT + 1);
            sem_post(&chopsticks[LEFT]);
            sleep(5);
        }
    }
}

void put_fork(int ph_num)
{
    printf("Philosopher %d putting fork %d and %d down\n", ph_num + 1, LEFT + 1, ph_num + 1);
    sem_post(&chopsticks[LEFT]);
    sleep(1);
    sem_post(&chopsticks[RIGHT]);
    sleep(1);
}
