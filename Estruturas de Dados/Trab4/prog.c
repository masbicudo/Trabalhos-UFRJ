// Aluno: Miguel Angelo
//  116033119
// Arvore-B
// 
// NOTA: fiz insercao e insercao, mas 

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
int dd = 2;

bool arv_busca(arvbno* no, int chave);
bool arv_insere(arvbno** no, int chave);
bool arv_remove(arvbno** no, int chave);
int busca_bin(int* lista, int cnt, int chave);
void imprime_arv(arvbno* raiz);
void imprime_arv_ordem(arvbno* raiz);
void imprime_arv_preordem(arvbno* raiz);

int main(int argc, char **argv, char **envp) {
    // lendo parametros
    bool dbg = 0;
    int itarg;

    // dd = 1;
    arvbno* raiz = NULL;
    // arv_insere(&raiz, 10);
    // arv_insere(&raiz, 30);
    // arv_insere(&raiz, 20);
    // arv_insere(&raiz, 40);
    // arv_insere(&raiz, 50);
    // arv_insere(&raiz, 60);
    // arv_insere(&raiz, 70);

    // imprime_arv(raiz);

    // arv_remove(&raiz, 40);
    // imprime_arv_ordem(raiz);

    // imprime_arv(raiz);

    // return 0;

    for (itarg = 0; itarg < argc; itarg++) {
        // -d       parametro dd da arvore-B
        if (strcmp("-d", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                dd = atoi(argv[itarg + 1]);
                itarg++;
            }
        }
        // -dbg     debug messages
        else if (strcmp("-dbg", argv[itarg]) == 0) {
            dbg = 1;
        }
    }
    dbg && printf("# d: %d\n", dd);
    dbg && printf("# min-els: %d\n", dd);
    dbg && printf("# max-els: %d\n", dd*2);
    dbg && printf("# min-filhos: %d\n", dd + 1);
    dbg && printf("# max-filhos: %d\n", dd*2 + 1);

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

            if (op == 'p') {
                dbg && printf("# p\n");
                imprime_arv_preordem(raiz);
            }
            else {
                dbg && printf("# %c %d\n", op, val);
                if (op == 'i') {
                    arv_insere(&raiz, val);
                }
                else if (op == 'r') {
                    arv_remove(&raiz, val);
                }
            }
        }
    }
}

bool arv_busca(arvbno* no, int chave) {
    while (no != NULL) {
        // como a lista de elementos eh ordenada,
        // podemos fazer uma busca binaria
        int idx = busca_bin(no->chaves, no->cnt, chave);
        if (idx >= 0)
            return 1;
        no = no->filhos[~idx];
    }
    return 0;
}

void divide_filho(arvbno* no, int idx) {
    arvbno* filho = no->filhos[idx];

    arvbno* novo = (arvbno*)malloc(sizeof(arvbno));
    novo->chaves = (int*)malloc((dd*2 + 1)*sizeof(int));

    // o novo irmao tera filhos se o irmao mais velho tambem tiver
    novo->filhos = filho->filhos != NULL
        ? (arvbno**)malloc((dd*2 + 2)*sizeof(arvbno*))
        : NULL;

    // ambos os irmaos ficam com o mesmo numero de elementos
    novo->cnt = dd;
    filho->cnt = dd;

    // dividindo os elementos entre os irmaos
    memmove(&novo->chaves[0], &filho->chaves[dd + 1], dd*sizeof(int));
    if (filho->filhos != NULL) {
        memmove(&novo->filhos[0], &filho->filhos[dd + 1], (dd + 1)*sizeof(arvbno*));
    }

    // copiando a chave para o pai
    memmove(&no->chaves[idx + 1], &no->chaves[idx], (no->cnt - idx)*sizeof(int));
    no->chaves[idx] = filho->chaves[dd];

    // adicionando o novo filho no pai
    memmove(&no->filhos[idx + 2], &no->filhos[idx + 1], (no->cnt - idx)*sizeof(arvbno*));
    no->filhos[idx + 1] = novo;

    no->cnt++;
}
bool arv_insere_filho(arvbno* no, int chave) {
    int idx = -1;

    idx = busca_bin(no->chaves, no->cnt, chave);
    if (idx < 0) {
        idx = ~idx;
        if (no->filhos != NULL) {
            arvbno* filho = no->filhos[idx];
            if (!arv_insere_filho(filho, chave))
                return false;

            if (filho->cnt > 2*dd) {
                // este noh esta cheio de mais e deve ser dividido em dois
                divide_filho(no, idx);
            }
        }
        else {
            // se os filhos sao nulos, entao eh folha: podemos inserir aqui.
            // primeiro temos que abrir um espaco no array na posicao certa
            memmove(&no->chaves[idx + 1], &no->chaves[idx], (no->cnt - idx)*sizeof(int));
            no->chaves[idx] = chave;
            no->cnt++;
        }

        return true;
    }

    return false;
}
bool arv_insere(arvbno** no, int chave) {
    int idx = -1;

    // caso o noh nao exista, temos que criar um novo e adicionar a chave nele
    if ((*no) == NULL) {
        (*no) = (arvbno*)malloc(sizeof(arvbno));
        (*no)->chaves = (int*)malloc((dd*2 + 1)*sizeof(int));
        (*no)->chaves[0] = chave;
        (*no)->filhos = NULL;
        (*no)->cnt = 1;
        return true;
    }

    // se o noh ja existir, tentamos inserir nele
    if (!arv_insere_filho(*no, chave))
        return false;

    // se inseriu e o noh ficou cheio de mais, temos que criar uma nova raiz,
    // e depois dividir a raiz antiga
    if ((*no)->cnt > 2*dd) {
        arvbno* novo = (arvbno*)malloc(sizeof(arvbno));
        novo->chaves = (int*)malloc((dd*2 + 1)*sizeof(int));
        novo->filhos = (arvbno**)malloc((dd*2 + 2)*sizeof(arvbno*));
        novo->filhos[0] = *no;
        novo->cnt = 0;
        divide_filho(novo, 0);
        *no = novo;
    }

    return true;
}

