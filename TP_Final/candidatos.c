#include "candidatos.h"
#include "fisica.h"
#include "otimizacao.h"
#include <stdlib.h>
#include <float.h>

// ─────────────────────────────────────────────
// Geração de ponto aleatório (método de rejeição)
// ─────────────────────────────────────────────
Point gera_ponto_aleatorio(double R) {
    Point p;
    do {
        p.x = ((double)rand() / RAND_MAX) * 2.0 * R - R;
        p.y = ((double)rand() / RAND_MAX) * 2.0 * R - R;
    } while (!dentro_dominio(p, R));
    return p;
}

// ─────────────────────────────────────────────
// Coleta nós não-raiz (cada um representa um segmento pai→filho)
// ─────────────────────────────────────────────
void coleta_filhos(ptrNo raiz, ptrNo *lista, int *total) {
    if (raiz == NULL) return;
    if (raiz->pai != NULL)          // raiz não tem pai, não define segmento
        lista[(*total)++] = raiz;
    coleta_filhos(raiz->esq, lista, total);
    coleta_filhos(raiz->dir, lista, total);
}

// ─────────────────────────────────────────────
// Coleta segmentos (usado para checar interseções)
// ─────────────────────────────────────────────
void coleta_segmentos(ptrNo raiz, Segment *lista, int *total) {
    if (raiz == NULL) return;
    if (raiz->pai != NULL) {
        lista[*total].a = raiz->pai->p;
        lista[*total].b = raiz->p;
        (*total)++;
    }
    coleta_segmentos(raiz->esq, lista, total);
    coleta_segmentos(raiz->dir, lista, total);
}

// ─────────────────────────────────────────────
// Seleção do melhor segmento para bifurcar (Partes F/G)
// ─────────────────────────────────────────────
ptrNo seleciona_melhor(Point novo, ptrNo raiz,
                       double R, double epsilon,
                       int *testadas, int *rejeitados,
                       double Qterm, double gamma, double mu,
                       int M_grade, Point *best_X_out) {

    // Novo terminal precisa estar dentro do domínio para ser candidato
    if (!dentro_dominio(novo, R)) {
        return NULL;
    }

    // Segmentos originais da árvore (estado "congelado", antes de
    // qualquer inserção temporária) — usados para checagem de interseção
    Segment segs_originais[MAX_NOS];
    int total_segs_orig = 0;
    coleta_segmentos(raiz, segs_originais, &total_segs_orig);

    // Cada filho define um segmento candidato: pai->p → filho->p
    ptrNo filhos[MAX_NOS];
    int total_filhos = 0;
    coleta_filhos(raiz, filhos, &total_filhos);

    ptrNo melhor_filho        = NULL;
    Point melhor_X            = {0.0, 0.0};
    double melhor_custo_geral = DBL_MAX;

    // IDs negativos para os nós temporários: nunca colidem com os IDs
    // reais (atribuídos por quem chama esta função, sempre >= 0), e os
    // nós são liberados antes de retornar, então não sobra rastro deles.
    int id_temp = -1;

    for (int i = 0; i < total_filhos; i++) {
        ptrNo filho = filhos[i];
        ptrNo pai   = filho->pai;

        Point A = pai->p;    // vértice proximal do segmento antigo
        Point B = filho->p;  // vértice distal do segmento antigo
        Point C = novo;      // novo ponto terminal

        // Ponto médio: posição inicial (placeholder) da bifurcação
        // temporária. Sua posição exata aqui não importa para o
        // resultado: raio/fluxo dependem só da topologia, e a busca em
        // grade a seguir vai testar todas as posições dentro de ABC.
        Point mid = { (A.x + B.x) / 2.0, (A.y + B.y) / 2.0 };
        if (!dentro_dominio(mid, R)) {
            (*rejeitados)++;
            continue;
        }

        (*testadas)++;

        // Monta a lista de segmentos "fixos" (resto da árvore, exceto o
        // segmento pai->filho que deixará de existir após a bifurcação)
        Segment segs_fixos[MAX_NOS];
        int n_fixos = 0;
        for (int k = 0; k < total_segs_orig; k++) {
            if (fabs(segs_originais[k].a.x - A.x) < EPSILON &&
                fabs(segs_originais[k].a.y - A.y) < EPSILON &&
                fabs(segs_originais[k].b.x - B.x) < EPSILON &&
                fabs(segs_originais[k].b.y - B.y) < EPSILON)
                continue;
            segs_fixos[n_fixos++] = segs_originais[k];
        }

        // Insere a bifurcação temporária no ponto médio
        ptrNo bifurc  = cria_no(mid,  NULL, id_temp--);
        ptrNo novo_no = cria_no(novo, NULL, id_temp--);
        if (!bifurc || !novo_no) {
            free(bifurc); free(novo_no);
            (*rejeitados)++;
            continue;
        }
        insere_bifurcacao(pai, filho, bifurc, novo_no);

        // Atualiza toda a física da árvore com a bifurcação temporária.
        // qtd_term_distal/fluxo/raio de bifurc, filho e novo_no ficam
        // corretos e FIXOS — não dependem de onde exatamente X está.
        atualizaGeometriaFisica(raiz, Qterm, gamma, mu);

        double vol_com_temp = funcaoCustoVolume(raiz);
        // Volume do resto da árvore, excluindo os 3 segmentos afetados
        // (pai->bifurc, bifurc->filho, bifurc->novo_no)
        double vol_base = vol_com_temp - bifurc->volume
                                        - filho->volume
                                        - novo_no->volume;

        double melhor_custo_local;
        Point X = otimizaBifurcacaoPorGrade(bifurc, filho, novo_no,
                                             A, B, C, M_grade,
                                             vol_base, epsilon,
                                             segs_fixos, n_fixos,
                                             &melhor_custo_local);

        // Desfaz a inserção temporária e restaura a física original
        // antes de avaliar o próximo candidato
        remove_bifurcacao(pai, filho, bifurc, novo_no);
        atualizaGeometriaFisica(raiz, Qterm, gamma, mu);

        // otimizaBifurcacaoPorGrade nunca atualiza *melhorCusto se
        // nenhuma posição da grade passou nas checagens geométricas —
        // nesse caso o candidato inteiro é geometricamente inválido.
        if (melhor_custo_local >= DBL_MAX) {
            (*rejeitados)++;
            continue;
        }

        if (melhor_custo_local < melhor_custo_geral) {
            melhor_custo_geral = melhor_custo_local;
            melhor_filho       = filho;
            melhor_X           = X;
        }
    }

    if (melhor_filho != NULL) {
        *best_X_out = melhor_X;
    }
    return melhor_filho;
}