#ifndef GRAFO_H
#define GRAFO_H

#include "arvore.h"
#include <stdbool.h>

// Representa uma aresta (conexão) na lista de adjacência
typedef struct Aresta {
    int destino_id;
    double comprimento;
    struct Aresta* prox;
} Aresta;

// Informações topológicas armazenadas em cada nó do grafo
typedef struct {
    int id;
    double x, y;
    double raio;
    int grau;
    int profundidade;
    int ordem_strahler;
    bool is_terminal;
    Aresta* adjacentes; // Cabeça da lista encadeada de arestas
} InfoNoGrafo;

// Estrutura principal do Grafo
typedef struct {
    InfoNoGrafo* nos; // Array dinâmico alocado com base no total_nos
    int total_nos;
    int max_profundidade;
    double max_comprimento_caminho;
} Grafo;

// ── Funções do Grafo ───────────────────────────────────────────────

// Aloca a memória e converte a estrutura da árvore CCO para Grafo
Grafo* constroi_grafo_da_arvore(ptrNo raiz, int total_nos);

// Algoritmos obrigatórios de busca
void executar_bfs(Grafo* g, int id_raiz);
void executar_dfs(Grafo* g, int id_raiz);

// Análise de topologia (Strahler, caminhos e profundidade)
void analisar_topologia(Grafo* g, int id_raiz);

// Liberação de toda a memória alocada dinamicamente pelas listas
void libera_grafo(Grafo* g);

#endif