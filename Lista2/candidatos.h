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

// ─────────────────────────────────────────────────────────────────────
// Seleção do melhor candidato de conexão (Partes F/G integradas)
// ─────────────────────────────────────────────────────────────────────
// Para cada segmento pai→filho existente na árvore (candidato K):
//   1. insere uma bifurcação temporária no ponto médio do segmento;
//   2. recalcula a física da árvore (qtd_term_distal, fluxo, raio —
//      dependem só da topologia, logo ficam corretos e FIXOS para
//      qualquer posição X dentro do triângulo);
//   3. roda a busca em grade (otimizaBifurcacaoPorGrade) para achar a
//      posição X que minimiza o volume intravascular total, checando
//      interseção geométrica contra o resto da árvore;
//   4. desfaz a inserção temporária (a árvore volta ao estado original);
//   5. mantém o candidato de menor volume total válido encontrado.
//
// Parâmetros:
//   novo               : novo ponto terminal a conectar
//   raiz               : raiz da árvore atual
//   R, epsilon         : raio do domínio e distância mínima entre segmentos
//   Qterm, gamma, mu   : parâmetros físicos para atualizaGeometriaFisica
//   M_grade            : resolução da busca em grade (Parte F)
//
// Saídas:
//   *testadas          : incrementado a cada candidato efetivamente avaliado
//   *rejeitados        : incrementado quando NENHUMA posição válida (sem
//                         interseção) é encontrada para aquele candidato
//   *best_X_out        : posição ótima de bifurcação do candidato vencedor
//
// Retorna o nó-filho do segmento vencedor (ainda não modificado — a
// inserção definitiva deve ser feita pelo chamador com insere_bifurcacao),
// ou NULL se nenhum candidato válido foi encontrado.
ptrNo seleciona_melhor(Point novo, ptrNo raiz,
                       double R, double epsilon,
                       int *testadas, int *rejeitados,
                       double Qterm, double gamma, double mu,
                       int M_grade, Point *best_X_out);

#endif