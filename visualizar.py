import sys
import csv
import numpy as np
import pyvista as pv

def carregar_segmentos(filename):
    pontos = []
    linhas = []
    mapa   = {}
    idx    = 0

    origens  = set()  # pontos que aparecem como x1,y1 (origem do segmento)
    destinos = set()  # pontos que aparecem como x2,y2 (destino do segmento)

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

            linhas.append([2, mapa[p1], mapa[p2]])
            origens.add(p1)
            destinos.add(p2)

    # Raiz: aparece como origem mas nunca como destino
    raiz_candidates = origens - destinos
    raiz = list(raiz_candidates)[0] if raiz_candidates else pontos[0]

    return np.array(pontos, dtype=float), linhas, np.array([raiz], dtype=float)


def main():
    filename = sys.argv[1] if len(sys.argv) > 1 else "arvore.csv"

    try:
        pontos, linhas, raiz = carregar_segmentos(filename)
    except FileNotFoundError:
        print(f"Erro: arquivo '{filename}' nao encontrado.")
        print("Execute primeiro: ./programa <Nterm> <R>")
        sys.exit(1)

    if len(linhas) == 0:
        print("Nenhum segmento encontrado no arquivo.")
        sys.exit(1)

    print(f"Pontos    : {len(pontos)}")
    print(f"Segmentos : {len(linhas)}")
    print(f"Raiz      : ({raiz[0][0]:.4f}, {raiz[0][1]:.4f})")

    # ── Monta a malha ────────────────────────────────────────────────
    mesh        = pv.PolyData()
    mesh.points = pontos
    mesh.lines  = np.hstack(linhas)

    # ── Visualização ─────────────────────────────────────────────────
    plotter = pv.Plotter()
    plotter.background_color = 'white'

    plotter.add_mesh(mesh,
                     line_width=2,
                     color='crimson',
                     label='Segmentos')

    plotter.add_mesh(pv.PolyData(pontos),
                     point_size=5,
                     render_points_as_spheres=True,
                     color='steelblue',
                     label='Nos')

    plotter.add_mesh(pv.PolyData(raiz),
                     point_size=14,
                     render_points_as_spheres=True,
                     color='gold',
                     label='Raiz')

    plotter.add_legend()
    plotter.view_xy()
    plotter.show(title="MiniCCO-0 — Arvore Arterial")


if __name__ == "__main__":
    main()