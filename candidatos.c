#include "candidatos.h"
#include <stdlib.h>
#include <float.h>

// ─────────────────────────────────────────────
// Geração de ponto aleatório (método de rejeição)
// ─────────────────────────────────────────────
Point gera_ponto_aleatorio(double R) {
    Point p;
    do {
        // Gera x e y no quadrado [-R, R] x [-R, R]
        p.x = ((double)rand() / RAND_MAX) * 2.0 * R - R;
        p.y = ((double)rand() / RAND_MAX) * 2.0 * R - R;
    } while (!dentro_dominio(p, R)); // rejeita se cair fora do círculo
    return p;
}

// ─────────────────────────────────────────────
// Coleta de nós (percurso pré-ordem)
// ─────────────────────────────────────────────
void coleta_nos(ptrNo raiz, ptrNo *lista, int *total) {
    if (raiz == NULL) return;
    lista[(*total)++] = raiz;
    coleta_nos(raiz->esq, lista, total);
    coleta_nos(raiz->dir, lista, total);
}

// ─────────────────────────────────────────────
// Coleta de segmentos (cada nó com seu pai)
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
// Validação de candidato
// ─────────────────────────────────────────────
int valida_candidato(Point novo, ptrNo candidato,
                     Segment *segs, int n_segs,
                     double R, double epsilon) {

    // Restrição 1: ponto deve estar dentro do domínio
    if (!dentro_dominio(novo, R))
        return 0;

    // Restrição 2 e 3: verificar contra todos os segmentos existentes
    Segment novo_seg = {candidato->p, novo};

    for (int i = 0; i < n_segs; i++) {

        // Pula segmentos que compartilham o ponto do candidato
        // (vizinhos diretos — interseção no extremo é esperada)
        if ((fabs(segs[i].a.x - candidato->p.x) < EPSILON &&
             fabs(segs[i].a.y - candidato->p.y) < EPSILON) ||
            (fabs(segs[i].b.x - candidato->p.x) < EPSILON &&
             fabs(segs[i].b.y - candidato->p.y) < EPSILON))
            continue;

        // Restrição 2: sem interseção com segmentos existentes
        if (intersecta(novo_seg, segs[i]))
            return 0;

        // Restrição 3: distância mínima ε do novo ponto a segmentos
        if (dist_ponto_segmento(novo, segs[i]) < epsilon)
            return 0;
    }

    return 1;
}

// ─────────────────────────────────────────────
// Função custo (distância euclidiana básica)
// ─────────────────────────────────────────────
double custo(Point novo, ptrNo candidato) {
    return distancia(novo, candidato->p);
}

// ─────────────────────────────────────────────
// Seleção do melhor candidato
// ─────────────────────────────────────────────
ptrNo seleciona_melhor(Point novo, ptrNo raiz,
                       double R, double epsilon,
                       int *rejeitados) {

    // Coleta todos os nós e segmentos da árvore atual
    ptrNo   lista_nos[MAX_NOS];
    Segment lista_segs[MAX_NOS];
    int total_nos  = 0;
    int total_segs = 0;

    coleta_nos(raiz, lista_nos, &total_nos);
    coleta_segmentos(raiz, lista_segs, &total_segs);

    ptrNo melhor      = NULL;
    double menor_custo = DBL_MAX;

    for (int i = 0; i < total_nos; i++) {

        // Nó já tem dois filhos — árvore binária, sem espaço
        if (lista_nos[i]->esq != NULL && lista_nos[i]->dir != NULL) {
            (*rejeitados)++;
            continue;
        }

        // Valida restrições geométricas
        if (!valida_candidato(novo, lista_nos[i], lista_segs, total_segs, R, epsilon)) {
            (*rejeitados)++;
            continue;
        }

        // Calcula custo e atualiza melhor
        double j = custo(novo, lista_nos[i]);
        if (j < menor_custo) {
            menor_custo = j;
            melhor = lista_nos[i];
        }
    }

    return melhor; // NULL se nenhum candidato válido foi encontrado
}
