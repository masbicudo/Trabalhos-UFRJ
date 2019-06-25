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

// SOLUTION:
// The philosophers will always get the even chopstick first
// and then get the odd one. This eliminates deadlocks
// because no philosopher will ever wait for another
// philosopher that is already waiting for something.
// When adjacent philosophers want a chopstick, they will
// either be running to get their first chopsticks, or for
// their second chopsticks.
//
// Exercise:
//  a) Explain why this solution is a derivative of the "Resource hierarchy solution"
//      https://en.wikipedia.org/wiki/Dining_philosophers_problem#Resource_hierarchy_solution
//  b) Implement the "Arbitrator solution" and explain your code. Optionally implement the modifications.
//      https://en.wikipedia.org/wiki/Dining_philosophers_problem#Arbitrator_solution
//  c) Is it easier to solve the problem if philosophers can choose where to sit? Explain! Optionally implement.
//  d) Is it possible to solve the problem with a queue? Explain! Optionally implement one or both.
//      - if philosopher can choose where to sit
//      - if philosopher place is predetermined
//  e) Is it possible to solve the problem by forcing an interval of quiescence?
//      For example: if one cannot eat for any reason, then anyone after that must wait until nobody is eating
//      Explain how and what are the problems.
//      Optionally implement and explain the code.

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "ansicolors.h"

#define N 5 // number of philosophers

#define EVEN (((ph_num & ~1) + 1) % N)
#define ODD  (((ph_num + 1) & ~1) % N)

sem_t chopsticks[N]; // binary semaphores controling the access to each chopstick

extern int errno;

void *philosopher(void *num);
void take_chopstick(int);
void put_chopstick(int);
void test(int);

int phil_num[N];

int main()
{
    int ph_num;
    for (ph_num = 0; ph_num < N; ph_num++) {
        printf(ANSI_TEXT_COLOR_BLUE "Philosopher %d needs even chopstick %d and then odd %d\n" ANSI_RESET, ph_num + 1, EVEN + 1, ODD + 1);
    }

    printf("Dining Philosophers: working example 1\n");

    int i, retval;
    pthread_t thread_id[N]; // each thread will simulate the behavior of one philosopher

    for (i = 0; i < N; i++)
    {
        retval = sem_init(&chopsticks[i], 0, 1);
        printf("(%d %d)\n", retval, errno);
    }

    for (i = 0; i < N; i++)
    {
        phil_num[i] = (N - i + 1) % N; // shuffling philosophers creation order
        printf("Created philosopher %d\n", phil_num[i] + 1);
        pthread_create(&thread_id[i], NULL, philosopher, &phil_num[i]);
    }

    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);
}

void *philosopher(void *num)
{
    int i = *((int*)num);
    printf("Philosopher %d is thinking\n", i + 1);
    while (1)
    {
        take_chopstick(i);
        printf(ANSI_TEXT_COLOR_GREEN "Philosopher %d is eating\n" ANSI_RESET, i + 1);
        sleep(0);
        put_chopstick(i);
    }
}

void take_chopstick(int ph_num)
{
    int retval1;

    // waiting for odd chopstick
    printf("Philosopher %d is Hungry and waits for chopstick %d\n", ph_num + 1, EVEN + 1);
    retval1 = sem_wait(&chopsticks[EVEN]);
    // got odd chopstick
    printf("Philosopher %d got chopstick %d (%d %d)\n", ph_num + 1, EVEN + 1, retval1, errno);

    sleep(1);

    // waiting for even chopstick
    printf("Philosopher %d now waits for chopstick %d\n", ph_num + 1, ODD + 1);
    retval1 = sem_wait(&chopsticks[ODD]);
    // got even chopstick
    printf("Philosopher %d took chopstick %d and %d (%d %d)\n", ph_num + 1, EVEN + 1, ODD + 1, retval1, errno);

}

void put_chopstick(int ph_num)
{
    printf("Philosopher %d putting chopstick %d and %d down\n", ph_num + 1, EVEN + 1, ODD + 1);
    printf("Philosopher %d is thinking\n", ph_num + 1);
    sem_post(&chopsticks[EVEN]);
    sleep(1);
    sem_post(&chopsticks[ODD]);
    sleep(1);
}
