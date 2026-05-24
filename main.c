/*
 * MiniCCO-0 — main.c
 * Uso: ./programa <Nterm> <R>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "geometria.h"
#include "arvore.h"
#include "exportacao.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ── Parâmetros ── */
#define DIST_MIN_SEG  0.3   /* distância mínima do novo seg aos existentes */
#define ALPHA         1.0   /* peso da distância na função custo           */
#define BETA          0.5   /* peso do ângulo na função custo              */
#define MAX_TENT      300   /* tentativas máximas por ponto terminal       */

/* ── Lista plana de nós (para iterar sem recursão) ── */
typedef struct {
    ptrNo *nos;
    int    tam;
    int    cap;
} ListaNos;

static void lista_nos_init(ListaNos *ln) {
    ln->nos = NULL; ln->tam = ln->cap = 0;
}
static void lista_nos_add(ListaNos *ln, ptrNo n) {
    if (ln->tam == ln->cap) {
        ln->cap = ln->cap ? ln->cap * 2 : 16;
        ln->nos = realloc(ln->nos, ln->cap * sizeof(ptrNo));
        if (!ln->nos) { fprintf(stderr, "Erro: realloc.\n"); exit(1); }
    }
    ln->nos[ln->tam++] = n;
}

/* ── Gera ponto aleatório dentro do domínio circular ── */
static Point gerar_ponto(double R) {
    Point p;
    do {
        p.x = ((double)rand() / RAND_MAX) * 2.0 * R - R;
        p.y = ((double)rand() / RAND_MAX) * 2.0 * R - R;
    } while (!dentro_dominio(p, R));
    return p;
}

/*
 * Verifica se conectar 'novo' ao nó 'alvo' é válido:
 *   - sem interseção com segmentos existentes
 *   - distância mínima respeitada
 * Segmentos adjacentes ao nó alvo são ignorados.
 */
static bool conexao_valida(Point novo, ptrNo alvo, ListaSegs *ls) {
    Segment ns = { alvo->p, novo };

    for (int i = 0; i < ls->tam; i++) {
        Segment s = ls->segs[i];

        /* ignora segmentos que tocam o nó alvo */
        bool adj = (fabs(s.a.x - alvo->p.x) < EPSILON && fabs(s.a.y - alvo->p.y) < EPSILON) ||
                   (fabs(s.b.x - alvo->p.x) < EPSILON && fabs(s.b.y - alvo->p.y) < EPSILON);
        if (adj) continue;

        if (intersecta(ns, s))                    return false;
        if (dist_ponto_segmento(novo, s) < DIST_MIN_SEG) return false;
    }
    return true;
}

/*
 * Função custo estendida (seção 6.2 do enunciado):
 *   J = alpha * (d / dmax) + beta * (theta / pi)
 */
static double custo(Point novo, ptrNo alvo, double dmax) {
    double d      = distancia(novo, alvo->p);
    double d_norm = (dmax > EPSILON) ? d / dmax : d;

    double theta_norm = 0.0;
    if (alvo->pai) {
        double ux = alvo->p.x - alvo->pai->p.x;
        double uy = alvo->p.y - alvo->pai->p.y;
        double vx = novo.x   - alvo->p.x;
        double vy = novo.y   - alvo->p.y;
        double nu = sqrt(ux*ux + uy*uy);
        double nv = sqrt(vx*vx + vy*vy);
        if (nu > EPSILON && nv > EPSILON) {
            double cosT = (ux*vx + uy*vy) / (nu * nv);
            if (cosT >  1.0) cosT =  1.0;
            if (cosT < -1.0) cosT = -1.0;
            theta_norm = acos(cosT) / M_PI;
        }
    }

    return ALPHA * d_norm + BETA * theta_norm;
}

/*
 * Percorre todos os nós, calcula dmax e retorna o nó de menor custo
 * cuja conexão seja válida. Conta rejeições.
 */
static ptrNo melhor_candidato(Point novo, ListaNos *ln,
                               ListaSegs *ls, int *rejeitadas) {
    /* dmax para normalização */
    double dmax = 0.0;
    for (int i = 0; i < ln->tam; i++) {
        double d = distancia(novo, ln->nos[i]->p);
        if (d > dmax) dmax = d;
    }

    ptrNo  melhor     = NULL;
    double menor_custo = 1e18;

    for (int i = 0; i < ln->tam; i++) {
        if (!conexao_valida(novo, ln->nos[i], ls)) {
            (*rejeitadas)++;
            continue;
        }
        double c = custo(novo, ln->nos[i], dmax);
        if (c < menor_custo) {
            menor_custo = c;
            melhor      = ln->nos[i];
        }
    }
    return melhor;
}

/* ─────────────────────────────────────────────
 *  MAIN
 * ───────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <Nterm> <R>\n", argv[0]);
        return 1;
    }

    int    Nterm = atoi(argv[1]);
    double R     = atof(argv[2]);

    if (Nterm <= 0 || R <= 0.0) {
        fprintf(stderr, "Erro: Nterm e R devem ser positivos.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    printf("╔══════════════════════════════════════╗\n");
    printf("║         MiniCCO-0  2026.1            ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  Nterm = %-5d   R = %-8.2f        ║\n", Nterm, R);
    printf("╚══════════════════════════════════════╝\n\n");

    /* estruturas */
    ListaNos  ln;  lista_nos_init(&ln);
    ListaSegs ls;  lista_segs_init(&ls);

    int id         = 0;
    int rejeitadas = 0;
    int inseridos  = 0;

    /* 1. Raiz na origem */
    Point origem  = {0.0, 0.0};
    ptrNo raiz    = cria_no(origem, NULL, id++);
    lista_nos_add(&ln, raiz);

    /* 2. Crescimento iterativo */
    for (int iter = 0; iter < Nterm; iter++) {
        Point novo;
        ptrNo pai = NULL;
        int   tent = 0;

        while (tent < MAX_TENT) {
            novo = gerar_ponto(R);
            pai  = melhor_candidato(novo, &ln, &ls, &rejeitadas);
            if (pai) break;
            tent++;
        }

        if (!pai) {
            fprintf(stderr, "  Aviso: ponto %d descartado (%d tentativas).\n",
                    iter + 1, MAX_TENT);
            continue;
        }

        ptrNo filho = cria_no(novo, pai, id++);
        if (!insere_filho(pai, filho)) {
            /* pai já tem dois filhos — libera e ignora este ponto */
            free(filho);
            continue;
        }

        lista_segs_add(&ls, pai->p, novo);
        lista_nos_add(&ln, filho);
        inseridos++;

        /* progresso */
        int passo = Nterm / 10;
        if (passo < 1) passo = 1;
        if ((iter + 1) % passo == 0 || iter == Nterm - 1)
            printf("  [%3d/%3d] nós na árvore: %d\n", iter+1, Nterm, (int)ln.tam);
    }

    /* 3. Estatísticas */
    int    folhas = conta_folhas(raiz);
    double comp   = comprimento_total(raiz);

    printf("\n══════════════ RESULTADOS ══════════════\n");
    printf("  Nós totais           : %d\n",   (int)ln.tam);
    printf("  Folhas               : %d\n",   folhas);
    printf("  Comprimento total    : %.4f\n", comp);
    printf("  Conexões rejeitadas  : %d\n",   rejeitadas);
    printf("════════════════════════════════════════\n\n");

    /* 4. Exportação */
    exportar_csv("arvore.csv", &ls);
    exportar_vtk("arvore.vtk", &ls);

    /* 5. Liberar memória */
    libera_arvore(raiz);
    free(ln.nos);
    lista_segs_free(&ls);

    return 0;
}