void rot_esq(arvbno* no, int idx) {
    // visto
    arvbno* dir = no->filhos[idx + 1];
    arvbno* esq = no->filhos[idx];

    esq->chaves[esq->cnt] = no->chaves[idx];
    if (dir->filhos != NULL)
        esq->filhos[esq->cnt + 1] = dir->filhos[0];
    esq->cnt++;

    no->chaves[idx] = dir->chaves[0];

    memmove(&dir->chaves[0], &dir->chaves[1], (dir->cnt - 1)*sizeof(int));
    if (dir->filhos != NULL)
        memmove(&dir->filhos[0], &dir->filhos[1], (dir->cnt)*sizeof(arvbno*));
    dir->cnt--;
}
void rot_dir(arvbno* no, int idx) {
    // visto
    arvbno* dir = no->filhos[idx + 1];
    arvbno* esq = no->filhos[idx];

    memmove(&dir->chaves[1], &dir->chaves[0], (dir->cnt)*sizeof(int));
    dir->chaves[0] = no->chaves[idx];
    if (dir->filhos != NULL) {
        memmove(&dir->filhos[1], &dir->filhos[0], (dir->cnt + 1)*sizeof(arvbno*));
        dir->filhos[0] = esq->filhos[esq->cnt];
    }
    dir->cnt++;

    no->chaves[idx] = esq->chaves[esq->cnt - 1];

    esq->cnt--;
}
void junta_filhos(arvbno* no, int idx) {
    // visto
    arvbno* dir = no->filhos[idx + 1];
    arvbno* esq = no->filhos[idx];

    esq->chaves[esq->cnt] = no->chaves[idx];
    memmove(&esq->chaves[esq->cnt + 1], &dir->chaves[0], (dir->cnt)*sizeof(int));
    if (esq->filhos != NULL)
        memmove(&esq->filhos[esq->cnt + 1], &dir->filhos[0], (dir->cnt + 1)*sizeof(arvbno*));
    esq->cnt += dir->cnt + 1;
    
    free(dir->chaves);
    free(dir->filhos);
    free(dir);

    memmove(&no->chaves[idx], &no->chaves[idx + 1], (no->cnt - idx - 1)*sizeof(int));
    memmove(&no->filhos[idx + 1], &no->filhos[idx + 2], (no->cnt - idx - 1)*sizeof(arvbno*));
    no->cnt--;
}
void concerta_filho(arvbno* no, int idx) {
    // visto
    arvbno* filho = no->filhos[idx];
    if (idx < no->cnt) {
        arvbno* outro = no->filhos[idx + 1];
        if (outro->cnt > dd) {
            rot_esq(no, idx);
            return;
        }
    }

    if (idx > 0) {
        arvbno* outro = no->filhos[idx - 1];
        if (outro->cnt > dd) {
            rot_dir(no, idx - 1);
            return;
        }
    }

    junta_filhos(no, idx - (idx < no->cnt ? 0 : 1));
}
int remove_primeiro(arvbno* no) {
    // visto
    if (no->filhos == NULL) {
        int menor_chave = no->chaves[0];
        memmove(&no->chaves[0], &no->chaves[1], (no->cnt - 1)*sizeof(int));
        no->cnt--;
        return menor_chave;
    }
    else {
        arvbno* primogenito = no->filhos[0];
        int menor_chave = remove_primeiro(primogenito);
        if (primogenito->cnt < dd)
            concerta_filho(no, 0);
        return menor_chave;
    }
}
bool arv_remove_filho(arvbno* no, int chave) {
    int idx = busca_bin(no->chaves, no->cnt, chave);
    if (idx < 0) {
        idx = ~idx;
        if (no->filhos == NULL) return false;
        arvbno* filho = no->filhos[idx];
        if (!arv_remove_filho(filho, chave)) return false;
        if (filho->cnt < dd)
            concerta_filho(no, idx);
    }
    else if (no->filhos != NULL) {
        arvbno* filho_maior = no->filhos[idx + 1];
        int sucessor = remove_primeiro(filho_maior);
        no->chaves[idx] = sucessor;
        if (filho_maior->cnt < dd)
            concerta_filho(no, idx + 1);
    }
    else {
        memmove(&no->chaves[idx], &no->chaves[idx + 1], (no->cnt - idx - 1)*sizeof(int));
        no->cnt--;
    }
    return true;
}
bool arv_remove(arvbno** no, int chave) {
    if (no == NULL)
        return false;

    if (!arv_remove_filho(*no, chave))
        return false;

    // se a raiz ficou sem chaves, ela pode ser substituida por um filho
    // se existir um filho, ou entao por NULL se nao existir
    if ((*no)->cnt == 0) {
        arvbno* lixo = *no;

        *no = (*no)->filhos != NULL
            ? (*no)->filhos[0]
            : NULL;

        free(lixo->chaves);
        free(lixo->filhos);
        free(lixo);
    }
}

