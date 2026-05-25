import sys
import csv
import numpy as np
import pyvista as pv

def carregar_segmentos(filename):
    """
    Lê o CSV gerado pelo MiniCCO-0 e monta arrays de pontos e linhas
    no formato esperado pelo PyVista.
    """
    pontos = []
    linhas = []
    mapa   = {}   # (x, y, z) -> índice
    idx    = 0

    with open(filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            p1 = (float(row['x1']), float(row['y1']), 0.0)
            p2 = (float(row['x2']), float(row['y2']), 0.0)

            if p1 not in mapa:
                mapa[p1] = idx
                pontos.append(p1)
                idx += 1
            if p2 not in mapa:
                mapa[p2] = idx
                pontos.append(p2)
                idx += 1

            # Formato PyVista: [n_pontos_da_linha, i1, i2]
            linhas.append([2, mapa[p1], mapa[p2]])

    return np.array(pontos, dtype=float), linhas


def main():
    filename = sys.argv[1] if len(sys.argv) > 1 else "arvore.csv"

    try:
        pontos, linhas = carregar_segmentos(filename)
    except FileNotFoundError:
        print(f"Erro: arquivo '{filename}' nao encontrado.")
        print("Execute primeiro: ./programa <Nterm> <R>")
        sys.exit(1)

    if len(linhas) == 0:
        print("Nenhum segmento encontrado no arquivo.")
        sys.exit(1)

    print(f"Pontos carregados : {len(pontos)}")
    print(f"Segmentos carregados: {len(linhas)}")

    # ── Monta a malha ────────────────────────────────────────────────
    mesh        = pv.PolyData()
    mesh.points = pontos
    mesh.lines  = np.hstack(linhas)

    # Ponto raiz (origem)
    raiz_mesh = pv.PolyData(np.array([[0.0, 0.0, 0.0]]))

    # ── Visualização ─────────────────────────────────────────────────
    plotter = pv.Plotter()
    plotter.background_color = 'white'

    # Segmentos da árvore
    plotter.add_mesh(mesh,
                     line_width=2,
                     color='crimson',
                     label='Segmentos')

    # Todos os pontos (nós)
    plotter.add_mesh(pv.PolyData(pontos),
                     point_size=5,
                     render_points_as_spheres=True,
                     color='steelblue',
                     label='Nos')

    # Raiz em destaque
    plotter.add_mesh(raiz_mesh,
                     point_size=12,
                     render_points_as_spheres=True,
                     color='gold',
                     label='Raiz')

    plotter.add_legend()
    plotter.view_xy()
    plotter.show(title="MiniCCO-0 — Arvore Arterial")


if __name__ == "__main__":
    main()
