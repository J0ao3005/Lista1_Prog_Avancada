#include "arvore.h"

ptrNo cria_no(Point p, ptrNo pai, int id) {
    No *a = malloc(sizeof(No));
    if (a == NULL) { fprintf(stderr, "Erro: falha ao alocar no.\n"); return NULL; }
    a->esq = NULL; a->dir = NULL; a->pai = pai;
    a->p   = p;    a->id  = id;
    a->raio = 0.0; a->comprimento = 0.0; a->fluxo = 0.0;
    a->resistencia = 0.0; a->volume = 0.0; a->qtd_term_distal = 0;
    return a;
}

bool insere_filho(ptrNo pai, ptrNo filho) {
    if (pai->esq == NULL) { pai->esq = filho; return true; }
    if (pai->dir == NULL) { pai->dir = filho; return true; }
    return false;
}

// Divide o segmento pai→filho_antigo inserindo bifurc no meio.
// Resultado: pai→bifurc→filho_antigo (esq) e bifurc→novo_terminal (dir)
void insere_bifurcacao(ptrNo pai, ptrNo filho_antigo,
                       ptrNo bifurc, ptrNo novo_terminal) {
    if (pai->esq == filho_antigo) pai->esq = bifurc;
    else                          pai->dir = bifurc;

    bifurc->pai       = pai;
    bifurc->esq       = filho_antigo;
    bifurc->dir       = novo_terminal;
    filho_antigo->pai = bifurc;
    novo_terminal->pai= bifurc;
}

// Inverso de insere_bifurcacao: restaura pai→filho_antigo e libera
// os nós temporários bifurc e novo_terminal.
void remove_bifurcacao(ptrNo pai, ptrNo filho_antigo,
                        ptrNo bifurc, ptrNo novo_terminal) {
    // Religa pai diretamente a filho_antigo
    if (pai->esq == bifurc) pai->esq = filho_antigo;
    else                    pai->dir = filho_antigo;

    filho_antigo->pai = pai;

    free(novo_terminal);
    free(bifurc);
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
    if (raiz->pai != NULL) comp = distancia(raiz->p, raiz->pai->p);
    return comp + comprimento_total(raiz->esq) + comprimento_total(raiz->dir);
}

void libera_arvore(ptrNo raiz) {
    if (raiz == NULL) return;
    libera_arvore(raiz->esq);
    libera_arvore(raiz->dir);
    free(raiz);
}