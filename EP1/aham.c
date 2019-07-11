#include<dirent.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define malloc_max 1000
#define dir_name_max 100000
void **buffer;
pthread_mutex_t mutex;  // Ver se vou usar
int wait;
typedef struct archive{
    char dir_name[dir_name_max];
	int dir_type;
	int number_words;
}arq_dir;
arq_dir *content_file;

int opem_list_directory(void){
    printf("AQUIIIIII\n\n");
    int quant = buffer[3];
    if (quant == (malloc_max+5)){
        wait=1;
        while (wait == 1){
            sleep(1);
        }
        buffer[3] = 4;   
    }
    printf("%s\n\n", buffer[0]);
    char name_directory_init[dir_name_max];
    strcpy(name_directory_init, buffer[0]);
    printf("OLHA->%s\n",name_directory_init);
	content_file = (arq_dir*)malloc(sizeof(arq_dir));
    if (content_file == NULL){
        printf("Erro ao Alocar content_File\n\n");
        return -1;
    }
    struct dirent *entry;
	DIR* dir = opendir(name_directory_init);
	    if((dir = opendir(name_directory_init)) == NULL){
        printf("Nao foi possivel abrir o diretorio\n\n");
        return -1;
    }
    for(;;){
		if ((entry = readdir(dir)) == NULL){
			closedir(dir);
			break;
		}
        // char aux[dir_name_max];
        // aux[0] = '/';
        // strcat(aux, entry->d_name);
        char *ponteiro = &name_directory_init;
        strcpy(content_file->dir_name, name_directory_init);

        snprintf(name_directory_init,sizeof(name_directory_init),"%s/%s", content_file->dir_name, entry->d_name);
        // strcat(name_directory_init, aux);
    printf("PORRA->%s\n\n", name_directory_init);

        if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type == 4)){
			printf("entrou no dir %s\n",entry->d_name);
            // snprintf((content_file+(content_file->position))->dir_name,sizeof((content_file+(content_file->position))->dir_name),"%s/%s", name_complete);
            // snprintf((content_file+(content_file->position))->word_search,sizeof((content_file+(content_file->position))->word_search),"%s/%s", word_search);
            strcpy(content_file->dir_name, name_directory_init);
            content_file->dir_type = entry->d_type;
            buffer[quant] = content_file;
            buffer[3] += 1;
    printf("%s\n\n", name_directory_init);
    printf("%s\n\n", content_file->dir_name);

            buffer[0] = name_directory_init;
			opem_list_directory();
		}else if ((strcmp(".",entry->d_name)) && (strcmp("..",entry->d_name)) && (entry->d_type != 4)){
			printf("%s\n", entry->d_name);
            // snprintf((content_file+(content_file->position))->dir_name,sizeof((content_file+(content_file->position))->dir_name),"%s/%s", name_complete);
            // snprintf((content_file+(content_file->position))->word_search,sizeof((content_file+(content_file->position))->word_search),"%s/%s", word_search);
            strcpy(content_file->dir_name, name_directory_init);
            content_file->dir_type = entry->d_type;
            buffer[quant] = content_file;
            buffer[3] += 1;            
            buffer[0] = name_directory_init;
		}
    }
    return 0;
};

void produce(void *id){
    int result = opem_list_directory();
    if (result == -1){
        printf("ERRO na LEITURA dos DIRETORIOS\n\n");
        exit(1);
    }
    // Uma hora ira voltar sem ter usado tudo
    printf("ACABOU\n\n\n");
    printf("%d\n\n", buffer[3]);
}

void consumer(void  *id){
    for (;;)
    {
        /* code */
    }
    
}

int main(int argc, const char *argv[]){
    buffer = (void**)malloc(5+malloc_max*sizeof(arq_dir*));
    if (buffer == NULL){
        printf("FAIL ALOCATION");
        exit(1);
    }
    
    char name_directory_initi[dir_name_max];
    char search_word[dir_name_max];
    int number_thread = atoi(argv[2]);
    strcpy(name_directory_initi, argv[1]);
    strcpy(search_word, argv[3]);
    buffer[0] = &name_directory_initi;
    buffer[1] = (int*)number_thread;
    buffer[2] = &search_word;
    buffer[3] = (int*)4;    
    pthread_t produce_one;
    pthread_mutex_init(&mutex,NULL);        //Mutex? Tenho que ver se vou usar!
    int verific = pthread_create(&produce_one, NULL, (void*)produce, NULL);
    if (verific){
    printf("ERROR; return code from pthread_create() is %d\n", verific);
        exit(-1);
    }
    printf("OI\n");
    if(pthread_join(produce_one, (void*)NULL))
        printf("failed to join thread\n");
    // free (buffer);
};