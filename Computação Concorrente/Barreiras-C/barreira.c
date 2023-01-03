#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NTHREADS 5
#define PASSOS 5

int threads = 0;

pthread_mutex_t mutex;

pthread_cond_t cond_bar;

void barreira()
{
  pthread_mutex_lock(&mutex);
  threads++;
  if (threads < NTHREADS)
  {
    pthread_cond_wait(&cond_bar, &mutex);
  }
  else
  {
    threads = 0;
    pthread_cond_broadcast(&cond_bar);
  }
  pthread_mutex_unlock(&mutex);
}

void *A(void *arg)
{
  int tid = *(int *)arg, i;
  int boba1, boba2;
  for (i = 0; i < PASSOS; i++)
  {
    printf("Thread %d: passo=%d\n", tid, i);

    //sincronizacao condicional
    barreira();

    /* faz alguma coisa inutil pra gastar tempo... */
    boba1 = 100;
    boba2 = -100;
    while (boba2 < boba1)
      boba2++;
  }

  pthread_exit(NULL);
}

void main()
{
  // NTHREADS: variavel global (numero de threads)
  pthread_cond_init(&cond_bar, NULL);
  pthread_mutex_init(&mutex, NULL);

  pthread_t *tid;
  tid = (pthread_t *)malloc(sizeof(pthread_t) * NTHREADS);
  int *arg;

  // cria as threads
  for (int k = 0; k < NTHREADS; k++)
  {
    if (arg = malloc(sizeof(int)))
    {
      *arg = k;
    }
    else
    {
      exit(-1);
    }

    if (pthread_create(&tid[k], NULL, A, (void *)arg) != 0)
    {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  // espera todas as threads terminarem
  for (int k = 0; k < NTHREADS; k++)
  {
    if (pthread_join(tid[k], NULL) != 0)
    {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_bar);
}
