#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>

int sem_value;
pthread_mutex_t mutex; // mutexes controling the access to each chopstick
pthread_cond_t cond_pode_continuar;
#define SEM_ENABLED
#define ITEMS 100

void wait();
void signal();

void *threadProd(void *num);
void *threadCons(void *num);

int main()
{
    printf("Custom semaphore implementation\n");

    pthread_t thread_id[2]; // each thread will simulate the behavior of one philosopher

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_pode_continuar, NULL);
    
    int num1 = 1;
    pthread_create(&thread_id[1], NULL, threadCons, &num1);

    int num0 = 0;
    pthread_create(&thread_id[0], NULL, threadProd, &num0);

    pthread_join(thread_id[0], NULL);
    pthread_join(thread_id[1], NULL);

    pthread_cond_destroy(&cond_pode_continuar);
    pthread_mutex_destroy(&mutex);
}

void wait() {
    pthread_mutex_lock(&mutex);

    // enquanto o valor for menor que 1
    // vai ficar esperando...
    // quando for maior ou igual a 1, quer dizer que
    // temos sinal verde para continuar
    while (sem_value < 1)
        pthread_cond_wait(&cond_pode_continuar, &mutex);

    // consumindo uma unidade do semáforo, que pode ter
    // vários sinais verdes
    sem_value--;

    pthread_mutex_unlock(&mutex);
}

void signal() {
    pthread_mutex_lock(&mutex);

    sem_value++;
    // escolhe uma thread qualquer que esteja esperando
    // e manda ela continuar
    pthread_cond_signal(&cond_pode_continuar);

    pthread_mutex_unlock(&mutex);
}


void *threadCons(void *num)
{
    int i = *((int *)num);
    for (int it = 0; it < ITEMS; it++) {
        #ifdef SEM_ENABLED
        wait();
        #endif
        sleep(0);
        printf("Consumindo %i\n", it);
        sleep(0);
    }
}

void *threadProd(void *num)
{
    int i = *((int *)num);
    for (int it = 0; it < ITEMS; it++) {
        sleep(0);
        printf("Produzindo %i\n", it);
        sleep(0);
        #ifdef SEM_ENABLED
        signal();
        #endif
    }
}
