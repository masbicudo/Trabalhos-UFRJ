#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

unsigned long mix(unsigned long a, unsigned long b, unsigned long c);
float frand();
void shuffle(int *array, size_t n);

void main(void) {
    srand(mix(clock(), time(NULL), 12789057));

    int problemas = (int)(powf(frand(), 2.0f)*9 + powf(frand(), 32.0f)*90 + 1);
    int idProb;
    for (idProb = 0; idProb < problemas; idProb++) {
        int tarefas = (int)(powf(frand(), 0.5f)*4 + powf(frand(), 4.0f)*15 + 1);
        // 1 0
        // 2 1
        // 3 3
        // 4 6
        // 5 10
        // x (x-1)*x/2
        int max_deps = (tarefas-1)*tarefas/2;
        int deps = (int)((powf(frand(), 0.5f)*0.25 + powf(frand(), 2.0f)*0.75)*(max_deps + 1));
        printf("%d %d # head\n", tarefas, deps);

        // criando um array com todas as tarefas desordenadas
        int* lista_tarefas = (int*)malloc(tarefas*sizeof(int));
        int i;
        for (i = 0; i < tarefas; i++)
            lista_tarefas[i] = i + 1;
        shuffle(lista_tarefas, tarefas);

        int* lista_deps = (int*)malloc(max_deps*sizeof(int));
        for (i = 0; i < max_deps; i++)
            lista_deps[i] = 0;

        // Imprimindo a lista de dependencias na tela, de forma que
        // uma tarefa so pode depender de tarefas que estiverem
        // antes dela na lista_tarefas.

        // Cada par de tarefas dependentes possui um elemento correspondente
        // em lista_deps, que indica se a relacao ja existe ou nao.
        // Ao imprimir uma dependencia, marcamos que a relacao existe.
        // Se a mesma ja existia, entao procuramos a proxima relacao que
        // ainda nao existe.

        for (i = 0; i < deps; i++) {
            int tar_id = (int)(frand()*(tarefas - 1)) + 1;
            int dep_tar_id = (int)(frand()*tar_id);

            // achando o indice da relacao
            int index;
            int j;
            index = dep_tar_id;
            for (j = 1; j < tar_id; j++)
                index += j;
            
            // vamos buscar a proxima relacao que estiver disponivel
            while (lista_deps[index % max_deps] != 0) {
                index++;
                if (index == max_deps)
                    index = 0;
                dep_tar_id++;
                if (dep_tar_id == tar_id) {
                    dep_tar_id = 0;
                    tar_id++;
                    if (tar_id == tarefas)
                        tar_id = 0;
                }
                if (index > max_deps*2)
                    printf("Error!");
            }

            lista_deps[index] = 1;
            printf("%d %d\n", lista_tarefas[tar_id], lista_tarefas[dep_tar_id]);
        }

        free(lista_deps);
        free(lista_tarefas);
    }
    printf("0 0 # end\n");
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

float frand() {
    return (float)(rand()/(RAND_MAX+1.0));
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}