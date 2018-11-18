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
// 
// The philosophers will always get both chopsticks atomically.
// In this process the philosopher check whether adjacent ones
// are feasting. If they are, he waits until he can feast.
// A philosopher always asks to adjacent ones whether they
// want to feast as soon as he finishes himself. If adjacent
// philosopher is waiting, then he can feast finally.
//
//
// PROBLEM: 2nd order starvation still exists!
// 
// This solution overcomes a starvation problem by making
// philosophers hint adjacent ones when he finishes feasting.
// This allows hungry adjacent philosophers to take action
// immediately. However, this is a partial solution for the
// starvation problem.
// We will name 3 of the philosophers as A, B and C.
// If it is the case that A or C is always feasting,
// then philosophers B can never feast.
// Possible timeline:
//  A: fffffffff        fffffffffffffff        fffffffffffff...
//  B:    wwwwww!wwwwwwwwwww!wwwwwwwwww!wwwwwwwwwww!wwwwwwww...
//  C:       fffffffffffffff        fffffffffffffff        f...
//  Legend: f = feasting; w = waiting; ! = poked by adjacent

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "ansicolors.h"

#define N 5

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define MODULO(n) ((((n) % N) + N) % N)
#define LEFT  (MODULO(ph_num + 1))
#define RIGHT (MODULO(ph_num - 1))

sem_t stone_mutex;
sem_t can_eat[N];

extern int errno;

void *philosopher(void *num);
void take_chopstick(int);
void put_chopstick(int);
void set_permition_to_eat_if_needed(int);

int state[N];
int phil_num[N];

int main()
{
    printf("Dining Philosophers: working example 2\n");

    int ph_num;
    for (ph_num = 0; ph_num < N; ph_num++) {
        printf(ANSI_TEXT_COLOR_BLUE "Philosopher %d shares chopsticks with %d and %d\n" ANSI_RESET, ph_num + 1, LEFT + 1, RIGHT + 1);
    }

    int i, retval;
    pthread_t thread_id[N];

    sem_init(&stone_mutex, 0, 1); // initialize locking object

    // initialize "can eat" signaling objects
    for (i = 0; i < N; i++)
        sem_init(&can_eat[i], 0, 0);

    for (i = 0; i < N; i++)
    {
        phil_num[i] = i;
        pthread_create(&thread_id[i], NULL, philosopher, &phil_num[i]);
    }
    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL); // waits forever
}

void *philosopher(void *num)
{
    int i = *((int*)num);
    printf("Philosopher %d is thinking\n", i + 1);
    while (1)
    {
        sleep(1);
        take_chopstick(i);
        printf(ANSI_TEXT_COLOR_GREEN "Philosopher %d is eating\n" ANSI_RESET, i + 1);
        sleep(0);
        put_chopstick(i);
    }
}

void take_chopstick(int ph_num)
{
    sem_wait(&stone_mutex); //  wait to get "stone" (lock for "stone")
    {
        state[ph_num] = HUNGRY;
        printf("Philosopher %d is Hungry\n", ph_num + 1);
        set_permition_to_eat_if_needed(ph_num);  
        // philosopher may or may not be allowed to eat!
    } 
    sem_post(&stone_mutex);  // returns "stone" to the center of the table
    					// (unlock "stone")

    sem_wait(&can_eat[ph_num]); // wait until philosopher can eat
    // will wait until either 
    // 1) he himself has given him permission to eat or 
    // 2) one of his neighbors gives him that permission

    sleep(1);
}
void set_permition_to_eat_if_needed(int ph_num)
{
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && 
    	state[RIGHT] != EATING)
    {
        printf("Philosopher %d takes chopsticks shared with %d and %d\n", 
        	ph_num + 1, LEFT + 1, RIGHT + 1);
        sem_post(&can_eat[ph_num]); // philosopher is allowed to eat
        // [philosopher may be "allowing himself" to eat :)]
        // note that this semaphore has the semantics of a "message" 
        state[ph_num] = EATING;

    }
}
void put_chopstick(int ph_num)
{
    sem_wait(&stone_mutex); // get "stone"
    	// (philosophers can only change global state if they 
    	// have the stone!)
    {
        state[ph_num] = THINKING;
        printf("Philosopher %d putting chopsticks shared with %d and %d down\n", 
        	ph_num + 1, LEFT + 1, RIGHT + 1);
        printf("Philosopher %d is thinking\n", ph_num + 1);

        // asking whether left philosopher wishes to eat
        set_permition_to_eat_if_needed(LEFT);

        // asking whether right philosopher wishes to eat
        set_permition_to_eat_if_needed(RIGHT);
    }
    sem_post(&stone_mutex); // return "stone"
    // (philosophers can only change global state if they have the stone!)
}

// TO-DOs:
//
// 1)   break set_permition_to_eat_if_needed into 
//      set_permition_to_myself_to_eat and
//      set_permition_to_neighbor_to_eat_if_needed
//
// 2)   reproduce the synchronization problem which will not allow a
//      given philosopher to eat (forever!) -- this break the fairness
//      property of starvation-freedom
//      (to simplify, consider the case of 3 philosophers)
//
// 3)   challenge: fix the code to prevent the problem above
//