#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct lista_no {
    struct lista_no* ptr;
    int x;
} lista_no;


void main(void) {
    const int MAX_LINE_SZ = 0x1000;
    char line[MAX_LINE_SZ];
    for (;;) {
        int tarefas, deps;
        fgets(line, MAX_LINE_SZ, stdin);
        sscanf(line, "%d %d", &tarefas, &deps);

        if (tarefas == 0 && deps == 0)
            break;

        /* printf("%d %d # head\n", tarefas, deps); */

        lista_no** lista_depois = (lista_no**)calloc(tarefas, sizeof(lista_no*));
        int* cnt_antes = (int*)calloc(tarefas, sizeof(int));

        int it_dep;
        for (it_dep = 0; it_dep < deps; it_dep++) {
            /* Temos duas tarefas, sendo que a primeira depende da segunda. */
            int tar, dep;
            fgets(line, MAX_LINE_SZ, stdin);
            sscanf(line, "%d %d", &dep, &tar);

            lista_no* no = (lista_no*)malloc(sizeof(lista_no));
            no->ptr = lista_depois[dep-1];
            no->x = tar;
            lista_depois[dep-1] = no;

            cnt_antes[tar-1]++;
        }

        /* Todas as tarefas que nao possuem dependencias podem ser impressas. */
        /* Vamos colocar tais tarefas no inicio da fila de impressao. */
        int* fila = (int*)calloc(tarefas, sizeof(int));

        int it_tar;
        int fila_fim = 0;
        for (it_tar = 0; it_tar < tarefas; it_tar++)
            if (cnt_antes[it_tar] == 0) fila[fila_fim++] = it_tar+1;

        int it_fila;
        for (it_fila = 0; it_fila < fila_fim; it_fila++) {
            /* Avisando a todos que vem depois que o item foi executado. */
            /* Se algum que vem depois ja puder ser executado sera colocado na fila. */
            int tar = fila[it_fila];
            if (it_fila) printf(" ");
            printf("%d", tar);
            lista_no* next = lista_depois[tar-1];
            while (next) {
                cnt_antes[next->x-1]--;
                if (cnt_antes[next->x-1] == 0)
                    fila[fila_fim++] = next->x;
                next = next->ptr;
            }
        }
        printf("\n");
    }
}