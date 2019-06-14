#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<assert.h>
#include<unistd.h>
#define iteration_max 100
/* FLAG check error */
#define MPI_SUCCESS 0

/* Macro error, copy the Matheus Tavares nd Giuliano Belinasse */
#define DIE(...){ \
    fprintf(stderr, __VA_ARGS__);\
    exit(EXIT_FAILURE);\
}

/* Macro for acess Matrix as vector */
#define buffer(i , j) _buffer[width * i + j]

static void do_MPI(int num_process, int task_id){

/* Macro for acess Matrix as vector */
#define buffer(i , j) _buffer[width * i + j]
    int width = 4;
    int height = 4;
    int index_vector = width*height;
    int vector[index_vector];
    /* To distribute the services for process/machine */
    /* Done for process father */
    if(task_id == 0){
        printf("PAI func\n");
        printf("index_vector ==> %d \n", index_vector);
        for(int g = 0; g < (width*height); g++){
            vector[g] = g;
            printf("vetor[%d] = %d\n", g, g);
        }
        for(int i=0; i< (width*height); i++){
            int d = i / width;
            int j = i % width;
            //printf("D ==> %d \n J ==> %d \n", d, j);
            //printf("vetor[%d][%d] = %d\n", d, j, vector[d+j]);

            float max_real = -1, min_real = -2;
            float max_imag = 1, min_imag = 0;

            float del_x = (max_real - min_real)/width;
            float del_y= (max_imag - min_imag)/height;

            int iteration = 0;

            float x_point = min_real + del_x * j;
            float y_point = max_imag - del_y * d;

            float z_y = y_point;
            float z_x = x_point;
            float z_x2 = z_x * z_x;
            float z_y2 = z_y * z_y;

            for(iteration = 0; iteration < iteration_max && ((z_x2 + z_y2) <= 4); iteration++){
                z_y = (2.0 * z_x * z_y)+ y_point;
                z_x = z_x2 - z_y2 + x_point;
                z_x2 = z_x * z_x;
                z_y2 = z_y * z_y;
            }

            vector[i] = iteration;
        }
    }

    printf("Esperando\n");
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Todos %d\n", task_id);
    if(task_id == 0){
        for(int g = 0; g < (width*height); g++){
            printf("vetor[%d] = %d\n", g, vector[g]);
        }
    }
}

int main(int argc, char *argv[]){
    int err = 0, task_id, num_process;

    err = MPI_Init(&argc, &argv);
    if(err != MPI_SUCCESS)
        DIE("Not start MPI.\n");

    err = MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    if(err != MPI_SUCCESS)
        DIE("Err int MPI_Comm_Size.\n");

    err = MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    if(err != MPI_SUCCESS)
        DIE("Err in MPI_Comm_rank.\n");


    do_MPI(num_process, task_id);

    if(task_id == 0)
        printf("PAI\n");
    else
        printf("FILHO\n");

    MPI_Finalize();
    return 0;
}
