#include "candidatos.h"
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
// Seleção do melhor segmento para bifurcar
// ─────────────────────────────────────────────
ptrNo seleciona_melhor(Point novo, ptrNo raiz,
                       double R, double epsilon,
                       int *rejeitados, Point *mid_out) {

    // Coleta todos os segmentos para checagem de interseção
    Segment segs[MAX_NOS];
    int total_segs = 0;
    coleta_segmentos(raiz, segs, &total_segs);

    // Coleta filhos — cada filho define um segmento candidato
    ptrNo filhos[MAX_NOS];
    int total_filhos = 0;
    coleta_filhos(raiz, filhos, &total_filhos);

    ptrNo melhor_filho = NULL;
    double menor_custo = DBL_MAX;

    for (int i = 0; i < total_filhos; i++) {
        ptrNo filho = filhos[i];
        ptrNo pai   = filho->pai;

        // Ponto médio do segmento pai → filho
        Point mid = {
            (pai->p.x + filho->p.x) / 2.0,
            (pai->p.y + filho->p.y) / 2.0
        };

        // Restrição 1: ponto médio e novo terminal dentro do domínio
        if (!dentro_dominio(mid, R) || !dentro_dominio(novo, R)) {
            (*rejeitados)++;
            continue;
        }

        // Segmento que será criado: mid → novo
        Segment novo_seg = {mid, novo};

        int valido = 1;
        for (int k = 0; k < total_segs; k++) {

            // Pula o segmento que está sendo dividido
            // (ele deixará de existir após a bifurcação)
            if (fabs(segs[k].a.x - pai->p.x)   < EPSILON &&
                fabs(segs[k].a.y - pai->p.y)   < EPSILON &&
                fabs(segs[k].b.x - filho->p.x) < EPSILON &&
                fabs(segs[k].b.y - filho->p.y) < EPSILON)
                continue;

            // Restrição 2: sem interseção com outros segmentos
            if (intersecta(novo_seg, segs[k])) {
                valido = 0;
                break;
            }

            // Restrição 3: distância mínima ε ao novo ponto
            if (dist_ponto_segmento(novo, segs[k]) < epsilon) {
                valido = 0;
                break;
            }
        }

        if (!valido) {
            (*rejeitados)++;
            continue;
        }

        // Custo: distância do ponto médio ao novo terminal
        double j = distancia(mid, novo);
        if (j < menor_custo) {
            menor_custo    = j;
            melhor_filho   = filho;
            *mid_out       = mid;
        }
    }

    return melhor_filho;
}
