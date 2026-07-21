#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "candidatos.h"
#include "exportar.h"
#include "fisica.h"

// ── Cálculo de raio médio via percurso ───────────────────────────────
static double soma_raios_g;
static int    conta_raios_g;
static void acumula_raio(ptrNo no) {
    if (no->pai == NULL) return;   // raiz não tem segmento
    soma_raios_g += no->raio;
    conta_raios_g++;
}

int main(int argc, char *argv[]) {

    // ── Argumentos ───────────────────────────────────────────────────
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <Nterm> <R> <gamma> <M>\n", argv[0]);
        fprintf(stderr, "  Nterm : terminais a inserir\n");
        fprintf(stderr, "  R     : raio do domínio [m]\n");
        fprintf(stderr, "  gamma : expoente da lei de bifurcação\n");
        fprintf(stderr, "  M     : resolução da busca em grade\n");
        return 1;
    }

    int    Nterm   = atoi(argv[1]);
    double R       = atof(argv[2]);
    double gamma   = atof(argv[3]);
    int    M_grade = atoi(argv[4]);
    double epsilon = 0.05 * R;
    double Qterm   = QPERF / Nterm;  // fluxo por terminal

    if (Nterm <= 0 || R <= 0.0 || gamma <= 0.0 || M_grade <= 0) {
        fprintf(stderr, "Erro: parâmetros inválidos.\n");
        return 1;
    }

    printf("=== MiniCCO-1 ===\n");
    printf("Nterm   : %d\n",   Nterm);
    printf("R       : %.4f m\n", R);
    printf("gamma   : %.2f\n", gamma);
    printf("M       : %d\n",   M_grade);
    printf("Qterm   : %.4e m3/s\n", Qterm);
    printf("Epsilon : %.4e m\n\n",  epsilon);

    // ── Inicialização ─────────────────────────────────────────────────
    srand((unsigned int)time(NULL));
    clock_t t_inicio = clock();

    Point raiz_pt  = {0.0, R};   // topo do círculo
    ptrNo raiz     = cria_no(raiz_pt, NULL, 0);
    int   proximo_id = 1;
    int   inseridos  = 0;
    int   testadas   = 0;
    int   rejeitados = 0;

    // ── Loop principal ────────────────────────────────────────────────
    for (int i = 0; i < Nterm; i++) {
        Point novo = gera_ponto_aleatorio(R);

        // ── Caso especial: primeira inserção (sem segmentos ainda) ────
        if (raiz->esq == NULL && raiz->dir == NULL) {
            if (!dentro_dominio(novo, R)) continue;
            ptrNo novo_no = cria_no(novo, raiz, proximo_id++);
            if (!novo_no) continue;
            insere_filho(raiz, novo_no);
            inseridos++;
            atualizaGeometriaFisica(raiz, Qterm, gamma, MU);
            continue;
        }

        // ── Caso geral: seleciona melhor segmento e posição ótima ─────
        Point best_X;
        ptrNo filho = seleciona_melhor(novo, raiz, R, epsilon,
                                       &testadas, &rejeitados,
                                       Qterm, gamma, MU,
                                       M_grade, &best_X);
        if (filho == NULL) continue;

        // Insere bifurcação na posição ótima encontrada
        ptrNo bifurc  = cria_no(best_X, filho->pai, proximo_id++);
        ptrNo novo_no = cria_no(novo,   bifurc,     proximo_id++);
        if (!bifurc || !novo_no) { free(bifurc); free(novo_no); continue; }

        insere_bifurcacao(filho->pai, filho, bifurc, novo_no);
        inseridos++;

        // Atualiza física definitiva após inserção real
        atualizaGeometriaFisica(raiz, Qterm, gamma, MU);
    }

    clock_t t_fim  = clock();
    double  elapsed = (double)(t_fim - t_inicio) / CLOCKS_PER_SEC;

    // ── Estatísticas finais ───────────────────────────────────────────
    int total_nos  = proximo_id;
    int total_segs = total_nos - 1;
    int terminais  = conta_folhas(raiz);

    soma_raios_g = 0.0; conta_raios_g = 0;
    percurso_nos(raiz, acumula_raio);
    double raio_medio = (conta_raios_g > 0) ? soma_raios_g / conta_raios_g : 0.0;

    printf("=== Resultados ===\n");
    printf("Nos totais          : %d\n",    total_nos);
    printf("Segmentos totais    : %d\n",    total_segs);
    printf("Terminais           : %d\n",    terminais);
    printf("Comprimento total   : %.6f m\n", comprimento_total(raiz));
    printf("Volume intravas.    : %.6e m3\n", funcaoCustoVolume(raiz));
    printf("Raio da raiz        : %.6e m\n",  raiz->raio);
    printf("Raio medio          : %.6e m\n",  raio_medio);
    printf("Conexoes testadas   : %d\n",    testadas);
    printf("Conexoes rejeitadas : %d\n",    rejeitados);
    printf("Tempo de execucao   : %.4f s\n\n", elapsed);

    // ── Exportação ────────────────────────────────────────────────────
    exporta_csv_fisico(raiz, "arvore.csv");
    exporta_vtk(raiz,        "arvore.vtk");

    libera_arvore(raiz);
    return 0;
}