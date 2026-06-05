#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "arvore.h"
#include "candidatos.h"
#include "exportar.h"

int main(int argc, char *argv[]) {

    // ── Leitura dos argumentos ──────────────────────────────────────
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <Nterm> <R>\n", argv[0]);
        fprintf(stderr, "  Nterm: numero de pontos terminais\n");
        fprintf(stderr, "  R    : raio do dominio circular\n");
        return 1;
    }

    int    Nterm   = atoi(argv[1]);
    double R       = atof(argv[2]);
    double epsilon = 0.01 * R;

    if (Nterm <= 0 || R <= 0.0) {
        fprintf(stderr, "Erro: Nterm e R devem ser positivos.\n");
        return 1;
    }

    printf("=== MiniCCO-0 ===\n");
    printf("Nterm   : %d\n", Nterm);
    printf("R       : %.2f\n", R);
    printf("Epsilon : %.4f\n\n", epsilon);

    // ── Inicialização ───────────────────────────────────────────────
    srand((unsigned int)time(NULL));

    // Raiz no ponto mais alto do círculo
    Point raiz_pt  = {0.0, R};
    ptrNo raiz     = cria_no(raiz_pt, NULL, 0);
    int   proximo_id = 1;
    int   rejeitados = 0;
    int   inseridos  = 0;

    // ── Loop principal ──────────────────────────────────────────────
    for (int i = 0; i < Nterm; i++) {

        Point novo = gera_ponto_aleatorio(R);

        // ── Caso especial: árvore só tem a raiz (sem segmentos ainda)
        if (raiz->esq == NULL && raiz->dir == NULL) {
            if (!dentro_dominio(novo, R)) continue;

            ptrNo novo_no = cria_no(novo, raiz, proximo_id++);
            if (novo_no == NULL) continue;

            insere_filho(raiz, novo_no);
            inseridos++;
            continue;
        }

        // ── Caso normal: encontra o melhor segmento para bifurcar
        Point mid;
        ptrNo filho = seleciona_melhor(novo, raiz, R, epsilon,
                                       &rejeitados, &mid);

        if (filho == NULL) continue; // nenhum segmento válido encontrado

        // Cria nó de bifurcação no ponto médio do segmento escolhido
        ptrNo bifurc  = cria_no(mid,  filho->pai, proximo_id++);
        ptrNo novo_no = cria_no(novo, bifurc,     proximo_id++);

        if (bifurc == NULL || novo_no == NULL) continue;

        insere_bifurcacao(filho->pai, filho, bifurc, novo_no);
        inseridos++;
    }

    // ── Estatísticas finais ─────────────────────────────────────────
    printf("=== Resultados ===\n");
    printf("Nos totais          : %d\n", proximo_id);
    printf("Nos inseridos       : %d\n", inseridos);
    printf("Folhas              : %d\n", conta_folhas(raiz));
    printf("Comprimento total   : %.4f\n", comprimento_total(raiz));
    printf("Conexoes rejeitadas : %d\n\n", rejeitados);

    // ── Exportação ──────────────────────────────────────────────────
    exporta_csv(raiz, "arvore.csv");

    // ── Liberação de memória ────────────────────────────────────────
    libera_arvore(raiz);

    return 0;
}
