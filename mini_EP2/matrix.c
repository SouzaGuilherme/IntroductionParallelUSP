#include "stdio.h"
#include "matrix.h"

#include <stdlib.h>

/* Módulo de x*/
#define ABS(x) ((x > 0)? (x): -(x))


void matrix_fill_rand(unsigned n, double *restrict _A)
{
#define A(i, j) _A[n*(i) + (j)]
    int i, j;

    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            A(i, j) = 10*(double) rand() / (double) RAND_MAX;

#undef A
}

void matrix_dgemm_0(unsigned n, double *restrict _C, double *restrict _A, double *restrict _B)
{
    /* Macros para acessar os elementos das matrizes mais facilmente*/
#define A(i, j) _A[n*(i) + (j)]
#define B(i, j) _B[n*(i) + (j)]
#define C(i, j) _C[n*(i) + (j)]
    unsigned i, j, k;

    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
        {
            double sum = 0;
            for (k = 0; k < n; ++k)
                sum += A(i, k)*B(k, j);
             C(i, j) = sum;
        }
    /*i=0, j=0, k=0;
    for (i = 0; i < n; ++i){
        for (j = 0; j < n; ++j)
            printf("%d ", C(i, j));
    unsigned i, j, k;

    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
        {
            double sum = 0;
            for (k = 0; k < n; ++k)
                sum += A(i, k)*B(k, j);
             C(i, j) = sum;
        }
        printf("\n");
    }*/
#undef A
#undef B
#undef C
}

void matrix_dgemm_1(unsigned n, double *restrict _C, double *restrict _A, double *restrict _B)
{
#define A(i, j) _A[n*(i) + (j)]
#define B(i, j) _B[n*(i) + (j)]
#define C(i, j) _C[n*(i) + (j)]


    unsigned i, j, k;

    for (i = 0; i < n; ++i){
        for (j = 0; j < n; ++j){
            for (k = 0; k < n; ++k)
                C(i, k) += A(i, j)*B(j, k);
        }
    }

    /* Aqui você não deve usar blocagem. Seu código deve ser BEM mais rápido
     * que o anterior (cerca de 10x). */
    /* Seu código aqui. */


#undef A
#undef B
#undef C
}

void matrix_dgemm_2(unsigned n, double *restrict _C, double *restrict _A, double *restrict _B)
{
#define A(i, j) _A[n*(i) + (j)]
#define B(i, j) _B[n*(i) + (j)]
#define C(i, j) _C[n*(i) + (j)]

    /* Aqui você deve usar blocagem, em conjunto com a técnica que você
     * desenvolveu em dgemm_1. Dependendo da sua máquina você pode conseguir
     * speedup de 2x em relação ao dgemm_1, Mas não fique triste se você conseguir
     * apenas 1 segundo ou 2.
     */
    /* Seu código aqui. */

    unsigned i, j, k,set_position_I=0, set_position_J=0, set_position_K=0, divisor=128;
    unsigned set_bloc=n/divisor, bloc_complet=set_bloc*set_bloc, count_bloc=((n*n)/bloc_complet), count_bloc_control=0;

    while(set_bloc != count_bloc_control){
        for (i = set_position_I; i < set_bloc; ++i){
            for (j = set_position_J; j < set_bloc; ++j)
            {
                for (k = set_position_K; k < set_bloc; ++k)
                    C(i, k) += A(i, j)*B(j, k);
            }
        }
            count_bloc_control+=1;
    }
#undef A
#undef B
#undef C
}


bool matrix_eq(unsigned n, double *restrict _A, double *restrict _B)
{
#define A(i, j) _A[n*(i) + (j)]
#define B(i, j) _B[n*(i) + (j)]

    unsigned i, j;

    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            if (ABS (A(i, j) - B(i, j)) > 1e-15)
                return false;

    return true;
#undef A
#undef B
}


bool matrix_which_dgemm(int algorithm, unsigned n,  double *restrict _C,
        double *restrict _A, double *restrict _B)
{
    bool ret;
    switch(algorithm)
    {
        case 0:
            matrix_dgemm_0(n, _C, _A, _B);
            ret = true;
            break;

        case 1:
            matrix_dgemm_1(n, _C, _A, _B);
            ret = true;
            break;

        case 2:
            matrix_dgemm_2(n, _C, _A, _B);
            ret = true;
            break;

        default:
            ret = false;
    }
    return ret;
}

