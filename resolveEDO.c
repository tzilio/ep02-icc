// THIAGO ZILIO GRR20234265

#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>
#include <string.h>
#include "utils.h"
#include "edo.h"
#include "likwid.h"    

int main(void) {
    // inicializa LIKWID markers
    LIKWID_MARKER_INIT;
    

    EDo edoeq;
    int eq_count = 0;

    if (scanf("%d", &edoeq.n) != 1) return 1;
    scanf("%lf %lf", &edoeq.a, &edoeq.b);
    scanf("%lf %lf", &edoeq.ya, &edoeq.yb);
    scanf("%lf %lf", &edoeq.p, &edoeq.q);

    // força arredondamento para baixo
    fesetround(FE_DOWNWARD);

    // para cada conjunto de coeficientes r1..r4
    while (scanf("%lf %lf %lf %lf",
                 &edoeq.r1, &edoeq.r2, &edoeq.r3, &edoeq.r4) == 4)
    {

        // monta sistema base (p, q fixos; mas r’s mudam)
        Tridiag *sl = genTridiag(&edoeq);
        prnEDOsl(&edoeq);

        string_t lu_mark = markerName("LU", eq_count);
        LIKWID_MARKER_START(lu_mark);
        factor_LU(sl);
        LIKWID_MARKER_STOP(lu_mark);
        free(lu_mark);

        string_t sol_mark = markerName("SOL", eq_count);
        rtime_t t0 = timestamp();
        LIKWID_MARKER_START(sol_mark);
        real_t *sol = solve_tridiag(sl);
        LIKWID_MARKER_STOP(sol_mark);
        free(sol_mark);    

        printf("\n");
        for (int i = 0; i < sl->n; ++i)
            printf(" % .15e", sol[i]);


        rtime_t t_elapsed = timestamp() - t0;
        printf("\n % .8e\n", t_elapsed);

        free(sol);
        free_tridiag(sl);

        printf("\n");
        eq_count++;
    }

    // relatório LIKWID
    LIKWID_MARKER_CLOSE;
    return 0;
}
