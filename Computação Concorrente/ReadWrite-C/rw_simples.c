#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NTHREADS 10
#define NTHREADS_LEIT 7
#define PASSOS 5

int threads = 0;

pthread_mutex_t mutex;

pthread_cond_t cond_pode_ler;
pthread_cond_t cond_pode_escrever;

int lendo = 0;
int escrevendo = 0;

unsigned long mix(unsigned long a, unsigned long b, unsigned long c);

void entra_leitura()
{
    pthread_mutex_lock(&mutex);

    // esperando para poder ler
    while (escrevendo)
    {
        pthread_cond_wait(&cond_pode_ler, &mutex);
    }

    // mais um lendo
    lendo++;

    pthread_mutex_unlock(&mutex);
}

void sai_leitura()
{
    pthread_mutex_lock(&mutex);

    // menos um lendo
    lendo--;

    // liberando escrita caso não haja mais ninguém lendo
    if (lendo == 0)
    {
        pthread_cond_signal(&cond_pode_escrever);
    }

    pthread_mutex_unlock(&mutex);
}

void entra_escrita()
{
    pthread_mutex_lock(&mutex);

    // esperando para poder escrever
    while (escrevendo || lendo)
    {
        pthread_cond_wait(&cond_pode_escrever, &mutex);
    }

    // mais um escrevendo
    escrevendo++;

    pthread_mutex_unlock(&mutex);
}

void sai_escrita()
{
    pthread_mutex_lock(&mutex);

    // menos um escrevendo
    escrevendo--;

    // liberando escrita e leituras
    pthread_cond_signal(&cond_pode_escrever);
    pthread_cond_broadcast(&cond_pode_ler);

    pthread_mutex_unlock(&mutex);
}

void *escrever_varios(void *arg)
{
    int tid = *(int *)arg;
    int boba1, boba2;
    int i;
    for (i = 0; i < PASSOS; i++)
    {
        entra_escrita();
        printf("entra_escrita   tid=%d => escrevendo=%d, lendo=%d\n", tid, escrevendo, lendo);
        
        /* faz alguma coisa inutil pra gastar tempo... */
        boba1 = rand()%10000;
        boba2 = -rand()%10000;
        while (boba2 < boba1)
            boba2++;

        sai_escrita();
        printf("sai_escrita     tid=%d => escrevendo=%d, lendo=%d\n", tid, escrevendo, lendo);
    }

    pthread_exit(NULL);
}

void *ler_varios(void *arg)
{
    int tid = *(int *)arg;
    int boba1, boba2;
    int i;
    for (i = 0; i < PASSOS; i++)
    {
        entra_leitura();
        printf("entra_leitura   tid=%d => escrevendo=%d, lendo=%d\n", tid, escrevendo, lendo);
        
        /* faz alguma coisa inutil pra gastar tempo... */
        boba1 = rand()%10000000;
        boba2 = -rand()%10000000;
        while (boba2 < boba1)
            boba2++;

        sai_leitura();
        printf("sai_leitura     tid=%d => escrevendo=%d, lendo=%d\n", tid, escrevendo, lendo);
    }

    pthread_exit(NULL);
}

void main()
{
    srand(mix(clock(), time(NULL), 12789057));
    srand(rand());

    // NTHREADS: variavel global (numero de threads)
    pthread_cond_init(&cond_pode_escrever, NULL);
    pthread_cond_init(&cond_pode_ler, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_t *tid;
    tid = (pthread_t *)malloc(sizeof(pthread_t) * NTHREADS);
    int *arg = malloc(sizeof(int) * NTHREADS);
    int k;
    for (k = 0; k < NTHREADS; k++)
    {
        arg[k] = k;
    }
    
    // cria as threads
    for (k = 0; k < NTHREADS - NTHREADS_LEIT; k++)
    {
        printf("escrever_varios\n");
        if (pthread_create(&tid[k], NULL, escrever_varios, (void *)&arg[k]) != 0)
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }
    for (; k < NTHREADS; k++)
    {
        printf("ler_varios\n");
        if (pthread_create(&tid[k], NULL, ler_varios, (void *)&arg[k]) != 0)
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    // espera todas as threads terminarem
    for (k = 0; k < NTHREADS; k++)
    {
        if (pthread_join(tid[k], NULL) != 0)
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    free(arg);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_pode_ler);
    pthread_cond_destroy(&cond_pode_escrever);
}

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}