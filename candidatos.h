#ifndef CANDIDATOS_H
#define CANDIDATOS_H

#include "arvore.h"

#define ALPHA    1.0
#define BETA     0.5
#define MAX_NOS  10000

// Gera um ponto aleatório dentro do domínio circular (método de rejeição)
Point gera_ponto_aleatorio(double R);

// Preenche lista[] com todos os nós da árvore e atualiza *total
void coleta_nos(ptrNo raiz, ptrNo *lista, int *total);

// Preenche lista[] com todos os segmentos pai->filho da árvore
void coleta_segmentos(ptrNo raiz, Segment *lista, int *total);

// Retorna 1 se a conexão (candidato->p → novo) é geometricamente válida
int valida_candidato(Point novo, ptrNo candidato,
                     Segment *segs, int n_segs,
                     double R, double epsilon);

// Custo básico: distância euclidiana
double custo(Point novo, ptrNo candidato);

// Percorre todos os nós, valida, calcula custo e retorna o melhor nó
// Incrementa *rejeitados a cada candidato inválido
ptrNo seleciona_melhor(Point novo, ptrNo raiz,
                       double R, double epsilon,
                       int *rejeitados);

#endif
