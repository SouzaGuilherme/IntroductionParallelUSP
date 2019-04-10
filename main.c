#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 100000
typedef unsigned char byte;

typedef struct archive{
	char dir_name[MAX];
	long int dir_type;
	int position;
	char word_search[20];
	int number_words;
}arq_dir;

typedef struct works{
	int start;
	int end;
	arq_dir *pointer_directory;
}works_threads;

void write(arq_dir *content_file, char * name_directory, char name[], int type){
	int point = content_file->position;
	snprintf((content_file+point)->dir_name,sizeof(arq_dir),"%s/%s", name_directory, name);
	(content_file+point)->dir_type = type;
	content_file->position = ((content_file->position)+1);
	return;
};

int openListDirectory(char *name_directory, arq_dir *content_file){
	char entrada[PATH_MAX];
	struct dirent* entry;
	// printf("abrindo name_directory %s\n", name_directory);
	DIR* dir = opendir(name_directory);
	if (dir == NULL){
		// printf("não foi possivel abrir o name_directory %s\n", name_directory);
		return -1;
	}

	for (;;){
		entry = readdir(dir);
		if (entry == NULL){
			// printf("fim do name_directory %s\n", name_directory);
			closedir(dir);
			break;
		}

		snprintf(entrada,sizeof(entrada),"%s/%s", name_directory, entry->d_name);

		if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type == 4)){
			// printf("entrou no dir %s\n",entry->d_name);
			write(content_file, name_directory, entry->d_name, entry->d_type);
			openListDirectory(entrada, content_file);

		}else if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type != 4)){
			write(content_file, name_directory, entry->d_name, entry->d_type);			
		}
	}
	return 0;
};

int boyerMoore(byte a[], int m, byte b[], int n){
	// printf("To no boyer\n");
	// printf("TEXTO; %s\n", a);
	// printf("Palavra:%d\n", m);
	// printf("PALAVRA:%s\n", b);
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

void greep(arq_dir *content_file){
	// printf("ENTROU NO Greep\n");
	FILE *arq;
	char linha[100];
	int number_ocorr=0;
	// printf("Tipo arquivo: %d\n", content_file->dir_type);
	if(content_file->dir_type != 4){
		arq = fopen(content_file->dir_name, "r");
		if (arq == NULL){
			printf("Erro na abertura do arquivo\n");
			return;
		}
		// printf("ABRIU ARQUIVO\n");
		char ch;
		int g=0;
		while((ch = fgetc(arq)) != EOF){
			linha[g] = ch;
			g++;
		}
		//POSSO CHAMAR O ALGORITOMO BOYER-MOORE
		// printf("Chega aqui?\n");
		char teste[20];
		strcpy(teste, (content_file->word_search));
		// int sizeWord = strlen(teste);
		// printf("AI->:%d\n", strlen(linha));
		// printf("AI->:%s\n", content_file->word_search);
		// printf("Antes de entrar no Boyer\n");
		number_ocorr = boyerMoore(content_file->word_search, (strlen(teste)-1), linha, (strlen(linha)-1));
		content_file->number_words = number_ocorr;
		// printf("NUMERO DE OCORRENCIA:%d\n", content_file->number_words);
	}
};

void mainThreads(void  *arg){
	// printf("TO mainThreads\n");
	works_threads *task = (works_threads*) arg;
	// printf("%d\n", task->start);
	// printf("%d\n", task->end);

	// (*(task->pointer_directory))+= task->start;

	for(int i = task->start; i < task->end; i++){
		// printf("Antes o greep\n");
		greep(task->pointer_directory);
		// printf("OLHA AQUI GIULIANO %s\n", task->pointer_directory->dir_name);
		(task->pointer_directory)++;
		//Eu mechi aqui era *(task->pointer_directory)++
	}
};

void printa(arq_dir *aham){
	for(int j = 0; j < (aham->position); j++){
		printf("%d-------> %p\n",j, (aham+j)->dir_name);
		printf("%d\n", (aham+j)->dir_type);		
	}
};

int main(int argc, char const *argv[]){
	if (argc != 4){
		printf("Numero de argumentos incorreto\nModo de uso:\n\n	./pgrep <diretorio> <numero de Threads> <palvra a ser buscada>\n\n");
		exit(1);
	}
	// List Directory
	char nameDirectorySpecific[200];
	char word[20];
	strcpy(word, argv[3]);
	strcpy(nameDirectorySpecific, argv[1]);
	arq_dir *content_file;
	content_file = (arq_dir*)malloc(MAX*sizeof(struct dirent));
	if (content_file == NULL) {
		printf("Error\n");
		exit(1);
	}
	// content_file->word_search = word;
	int resp = openListDirectory(nameDirectorySpecific, content_file);
	if (resp == -1) {
		printf("Diretorio Não encontrado\n");
		exit(1);	
	}
	
	for(int m=0; m<content_file->position; m++){
		strcpy((content_file+m)->word_search, word);
		// printf("%s\n", (content_file+m)->word_search);
	}
	printa(content_file);
	// Create Threads
	pthread_t threads[atoi(argv[2])];
	works_threads *task = malloc(atoi(argv[2])*sizeof(works_threads));
	int divisionWork = (content_file->position)%(atoi(argv[2]));
	for(int i = 0; i < atoi(argv[2]); i++){
		int workSize = (content_file->position)/(atoi(argv[2]));
		// printf("%d\n", workSize);
		if (i < divisionWork) 
			workSize += 1;
		int start = i * workSize;
		task[i].start = start;
		// printf("%d\n", task[i].start);
		int end = (i+1) * workSize;
		task[i].end = end;
		// printf("%d\n", task[i].end);
		
		task[i].pointer_directory = (content_file+start+start);
		// printf("ALOUUUU->>%p\n", task[i].pointer_directory);
		// printa(task[i].pointer_directory);
		// printf("AHAM:%p\n", content_file);
		// printf("--------THREAD[%d]--------\n",i);	
		int rc = pthread_create(&threads[i], NULL, (void*)mainThreads, (void*)&task[i]);
      	if (rc){
        	printf("ERROR; return code from pthread_create() is %d\n", rc);
         	exit(-1);
      	}

		// for(int h=0; h<end; h++){
		// 	content_file++;
		// }
		// content_file+end;
		// printf("AQUI AQUI ---->>>%s\n", content_file->dir_name);
		// content_file+=(task[i].end);
		// printf("%p\n", content_file);
	}
	// printa(content_file);
	// printf("antes de matar as threads\n");
	for (int i = 0; i < atoi(argv[2]); ++i) {
        if(pthread_join(threads[i], NULL))
        	printf("failed to join thread %d\n", i);
	}
	// pthread_exit(NULL);
	// printf("ta aqui\n");
	for(int m=0; m<content_file->position; m++){
		if((content_file+m)->number_words != 0){
			printf("%s : %d\n",(content_file+m)->dir_name, (content_file+m)->number_words);
		}
	}
	printf("ZERO\n");
	free(content_file);
	free(task);
};
