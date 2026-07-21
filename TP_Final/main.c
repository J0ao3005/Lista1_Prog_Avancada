#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "candidatos.h"
#include "exportar.h"
#include "fisica.h"
#include "grafo.h"

static double soma_raios_g;
static int    conta_raios_g;
static void acumula_raio(ptrNo no) {
    if (no->pai == NULL) return;
    soma_raios_g += no->raio;
    conta_raios_g++;
}

int main(int argc, char *argv[]) {
    // ── 1. Argumentos Exigidos no CCO-X (Seção 8) ────────────────────
    if (argc != 7) {
        fprintf(stderr, "Uso: %s <Nterm> <R> <gamma> <M> <modo> <seed>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 100 10.0 3.0 20 graph 123\n", argv[0]);
        return 1;
    }

    int    Nterm   = atoi(argv[1]);
    double R       = atof(argv[2]);
    double gamma   = atof(argv[3]);
    int    M_grade = atoi(argv[4]);
    char*  modo    = argv[5];
    int    seed    = atoi(argv[6]);
    
    double epsilon = 0.05 * R;
    double Qterm   = QPERF / Nterm;

    printf("=== CCO-X: Variante %s ===\n", modo);
    printf("Semente : %d\n", seed);
    printf("Nterm   : %d | R: %.2f | gamma: %.2f | M: %d\n\n", Nterm, R, gamma, M_grade);

    // Usa a semente passada pelo utilizador para garantir reprodutibilidade
    srand((unsigned int)seed);
    clock_t t_inicio = clock();

    // ── 2. Motor Central do CCO ───────
    Point raiz_pt  = {0.0, R};
    ptrNo raiz     = cria_no(raiz_pt, NULL, 0);
    int   proximo_id = 1;
    int   inseridos  = 0, testadas = 0, rejeitados = 0;

    for (int i = 0; i < Nterm; i++) {
        Point novo = gera_ponto_aleatorio(R);

        if (raiz->esq == NULL && raiz->dir == NULL) {
            if (!dentro_dominio(novo, R)) continue;
            ptrNo novo_no = cria_no(novo, raiz, proximo_id++);
            if (!novo_no) continue;
            insere_filho(raiz, novo_no);
            inseridos++;
            atualizaGeometriaFisica(raiz, Qterm, gamma, MU);
            continue;
        }

        Point best_X;
        ptrNo filho = seleciona_melhor(novo, raiz, R, epsilon, &testadas, &rejeitados, Qterm, gamma, MU, M_grade, &best_X);
        if (filho == NULL) continue;

        ptrNo bifurc  = cria_no(best_X, filho->pai, proximo_id++);
        ptrNo novo_no = cria_no(novo,   bifurc,     proximo_id++);
        if (!bifurc || !novo_no) { free(bifurc); free(novo_no); continue; }

        insere_bifurcacao(filho->pai, filho, bifurc, novo_no);
        inseridos++;
        atualizaGeometriaFisica(raiz, Qterm, gamma, MU);
    }

    clock_t t_fim  = clock();
    double  elapsed = (double)(t_fim - t_inicio) / CLOCKS_PER_SEC;

    // ── 3. Estatísticas Básicas ───────────────────────────────────────
    int total_nos  = proximo_id;
    int total_segs = total_nos - 1;
    int terminais  = conta_folhas(raiz);
    
    soma_raios_g = 0.0; conta_raios_g = 0;
    percurso_nos(raiz, acumula_raio);
    double raio_medio = (conta_raios_g > 0) ? soma_raios_g / conta_raios_g : 0.0;

    // ── 4. Inovação: GraphCCO (Projeto 1) ─────────────────────────────
    printf("=== Analise de Grafos (Projeto 1 - GraphCCO) ===\n");
    Grafo* grafo_rede = constroi_grafo_da_arvore(raiz, total_nos);
    
    executar_bfs(grafo_rede, raiz->id);
    executar_dfs(grafo_rede, raiz->id);
    analisar_topologia(grafo_rede, raiz->id);
    
    // Cálculo do Grau Médio da rede
    double soma_graus = 0;
    for(int i = 0; i < total_nos; i++) {
        soma_graus += grafo_rede->nos[i].grau;
    }
    double grau_medio = soma_graus / total_nos;

    // ── 5. Exportação Obrigatória ────────────────────
    exporta_csv_fisico(raiz, "arvore.csv");
    exporta_vtk(raiz,        "arvore.vtk");
    
    FILE *f_metricas = fopen("metricas.txt", "w");
    if (f_metricas) {
        fprintf(f_metricas, "Nterm, Nseg, comprimento_total, volume_total, raio_raiz, raio_medio, profundidade_maxima, tempo_execucao, ordem_strahler, grau_medio\n");
        fprintf(f_metricas, "%d, %d, %.6f, %.6e, %.6e, %.6e, %d, %.4f, %d, %.2f\n", 
                terminais, total_segs, comprimento_total(raiz), funcaoCustoVolume(raiz), 
                raiz->raio, raio_medio, grafo_rede->max_profundidade, elapsed, grafo_rede->nos[raiz->id].ordem_strahler, grau_medio);
        fclose(f_metricas);
        printf("\nExportado: 'metricas.txt'\n");
    }

    libera_grafo(grafo_rede);
    libera_arvore(raiz);
    
    printf("\nProcesso concluido com sucesso!\n");
    return 0;
}