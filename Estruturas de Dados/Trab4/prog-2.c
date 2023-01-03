#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct _arvb_node {
    int cnt;
    int* chaves;
    struct _arvb_node **filhos;
} arvbno;

int arv_error = 0;

bool arv_busca(arvbno* no, int valor);
int arv_insere(arvbno* no, int valor, int d, arvbno** split2);
void arv_remove(arvbno* no, int valor);
int busca_bin(int* lista, int cnt, int valor);
char* arv_err_str(int err);

int main(int argc, char **argv, char **envp) {
    // lendo parametros
    bool dbg = 0;
    int d = 2;
    int itarg;

    arvbno* raiz;
    int r = arv_insere(NULL, 10, d, &raiz);
    if (r != 0)
        printf("%s", arv_err_str(r));

    return 0;

    for (itarg = 0; itarg < argc; itarg++) {
        // -d       parametro d da arvore-B
        if (strcmp("-d", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                d = atoi(argv[itarg + 1]);
                itarg++;
            }
        }
        // -dbg     debug messages
        else if (strcmp("-dbg", argv[itarg]) == 0) {
            dbg = 1;
        }
    }
    dbg && printf("# d: %d\n", d);
    dbg && printf("# min-els: %d\n", d);
    dbg && printf("# max-els: %d\n", d*2);
    dbg && printf("# min-filhos: %d\n", d + 1);
    dbg && printf("# max-filhos: %d\n", d*2 + 1);

    // lendo itens e fazendo operacoes
    // ********************************
    const int MAX_LINE_SZ = 0x1000;
    char line[MAX_LINE_SZ];
    int i = 0;
    for (;; i++) {
        if (fgets(line, MAX_LINE_SZ, stdin) == 0) break;
        if (line[0] == '#') {
            dbg && printf("%s", line);
        }
        else {
            char op;
            int val;
            sscanf(line, "%c %d\n", &op, &val);

            if (op == 'p') printf("# p\n");
            else printf("# %c %d\n", op, val);
        }
    }
}

bool arv_busca(arvbno* no, int valor) {
    // como a lista de elementos eh ordenada,
    // podemos fazer uma busca binaria
    while (no != NULL) {
        int pt = busca_bin(no->chaves, no->cnt, valor);
        if (pt >= 0)
            return 1;
        no = no->filhos[~pt];
    }
    return 0;
}

int arv_insere(arvbno* no, int valor, int d, arvbno** out_split2) {
    int pt = -1;

    if (no == NULL) {
        // achando posicao onde inserir o valor
        pt = busca_bin(no->chaves, no->cnt, valor);
        if (pt >= 0)
            return 1; // inserir valor falhou pois ja existe na arvore
    }

    pt = ~pt;
    if (no->filhos != NULL) {
        arvbno* ptr_filho = no->filhos[pt];

        arvbno* split2;
        int r = arv_insere(ptr_filho, valor, d, &split2);
        if (split2 != NULL) {
            arvbno* split1 = no;
            int valor_sobe = split1->chaves[split1->cnt - 1];
            split1->cnt--;

            no = (arvbno*)malloc(sizeof(arvbno));
            no->cnt = 1;
            no->chaves = (int*)malloc(d*2*sizeof(int));
            no->filhos = (arvbno**)malloc((d*2 + 1)*sizeof(arvbno*));
            no->chaves[0] = valor_sobe;
            no->filhos[0] = split1;
            no->filhos[1] = split2;
        }
    }

    // encontramos uma folha, podemos inserir o valor aqui
    if (no->cnt == d) {
        // temos que dividir o noh, ou emprestar para um dos vizinhos,
        // no momento, vou apenas dividir o noh, sem emprestar nada
        if (pt > d) {
            // quando pt > d, o valor novo vai para o segundo subnoh
            arvbno* no2 = (arvbno*)malloc(sizeof(arvbno));

            pt = pt - d - 1;
            no2->chaves = (int*)malloc(d*2*sizeof(int));
            memmove(&no->chaves[d + 1], no2->chaves, pt*sizeof(int));
            no2->chaves[pt] = valor;
            memmove(&no->chaves[d + pt + 2], &no2->chaves[pt + 1], (d - pt - 1)*sizeof(int));
            no2->cnt = d;
            no->cnt = d + 1;

            if (no->filhos != NULL) {
                pt++;
                no2->filhos = (arvbno**)malloc((d*2 + 1)*sizeof(arvbno*));
                memmove(&no->filhos[d + 1], no2->filhos, pt*sizeof(arvbno*));
                no->filhos[d + 1] = no2;
                no2->filhos[pt] = NULL;
                memmove(&no->filhos[d + pt + 2], &no2->chaves[pt + 1], (d - pt - 1)*sizeof(arvbno*));
            }
        } else {
            // neste caso o valor novo vai para o primeiro subnoh
            arvbno* no2 = (arvbno*)malloc(sizeof(arvbno));

        }
    } else {
        // podemos inserir diretamente neste noh
        no->cnt++;

    }
}

void arv_remove(arvbno* no, int valor) {

}

int busca_bin(int* lista, int cnt, int valor) {
    int min = 0;
    int max = cnt;

    while (min <= max) {
        int it = (min + max)/2;
        if (lista[it] < valor) {
            min = it + 1;
        } else if (lista[it] > valor) {
            max = it - 1;
        }
        else {
            return it;
        }
    }
    return ~max;
}

char* arv_err_str(int err) {
    if (err == 0)
        return "Sucesso";
    if (err == 1)
        return "Chave ja existe";
}
