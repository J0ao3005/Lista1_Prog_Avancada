#include "exportar.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

static FILE *arq_saida;

// ── CSV simples ───────────────────────────────────────────────────────

static void escreve_segmento_csv(ptrNo no) {
    if (no->pai == NULL) return;
    fprintf(arq_saida, "%.6f,%.6f,%.6f,%.6f\n",
            no->pai->p.x, no->pai->p.y, no->p.x, no->p.y);
}

void exporta_csv(ptrNo raiz, const char *filename) {
    arq_saida = fopen(filename, "w");
    if (!arq_saida) { fprintf(stderr, "Erro ao abrir '%s'\n", filename); return; }
    fprintf(arq_saida, "x1,y1,x2,y2\n");
    percurso_nos(raiz, escreve_segmento_csv);
    fclose(arq_saida);
    printf("Exportado: '%s'\n", filename);
}

// ── CSV físico ────────────────────────────────────────────────────────

static void escreve_segmento_fisico(ptrNo no) {
    int pai_id = (no->pai != NULL) ? no->pai->id : -1;
    double x0  = (no->pai != NULL) ? no->pai->p.x : no->p.x;
    double y0  = (no->pai != NULL) ? no->pai->p.y : no->p.y;

    fprintf(arq_saida,
            "%d,%d,%.6f,%.6f,%.6f,%.6f,%.8e,%.6f,%.8e,%.6f,%.8e\n",
            no->id, pai_id,
            x0, y0, no->p.x, no->p.y,
            no->raio, no->comprimento,
            no->fluxo, no->resistencia, no->volume);
}

void exporta_csv_fisico(ptrNo raiz, const char *filename) {
    arq_saida = fopen(filename, "w");
    if (!arq_saida) { fprintf(stderr, "Erro ao abrir '%s'\n", filename); return; }
    fprintf(arq_saida,
            "id,pai_id,x0,y0,x1,y1,raio,comprimento,fluxo,resistencia,volume\n");
    percurso_nos(raiz, escreve_segmento_fisico);
    fclose(arq_saida);
    printf("Exportado: '%s'\n", filename);
}

// ── VTK ──────────────────────────────────────────────────────────────

static FILE   *arq_vtk;
static int     contagem_nos_vtk;
static int     contagem_segs_vtk;

// Mapa id_do_no -> índice na lista POINTS do VTK. O id de aplicação do
// nó (No.id) NÃO coincide, em geral, com sua posição em percurso_nos:
// bifurcações inseridas depois recebem id maior mas podem aparecer
// ANTES na pré-ordem. Índices de LINES no VTK são posicionais (0-based,
// na ordem em que os pontos foram escritos), então usamos este mapa em
// vez do id bruto do nó.
static int *vtk_indice_por_id;
static int  vtk_indice_min_id;
static int  vtk_proximo_indice;

static void conta_no_vtk(ptrNo no)  { (void)no; contagem_nos_vtk++;  }
static void conta_seg_vtk(ptrNo no) { if (no->pai) contagem_segs_vtk++; }

static void indexa_no_vtk(ptrNo no) {
    vtk_indice_por_id[no->id - vtk_indice_min_id] = vtk_proximo_indice++;
}
static void escreve_ponto_vtk(ptrNo no) {
    fprintf(arq_vtk, "%.6f %.6f 0.0\n", no->p.x, no->p.y);
}
static void escreve_linha_vtk(ptrNo no) {
    if (no->pai) {
        int idx_pai = vtk_indice_por_id[no->pai->id - vtk_indice_min_id];
        int idx_no  = vtk_indice_por_id[no->id      - vtk_indice_min_id];
        fprintf(arq_vtk, "2 %d %d\n", idx_pai, idx_no);
    }
}

static int vtk_id_max;

static void acha_min_id(ptrNo no) {
    if (no->id < vtk_indice_min_id) vtk_indice_min_id = no->id;
}
static void acha_max_id(ptrNo no) {
    if (no->id > vtk_id_max) vtk_id_max = no->id;
}

void exporta_vtk(ptrNo raiz, const char *filename) {
    contagem_nos_vtk = contagem_segs_vtk = 0;
    percurso_nos(raiz, conta_no_vtk);
    percurso_nos(raiz, conta_seg_vtk);

    // Descobre a faixa de ids presentes para dimensionar o mapa id->índice
    vtk_indice_min_id = INT_MAX;
    vtk_id_max        = INT_MIN;
    percurso_nos(raiz, acha_min_id);
    percurso_nos(raiz, acha_max_id);

    int faixa = vtk_id_max - vtk_indice_min_id + 1;
    vtk_indice_por_id = malloc(sizeof(int) * (size_t)faixa);
    if (!vtk_indice_por_id) { fprintf(stderr, "Erro ao alocar mapa de indices VTK\n"); return; }

    vtk_proximo_indice = 0;
    percurso_nos(raiz, indexa_no_vtk);

    arq_vtk = fopen(filename, "w");
    if (!arq_vtk) { fprintf(stderr, "Erro ao abrir '%s'\n", filename); free(vtk_indice_por_id); return; }

    fprintf(arq_vtk, "# vtk DataFile Version 3.0\nMiniCCO-1\nASCII\nDATASET POLYDATA\n");
    fprintf(arq_vtk, "POINTS %d float\n", contagem_nos_vtk);
    percurso_nos(raiz, escreve_ponto_vtk);
    fprintf(arq_vtk, "LINES %d %d\n", contagem_segs_vtk, contagem_segs_vtk * 3);
    percurso_nos(raiz, escreve_linha_vtk);

    fclose(arq_vtk);
    free(vtk_indice_por_id);
    printf("Exportado: '%s'\n", filename);
}