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

// SOLUTION:
// The philosophers will always get both chopsticks atomically.
// In this processo the philosopher check whether adjacent ones
// are feasting. If they are, he waits until he can feast.
// A philosopher always asks to adjacent ones whether they
// want to feast as soon as he finishes himself. If adjacent
// philosopher is waiting, then he can feast finally.

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define N 5

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define LEFT ((ph_num + N - 1) % N)
#define RIGHT ((ph_num + 1) % N)

sem_t mutex;
sem_t can_eat[N];

extern int errno;

void *philospher(void *num);
void take_chopstick(int);
void put_chopstick(int);
void set_permition_to_eat_if_needed(int);

int state[N];
int phil_num[N];

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main()
{
    printf("Dining Philosophers: working example 2\n");

    int i, retval;
    pthread_t thread_id[N];

    sem_init(&mutex, 0, 1); // initialize locking object

    // initialize "can eat" signaling objects
    for (i = 0; i < N; i++)
        sem_init(&can_eat[i], 0, 0);

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
    int i = *((int*)num);
    printf("Philosopher %d is thinking\n", i + 1);
    while (1)
    {
        sleep(1);
        take_chopstick(i);
        printf(ANSI_COLOR_GREEN "Philosopher %d is eating\n" ANSI_COLOR_RESET, i + 1);
        sleep(0);
        put_chopstick(i);
    }
}

void take_chopstick(int ph_num)
{
    sem_wait(&mutex);
    {
        state[ph_num] = HUNGRY;
        printf("Philosopher %d is Hungry\n", ph_num + 1);
        set_permition_to_eat_if_needed(ph_num);
        // philosopher may or may not be allowed to eat
    }
    sem_post(&mutex);

    sem_wait(&can_eat[ph_num]); // wait until philosopher can eat

    sleep(1);
}

void set_permition_to_eat_if_needed(int ph_num)
{
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[ph_num] = EATING;
        printf("Philosopher %d takes chopstick %d and %d\n", ph_num + 1, LEFT + 1, ph_num + 1);
        sem_post(&can_eat[ph_num]); // philosopher is allowed to eat
    }
}

void put_chopstick(int ph_num)
{
    sem_wait(&mutex);
    {
        state[ph_num] = THINKING;
        printf("Philosopher %d putting chopstick %d and %d down\n", ph_num + 1, LEFT + 1, ph_num + 1);
        printf("Philosopher %d is thinking\n", ph_num + 1);

        // asking whether left philosopher wishes to eat
        set_permition_to_eat_if_needed(LEFT);

        // asking whether right philosopher wishes to eat
        set_permition_to_eat_if_needed(RIGHT);
    }
    sem_post(&mutex);
}