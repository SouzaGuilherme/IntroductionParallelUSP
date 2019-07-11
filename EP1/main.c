#include<regex.h>
#include<dirent.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define malloc_max 100
#define dir_name_max 100000
#define word_max 100000
#define text_max 1000000
pthread_mutex_t mutex;
regex_t reg;

typedef struct archive{
    char dir_name[dir_name_max];
	int dir_type;
	int position;
	int contains_line[text_max];
}arq_dir;

typedef struct works{
	int start;
	int end;
	arq_dir *pointer_directory;
}works_threads;

int search_reg(char b[]){   
    if ((regexec(&reg, (char*)b, 0, (regmatch_t *)NULL, 0)) == 0){
        return 1;
    }else{
        return 0;
    }
};

void greep(arq_dir *content_file){
    FILE *arq;
	char line[text_max];
	int number_ocorr=0;
	arq = fopen(content_file->dir_name, "rt");
	if (arq == NULL){
		printf("Erro na abertura do arquivo\n");
		return;
	}
	int g=-1, i=0;
    while(fgets(line, sizeof(line), arq) != NULL){
		g++;
        number_ocorr = search_reg(line);
	    if (number_ocorr == 1){
            content_file->contains_line[i] = g;
            i++;
        }
	}
    content_file->contains_line[i] = -1;
};

void main_threads(void *arg){
    works_threads *task = (works_threads*)arg;
    arq_dir *pointer = task->pointer_directory;
    for(int i = task->start; i < task->end; i++){
        greep(pointer);
		pointer++;
	}
};

int cria_thread(arq_dir *content_file, int number_thread){
    pthread_t threads[number_thread];
	works_threads *task = malloc(number_thread*sizeof(works_threads));
    int division_work = (content_file->position)%(number_thread);
    int offset = -1;
    int pos = content_file->position;
	for(int i = 0; i < number_thread; i++){
		int work_size = (pos / number_thread);
		if (i < division_work) 
			work_size += 1;
        int start = -1;
        if (offset == -1) {
            offset = 0;
            start = 0;
        } else {
            start = offset;
        }
		task[i].start = start;
		int end = offset + (work_size);
		task[i].end = end;
		task[i].pointer_directory = content_file;
        offset = end;

		int rc = pthread_create(&threads[i], NULL, (void*)main_threads, (void*)&task[i]);
      	if (rc){
        	printf("ERROR; return code from pthread_create() is %d\n", rc);
         	return -1;
      	}
        content_file += (end-start);
	}

    for (int i = 0; i < number_thread; ++i) {
        if(pthread_join(threads[i], NULL)){
        	printf("failed to join thread %d\n", i);
            return -1;
        }
	}

    free(task);
    return 0;
};

int list_directory(char *directory, arq_dir *content_file, int number_thread){
    char name_complete[dir_name_max];
    struct dirent *entry;
    DIR *dir;
    if((dir = opendir(directory)) == NULL){
        printf("Nao foi possivel abrir o diretorio\n\n");
		closedir(dir);
        return -1;
    }
    for(;;){
		if ((entry = readdir(dir)) == NULL){
			closedir(dir);
			break;
		}

        if (content_file->position == malloc_max){
            int response = cria_thread(content_file, number_thread);
            if (response == -1){
                printf("Erro na funcao de criação das Threads\n\n");
                exit(1);
            }

            pthread_mutex_lock(&mutex);
            for(int h = 0; h < content_file->position; h++){
                if ((content_file+h)->contains_line[0] != -1){
                    int k=0;
                    while ((content_file+h)->contains_line[k] != -1){
                        printf("%s: ", (content_file+h)->dir_name);
                        printf("%d\n", (content_file+h)->contains_line[k]);
                        k++;
                        if ((content_file+h)->contains_line[k] == 0){
                            break;
                        }
                    }
                }
            }
            content_file->position = 0;
            pthread_mutex_unlock(&mutex);
        }
        
        snprintf(name_complete,sizeof(name_complete),"%s/%s", directory, entry->d_name);       
        
        if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type == 4)){
            strcpy((content_file+(content_file->position))->dir_name, name_complete);
            (content_file+(content_file->position))->dir_type = entry->d_type;
            (content_file)->position += 1;
			list_directory(name_complete, content_file, number_thread);

		}else if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type != 4)){
            strcpy((content_file+(content_file->position))->dir_name, name_complete);
            (content_file+(content_file->position))->dir_type = entry->d_type;
            (content_file)->position += 1;            
        }
    }
    return 0;
};

int main(int argc, const char *argv[]){
    if (argc < 4) {
        printf("Comando invalido\nComando:\n\n    ./pgrep <numero de threads> <palavra a ser buscada> <diretorio>\n\n");
        exit(1);
    }
    
    int number_thread = atoi(argv[1]);
    char directory[word_max];
    strcpy(directory, argv[3]);

    if (regcomp(&reg , argv[2], REG_EXTENDED|REG_NOSUB) != 0) {
		fprintf(stderr,"erro regcomp\n");
		exit(1);
	}

    arq_dir *content_file = (arq_dir*)malloc(malloc_max*sizeof(arq_dir)); 
    if(content_file == NULL){
        printf("ERRO ao ALOCAR content_file\n");
        exit(1);   
    }
	pthread_mutex_init(&mutex,NULL);
    int respost=0; 
    if((respost=list_directory(directory, content_file, number_thread)) == -1){
        printf("Erro durante a abertura dos diretorios ou Diretorio invalido\n\n");
        exit(1);
    }
    if (content_file->position != 0){
        int response = cria_thread(content_file, number_thread);
        if (response == -1){
            printf("Erro na funcao de criacao das Threads\n\n");
            exit(1);
        }
        pthread_mutex_lock(&mutex);
        for(int h = 0; h < content_file->position; h++){
            if ((content_file+h)->contains_line[0] != -1){
                int k=0;
                while ((content_file+h)->contains_line[k] != -1){
                    printf("%s: ", (content_file+h)->dir_name);
                    printf("%d\n", (content_file+h)->contains_line[k]);
                    k++;
                    if ((content_file+h)->contains_line[k] == 0){
                        break;
                    }
                }
            }
        }
        content_file->position = 0;
        pthread_mutex_unlock(&mutex);
    }

    free(content_file);
}