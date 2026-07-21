# CCO-X — GraphCCO: Malhas Arteriais e Análise Topológica em Grafos

Projeto prático final da disciplina **Algoritmos e Programação Avançada** — UFOP, 2026.1..

Este projeto é a evolução do núcleo Constrained Constructive Optimization (CCO). Além de gerar uma árvore arterial com propriedades físicas reais (raio, fluxo, resistência e volume) e otimização por busca em grade baricêntrica, esta versão introduz a variante inovadora GraphCCO. A malha final é convertida numa estrutura de Grafo Ponderado Direcionado para análise topológica completa, execução de buscas clássicas e cálculo da Ordem de Strahler da rede.

---

## Estrutura do Projeto

```
.
├── main.c            # Algoritmo principal, CLI, loop de crescimento e orquestração do grafo
├── geometria.c/h     # Funções geométricas (distância, interseção, orientação paramétrica)
├── arvore.c/h        # Estrutura de árvore binária e operações sobre os nós construtivos
├── candidatos.c/h    # Geração de pontos e seleção das bifurcações ótimas
├── fisica.c/h        # Leis físicas (comprimento, resistência, fluxo, escala de Murray e volume)
├── otimizacao.c/h    # Coordenadas baricêntricas e busca em grade espacial
├── grafo.c/h         # Inovação CCO-X — Lista de adjacência, BFS, DFS, e Ordem de Strahler
├── exportar.c/h      # Exportação de dados geométricos e métricas (.csv, .vtk, .txt)
└── visualizar.py     # Script de visualização 2D em alta resolução

```

---

## Compilação

```bash
gcc -O2 -Wall -Wextra -std=c11 -o cco_final main.c geometria.c arvore.c candidatos.c exportar.c fisica.c otimizacao.c grafo.c -lm
```

---

## Execução

```bash
./cco_final <Nterm> <R> <gamma> <M> <modo> <seed>
```

| Parâmetro | Descrição |
|-----------|-----------|
| `Nterm`   | Número de pontos terminais a inserir na perfusão |
| `R`       | Raio do domínio circular restritivo [m] |
| `gamma`   | Expoente da lei de bifurcação (sugerido: 2.7 ou 3.0) |
| `M`       | Resolução da busca em grade no triângulo (sugerido: 10, 20 ou 50) |
| `modo`    | Variante computacional implementada (usar: graph) |
| `seed`    | Semente do gerador aleatório para reprodutibilidade |

**Exemplos (testes numéricos exigidos no roteiro):**
```bash
./cco_final 20 10.0 3.0 20 graph 123
./cco_final 50 10.0 3.0 20 graph 42
./cco_final 100 10.0 3.0 20 graph 7
./cco_final 100 10.0 2.7 20 graph 99
```
O programa exibe no console o processo de busca do Grafo (BFS e DFS), a profundidade máxima alcançada e a Ordem de Strahler da raiz.
Três arquivos são gerados na saída:
1. `arvore.csv`: Campos físicos e geométricos completos.
2. `arvore.vtk`: Formato PolyData 3D para renderização no ParaView.
3. `metricas.txt`: Resumo estatístico da rede exigido pelo projeto.

---

## Visualização

```bash
python3 visualizar.py arvore.csv
```

Instalar dependências:
```bash
pip install pandas numpy matplotlib
```

---

## Método e Função Custo

O algoritmo evolui a rede iterativamente a partir do topo do domínio:

1. Gera um ponto terminal por método de rejeição.
2. Para cada segmento existente, cria uma bifurcação temporária e avalia uma grade espacial parametrizada em coordenadas baricêntricas.
3. Valida as posições candidatas contra colisões espaciais e a distância mínima de tolerância de tecido (ε).
4. Seleciona a posição ótima que minimiza o Volume Intravascular Total de toda a malha, encurtando o caminho das artérias mais espessas.

A função custo minimizada matematicamente na grade é:

```
J = V_total = Σ (π * r² * l)
```

---

Após a estabilização física, a árvore é convertida numa lista de adjacência, acionando a inovação topológica que avalia a complexidade hierárquica e capilar do sistema final.
