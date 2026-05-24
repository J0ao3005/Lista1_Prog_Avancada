#include "exportacao.h"
#include <stdlib.h>
#include <stdio.h>

/* ── Lista dinâmica de segmentos ── */

void lista_segs_init(ListaSegs *ls) {
    ls->segs = NULL;
    ls->tam  = 0;
    ls->cap  = 0;
}

void lista_segs_add(ListaSegs *ls, Point a, Point b) {
    if (ls->tam == ls->cap) {
        ls->cap = ls->cap ? ls->cap * 2 : 16;
        ls->segs = realloc(ls->segs, ls->cap * sizeof(Segment));
        if (!ls->segs) { fprintf(stderr, "Erro: realloc falhou.\n"); exit(1); }
    }
    ls->segs[ls->tam].a = a;
    ls->segs[ls->tam].b = b;
    ls->tam++;
}

void lista_segs_free(ListaSegs *ls) {
    free(ls->segs);
    ls->segs = NULL;
    ls->tam  = ls->cap = 0;
}

/* ── CSV ── */

void exportar_csv(const char *nome, ListaSegs *ls) {
    FILE *f = fopen(nome, "w");
    if (!f) { fprintf(stderr, "Erro ao criar %s\n", nome); return; }

    fprintf(f, "x1,y1,x2,y2\n");
    for (int i = 0; i < ls->tam; i++) {
        fprintf(f, "%.6f,%.6f,%.6f,%.6f\n",
                ls->segs[i].a.x, ls->segs[i].a.y,
                ls->segs[i].b.x, ls->segs[i].b.y);
    }
    fclose(f);
    printf("Exportado: %s  (%d segmentos)\n", nome, ls->tam);
}

/* ── VTK (PolyData ASCII) ── */

void exportar_vtk(const char *nome, ListaSegs *ls) {
    FILE *f = fopen(nome, "w");
    if (!f) { fprintf(stderr, "Erro ao criar %s\n", nome); return; }

    int n = ls->tam;
    fprintf(f, "# vtk DataFile Version 3.0\n");
    fprintf(f, "MiniCCO-0\n");
    fprintf(f, "ASCII\n");
    fprintf(f, "DATASET POLYDATA\n");
    fprintf(f, "POINTS %d float\n", n * 2);
    for (int i = 0; i < n; i++) {
        fprintf(f, "%.6f %.6f 0.0\n", ls->segs[i].a.x, ls->segs[i].a.y);
        fprintf(f, "%.6f %.6f 0.0\n", ls->segs[i].b.x, ls->segs[i].b.y);
    }
    fprintf(f, "LINES %d %d\n", n, n * 3);
    for (int i = 0; i < n; i++)
        fprintf(f, "2 %d %d\n", i * 2, i * 2 + 1);

    fclose(f);
    printf("Exportado: %s  (%d segmentos)\n", nome, n);
}