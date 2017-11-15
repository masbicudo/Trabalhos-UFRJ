#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct arv_no {
    struct arv_no* f0;
    struct arv_no* f1;
    char* nome;
} arv_no;

arv_no arvNoLimpo;

typedef unsigned char byte;
typedef unsigned int uint;

void main(void) {
    const int MAX_LINE_SZ = 0x1000;
    char line[MAX_LINE_SZ];
    int cnt = 0;
    fgets(line, MAX_LINE_SZ, stdin);
    sscanf(line, "%d", &cnt);

    // A estrutura que eu vou usar vai ser uma arvore em que cada no possui 2 filhos.
    // O filho 0 indica um bit 0 no endereco IP, o filho 1 indica um bit 1 no IP.
    // Cada no possui um nome que pode estar associado a ele.
    arv_no raiz = arvNoLimpo;

    int i = 0;
    for (; i < cnt; i++) {
        uint a, b, c, d, mask;
        char nome[MAX_LINE_SZ];
        fgets(line, MAX_LINE_SZ, stdin);
        sscanf(line, "%d.%d.%d.%d/%d%*[\t ]%[^\n]", &a, &b, &c, &d, &mask, nome);
        // DBG: printf("CDIR: %d.%d.%d.%d/%d '%s'\n", a, b, c, d, mask, nome);

        uint ip = (a<<24) + (b<<16) + (c<<8) + d;
        // DBG: printf("CDIR: %d.%d.%d.%d %08x\n", a, b, c, d, ip);

        // inserindo o no na arvore
        arv_no* current = &raiz;
        int i;
        for (i = 0; i <= 32; i++) {
            if (i == mask || i == 32) {
                current->nome = strdup(nome);
                break;
            }
            uint bit = (ip >> (31-i)) & 1;
            arv_no* filho;
            if (bit == 0) {
                filho = current->f0;
                if (filho == 0) {
                    filho = (arv_no*)malloc(sizeof(arv_no));
                    *filho = arvNoLimpo;
                    current->f0 = filho;
                }
            }
            else {
                filho = current->f1;
                if (filho == 0) {
                    filho = (arv_no*)malloc(sizeof(arv_no));
                    *filho = arvNoLimpo;
                    current->f1 = filho;
                }
            }
            current = filho;
        }
    }

    while(fgets(line, MAX_LINE_SZ, stdin) != 0) {
        // DBG: printf("Digitou: %s", line);

        uint a, b, c, d;
        sscanf(line, "%d.%d.%d.%d", &a, &b, &c, &d);
        uint ip = (a<<24) + (b<<16) + (c<<8) + d;

        // procurando o no mais significativo na arvore
        arv_no* current = &raiz;
        arv_no* melhor = 0;
        int i;
        for (i = 0; i <= 32; i++) {
            if (current == 0) break;
            if (current->nome != 0) {
                melhor = current;
            }
            if (i == 32) break;
            uint bit = (ip >> (31-i)) & 1;
            if (bit == 0) {
                current = current->f0;
            }
            else {
                current = current->f1;
            }
        }

        char* nome = melhor != 0 ? melhor->nome : "[Nao localizado]";
        printf("%d.%d.%d.%d %s\n", a, b, c, d, nome);
    }
}
