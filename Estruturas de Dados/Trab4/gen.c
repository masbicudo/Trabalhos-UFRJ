#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define ANSI_COLOR_RGB(r,g,b)      "\x1b[38;2;" r ";" g ";" b "m"
#define ANSI_COLOR(x)      "\x1b[38;5;" x "m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// -m Merge-sort
// -q Quick-sort

typedef double RandFunc(void*);
unsigned long mix(unsigned long a, unsigned long b, unsigned long c);
double frand();
double rexp(double lambda);
double rnorm(double mu, double sigma);
void shuffle(int *array, size_t n);
void shufflefn(int *array, size_t n, RandFunc* fn, void* ptr_param);
double frandsq();
double frandn(double n);



double my_frand(void* n) {
    return frandn(*(double*)n);
}

int main(int argc, char **argv, char **envp) {
    // Este programa gera uma lista de insercoes e remocoes
    // para uma arvore-B, seguindo o formato:
    // i #  insere o numero # na arvore
    // r #  remove o numero # da arvore

    // Parametros:
    // - ops #  operacoes
    // - max #  maximo de itens

    srand(mix(clock(), time(NULL), 12789057));

    // lendo parametros
    bool dbg = 0;
    int operacoes = 15;
    int max_items = 20;
    bool clear_all = 0;
    int itarg;
    for (itarg = 0; itarg < argc; itarg++) {
        // -ops     numero de operacoes que serao feitas
        if (strcmp("-ops", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                operacoes = atof(argv[itarg + 1]);
                itarg++;
            }
        }
        // -max     maximo de itens
        else if (strcmp("-max", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                max_items = atof(argv[itarg + 1]);
                itarg++;
            }
        }
        // -dbg     debug messages
        else if (strcmp("-dbg", argv[itarg]) == 0) {
            dbg = 1;
        }
        // -c       clear items of the tree at the end
        else if (strcmp("-c", argv[itarg]) == 0) {
            clear_all = 1;
        }
        // -h ou --help
        else if (strcmp("-h", argv[itarg]) == 0 || strcmp("--help", argv[itarg]) == 0) {
            printf(ANSI_COLOR("203") "Ajuda: Parametros\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR("64") "-ops" ANSI_COLOR_RESET "    numero de operacoes que serao feitas" "\n");
            printf(ANSI_COLOR("64") "-max" ANSI_COLOR_RESET "    numero maximo de elementos" "\n");
            printf(ANSI_COLOR("64") "-c" ANSI_COLOR_RESET "      tenta limpar todos os itens da arvore ao final\n" "\n");
            printf(ANSI_COLOR("64") "-dbg" ANSI_COLOR_RESET "    mensagens de debug" "\n");
            printf(ANSI_COLOR("64") "-h" ANSI_COLOR_RESET "      esta tela de ajuda" "\n");
            return 0;
        }
    }
    dbg && printf("# operacoes: %d\n", operacoes);
    dbg && printf("# max_items: %d\n", max_items);

    // Gerando os numeros
    // ******************

    int cnt = 0;
    int* inseridos = (int*)malloc(sizeof(int)*max_items);

    // Inserindo e removendo elementos
    int num_op = 0;
    while(num_op < operacoes) {
        bool b_ins = clear_all ?
            frand() < (1.0 - (double)(cnt + 1)/(operacoes - num_op)) :
            frand()*max_items >= cnt;

        if (!b_ins && cnt == 0)
            b_ins = 1;
        
        if (b_ins) {
            // inserindo um elemento
            int valor = (int)rexp(1.0/(double)max_items);
            printf("i %d\n", valor);

            // achando um lugar para colocar na lista
            int it;
            for (it = 0; it < cnt; it++) {
                if (valor > inseridos[it])
                    break;
            }
            // abrindo espaco para colocar na lista
            int it2;
            for (it2 = 0; it + it2 < cnt; it2++) {
                inseridos[cnt - it2] = inseridos[cnt - it2 - 1];
            }
            // inserindo valor na lista
            inseridos[it] = valor;
            cnt++;
            
        } else {
            // removendo um dos elementos que ja existem
            int idx_remove = frand()*cnt;
            int valor = inseridos[idx_remove];
            printf("r %d\n", valor);

            // removendo da lista
            cnt--;
            int it;
            for (it = idx_remove; it < cnt; it++) {
                inseridos[it] = inseridos[it+1];
            }
        }
        num_op++;

        if (dbg) {
            printf("# itens(%d): ", cnt);
            int it;
            for (it = 0; it < cnt; it++) {
                if (it > 0) printf(" ");
                printf("%d", inseridos[it]);
            }
            printf("\n");
        }

        // imprimindo uma vez a cada 5 operacoes executadas
        frand() > 0.8 && printf("p\n");
    }
    printf("p\n");
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

double frand() {
    return (double)(rand()/(RAND_MAX+1.0));
}

double frandsq() {
    return frand()*frand();
}

double frandn(double n) {
    return pow(frand(), n);
}

double rexp(double lambda) {
    return -log(1 - frand())/lambda;
}

double rnorm(double mu, double sigma) {
    double r = 0.0;
    const int LOOPS = 10;
    int k;
    for (k = 0; k < LOOPS; k++)
        r += frand();
    return (r - 0.5*(double)LOOPS)*sigma + mu;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shufflefn(int *array, size_t n, RandFunc* fn, void* ptr)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + (*fn)(ptr)*(n - i);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void shuffle(int *array, size_t n)
{
    shufflefn(array, n, &frand, 0);
}