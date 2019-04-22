#include<dirent.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define malloc_max 100
#define dir_name_max 100000
#define word_max 100000
#define text_max 1000000
typedef unsigned char byte;

typedef struct archive{
    char dir_name[dir_name_max];
	int dir_type;
	int position;
	int lane[text_max];
    char word_search[word_max];
}arq_dir;

typedef struct works{
	int start;
	int end;
	arq_dir *pointer_directory;
}works_threads;

pthread_mutex_t mutex;

int boyerMoore(byte a[], int m, byte b[], int n){
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
    if (ocorre > 0)
        return 1;
    else
        return 0;
};

void greep(arq_dir *content_file){
    FILE *arq;
	char linha[text_max];
	int number_ocorr=0;
    char teste[20];
	strcpy(teste, (content_file->word_search));
	// if(content_file->dir_type != 4){
		arq = fopen(content_file->dir_name, "rt");
		if (arq == NULL){
			printf("Erro na abertura do arquivo\n");
			return;
		}
		int g=-1, i=0;
        while(fgets(linha, sizeof(linha), arq) != NULL){
			g++;
            number_ocorr = boyerMoore((byte*)teste, (strlen(teste)-1), (byte*)linha, (strlen(linha)-1));
		    if (number_ocorr == 1){
                content_file->lane[i] = g;
                i++;
            }
		}
        content_file->lane[i] = -1;
	// }
};

void main_threads(void *arg){
    works_threads *task = (works_threads*)arg;
    arq_dir *pointer = task->pointer_directory;
    if (task->end == 0 && task->start == 0)
        greep(pointer);        
    
    for(int i = task->start; i < task->end; i++){
        greep(pointer);
		pointer++;
	}
};

int cria_thread(arq_dir *content_file, int number_thread){
    pthread_t threads[number_thread];
	works_threads *task = malloc(number_thread*sizeof(works_threads));
    int division_work = (content_file->position)%(number_thread);
	for(int i = 0; i < number_thread; i++){
		int work_size = (content_file->position)/(number_thread);
		if (i < division_work) 
			work_size += 1;
		int start = i * work_size;
		task[i].start = start;
		int end = (i+1) * work_size;
		task[i].end = end;
		task[i].pointer_directory = content_file;
        
		int rc = pthread_create(&threads[i], NULL, (void*)main_threads, (void*)&task[i]);
      	if (rc){
        	printf("ERROR; return code from pthread_create() is %d\n", rc);
         	return -1;
      	}
        if (start == 0 && end == 0){
            content_file++;
        }else{
            for (int p = 0; p < end+1; p++){
                content_file++;
            }
        }
	}

    for (int i = 0; i < number_thread; ++i) {
        if(pthread_join(threads[i], NULL)){
        	printf("failed to join thread %d\n", i);
            return -1;
        }
	}

    free(task);
    // pthread_exit(NULL);
    return 0;
};

int list_directory(char *directory, arq_dir *content_file, char *word_search, int number_thread){
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
            printf("TO AQUI DENOVO\n");
            int response = cria_thread(content_file, number_thread);
            if (response == -1){
                printf("DEU MERDA\n\n");
                exit(1);
            }
            printf("PASSA?");
            // Volto
            // Printo (isso tem que ser uma secao critica)
            pthread_mutex_lock(&mutex);
            for(int h = 0; h < content_file->position; h++){
                if ((content_file+h)->lane[0] != -1){
                    printf("%s\n", (content_file+h)->dir_name);
                    int k=0;
                    while ((content_file+h)->lane[k] != -1){
                        printf("Number lane: %d\n", (content_file+h)->lane[k]);
                        k++;
                        if ((content_file+h)->lane[k] == 0){
                            break;
                        }
                    }
                    printf("Type: %d\n", (content_file+h)->dir_type);
                    printf("%d\n\n", h);
                }
            }
            content_file->position = 0;
            pthread_mutex_unlock(&mutex);
        }
        
        snprintf(name_complete,sizeof(name_complete),"%s/%s", directory, entry->d_name);       
        
        if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type == 4)){
            strcpy((content_file+(content_file->position))->dir_name, name_complete);
            strcpy((content_file+(content_file->position))->word_search, word_search);            
            (content_file+(content_file->position))->dir_type = entry->d_type;
            (content_file)->position += 1;
			list_directory(name_complete, content_file, word_search, number_thread);

		}else if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type != 4)){
            strcpy((content_file+(content_file->position))->dir_name, name_complete);
            strcpy((content_file+(content_file->position))->word_search, word_search);                        
            (content_file+(content_file->position))->dir_type = entry->d_type;
            (content_file)->position += 1;            
        }
    }
    return 0;
};

int main(int argc, const char *argv[]){
    if (argc < 4) {
        printf("Comando invalido\nComando:\n\n    ./pgrep <diretorio> <numero de threads> <palavra a ser buscada>\n\n");
        exit(1);
    }
    int number_thread = atoi(argv[2]);
    char directory[word_max];
    char word_search[word_max];
    strcpy(directory, argv[1]);
    strcpy(word_search, argv[3]);

    arq_dir *content_file = (arq_dir*)malloc(malloc_max*sizeof(arq_dir)); 
    if(content_file == NULL){
        printf("ERRO ao ALOCAR content_file\n");
        exit(1);   
    }

	pthread_mutex_init(&mutex,NULL);

    int respost=0; 
    if((respost=list_directory(directory, content_file, word_search, number_thread)) == -1){
        printf("Erro durante a abertura dos diretorios ou Diretorio invalido\n\n");
        exit(1);
    }
    //Volto porem ainda tem coisa dentro
    
    if (content_file->position != 0){
        // Chamo a funcao que cria thread
        int response = cria_thread(content_file, number_thread);
        if (response == -1){
            printf("DEU MERDA\n\n");
            exit(1);
        }
        // Volto
        // Printo (isso tem que ser uma secao critica)
        pthread_mutex_lock(&mutex);
        for(int h = 0; h < content_file->position; h++){
            if ((content_file+h)->lane[0] != -1){
                printf("%s\n", (content_file+h)->dir_name);
                int k=0;
                while ((content_file+h)->lane[k] != -1){
                    printf("Number lane: %d\n", (content_file+h)->lane[k]);
                    k++;
                    if ((content_file+h)->lane[k] == 0){
                        break;
                    }
                }
                printf("Type: %d\n", (content_file+h)->dir_type);
                printf("%d\n\n", h);   
            }
        }
        content_file->position = 0;
        pthread_mutex_unlock(&mutex);
    }

    // pthread_exit(NULL);
    free(content_file);
}