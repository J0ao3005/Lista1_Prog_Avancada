#include "fisica.h"

// ═══════════════════════════════════════════════════════════════════════
// PARTE A — Cálculos físicos por segmento
// ═══════════════════════════════════════════════════════════════════════

// Comprimento: distância euclidiana entre o nó e seu pai.
// A raiz não tem pai, então seu comprimento é 0.
double calculaComprimento(ptrNo no) {
    if (no == NULL || no->pai == NULL) return 0.0;
    return distancia(no->p, no->pai->p);
}

// Resistência hidráulica pela Lei de Poiseuille:
//   R = 8·µ·l / (π·r⁴)
// Quanto menor o raio, MUITO maior a resistência (r na 4ª potência).
// Protegemos contra raio zero para evitar divisão por zero.
double calculaResistencia(double mu, double comprimento, double raio) {
    if (raio <= 0.0) return 0.0;
    return (8.0 * mu * comprimento) / (M_PI_F * raio * raio * raio * raio);
}

// Volume intravascular do segmento cilíndrico:
//   V = π·r²·l
double calculaVolume(double comprimento, double raio) {
    return M_PI_F * raio * raio * comprimento;
}

// Soma recursiva dos volumes de toda a árvore.
// Cada nó contribui com o volume do seu próprio segmento (nó → pai).
double calculaVolumeTotal(ptrNo raiz) {
    if (raiz == NULL) return 0.0;
    return raiz->volume
           + calculaVolumeTotal(raiz->esq)
           + calculaVolumeTotal(raiz->dir);
}

// ═══════════════════════════════════════════════════════════════════════
// PARTE B — Contagem de terminais distais (pós-ordem)
// ═══════════════════════════════════════════════════════════════════════

// Percorre a árvore em pós-ordem (filhos antes do pai).
// Um nó folha (sem filhos) tem qtd_term_distal = 1.
// Um nó interno soma as contagens de seus dois filhos.
// Esse valor é usado depois para distribuir o fluxo proporcionalmente.
int atualizaQtdTerminaisDistais(ptrNo no) {
    if (no == NULL) return 0;

    if (no->esq == NULL && no->dir == NULL) {
        // Nó folha: é ele próprio um terminal
        no->qtd_term_distal = 1;
    } else {
        // Nó interno: soma os terminais dos dois ramos
        no->qtd_term_distal =
            atualizaQtdTerminaisDistais(no->esq) +
            atualizaQtdTerminaisDistais(no->dir);
    }
    return no->qtd_term_distal;
}

// ═══════════════════════════════════════════════════════════════════════
// PARTE C — Fluxo em cada segmento
// ═══════════════════════════════════════════════════════════════════════

// Qj = qtd_term_distal(j) × Qterm
// O fluxo é proporcional ao número de terminais supridos pelo segmento.
// A raiz tem qtd_term_distal = Nterm, logo Qraiz = Qperf (correto).
void atualizaFluxos(ptrNo no, double Qterm) {
    if (no == NULL) return;
    no->fluxo = no->qtd_term_distal * Qterm;
    atualizaFluxos(no->esq, Qterm);
    atualizaFluxos(no->dir, Qterm);
}

// ═══════════════════════════════════════════════════════════════════════
// PARTE D — Lei de bifurcação e escala dos raios
// ═══════════════════════════════════════════════════════════════════════

// rj = Qj^(1/γ)
// Deriva da lei de Murray: r_pai^γ = r_esq^γ + r_dir^γ
// Como Qpai = Qesq + Qdir, e r ∝ Q^(1/γ), a lei é automaticamente
// satisfeita quando todos os raios seguem essa fórmula.
void atualizaRaiosPorFluxo(ptrNo no, double gamma) {
    if (no == NULL) return;
    if (no->fluxo > 0.0)
        no->raio = pow(no->fluxo, 1.0 / gamma);
    else
        no->raio = 0.0;
    atualizaRaiosPorFluxo(no->esq, gamma);
    atualizaRaiosPorFluxo(no->dir, gamma);
}

// ── Auxiliar interno: atualiza comprimento, resistência e volume ──────
// Chamada depois que raios já foram calculados.
static double mu_global; // parâmetro passado via variável estática

static void atualiza_segmento(ptrNo no) {
    no->comprimento = calculaComprimento(no);
    no->resistencia = calculaResistencia(mu_global,
                                         no->comprimento,
                                         no->raio);
    no->volume      = calculaVolume(no->comprimento, no->raio);
}

// Orquestra tudo na ordem obrigatória:
//  1. qtd_term_distal  (pós-ordem — base para tudo)
//  2. fluxos           (depende de qtd_term_distal)
//  3. raios            (depende de fluxo)
//  4. comprimento, resistência, volume  (dependem de raio e posição)
void atualizaGeometriaFisica(ptrNo raiz, double Qterm,
                              double gamma, double mu) {
    if (raiz == NULL) return;
    mu_global = mu;
    atualizaQtdTerminaisDistais(raiz);
    atualizaFluxos(raiz, Qterm);
    atualizaRaiosPorFluxo(raiz, gamma);
    percurso_nos(raiz, atualiza_segmento);
}

// ═══════════════════════════════════════════════════════════════════════
// PARTE E — Função custo: volume intravascular total
// ═══════════════════════════════════════════════════════════════════════

// Wrapper explícito para deixar claro o papel da função na otimização.
// Durante a busca em grade, esta função será chamada repetidamente
// para comparar diferentes posições de bifurcação.
double funcaoCustoVolume(ptrNo raiz) {
    return calculaVolumeTotal(raiz);
}