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
// and then get the right one. A deadlock will eventually happen
// in this code.
//
// Devise a way to change this code so that the deadlock
// problem shows up immediatelly, without changing the overall
// behaviour.
//
// Hint: look at the take_chopstick function? Is this function atomic?

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 // number of philosophers

#define LEFT  ((ph_num + 0) % N)
#define RIGHT ((ph_num + 1) % N)

sem_t chopsticks[N]; // binary semaphores controling the access to each chopstick

void *philospher(void *num);
void take_chopstick(int);
void put_chopstick(int);

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main()
{
    printf("Dining Philosophers: deadlock example 1\n");

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
    int i = *((int*)num);
    printf("Philosopher %d is thinking\n", i + 1);
    while (1)
    {
        take_chopstick(i);
        sleep(0);
        put_chopstick(i);
    }
}

void take_chopstick(int ph_num)
{
    // waiting for left chopstick
    printf("Philosopher %d is Hungry and waits for chopstick %d\n", ph_num + 1, LEFT + 1);
    sem_wait(&chopsticks[LEFT]);
    // got left chopstick
    printf("Philosopher %d got chopstick %d\n", ph_num + 1, LEFT + 1);

    // waiting for right chopstick
    printf("Philosopher %d now waits for chopstick %d\n", ph_num + 1, RIGHT + 1);
    sem_wait(&chopsticks[RIGHT]);
    // got right chopstick
    printf("Philosopher %d took chopstick %d and %d, and now is Eating\n", ph_num + 1, LEFT + 1, RIGHT + 1);

}

void put_chopstick(int ph_num)
{
    printf("Philosopher %d putting chopstick %d and %d down\n", ph_num + 1, LEFT + 1, ph_num + 1);
    printf("Philosopher %d is thinking\n", ph_num + 1);
    sem_post(&chopsticks[LEFT]);
    sleep(1);
    sem_post(&chopsticks[RIGHT]);
    sleep(1);
}
