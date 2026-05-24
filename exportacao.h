#ifndef EXPORTACAO_H
#define EXPORTACAO_H

#include "arvore.h"

/* Lista dinâmica de segmentos gerados durante o crescimento */
typedef struct {
    Segment *segs;
    int      tam;
    int      cap;
} ListaSegs;

void lista_segs_init(ListaSegs *ls);
void lista_segs_add(ListaSegs *ls, Point a, Point b);
void lista_segs_free(ListaSegs *ls);

/* Exportação */
void exportar_csv(const char *nome, ListaSegs *ls);
void exportar_vtk(const char *nome, ListaSegs *ls);

#endif