#include "grafo.h"
#include <stdlib.h>
#include <stdio.h>

// ── Auxiliar: Adiciona aresta à lista de adjacência ──────────────────
static void adiciona_aresta(Grafo* g, int origem, int destino, double comp) {
    Aresta* nova = (Aresta*)malloc(sizeof(Aresta));
    nova->destino_id = destino;
    nova->comprimento = comp;
    
    // Inserção no início da lista encadeada
    nova->prox = g->nos[origem].adjacentes;
    g->nos[origem].adjacentes = nova;
}

// ── 1. Conversão: Varredura da Árvore CCO para Grafo ─────────────────
static void preenche_grafo_recursivo(Grafo* g, ptrNo atual) {
    if (!atual) return;
    
    int id = atual->id;
    g->nos[id].id = id;
    g->nos[id].x = atual->p.x;
    g->nos[id].y = atual->p.y;
    g->nos[id].raio = atual->raio;
    g->nos[id].adjacentes = NULL;
    g->nos[id].is_terminal = (atual->esq == NULL && atual->dir == NULL);
    g->nos[id].grau = (atual->pai != NULL) ? 1 : 0; // Contabiliza a aresta vinda do pai

    // Regista arestas direcionadas (Pai -> Filho)
    if (atual->esq) {
        g->nos[id].grau++;
        adiciona_aresta(g, id, atual->esq->id, atual->esq->comprimento);
        preenche_grafo_recursivo(g, atual->esq);
    }
    if (atual->dir) {
        g->nos[id].grau++;
        adiciona_aresta(g, id, atual->dir->id, atual->dir->comprimento);
        preenche_grafo_recursivo(g, atual->dir);
    }
}

Grafo* constroi_grafo_da_arvore(ptrNo raiz, int total_nos) {
    Grafo* g = (Grafo*)malloc(sizeof(Grafo));
    g->total_nos = total_nos;
    g->max_profundidade = 0;
    g->max_comprimento_caminho = 0.0;
    
    // Alocamos total_nos + 2 para garantir que o array acomode perfeitamente
    // os IDs, assumindo que eles começam em 1 e podem ter pequenos saltos
    g->nos = (InfoNoGrafo*)calloc(total_nos + 2, sizeof(InfoNoGrafo)); 
    
    preenche_grafo_recursivo(g, raiz);
    return g;
}

// ── 2. Busca em Largura (BFS) com Fila Dinâmica ──────────────────────
void executar_bfs(Grafo* g, int id_raiz) {
    printf("--- Executando BFS (Busca em Largura) ---\n");
    
    bool* visitado = (bool*)calloc(g->total_nos + 2, sizeof(bool));
    int* fila = (int*)malloc((g->total_nos + 2) * sizeof(int));
    int inicio = 0, fim = 0;

    fila[fim++] = id_raiz;
    visitado[id_raiz] = true;
    int nos_visitados = 0;

    while (inicio < fim) {
        int atual = fila[inicio++];
        nos_visitados++;

        Aresta* adj = g->nos[atual].adjacentes;
        while (adj) {
            if (!visitado[adj->destino_id]) {
                visitado[adj->destino_id] = true;
                fila[fim++] = adj->destino_id;
            }
            adj = adj->prox;
        }
    }
    printf("BFS concluiu visitando %d nos.\n", nos_visitados);
    
    free(fila);
    free(visitado);
}

// ── 3. Busca em Profundidade (DFS) com Recursão ──────────────────────
static void dfs_recursivo(Grafo* g, int atual, bool* visitado, int* cont) {
    visitado[atual] = true;
    (*cont)++;
    
    Aresta* adj = g->nos[atual].adjacentes;
    while (adj) {
        if (!visitado[adj->destino_id]) {
            dfs_recursivo(g, adj->destino_id, visitado, cont);
        }
        adj = adj->prox;
    }
}

void executar_dfs(Grafo* g, int id_raiz) {
    printf("--- Executando DFS (Busca em Profundidade) ---\n");
    bool* visitado = (bool*)calloc(g->total_nos + 2, sizeof(bool));
    int cont = 0;
    
    dfs_recursivo(g, id_raiz, visitado, &cont);
    
    printf("DFS concluiu visitando %d nos.\n", cont);
    free(visitado);
}

// ── 4. Análise Topológica (Profundidade e Strahler) ──────────────────
static int calcula_strahler_recursivo(Grafo* g, int id_atual, int prof_atual, double comp_caminho) {
    g->nos[id_atual].profundidade = prof_atual;
    
    if (prof_atual > g->max_profundidade) {
        g->max_profundidade = prof_atual;
    }

    Aresta* adj = g->nos[id_atual].adjacentes;
    
    // Se não tem arestas de saída, é um nó terminal
    if (!adj) {
        if (comp_caminho > g->max_comprimento_caminho) {
            g->max_comprimento_caminho = comp_caminho;
        }
        g->nos[id_atual].ordem_strahler = 1;
        return 1;
    }

    int ordens[2] = {0, 0};
    int qtd_filhos = 0;
    
    // Desce para os filhos
    while (adj) {
        ordens[qtd_filhos] = calcula_strahler_recursivo(g, adj->destino_id, prof_atual + 1, comp_caminho + adj->comprimento);
        qtd_filhos++;
        adj = adj->prox;
    }

    // Processa a Ordem de Strahler
    if (qtd_filhos == 1) {
        g->nos[id_atual].ordem_strahler = ordens[0];
    } else if (qtd_filhos == 2) {
        if (ordens[0] == ordens[1]) {
            g->nos[id_atual].ordem_strahler = ordens[0] + 1;
        } else {
            g->nos[id_atual].ordem_strahler = (ordens[0] > ordens[1]) ? ordens[0] : ordens[1];
        }
    }
    
    return g->nos[id_atual].ordem_strahler;
}

void analisar_topologia(Grafo* g, int id_raiz) {
    calcula_strahler_recursivo(g, id_raiz, 0, 0.0);
    
    printf("--- Analise Topologica Concluida ---\n");
    printf("Profundidade Maxima    : %d niveis\n", g->max_profundidade);
    printf("Maior Caminho Fisico   : %.6f m\n", g->max_comprimento_caminho);
    printf("Ordem de Strahler Raiz : %d\n", g->nos[id_raiz].ordem_strahler);
}

// ── 5. Limpeza de Memória ────────────────────────────────────────────
void libera_grafo(Grafo* g) {
    if (!g) return;
    for (int i = 0; i <= g->total_nos + 1; i++) {
        Aresta* adj = g->nos[i].adjacentes;
        while (adj) {
            Aresta* temp = adj;
            adj = adj->prox;
            free(temp);
        }
    }
    free(g->nos);
    free(g);
}