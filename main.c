#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 10000
typedef unsigned char byte;

typedef struct archive{
	char dir_name[MAX];
	int dir_type;
	int position;
	char wordSearch[20];
	int numberWords;
}arqDir;

typedef struct works{
	int start;
	int end;
	arqDir *pointerDirectory;
}worksThreads;

void write(arqDir *contentFile, char * nameDirectory, char name[], int type){
	int point = contentFile->position;
	snprintf((contentFile+point)->dir_name,sizeof(arqDir),"%s/%s", nameDirectory, name);
	(contentFile+point)->dir_type = type;
	contentFile->position = ((contentFile->position)+1);
	return;
};

int openListDirectory(char *nameDirectory, arqDir *contentFile){
	char entrada[PATH_MAX];
	struct dirent* entry;
	// printf("abrindo nameDirectory %s\n", nameDirectory);
	DIR* dir = opendir(nameDirectory);
	if (dir == NULL){
		// printf("não foi possivel abrir o nameDirectory %s\n", nameDirectory);
		return -1;
	}

	for (;;){
		entry = readdir(dir);
		if (entry == NULL){
			// printf("fim do nameDirectory %s\n", nameDirectory);
			closedir(dir);
			break;
		}

		snprintf(entrada,sizeof(entrada),"%s/%s", nameDirectory, entry->d_name);

		if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type == 4)){
			// printf("entrou no dir %s\n",entry->d_name);
			write(contentFile, nameDirectory, entry->d_name, entry->d_type);
			openListDirectory(entrada, contentFile);

		}else if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type != 4)){
			write(contentFile, nameDirectory, entry->d_name, entry->d_type);			
			// printf("%s\n", entry->d_name);
		}
	}
};

int boyerMoore(byte a[], int m, byte b[], int n){
	// printf("To no boyer\n");
	printf("TEXTO; %s\n", a);
	// printf("Palavra:%d\n", m);
	printf("PALAVRA:%s\n", b);
	// printf("Texto:%d\n", n);

	int ult[256]; // o alfabeto é 0..255

   // pré-processamento da palavra a
   for (int f = 0; f < 256; ++f) ult[f] = 0;
   for (int i = 1; i <=  m; ++i) ult[a[i]] = i;

   // busca da palavra a no texto b
   int ocorre = 0;
   int k = m;
   while (k <= n) {
      // a[1..m] casa com b[k-m+1..k]?
      int i = m, j = k;
      while (i >= 1 && a[i] == b[j]) 
         --i, --j;   
      if (i < 1) ++ocorre;
      if (k == n) k += 1;
      else k += m - ult[b[k+1]] + 1;
   }
   return ocorre;
};	

void greep(arqDir *contentFile){
	printf("ENTROU NO Greep\n");
	FILE *arq;
	int i=0;
	char linha[100];
	char *result;
	int numberOcorr=0;
	printf("Tipo arquivo: %d\n", contentFile->dir_type);
	if(contentFile->dir_type != 4){
		arq = fopen(contentFile->dir_name, "r");
		if (arq == NULL){
			printf("Erro na abertura do arquivo\n");
			return;
		}
		printf("ABRIU ARQUIVO\n");
		char ch;
		int g=0;
		while((ch = fgetc(arq)) != EOF){
			linha[g] = ch;
			g++;
		}
		//POSSO CHAMAR O ALGORITOMO BOYER-MOORE
		printf("Chega aqui?\n");
		char teste[20];
		strcpy(teste, (contentFile->wordSearch));
		int sizeWord = strlen(teste);
		printf("AI->:%d\n", strlen(linha));
		printf("AI->:%s\n", contentFile->wordSearch);
		printf("Antes de entrar no Boyer\n");
		numberOcorr = boyerMoore(contentFile->wordSearch, (strlen(teste)-1), linha, (strlen(linha)-1));
		contentFile->numberWords = numberOcorr;
		printf("NUMERO DE OCORRENCIA:%d\n", contentFile->numberWords);
	}
};

void *mainThreads(void  *arg){
	printf("TO mainThreads\n");
	worksThreads *task = (worksThreads*) arg;
	printf("%d\n", task->start);
	printf("%d\n", task->end);
	
	for(int i = task->start; i < task->end; i++){
		printf("Antes o greep\n");
		greep(task->pointerDirectory);
		printf("%s\n", task->pointerDirectory->dir_name);
		printf("passei o greep\n");
		*(task->pointerDirectory)++;
		printf("passei o greep\n");
	}
};

void* printa(arqDir *aham){
	// Testing
	for(int j = 0; j < (aham->position); j++){
		printf("%d-------> %s\n",j, (aham+j)->dir_name);
		printf("%d\n", (aham+j)->dir_type);		
	}
};

int main(int *argc, char const *argv[]){
	// printf("%s\n", argv[1]);
	// List Directory
	char nameDirectorySpecific[200];
	char word[20];
	strcpy(word, argv[3]);
	strcpy(nameDirectorySpecific, argv[1]);
	arqDir *contentFile;
	contentFile = (arqDir*)malloc(MAX*sizeof(struct dirent));
	if (contentFile == NULL) {
		printf("Error\n");
		exit(1);
	}
	// contentFile->wordSearch = word;
	openListDirectory(nameDirectorySpecific, contentFile);
	for(int m=0; m<contentFile->position; m++){
		strcpy((contentFile+m)->wordSearch, word);
		printf("%s\n", (contentFile+m)->wordSearch);
	}
	// Create Threads
	pthread_t threads[atoi(argv[2])];
	worksThreads *task = malloc(atoi(argv[2])*sizeof(worksThreads));
	int divisionWork = (contentFile->position)%(atoi(argv[2]));
	for(int i = 0; i < atoi(argv[2]); i++){
		int workSize = (contentFile->position)/(atoi(argv[2]));
		printf("%d\n", workSize);
		if (i < divisionWork) 
			workSize += 1;
		int start = i * workSize;
		task[i].start = start;
		printf("%d\n", task[i].start);
		int end = (i+1) * workSize;
		task[i].end = end;
		printf("%d\n", task[i].end);
		task[i].pointerDirectory = contentFile;
		printf("AHAM:%p\n", contentFile);
		int rc = pthread_create(&threads[i], NULL, (void*)mainThreads, (void*)&task[i]);
      	if (rc){
        	printf("ERROR; return code from pthread_create() is %d\n", rc);
         	exit(-1);
      	}
		// contentFile+=(task[i].end);
		// printf("%p\n", contentFile);
	}
	// printa(contentFile);
	printf("antes de matar as threads\n");
	for (int i = 0; i < atoi(argv[2]); ++i) {
        if(pthread_join(threads[i], NULL))
        	printf("failed to join thread %d\n", i);
	}
	// pthread_exit(NULL);
	printf("ta aqui\n");
	for(int m=0; m<contentFile->position; m++){
		if((contentFile+m)->numberWords != 0){
			printf("%s : %d\n",(contentFile+m)->dir_name, (contentFile+m)->numberWords);
		}
	}
	printf("ZERO\n");
};
