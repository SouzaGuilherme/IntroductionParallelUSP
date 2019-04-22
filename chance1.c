#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define path_name_max 3000
#define path_lane_max 1000
#define word_max 20
#define dir_name_max 10000
#define lane_max 100000

typedef unsigned char byte;

typedef struct archive{
    char dir_name[dir_name_max];
	int dir_type;
	int position;
	int lane[lane_max];
    char word_search[word_max];
}arq_dir;

int search_word(byte a[], int m, byte b[], int n){
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
}

int open_archive(char *nome, char *word_search_text){
    char name_archive[path_name_max];
    strcpy(name_archive, nome);
    char word[path_name_max];
    strcpy(word, word_search_text);
    char text_search[path_lane_max];
    int number_lane=-1, awnser=0;
    FILE *arq;

    arq = fopen(name_archive, "r");
    if (arq == NULL){
        printf("Not posible read archive");
    }else{
        while ((fgets(text_search, sizeof(text_search), arq)) != NULL){
            number_lane++;
            awnser = search_word((byte*)word, (strlen(word)-1), (byte*)text_search, (strlen(text_search)-1));
            if (awnser == 1){
                // Printa path e lane
                printf("Linha: %d || Texto:%s", number_lane, text_search);
                content_file->lane[number_lane] = number_lane;
            }
        }    
    }
    return 0;        
};

int main(int argc, const char *argv[]){
    char nome[]="/home/souza/Documents/Paralell/EP1/teste/arq";
    char word[]="teste";
    open_archive(nome, word);
}