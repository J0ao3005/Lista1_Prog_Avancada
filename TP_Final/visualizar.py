import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

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

    # Configura a figura para alta resolução
    plt.figure(figsize=(10, 10))

    if tem_fisica:
        # ── Visualização com linhas proporcionais ao raio ──────────────
        segs = dados[dados['pai_id'] != -1]

        raio_max = segs['raio'].max()
        raio_min = segs['raio'].min()
        print(f"Raio max: {raio_max:.4e} m  |  Raio min: {raio_min:.4e} m")

        for _, row in segs.iterrows():
            x = [row['x0'], row['x1']]
            y = [row['y0'], row['y1']]

            # ignora segmentos degenerados
            if np.allclose([x[0], y[0]], [x[1], y[1]]): continue  

            raio = max(float(row['raio']), 1e-9)

            # Mapeamento de cor (sua lógica original mantida!)
            norm  = (raio - raio_min) / (raio_max - raio_min + 1e-30)
            cor   = (norm, 0.2, 1.0 - norm)  # RGB (Vermelho=grosso, Azul=fino)
            
            # Espessura da linha variando de 1 a 10 para imitar o "tubo" 3D
            espessura = 1.0 + 9.0 * norm

            plt.plot(x, y, color=cor, linewidth=espessura, zorder=1, solid_capstyle='round')

        # Raiz em destaque
        raiz_row = dados[dados['pai_id'] == -1]
        if not raiz_row.empty:
            plt.scatter([raiz_row.iloc[0]['x1']], [raiz_row.iloc[0]['y1']], 
                        color='gold', edgecolor='black', s=150, zorder=3, label='Raiz')

        # Terminais
        origens  = set(zip(dados['x0'].round(6), dados['y0'].round(6)))
        destinos = set(zip(dados['x1'].round(6), dados['y1'].round(6)))
        terminais_pts = destinos - origens
        if terminais_pts:
            tx, ty = zip(*terminais_pts)
            plt.scatter(tx, ty, color='steelblue', s=30, zorder=2, label='Terminais')

    else:
        # ── Fallback: visualização simples (MiniCCO-0) ────────────────
        origens, destinos = set(), set()

        for _, row in dados.iterrows():
            x1, y1 = float(row['x1']), float(row['y1'])
            x2, y2 = float(row['x2']), float(row['y2'])
            
            plt.plot([x1, x2], [y1, y2], color='crimson', linewidth=2, zorder=1)
            plt.scatter([x1, x2], [y1, y2], color='steelblue', s=15, zorder=2)
            
            origens.add((x1, y1))
            destinos.add((x2, y2))

        raiz_cands = origens - destinos
        if raiz_cands:
            rx, ry = list(raiz_cands)[0]
            plt.scatter([rx], [ry], color='gold', edgecolor='black', s=150, zorder=3, label='Raiz')

    # Ajustes finais do gráfico
    plt.title("MiniCCO — Árvore Arterial")
    plt.axis('equal')
    plt.grid(True, linestyle='--', alpha=0.5)
    
    # Remove legendas duplicadas
    handles, labels = plt.gca().get_legend_handles_labels()
    by_label = dict(zip(labels, handles))
    if by_label:
        plt.legend(by_label.values(), by_label.keys())

    # Salva a imagem sem tentar abrir janela!
    nome_saida = "arvore_vis.png"
    plt.savefig(nome_saida, dpi=300, bbox_inches='tight')
    print(f"\nSucesso! Imagem gerada e salva como '{nome_saida}'.")

if __name__ == "__main__":
    main()