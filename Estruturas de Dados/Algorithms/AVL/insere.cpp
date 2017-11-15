#include "avl.hpp"
#include <cstddef>
#include <stdlib.h>

bool avl_insere(tno* &raiz, int x) {
    if (raiz == NULL) {
        raiz = (tno*)malloc(sizeof(tno));
        raiz->chave = x;
        raiz->esq = NULL;
        raiz->dir = NULL;
        raiz->reg = 0;
        return true;
    }
    else {
        // escolhendo em qual subarvore devemos inserir
        int novo_reg = 0;
        if (x < raiz->chave) {
            bool aumentou = avl_insere(raiz->esq, x);
            if (aumentou) novo_reg--;
        }
        else {
            bool aumentou = avl_insere(raiz->dir, x);
            if (aumentou) novo_reg++;
        }

        if (novo_reg != 0) {
            // agora que ja inserimos e temos o balanco relativo,
            // vamos determinar o balanco absoluto para ver se a
            // arvore esta balanceada
            raiz->reg += novo_reg;

            if (raiz->reg < -1) {
                // arvore esta desbalanceada com maior altura na esquerda
                // temos que rotacionar para a direita
                if (x < raiz->esq->chave) {
                    // rotacao simple
                    avl_rot_1_dir(raiz);
                }
                else {
                    // rotacao dupla
                    avl_rot_2_dir(raiz);
                }

                // a rotacao desfaz o aumento
                return false;
            }
            else if (raiz->reg > +1) {
                // arvore esta desbalanceada com maior altura na direita
                // temos que rotacionar para a esquerda
                if (x < raiz->dir->chave) {
                    // rotacao simple
                    avl_rot_1_esq(raiz);
                }
                else {
                    // rotacao dupla
                    avl_rot_2_esq(raiz);
                }

                // a rotacao desfaz o aumento
                return false;
            }

            // nao vai haver rotacao, a arvore esta balanceada,
            // mas temos que lembrar que novo_reg != 0, o que
            // significa que a arvore aumentou
            return true;
        }
        
        // nada mudou na arvore, nem ficou desbalanceada, e nem aumentou
        return false;
    }
}
