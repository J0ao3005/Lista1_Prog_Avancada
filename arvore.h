#ifndef ARVORE_H
#define ARVORE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "geometria.h"

typedef struct No {
    struct No *esq;
    struct No *dir;
    struct No *pai;
    Point  p;
    int    id;
    double raio;
    double comprimento;
    double fluxo;
    double resistencia;
    double volume;
    int    qtd_term_distal;
} No;

typedef No* ptrNo;

ptrNo  cria_no(Point p, ptrNo pai, int id);
bool   insere_filho(ptrNo pai, ptrNo filho);
void   insere_bifurcacao(ptrNo pai, ptrNo filho_antigo,
                         ptrNo bifurc, ptrNo novo_terminal);


void   remove_bifurcacao(ptrNo pai, ptrNo filho_antigo,
                          ptrNo bifurc, ptrNo novo_terminal);

void   percurso_nos(ptrNo raiz, void (*callback)(ptrNo));
int    conta_folhas(ptrNo raiz);
double comprimento_total(ptrNo raiz);
void   libera_arvore(ptrNo raiz);

#endif