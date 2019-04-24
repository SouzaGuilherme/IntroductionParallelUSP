#include<stdio.h>
int main(){
	int A[2][2], B[2][2], C[2][2];
	A[0][0] = 1;	
	A[0][1] = 3;	
	A[1][0] = 2;	
	A[1][1] = 4;
	B[0][0] = 4;	
	B[0][1] = 3;	
	B[1][0] = 2;	
	B[1][1] = 1;
	int n=2;	
    	unsigned i, j, k;
	
    	for (i = 0; i < n; ++i)
        	for (j = 0; j < n; ++j){
            		double sum = 0;
            		for (k = 0; k < n; ++k)
                		sum += A[i][k] * B[k][j];
            		C[i][j] = sum;
       	 	}

    	for (i = 0; i < n; ++i)
        	for (j = 0; j < n; ++j)
			printf("%d\n", C[i][j]);
}
