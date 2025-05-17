// THIAGO ZILIO GRR202342

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "utils.h"
#include "edo.h"

Tridiag *genTridiag (EDo *edo)
{
  Tridiag *sl;
  real_t x, rx;
  int n = edo->n;
  
  sl = (Tridiag *) malloc (sizeof(Tridiag));
  sl->n = edo->n;

  sl->D = (real_t *) calloc(n, sizeof(real_t));
  sl->Di = (real_t *) calloc(n, sizeof(real_t));
  sl->Ds = (real_t *) calloc(n, sizeof(real_t));
  sl->B = (real_t *) calloc(n, sizeof(real_t));

  real_t h = (edo->b - edo->a)/(n+1);

  for (int i=0; i < n; ++i) {
    x = edo->a + (i+1)*h;
    rx = edo->r1*x + edo->r2*x*x + edo->r3*cos(x) + edo->r4*exp(x);
    
    sl->B[i] = h*h * rx;
    sl->Di[i] = 1 - h * edo->p/2.0;
    sl->D[i] = -2 + h*h * edo->q;
    sl->Ds[i] = 1 + h * edo->p/2.0;
  }

  sl->B[0] -= edo->ya * (1 - h*edo->p/2.0);
  sl->B[n-1] -= edo->yb * (1 + h*edo->p/2.0);
  
  return sl;
}


// Exibe SL na saída padrão
void prnEDOsl (EDo *edoeq)
{
  int n = edoeq->n, i, j;
  real_t x, b, d, di, ds,rx;
  real_t h = (edoeq->b - edoeq->a)/(n+1);

  printf ("%d\n", n);

  for (i=0; i < n; ++i) {
    x = edoeq->a + (i+1)*h;
    rx = edoeq->r1*x + edoeq->r2*x*x + edoeq->r3*cos(x) + edoeq->r4*exp(x);
    
    b = h*h * rx; 
    di = 1 - h * edoeq->p/2.0;
    d = -2 + h*h * edoeq->q;
    ds = 1 + h * edoeq->p/2.0;
      
    for (j=0; j < n; ++j) {
      if (i == j)
	printf (FORMAT,d);
      else if (j == i-1 && i)
	printf (FORMAT,di);
      else if (j == i+1 && i != n-1)
	printf (FORMAT,ds);
      else
	printf(FORMAT, 0.0);
    }
      
    if (i == 0)
      b -= edoeq->ya * (1 - h*edoeq->p/2.0);
    else if (i == n-1)
      b -= edoeq->yb * (1 + h*edoeq->p/2.0);

    printf (FORMAT, b);
      
    printf ("\n");
  }
}

void factor_LU(Tridiag *sl) {
  int n = sl->n;
  for (int i = 1; i < n; i++) {
    sl->Di[i] /= sl->D[i-1];
    sl->D[i] -= sl->Di[i] * sl->Ds[i-1];
  }
}

real_t *solve_tridiag(Tridiag *sl) {
  int n = sl->n;
  real_t *y = malloc(n *sizeof(real_t));
  real_t *x = malloc(n * sizeof(real_t));
  if (!y || !x) {
    fprintf(stderr, "Erro Memória");
    exit(EXIT_FAILURE);
  }

  y[0] = sl->B[0];
  for (int i = 1; i < n; ++i) y[i] = sl->B[i] - sl->Di[i] * y[i-1];

  x[n-1] = y[n-1] / sl->D[n-1];
  for (int i = n-2; i >=0; --i) x[i] = (y[i] - sl->Ds[i] * x[i+1]) / sl->D[i];

  free(y);
  return x;
}


void free_tridiag(Tridiag *sl) {
  free(sl->Di);
  free(sl->D);
  free(sl->Ds);
  free(sl->B);
  free(sl);
}
