# ep02
**Thiago Zilio**
GRR20234265

# Documentação do Projeto `resolveEDO`

Este projeto implementa a solução de equações diferenciais ordinárias (EDOs) de segunda ordem através do método de fatoração LU em sistemas tridiagonais.

---

## Estruturas de Dados

### `EDo`

```c
typedef struct {
    int n;         // número de pontos da malha
    double a, b;   // limites do intervalo [a, b]
    double ya, yb; // condições de contorno y(a) e y(b)
    double p, q;   // coeficientes da EDO (y'' + p y' + q y = r(x))
    double r1, r2, r3, r4; // coeficientes do termo r(x)
} EDo;
```

* **n**: número de subintervalos (pontos internos)
* **a, b**: extremos do domínio
* **ya, yb**: valores de contorno em a e b
* **p, q**: coeficientes constantes da EDO
* **r1...r4**: coeficientes do termo não homogêneo r(x)

### `Tridiag`

```c
typedef struct {
    int n;       // dimensão da matriz tridiagonal
    real_t *Di;  // elemento abaixo da diagonal principal (subdiagonal)
    real_t *D;   // elementos da diagonal principal
    real_t *Ds;  // elemento acima da diagonal principal (superdiagonal)
    real_t *B;   // vetor do lado direito (termo constante)
} Tridiag;
```

* **n**: tamanho do sistema linear
* **Di**, **D**, **Ds**: vetores de tamanho n contendo as três diagonais
* **B**: vetor de constantes para o sistema tridiagonal

---

## Módulos e Funções

### `genTridiag(EDo *edo)`

* **Descrição:** Gera a estrutura `Tridiag` a partir dos parâmetros da EDO, montando as diagonais e o vetor B.
* **Parâmetros:**

  * `EDo *edo`: ponteiro para estrutura contendo dados da EDO.
* **Retorno:**

  * `Tridiag *`: ponteiro para o sistema montado.
* **Códigos de erro:**

  * Falha de alocação (`malloc`/`calloc` retorna NULL): imprime mensagem de erro genérica e encerra o programa.

---

### `prnEDOsl(EDo *edoeq)`

* **Descrição:** Imprime na saída padrão a matriz tridiagonal completa e o vetor B no formato exigido.
* **Parâmetros:**

  * `EDo *edoeq`: ponteiro para estrutura da EDO com dados já preenchidos.
* **Retorno:**

  * `void`.
* **Códigos de erro:**

  * Nenhum.

---

### `factor_LU(Tridiag *sl)`

* **Descrição:** Realiza a fatoração LU *in place* sobre a estrutura tridiagonal, atualizando `Di` e `D`.
* **Parâmetros:**

  * `Tridiag *sl`: ponteiro para sistema tridiagonal a ser fatorado.
* **Retorno:**

  * `void`.
* **Códigos de erro:**

  * Nenhum (assume que não há zeros na diagonal principal).

---

### `solve_tridiag(Tridiag *sl)`

* **Descrição:** Resolve o sistema tridiagonal fatorado por LU, retornando o vetor solução.
* **Parâmetros:**

  * `Tridiag *sl`: ponteiro para sistema já fatorado.
* **Retorno:**

  * `real_t *`: vetor de solução (alocado internamente, deve ser liberado pelo chamador).
* **Códigos de erro:**

  * Falha de alocação (`malloc` retorna NULL): imprime "Erro Memória" e encerra o programa.

---

### `free_tridiag(Tridiag *sl)`

* **Descrição:** Libera toda memória associada à estrutura `Tridiag`.
* **Parâmetros:**

  * `Tridiag *sl`: ponteiro para sistema tridiagonal.
* **Retorno:**

  * `void`.
* **Códigos de erro:**

  * Nenhum.

---

### `main()` em `resolveEDO.c`

* **Descrição:** Lê parâmetros da EDO de *stdin*, monta e imprime sistemas, executa fatoração e solução, mede tempo parcial, e gerencia marcações LIKWID.
* **Fluxo:**

  1. Inicializa Marker API (LIKWID).
  2. Lê `n`, `a`, `b`, `ya`, `yb`, `p`, `q`.
  3. Para cada conjunto `r1`..`r4`:

     * Monta `Tridiag` via `genTridiag`.
     * Imprime sistema (`prnEDOsl`).
     * Inicia marcação "LU" e chama `factor_LU`.
     * Inicia marcação "SOL", chama `solve_tridiag`, pára marcação e imprime solução.
     * Libera solução e o sistema.
  4. Finaliza marcações LIKWID.
* **Códigos de erro:**

  * Leitura mal formatada no `scanf` principal: retorna exit code 1.
  * Falha de alocação em `solve_tridiag`: encerra com `EXIT_FAILURE`.

---

## Algoritmo

1. **Discretização:** Método das diferenças finitas em malha uniforme, transformando EDO em sistema linear tridiagonal.
2. **Fatoração LU:** Fatoração sem pivotamento, adequada para matrizes tridiagonais, com custo O(n).
3. **Resolução:** Substituição direta (back-substitution) em duas passagens, também O(n).
4. **Medição de desempenho:** Uso de LIKWID Marker API para isolar tempo de fatoração e de solução.

---

## Observações

* As bibliotecas LIKWID e HWLOC podem deixar blocos "still reachable" em Valgrind; isso é normal e não indica vazamento do código do usuário.
* Para remover marcações LIKWID, basta comentar as macros de `LIKWID_MARKER_*` e retirar o `-m` do comando `likwid-perfctr`.
