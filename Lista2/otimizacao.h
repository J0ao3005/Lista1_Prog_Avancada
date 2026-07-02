#ifndef OTIMIZACAO_H
#define OTIMIZACAO_H

#include "fisica.h"

// ── Parte F: Coordenadas baricêntricas ───────────────────────────────
//
// Um ponto dentro de um triângulo ABC pode ser expresso como:
//   X = α·A + β·B + λ·C   com   α + β + λ = 1  e  α,β,λ ≥ 0
//
// Isso permite amostrar sistematicamente todo o interior do triângulo
// variando α e β em passos de 1/M.

// Calcula X = α·A + β·B + λ·C
Point pontoBaricentrico(Point A, Point B, Point C,
                        double alpha, double beta, double lambda);

// Retorna 1 se P está dentro (ou na borda) do triângulo ABC, 0 caso contrário
int pontoDentroTriangulo(Point A, Point B, Point C, Point P);

// ── Parte F/G: Busca em grade ─────────────────────────────────────────
//
// Com a bifurcação já inserida na árvore e física calculada, testa M²
// posições dentro do triângulo ABC. Para cada posição X candidata:
//   - Calcula o volume dos 3 segmentos afetados (A→X, X→B, X→C)
//   - Verifica interseções geométricas
//   - Guarda a posição de menor custo (menor volume total)
//
// Parâmetros:
//   bifurc        : nó de bifurcação (já na árvore, com raio calculado)
//   filho_antigo  : nó filho original do segmento dividido
//   novo_terminal : novo nó terminal
//   A, B, C       : vértices do triângulo (A=pai, B=filho, C=novo terminal)
//   M             : resolução da grade (M² posições testadas)
//   vol_base      : volume da árvore excluindo os 3 segmentos afetados
//   segs, n_segs  : segmentos existentes para checar interseção
//   *melhorCusto  : saída com o menor volume encontrado
//
// Retorna a posição X que minimiza o volume intravascular.
Point otimizaBifurcacaoPorGrade(ptrNo bifurc, ptrNo filho_antigo, ptrNo novo_terminal,
                                 Point A, Point B, Point C,
                                 int M, double vol_base, double epsilon,
                                 Segment *segs, int n_segs,
                                 double *melhorCusto);

#endif