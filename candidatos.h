#ifndef CANDIDATOS_H
#define CANDIDATOS_H

#include "arvore.h"

#define MAX_NOS 10000

// Gera ponto aleatório dentro do domínio circular (método de rejeição)
Point gera_ponto_aleatorio(double R);

// Coleta todos os nós não-raiz da árvore.
// Cada nó coletado define um segmento: nó->pai->p → nó->p
void coleta_filhos(ptrNo raiz, ptrNo *lista, int *total);

// Coleta todos os segmentos (para checagem de interseção)
void coleta_segmentos(ptrNo raiz, Segment *lista, int *total);

// Percorre todos os segmentos existentes como candidatos.
// Para cada segmento calcula seu ponto médio e avalia a conexão mid → novo.
// Retorna o nó-filho do melhor segmento e escreve o ponto médio em *mid_out.
// Retorna NULL se nenhum segmento válido for encontrado.
ptrNo seleciona_melhor(Point novo, ptrNo raiz,
                       double R, double epsilon,
                       int *rejeitados, Point *mid_out);

#endif
