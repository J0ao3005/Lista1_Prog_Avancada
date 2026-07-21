import sys
import pandas as pd
import numpy as np
import pyvista as pv

def main():
    filename = sys.argv[1] if len(sys.argv) > 1 else "arvore.csv"

    try:
        dados = pd.read_csv(filename)
    except FileNotFoundError:
        print(f"Erro: '{filename}' não encontrado. Execute ./minicco1 primeiro.")
        sys.exit(1)

    # Detecta se é CSV físico (tem coluna 'raio') ou simples (MiniCCO-0)
    tem_fisica = 'raio' in dados.columns

    print(f"Segmentos carregados : {len(dados)}")
    if tem_fisica:
        print(f"Volume total         : {dados['volume'].sum():.4e} m³")
        print(f"Raio médio           : {dados['raio'].mean():.4e} m")

    plotter = pv.Plotter()
    plotter.background_color = 'white'

    if tem_fisica:
        # ── Visualização com tubos proporcionais ao raio ──────────────
        # Remove a linha da raiz (pai_id == -1) pois não tem segmento
        segs = dados[dados['pai_id'] != -1]

        raio_max = segs['raio'].max()
        raio_min = segs['raio'].min()
        print(f"Raio max: {raio_max:.4e} m  |  Raio min: {raio_min:.4e} m")

        for _, row in segs.iterrows():
            p0 = np.array([row['x0'], row['y0'], 0.0])
            p1 = np.array([row['x1'], row['y1'], 0.0])

            if np.allclose(p0, p1): continue  # ignora segmentos degenerados

            linha = pv.Line(p0, p1)
            raio  = max(float(row['raio']), 1e-9)   # evita raio zero
            tubo  = linha.tube(radius=raio)

            # Mapeamento de cor por raio (vermelho=grosso, azul=fino)
            norm  = (raio - raio_min) / (raio_max - raio_min + 1e-30)
            cor   = (norm, 0.2, 1.0 - norm)          # RGB
            plotter.add_mesh(tubo, color=cor)

        # Raiz em destaque (ponto dourado no topo)
        raiz_row = dados[dados['pai_id'] == -1]
        if not raiz_row.empty:
            raiz_pt = np.array([[raiz_row.iloc[0]['x1'],
                                  raiz_row.iloc[0]['y1'], 0.0]])
            plotter.add_mesh(pv.PolyData(raiz_pt),
                             point_size=14,
                             render_points_as_spheres=True,
                             color='gold', label='Raiz')

        # Terminais: nós que aparecem em x1,y1 mas nunca em x0,y0
        origens  = set(zip(dados['x0'].round(6), dados['y0'].round(6)))
        destinos = set(zip(dados['x1'].round(6), dados['y1'].round(6)))
        terminais_pts = destinos - origens
        if terminais_pts:
            t_arr = np.array([[x, y, 0.0] for x, y in terminais_pts])
            plotter.add_mesh(pv.PolyData(t_arr),
                             point_size=8,
                             render_points_as_spheres=True,
                             color='steelblue', label='Terminais')

    else:
        # ── Fallback: visualização simples (MiniCCO-0) ────────────────
        pontos, linhas, raiz_arr = [], [], np.array([[0.0, 0.0, 0.0]])
        mapa, idx = {}, 0
        origens, destinos = set(), set()

        for _, row in dados.iterrows():
            p1 = (float(row['x1']), float(row['y1']), 0.0)
            p2 = (float(row['x2']), float(row['y2']), 0.0)
            for p in (p1, p2):
                if p not in mapa:
                    mapa[p] = idx; pontos.append(p); idx += 1
            linhas.append([2, mapa[p1], mapa[p2]])
            origens.add(p1); destinos.add(p2)

        mesh        = pv.PolyData()
        mesh.points = np.array(pontos)
        mesh.lines  = np.hstack(linhas)
        raiz_cands  = origens - destinos
        raiz_arr    = np.array([list(raiz_cands)[0]]) if raiz_cands else np.array([[0,0,0]])

        plotter.add_mesh(mesh, line_width=2, color='crimson', label='Segmentos')
        plotter.add_mesh(pv.PolyData(np.array(pontos)), point_size=5,
                         render_points_as_spheres=True, color='steelblue')
        plotter.add_mesh(pv.PolyData(raiz_arr), point_size=14,
                         render_points_as_spheres=True, color='gold', label='Raiz')

    plotter.add_legend()
    plotter.view_xy()
    plotter.show(title="MiniCCO-1 — Árvore Arterial com Raios")

if __name__ == "__main__":
    main()