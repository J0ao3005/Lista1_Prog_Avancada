#ifndef FISICA_H
#define FISICA_H

#include "arvore.h"
#include <math.h>

// ── Parâmetros físicos fixos (unidades SI) ────────────────────────────
#define QPERF  8.33e-6   // Fluxo total de perfusão   [m³/s]
#define PPERF  1.33e4    // Pressão de perfusão       [Pa]
#define PTERM  7.98e3    // Pressão terminal          [Pa]
#define MU     3.6e-3    // Viscosidade sanguínea     [Pa·s]
#define M_PI_F 3.14159265358979323846

// ── Parte A: cálculos por segmento ───────────────────────────────────

// Comprimento do segmento (distância do nó ao seu pai)
double calculaComprimento(ptrNo no);

// Resistência hidráulica: R = 8·µ·l / (π·r⁴)   [Pa·s/m³]
double calculaResistencia(double mu, double comprimento, double raio);

// Volume intravascular: V = π·r²·l              [m³]
double calculaVolume(double comprimento, double raio);

// Soma dos volumes de toda a árvore
double calculaVolumeTotal(ptrNo raiz);

// ── Parte B: terminais distais ────────────────────────────────────────
// Percorre em pós-ordem e preenche qtd_term_distal de cada nó.
// Retorna a contagem do nó atual.
int atualizaQtdTerminaisDistais(ptrNo no);

// ── Parte C: fluxos ──────────────────────────────────────────────────
// Qj = qtd_term_distal(j) × Qterm
void atualizaFluxos(ptrNo no, double Qterm);

// ── Parte D: raios por lei de bifurcação ─────────────────────────────
// rj = Qj^(1/γ)   (com C = 1)
void atualizaRaiosPorFluxo(ptrNo no, double gamma);

// Orquestra a atualização completa na ordem correta:
// terminais → fluxos → raios → comprimento/resistência/volume
void atualizaGeometriaFisica(ptrNo raiz, double Qterm,
                              double gamma, double mu);

// ── Parte E: função custo ─────────────────────────────────────────────
// J = Vtotal = Σ π·r²·l
double funcaoCustoVolume(ptrNo raiz);

#endif