#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 10000

typedef struct archive{
	char dir_name[MAX];
	int dir_type;
	int position;
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

void *mainThreads(void  *arg){
	worksThreads *task = (worksThreads*) arg;
	printf("%d\n", task->start);
	printf("%d\n", task->end);
	// for(int i = task->start; i < task->end; i++){
	// 	printf("%s\n", task->pointerDirectory->dir_name);
	// 	(task->pointerDirectory) += i;
	// }
	for(int i = 0; i < 29; i++){
		printf("%s\n", task->pointerDirectory->dir_name);
		*(task->pointerDirectory)++;
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
	strcpy(nameDirectorySpecific, argv[1]);
	arqDir *contentFile;
	contentFile = (arqDir*)malloc(MAX*sizeof(struct dirent));
	if (contentFile == NULL) {
		printf("Error\n");
		exit(1);
	}
	openListDirectory(nameDirectorySpecific, contentFile);
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
		printf("%p\n", contentFile);
		int rc = pthread_create(&threads[i], NULL, (void*)mainThreads, (void*)&task[i]);
      	if (rc){
        	printf("ERROR; return code from pthread_create() is %d\n", rc);
         	exit(-1);
      	}
		// contentFile+=(task[i].end);
		// printf("%p\n", contentFile);
	}
	// printa(contentFile);
	pthread_exit(NULL);
};
