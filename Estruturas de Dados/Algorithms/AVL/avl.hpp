#include <stdbool.h>

typedef struct _tno {
    int chave;
    int reg;
    struct _tno* esq;
    struct _tno* dir;
} tno;

bool avl_insere(tno* &raiz, int x);