char* str_concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}
void _imprime_arv(arvbno* no, int* pos, int nivel, char*** strs, int* h) {
    int idx;
    for (idx = 0; idx < no->cnt; idx++) {
        if (no->filhos != NULL)
            _imprime_arv(no->filhos[idx], pos, nivel + 1, strs, h);

        if (nivel >= *h) {
            *strs = (char**)realloc(*strs, (nivel + 1)*sizeof(char**));
            int k;
            for (k = *h; k <= nivel; k++)
                (*strs)[k] = "";
            *h = nivel + 1;
        }

        int chave = no->chaves[idx];

        char* str = (*strs)[nivel];
        size_t len = strlen(str);

        int len_nova = (*pos + 1)*4 + 1;
        char* nova = (char*)malloc(len_nova*sizeof(char));
        memmove(nova, str, len*sizeof(char));
        snprintf(&nova[len], (*pos)*4 + 5 - len, "%*d ", (*pos)*4 + 3 - len, chave);

        (*strs)[nivel] = nova;
        (*pos) = (*pos) + 1;
    }
    if (no->filhos != NULL)
        _imprime_arv(no->filhos[idx], pos, nivel + 1, strs, h);
}
void imprime_arv(arvbno* raiz) {
    if (raiz != NULL) {
        char** strs = NULL;
        int pos = 0;
        int h = 0;
        _imprime_arv(raiz, &pos, 0, &strs, &h);
        int k;
        for (k = 0; k < h; k++)
            printf("%s\n", strs[k]);
    }
}

void _imprime_arv_ordem(arvbno* no, int* pos) {
    int k;
    for (k = 0; k < no->cnt; k++) {
        if (no->filhos != NULL) {
            _imprime_arv_ordem(no->filhos[k], pos);
        }
        
        if (*pos != 0)
            printf(" ");

        printf("%d", no->chaves[k]);

        (*pos) = (*pos) + 1;
    }
    if (no->filhos != NULL) {
        _imprime_arv_ordem(no->filhos[k], pos);
    }
}
void imprime_arv_ordem(arvbno* raiz) {
    if (raiz != NULL) {
        int pos = 0;
        _imprime_arv_ordem(raiz, &pos);
    }
    printf("\n");
}

void _imprime_arv_preordem(arvbno* no, int* pos) {
    int k;
    for (k = 0; k < no->cnt; k++) {
        if (*pos != 0)
            printf(" ");

        printf("%d", no->chaves[k]);

        (*pos) = (*pos) + 1;
    }
    if (no->filhos != NULL) {
        for (k = 0; k < no->cnt + 1; k++) {
            _imprime_arv_preordem(no->filhos[k], pos);
        }
    }
}
void imprime_arv_preordem(arvbno* raiz) {
    if (raiz != NULL) {
        int pos = 0;
        _imprime_arv_preordem(raiz, &pos);
    }
    printf("\n");
}

int busca_bin(int* lista, int cnt, int chave) {
    int min = 0;
    int max = cnt - 1;

    while (min <= max) {
        int it = (min + max)/2;
        if (lista[it] < chave) {
            min = it + 1;
        } else if (lista[it] > chave) {
            max = it - 1;
        }
        else {
            return it;
        }
    }
    return ~min;
}
