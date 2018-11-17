/***********************************************************
 * == DERIVATIVE WORK LICENSE ==
 * This code is a modified version of the one found at
 * www.c-program-example.com. This code is subject to the
 * original license as stated below. As for the changes that
 * I did, you can use it however you wish, as long as you
 * don't hold me liable. This means that if
 * the original author gives permission for you to use his
 * code in some way, then you can also use this derivative
 * work the same way.
 *
 * == UNDERLYING WORK LICENSE ==
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
// This code works when philosophers have out-of-synchrony timings,
// but there is a problem when timings align.
// If the philosophers somehow sincronize the access to
// the chopsticks, so that one gets them and uses them
// for 6 seconds, and the other one tries to get the
// chopsticks in the middle of this 6 seconds, every 6
// seconds after that, then the first will always be able to feast,
// and the other one will always try to feast unsuccessfully
// while the first one is feasting.
// 
// Hint: To see why this happens in this code,
// count the number of seconds that a successful
// attempt to feast takes, and then count the
// number of seconds that an unsuccessful attempt takes.

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "ansicolors.h"

#define N 5 // number of philosophers

#define MODULO(n) ((((n) % N) + N) % N)
#define LEFT  (MODULO(ph_num + 0))
#define RIGHT (MODULO(ph_num - 1))

sem_t chopsticks[N]; // binary semaphores controling the access to each chopstick

void *philosopher(void *num);
void take_chopstick(int);
void put_chopstick(int);

int main()
{
    printf("Dining Philosophers: starving example 1\n");

    int ph_num;
    for (ph_num = 0; ph_num < N; ph_num++) {
        printf(ANSI_TEXT_COLOR_BLUE "Philosopher %d needs left chopstick %d and then right chopstick %d\n" ANSI_RESET, ph_num + 1, LEFT + 1, RIGHT + 1);
    }

    int phil_num[N];
    int i;
    pthread_t thread_id[N]; // each thread will simulate the behavior of one philosopher

    for (i = 0; i < N; i++)
        sem_init(&chopsticks[i], 0, 1);

    for (i = 0; i < N; i++)
    {
        phil_num[i] = i;
        pthread_create(&thread_id[i], NULL, philosopher, &phil_num[i]);
    }

    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);
}

void *philosopher(void *num)
{
    int i = *((int *)num);
    while (1)
    {
        printf("Philosopher %d is thinking\n", i + 1);
        sleep(1);

        printf("Philosopher %d is hungry\n", i + 1);
        take_chopstick(i);

        printf(ANSI_TEXT_COLOR_GREEN "Philosopher %d is eating\n" ANSI_RESET, i + 1);
        sleep(1);

        put_chopstick(i);
    }
}

void take_chopstick(int ph_num)
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
            printf(ANSI_TEXT_COLOR_RED "Philosopher %d unable to get both chopsticks %d and %d, releasing left one\n" ANSI_RESET, ph_num + 1, LEFT + 1, RIGHT + 1);
            sem_post(&chopsticks[LEFT]);
            sleep(5);
        }
    }
}

void put_chopstick(int ph_num)
{
    printf("Philosopher %d putting chopstick %d and %d down\n", ph_num + 1, LEFT + 1, RIGHT + 1);
    sem_post(&chopsticks[LEFT]);
    sleep(1);
    sem_post(&chopsticks[RIGHT]);
    sleep(1);
}
