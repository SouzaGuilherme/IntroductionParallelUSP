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

    unsigned ii, jj, kk, i, j, k, sem_nome=128;

    for (i = 0; i < n; i+=sem_nome){
            for (j = 0; j < n; j+=sem_nome){
                for (k = 0; k < n; k+=sem_nome){
                	for(ii=i; ii<i+sem_nome && ii<n; ii++){
						for(jj=j; jj<j+sem_nome && jj<n; jj++){
							for(kk=k; kk<k+sem_nome && kk<n; kk++){
								C(ii, kk) += A(ii, jj) * B(jj, kk);
							}
						}
					}
				}
            }
        }

           /* for (j = 0; j < n; j+=sem_nome){
                for (k = 1; k < n; k+=sem_nome){
                	for(ii=1; ii<n; ii++){
						for(jj=j; jj<j+sem_nome && jj<n; jj++){
							//Bkj = B[j*n+k];
							for(kk=k; kk<k+sem_nome && kk<n; kk++){
								C(jj, ii) += A(ii, kk) * B(jj, kk);
							}
						}
					}
				}
            }*/
           // count_bloc_control+=1;
    //}

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

