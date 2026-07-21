#ifndef EXPORTAR_H
#define EXPORTAR_H

#include "arvore.h"

// Exporta segmentos no formato CSV simples (compatível com MiniCCO-0)
void exporta_csv(ptrNo raiz, const char *filename);

// Exporta CSV com campos físicos completos:
// id, pai_id, x0, y0, x1, y1, raio, comprimento, fluxo, resistencia, volume
void exporta_csv_fisico(ptrNo raiz, const char *filename);

// Exporta no formato VTK PolyData ASCII
void exporta_vtk(ptrNo raiz, const char *filename);

#endif