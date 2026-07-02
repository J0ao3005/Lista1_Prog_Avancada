# MiniCCO-2 — Escala dos Raios e Otimização Geométrica de Bifurcações

Projeto da disciplina **Algoritmos e Programação Avançada** — UFOP, 2026.1.

Extensão do MiniCCO-0: além da árvore geométrica, cada segmento agora tem
raio, comprimento, fluxo, resistência hidráulica e volume intravascular,
com a posição de cada nova bifurcação otimizada por busca em grade dentro
do triângulo formado pelo segmento antigo e o novo terminal.

---

## Estrutura do Projeto

```
.
├── main.c            # Algoritmo principal, CLI e loop de crescimento
├── geometria.c/h     # Funções geométricas (distância, interseção, orientação)
├── arvore.c/h        # Estrutura de árvore binária e operações sobre nós
├── candidatos.c/h    # Geração de pontos e seleção de candidatos (Partes F/G)
├── fisica.c/h        # Comprimento, resistência, fluxo, raio, volume (Partes A-E)
├── otimizacao.c/h    # Coordenadas baricêntricas e busca em grade (Parte F)
├── exportar.c/h      # Exportação dos segmentos (.csv e .vtk)
└── visualizar.py     # Script de visualização (PyVista)
```

---

## Compilação

```bash
gcc -O2 -Wall -Wextra -std=c11 -o minicco1 main.c geometria.c arvore.c candidatos.c exportar.c fisica.c otimizacao.c -lm
```

---

## Execução

```bash
./minicco1 <Nterm> <R> <gamma> <M>
```

| Parâmetro | Descrição |
|-----------|-----------|
| `Nterm`   | Número de pontos terminais a inserir |
| `R`       | Raio do domínio circular [m] |
| `gamma`   | Expoente da lei de bifurcação (sugerido: 3.0) |
| `M`       | Resolução da busca em grade no triângulo (sugerido: 10, 20 ou 50) |

**Exemplos (testes numéricos sugeridos no enunciado):**
```bash
./minicco1 10 1.0 3.0 10
./minicco1 30 1.0 3.0 20
./minicco1 50 1.0 3.0 20
./minicco1 50 1.0 2.7 20
./minicco1 50 1.0 3.0 50
```

O programa imprime: número total de nós, segmentos, terminais, comprimento
total, volume intravascular total, raio da raiz, raio médio, conexões
testadas, conexões rejeitadas e tempo de execução.
Dois arquivos são gerados: `arvore.csv` (com os campos físicos completos)
e `arvore.vtk`.

---

## Visualização

```bash
python visualizar.py arvore.csv
```

Instalar dependências:
```bash
pip install pyvista numpy
```

---

## Método

O algoritmo cresce a árvore iterativamente a partir de um nó raiz no ponto `(0, R)` (topo do domínio):

1. Gera um ponto aleatório dentro do domínio circular
2. Para cada segmento existente, calcula seu ponto médio como candidato de bifurcação
3. Valida o novo segmento `meio → novo` contra três restrições:
   - Ponto dentro do domínio circular
   - Sem interseção com segmentos existentes
   - Distância mínima ε entre o segmento inteiro e todos os demais
4. Seleciona o candidato de menor custo (distância euclidiana)
5. Insere um nó de bifurcação no ponto médio, dividindo o segmento original

A função custo básica é a distância euclidiana:

```
J = d(meio, novo_ponto)
```

---

## Parâmetro ε (epsilon)

`epsilon = 0.05 * R`

Controla o espaçamento mínimo entre segmentos. A checagem usa a distância
mínima entre o **segmento inteiro** `meio→novo` e cada segmento existente —
não apenas o ponto terminal. Isso garante melhor distribuição no domínio
e evita sobreposição visual entre ramos da árvore.
