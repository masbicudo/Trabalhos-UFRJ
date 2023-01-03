// Aluno: Miguel Angelo (masbicudo@gmail.com)
// DRE: 116033119 / UFRJ
// arrData: 2017-06

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void quicksort(int* lista, int ini, int fim);
void mergesort(int* lista, int ini, int fim);

int main(int argc, char **argv, char **envp) {

    int capacidade = 4*1024;
    int contagem = 0;
    int* lista = (int*)malloc(capacidade*sizeof(int));

    // lista de parametros
    bool dbg = 0;
    bool use_quicksort = 0;
    bool use_mergesort = 0;
    int itarg;
    bool opt_items = 0;
    bool use_stdin = 1;
    for (itarg = 0; itarg < argc; itarg++) {
        char* arg = argv[itarg];

        if (opt_items) {
            char *ptr;
            long n = strtol(arg, &ptr, 10);
            if (ptr[0] != 0) {
                opt_items = 0;
            }
            else {
                dbg && printf("# %ld\n", n);

                // adicionando elemento na lista
                if (contagem == capacidade) {
                    capacidade *= 2;
                    lista = (int*)realloc(lista, capacidade*sizeof(int));
                }
                lista[contagem] = n;
                contagem++;

                continue;                
            }
        }

        // -i       sequencia de itens a usar (nao usar stdin)
        if (strcmp("-i", arg) == 0) {
            opt_items = 1;
            use_stdin = 0;
        }
        // -q       usar quicksort
        else if (strcmp("-q", arg) == 0) {
            use_quicksort = 1;
        }
        // -m       usar mergesort
        else if (strcmp("-m", arg) == 0) {
            use_mergesort = 1;
        }
        // -dbg     debug messages
        else if (strcmp("-dbg", arg) == 0) {
            dbg = 1;
        }
    }
    dbg && printf("# use_quicksort: %d\n", use_quicksort);
    dbg && printf("# use_mergesort: %d\n", use_mergesort);

    // lista de itens a serem ordenados
    // ********************************
    if (use_stdin) {
        const int MAX_LINE_SZ = 0x1000;
        char line[MAX_LINE_SZ];
        int i = 0;
        for (;; i++) {
            int n;
            if (fgets(line, MAX_LINE_SZ, stdin) == 0) break;
            if (line[0] == '#') {
                dbg && printf("%s", line);
            }
            else {
                sscanf(line, "%d\n", &n);
                dbg && printf("# %d\n", n);
            }

            // adicionando elemento na lista
            if (contagem == capacidade) {
                capacidade *= 2;
                lista = (int*)realloc(lista, capacidade*sizeof(int));
            }
            lista[contagem] = n;
            contagem++;
        }
    }


    // Programa de ordenacao
    // ************************

    if (use_mergesort) {
        printf("# mergesort\n");
        mergesort(lista, 0, contagem-1);

        int it = 0;
        for (; it < contagem; it++) {
            printf("%d\n", lista[it]);
        }
    }
    else if (use_quicksort) {
        printf("# quicksort\n");
        quicksort(lista, 0, contagem-1);

        int it = 0;
        for (; it < contagem; it++) {
            printf("%d\n", lista[it]);
        }
    }
}

void quicksort(int* lista, int ini, int fim) {
    // Quicksort se baseia em escolher um cara qualquer para servir de pivot,
    // depois mover todos os elementos menores que o pivot para o inicio,
    // e tudo maior para o final do array.
    // Isso eh feito usando dois apontadores, um no inicio e outro no fim,
    // sendo que eles podem andar. Se o item no inicio for menor que o pivot,
    // o inicio anda 1 casa, se o item no fim for maior que o pivot o
    // fim desce 1 casa. Se o item no inicio for maior que o pivot, saio
    // do loop que anda com o inicio e vou para o loop que desce o fim.
    // Ai vou descer o fim, ate encontrar um item que seja menor que o pivot.
    // Se encontrar, trocar os itens do inicio e do fim. Se o fim passar para
    // baixo do inicio paro e chamo recursivamente.

    // Procurei saber, e existe uma variante do quicksort estavel,
    // que consta em usar um campo secundario que ordena entre os
    // elementos de mesmo valor. Isso iria afetar a performace,
    // alem de requerer mais n de memoria.

    if (ini >= fim) return;

    // obtendo o pivot
    int pivot = lista[ini/2 + fim/2 + (ini & fim & 1)];

    int a = ini;
    int b = fim;
    while (true) {
        while (lista[a] < pivot) {
            a++;
        }
        while (lista[b] > pivot) {
            b--;
        }

        if (a >= b)
            break;

        int temp = lista[a];
        lista[a] = lista[b];
        lista[b] = temp;
        a++;
        b--;
    }

    quicksort(lista, ini, b);
    quicksort(lista, b + 1, fim);
}

void _mergesort(int sel, int* arrData, int szData, int* arrExtra, int szExtra);

void mergesort(int* lista, int ini, int fim) {
    // Mergesort se baseia em unir dois arrays ordenados
    // de forma parecida com um zipper. Ele se chama
    // recursivamente para cada metade de um array nao
    // ordenado e depois junta essas duas metadas ordenadas.
    // Esse algoritmo requer memoria extra para fazer
    // a uniao das partes, usando 1/2*n de memoria.

    // Vou alternar os arrays original e o extra para
    // diminuir a quantidade de copias das valores na
    // memoria de um lado para o outro.

    // Procurei saber mais, e existe um mergesort in-place,
    // e ate consegui imaginar uma forma de fazer usando
    // um buffer extra de tamanho variavel, mas achei fora
    // do escopo deste problema.

    int szData = fim - ini + 1;
    int szExtra = szData - szData/2;
    int* arrExtra = (int*)malloc(szExtra*sizeof(int));
    _mergesort(0, lista, szData, arrExtra, szExtra);
}

void _mergesort(int sel, int* arrData, int szData, int* arrExtra, int szExtra) {
    int len2 = szData/2;
    int len1 = szData - len2;

    if (len2 > 1)
        _mergesort(0    , &arrData[len1], len2, arrExtra, szExtra);

    if (len1 > 1)
        _mergesort(1-sel, arrData       , len1, arrExtra, szExtra);
    else if (1-sel)
        arrExtra[0] = arrData[0];

    int* dst = sel == 0 ? arrData : arrExtra;
    int* src1 = sel == 0 ? arrExtra : arrData;
    int* src2 = &arrData[len1];

    int i1 = 0, i2 = 0;
    while (i1 + i2 < szData) {
        int cmp;

        cmp = i2 < len2 ? src2[i2] : (int)(~(0U)>>1);
        for (; i1 < len1; i1++) {
            if (src1[i1] > cmp)
                break;
            dst[i1 + i2] = src1[i1];
        }

        cmp = i1 < len1 ? src1[i1] : (int)(~(0U)>>1);
        for (; i2 < len2; i2++) {
            if (src2[i2] > cmp)
                break;
            dst[i1 + i2] = src2[i2];
        }
    }
}
