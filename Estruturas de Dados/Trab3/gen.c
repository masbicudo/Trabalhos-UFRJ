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
    srand(mix(clock(), time(NULL), 12789057));

    // lendo parametros
    bool dbg = 0;
    bool allow_reps = 0;
    bool allow_negs = 0;
    bool has_user_def_inc = 0;
    int user_def_inc = 0;
    int itarg;
    double fator_mistura = 1.0;
    int nums = -1;
    for (itarg = 0; itarg < argc; itarg++) {
        // -r       repeticoes de numeros na sequencia de dados
        if (strcmp("-r", argv[itarg]) == 0) {
            allow_reps = 1;
        }
        // -n       numeros negativos
        else if (strcmp("-n", argv[itarg]) == 0) {
            allow_negs = 1;
        }
        // -dbg     debug messages
        else if (strcmp("-dbg", argv[itarg]) == 0) {
            dbg = 1;
        }
        // -i #     incrementos fixos de tamanho #
        else if (strcmp("-i", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                has_user_def_inc = 1;
                user_def_inc = atoi(argv[itarg + 1]);
                itarg++;
            }
        }
        // -m #     fator de mistura: 1 totalmente aleatório; >1 menos aleatório
        else if (strcmp("-m", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                fator_mistura = atof(argv[itarg + 1]);
                itarg++;
            }
        }
        // -c #     contagem de numeros aleatorios
        else if (strcmp("-c", argv[itarg]) == 0) {
            if (itarg + 1 < argc) {
                nums = atoi(argv[itarg + 1]);
                itarg++;
            }
        }
        // -h ou --help
        else if (strcmp("-h", argv[itarg]) == 0 || strcmp("--help", argv[itarg]) == 0) {
            printf(ANSI_COLOR("203") "Ajuda: Parametros\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR("64") "-r" ANSI_COLOR_RESET "    repeticoes de numeros na sequencia" "\n");
            printf(ANSI_COLOR("64") "-n" ANSI_COLOR_RESET "    numeros negativos" "\n");
            printf(ANSI_COLOR("64") "-i #" ANSI_COLOR_RESET "  incrementos fixos de tamanho #" "\n");
            printf(ANSI_COLOR("64") "-m #" ANSI_COLOR_RESET "  fator de mistura: 1 = totalmente aleatorio; >1.0 ~ +inf = menos aleatorio" "\n");
            printf(ANSI_COLOR("64") "-c #" ANSI_COLOR_RESET "  quantidade de numeros aleatorios #" "\n");
            printf(ANSI_COLOR("64") "-dbg" ANSI_COLOR_RESET "  mensagens de debug" "\n");
            printf(ANSI_COLOR("64") "-h" ANSI_COLOR_RESET "    esta tela de ajuda" "\n");
            return 0;
        }
    }
    dbg && printf("# allow_reps: %d\n", allow_reps);
    dbg && printf("# allow_negs: %d\n", allow_negs);
    dbg && printf("# has_user_def_inc: %d\n", has_user_def_inc);
    dbg && printf("# user_def_inc: %d\n", user_def_inc);

    // Gerando os numeros
    // ******************

    if (nums < 0) nums = (int)(rexp(1.0/20.0) + rexp(1.0/5.0)) + 1;

    dbg && printf("# nums: %d\n", nums);

    // Preenchendo lista com elementos em ordem
    // e distancias medias entre si de 2,
    // e depois misturando os elementos de acordo
    // com uma distribuicao qualquer
    int iterador;
    int* lista = (int*)malloc(sizeof(int)*nums);
    double next;
    while (1) {
        next = rnorm(5.0, 10.0);
        if (allow_negs) break;
        if (next >= 0) break;
    }
    dbg && printf("# first: %f\n", (float)next);
    dbg && printf("# list: ");
    for (iterador = 0; iterador < nums; iterador++) {
        next = iterador == 0 ? next : (
            has_user_def_inc ? next + user_def_inc :
            (allow_reps ? next + (rexp(1.0/1.0)) + 0.5 : next + (rexp(1.0/1.0))*0.5 + 1.0)
            );
        lista[iterador] = floor(next);
        if (iterador) dbg && printf(" ");
        dbg && printf("%d", (int)floor(next));
    }
    dbg && printf("\n");

    // misturando tudo usando um fator de mistura
    shufflefn(lista, nums, &my_frand, &fator_mistura);

    // imprimindo os numeros
    for (iterador = 0; iterador < nums; iterador++) {
        printf("%d\n", lista[iterador]);
    }
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