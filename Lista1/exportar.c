#include "exportar.h"
#include <stdio.h>

// Arquivo global para o callback do percurso
static FILE *arq_saida;

// Callback: escreve o segmento pai → este nó
static void escreve_segmento(ptrNo no) {
    if (no->pai == NULL) return; // raiz não tem segmento
    fprintf(arq_saida, "%.6f,%.6f,%.6f,%.6f\n",
            no->pai->p.x, no->pai->p.y,
            no->p.x,      no->p.y);
}

void exporta_csv(ptrNo raiz, const char *filename) {
    arq_saida = fopen(filename, "w");
    if (arq_saida == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s' para escrita.\n", filename);
        return;
    }

    // Cabeçalho
    fprintf(arq_saida, "x1,y1,x2,y2\n");

    // Percorre toda a árvore escrevendo cada segmento
    percurso_nos(raiz, escreve_segmento);

    fclose(arq_saida);
    printf("Arvore exportada para '%s'\n", filename);
}
