#include "otimizacao.h"
#include <float.h>

// ═══════════════════════════════════════════════════════════════════════
// PARTE F — Coordenadas baricêntricas e teste de ponto em triângulo
// ═══════════════════════════════════════════════════════════════════════

// X = α·A + β·B + λ·C
// Qualquer ponto dentro do triângulo ABC pode ser representado assim,
// com α + β + λ = 1 e α, β, λ ≥ 0.
Point pontoBaricentrico(Point A, Point B, Point C,
                        double alpha, double beta, double lambda) {
    Point X;
    X.x = alpha * A.x + beta * B.x + lambda * C.x;
    X.y = alpha * A.y + beta * B.y + lambda * C.y;
    return X;
}

// P está dentro de ABC se os três produtos vetoriais
// orient(A,B,P), orient(B,C,P), orient(C,A,P) tiverem o mesmo sinal.
// Isso equivale a P estar do mesmo lado de cada aresta.
int pontoDentroTriangulo(Point A, Point B, Point C, Point P) {
    double o1 = orient(A, B, P);
    double o2 = orient(B, C, P);
    double o3 = orient(C, A, P);
    return (o1 >= -EPSILON && o2 >= -EPSILON && o3 >= -EPSILON) ||
           (o1 <=  EPSILON && o2 <=  EPSILON && o3 <=  EPSILON);
}

// ── Auxiliares internos ───────────────────────────────────────────────

static int ponto_igual(Point a, Point b) {
    return fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON;
}

// Verifica se o segmento 's' é geometricamente válido frente a segs[].
// Ignora segmentos que compartilham 'ponto_fixo' (a extremidade fixa de 's'),
// pois vizinhos diretos sempre compartilham um vértice.
static int sem_intersecao(Segment s, Point ponto_fixo,
                           Segment *segs, int n_segs, double epsilon) {
    for (int k = 0; k < n_segs; k++) {
        if (ponto_igual(segs[k].a, ponto_fixo) ||
            ponto_igual(segs[k].b, ponto_fixo))
            continue;
        if (intersecta(s, segs[k]))                        return 0;
        if (dist_segmento_segmento(s, segs[k]) < epsilon)  return 0;
    }
    return 1;
}

// Para o segmento X→C, nenhum ponto fixo existe na árvore — C é novo.
// Apenas checamos interseção direta.
static int sem_intersecao_livre(Segment s, Segment *segs, int n_segs, double epsilon) {
    for (int k = 0; k < n_segs; k++) {
        if (intersecta(s, segs[k]))                        return 0;
        if (dist_segmento_segmento(s, segs[k]) < epsilon)  return 0;
    }
    return 1;
}

// ═══════════════════════════════════════════════════════════════════════
// PARTE F/G — Busca em grade dentro do triângulo
// ═══════════════════════════════════════════════════════════════════════
//
// Eficiência: como α e β variam mas a topologia (e portanto os raios)
// são FIXOS para um dado candidato, apenas os 3 comprimentos mudam com X.
// Logo o custo é calculado em O(1) por posição:
//
//   custo(X) = vol_base
//            + π·r_bif² · d(A,X)
//            + π·r_filho² · d(X,B)
//            + π·r_novo² · d(X,C)
//
// O check de interseção é O(n_segs) por posição — só feito quando o custo
// bate o mínimo atual (poda por custo antes de checar geometria).

Point otimizaBifurcacaoPorGrade(ptrNo bifurc, ptrNo filho_antigo, ptrNo novo_terminal,
                                 Point A, Point B, Point C,
                                 int M, double vol_base, double epsilon,
                                 Segment *segs, int n_segs,
                                 double *melhorCusto) {

    double r_bif   = bifurc->raio;
    double r_filho = filho_antigo->raio;
    double r_novo  = novo_terminal->raio;

    // Fallback: centroide do triângulo caso nenhuma posição seja válida
    Point melhor_X   = pontoBaricentrico(A, B, C, 1.0/3.0, 1.0/3.0, 1.0/3.0);
    *melhorCusto     = DBL_MAX;

    for (int i = 0; i <= M; i++) {
        for (int j = 0; j <= M - i; j++) {
            double alpha  = (double)i / M;
            double beta   = (double)j / M;
            double lambda = 1.0 - alpha - beta;

            if (lambda < 0.0) lambda = 0.0;

            Point X = pontoBaricentrico(A, B, C, alpha, beta, lambda);

            // Custo desta posição X (O(1))
            double vol_AX = M_PI_F * r_bif   * r_bif   * distancia(A, X);
            double vol_XB = M_PI_F * r_filho  * r_filho  * distancia(X, B);
            double vol_XC = M_PI_F * r_novo   * r_novo   * distancia(X, C);
            double custo  = vol_base + vol_AX + vol_XB + vol_XC;

            // Poda: só verifica geometria se o custo for melhor que o atual
            if (custo >= *melhorCusto) continue;

            // Verificação geométrica dos 3 segmentos candidatos
            Segment s_AX = {A, X};
            Segment s_XB = {X, B};
            Segment s_XC = {X, C};

            // A→X: ignora segmentos que passam por A
            if (!sem_intersecao(s_AX, A, segs, n_segs, epsilon)) continue;
            // X→B: ignora segmentos que passam por B
            if (!sem_intersecao(s_XB, B, segs, n_segs, epsilon)) continue;
            // X→C: C é ponto novo, checa tudo
            if (!sem_intersecao_livre(s_XC, segs, n_segs, epsilon)) continue;

            *melhorCusto = custo;
            melhor_X     = X;
        }
    }

    return melhor_X;
}