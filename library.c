#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int boyerMoore(byte palavra[], int m, byte texto[], int n){
	int *jump = malloc((m+1)*sizeof(int));
	//usa-se jump[1..m]

	//Pre processamento da palavra a
	int h = m, k = m-1;
	while(h >= m && k >=1){
		if (palavra[i] == palavra[j]){
			--i;
			--j;
		}else{
			i=m;
			j=--k;
		}
		jump[h--] = k;
	}
	while(h>=1){
		jump[h--] = k;
	}

	//Buscando a palavra no texto
	int ocorrencia = 0;
	k=m;
	while(k<= n){
		int i=m, j=k;
		while(i >= 1 && palavra[i] == texto[j]){
			--i;
			--j;

		}
		if (i<1){
			++ocorrencia;
		}
		if (i==m){
			k+= 1;
		}else{
			k += m - jump[i+1];
		}
	}
	return ocorrencia
};

void *greep("TENHO QUE RECEBER O VETOR COM OS ARQUIVOS"){
	FILE arq*;
	int i=0
	char linha[100];
	char *result;
	
	arq = fopen("ARQUIVO DA POSICAO TAL", "rt");
	if (arq == NULL){
		printf("Erro na abertura do arquivo\n");
		return;
	}

	while(!feof(arq)){
		result = fgets(linha, 100, arq)
		if (result == NULL){
			printf("Erro ao ler a Linha\n");
			return;
		}
		//POSSO CHAMAR O ALGORITOMO BOYER-MOORE

	}
};
