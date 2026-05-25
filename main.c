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
    double epsilon = 0.01 * R;   // distância mínima entre segmentos

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

    Point origem    = {0.0, 0.0};
    ptrNo raiz      = cria_no(origem, NULL, 0);
    int   proximo_id = 1;
    int   rejeitados = 0;
    int   inseridos  = 0;

    // ── Loop principal ──────────────────────────────────────────────
    for (int i = 0; i < Nterm; i++) {

        // 1. Gera ponto aleatório dentro do domínio
        Point novo = gera_ponto_aleatorio(R);

        // 2. Seleciona o melhor nó da árvore para conectar
        ptrNo melhor = seleciona_melhor(novo, raiz, R, epsilon, &rejeitados);

        // 3. Se encontrou candidato válido, insere na árvore
        if (melhor == NULL) {
            // Nenhuma conexão válida encontrada para este ponto
            continue;
        }

        ptrNo novo_no = cria_no(novo, melhor, proximo_id++);
        if (novo_no == NULL) continue; // falha de alocação

        insere_filho(melhor, novo_no);
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
