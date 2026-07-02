#include "arvore.h"

ptrNo cria_no(Point p, ptrNo pai, int id) {
    No *a = malloc(sizeof(No));
    if (a == NULL) {
        fprintf(stderr, "Erro: falha ao alocar no.\n");
        return NULL;
    }
    a->esq = NULL;
    a->dir = NULL;
    a->pai = pai;
    a->p   = p;
    a->id  = id;
    return a;
}

bool insere_filho(ptrNo pai, ptrNo filho) {
    if (pai->esq == NULL) { pai->esq = filho; return true; }
    if (pai->dir == NULL) { pai->dir = filho; return true; }
    return false;
}

// ─────────────────────────────────────────────────────────────────────
// Inserção por bifurcação
//
// Antes:   pai ──────────────── filho_antigo
//
// Depois:  pai ───── bifurc ─── filho_antigo  (esq)
//                       │
//                   novo_terminal              (dir)
// ─────────────────────────────────────────────────────────────────────
void insere_bifurcacao(ptrNo pai, ptrNo filho_antigo,
                       ptrNo bifurc, ptrNo novo_terminal) {

    // 1. Substitui filho_antigo por bifurc nos filhos do pai
    if (pai->esq == filho_antigo)
        pai->esq = bifurc;
    else
        pai->dir = bifurc;

    // 2. Liga bifurc ao pai
    bifurc->pai = pai;

    // 3. Bifurc recebe os dois ramos
    bifurc->esq = filho_antigo;
    bifurc->dir = novo_terminal;

    // 4. Atualiza o pai dos dois novos filhos
    filho_antigo->pai  = bifurc;
    novo_terminal->pai = bifurc;
}

void percurso_nos(ptrNo raiz, void (*callback)(ptrNo)) {
    if (raiz == NULL) return;
    callback(raiz);
    percurso_nos(raiz->esq, callback);
    percurso_nos(raiz->dir, callback);
}

int conta_folhas(ptrNo raiz) {
    if (raiz == NULL) return 0;
    if (raiz->esq == NULL && raiz->dir == NULL) return 1;
    return conta_folhas(raiz->esq) + conta_folhas(raiz->dir);
}

double comprimento_total(ptrNo raiz) {
    if (raiz == NULL) return 0;
    double comp = 0;
    if (raiz->pai != NULL)
        comp = distancia(raiz->p, raiz->pai->p);
    return comp + comprimento_total(raiz->esq) + comprimento_total(raiz->dir);
}

void libera_arvore(ptrNo raiz) {
    if (raiz == NULL) return;
    libera_arvore(raiz->esq);
    libera_arvore(raiz->dir);
    free(raiz);
}
