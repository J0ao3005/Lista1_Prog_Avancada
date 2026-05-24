# MiniCCO-0 — Núcleo Geométrico para Crescimento Arterial

Projeto da disciplina **Algoritmos e Programação Avançada** — Universidade Federal de Ouro Preto, 2026.1.

Implementação de um núcleo simplificado baseado no método **CCO (Constrained Constructive Optimization)** para construção de árvores arteriais em 2D dentro de um domínio circular.

---

## Estrutura do Projeto

```
.
├── main.c          # Algoritmo principal e loop de crescimento
├── geometria.c/h   # Funções geométricas (distância, interseção, orientação)
├── arvore.c/h      # Estrutura de árvore binária e operações sobre nós
├── exportacao.c/h  # Exportação dos segmentos (.csv e .vtk)
└── visualizar.py   # Script de visualização (Matplotlib / PyVista)
```

---

## Compilação

```bash
gcc -O2 -Wall -o programa main.c geometria.c arvore.c exportacao.c -lm
```

---

## Execução

```bash
./programa <Nterm> <R>
```

| Parâmetro | Descrição |
|-----------|-----------|
| `Nterm`   | Número de pontos terminais a inserir |
| `R`       | Raio do domínio circular |

**Exemplos:**
```bash
./programa 10 10.0    # pequeno
./programa 50 10.0    # médio
./programa 100 10.0   # grande
```

O programa imprime ao final: número de nós, folhas, comprimento total da árvore e conexões rejeitadas. Dois arquivos são gerados automaticamente: `arvore.csv` e `arvore.vtk`.

---

## Visualização

```bash
python3 visualizar.py arvore.csv 10.0
```

O script tenta usar **PyVista** primeiro; se não estiver instalado, usa **Matplotlib** e salva a imagem em `arvore_vis.png`.

Instalar dependências:
```bash
pip install matplotlib numpy pyvista
```

---

## Método

O algoritmo cresce a árvore iterativamente:

1. Inicia com um nó raiz na origem
2. Gera pontos aleatórios dentro do domínio circular
3. Para cada ponto, avalia conexões candidatas com a árvore existente
4. Valida restrições geométricas (sem interseção, distância mínima respeitada)
5. Seleciona a conexão de menor custo e insere o novo nó

A função custo combina distância euclidiana e penalização angular:

```
J = α · (d / d_max) + β · (θ / π)
```

com `α = 1.0` e `β = 0.5`.