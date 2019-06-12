#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<assert.h>
#include<unistd.h>
/* FLAG check error */
#define MPI_SUCCESS 0

/* Macro error, copy the Matheus Tavares nd Giuliano Belinasse */
#define DIE(...){ \
    fprintf(stderr, __VA_ARGS__);\
    exit(EXIT_FAILURE);\
}

static void do_MPI(int num_process, int task_id){
    if(task_id == 0){
        printf("PAI func\n");
        sleep(1);
    }

    //MPI_Barrier(MPI_COMM_WORLD);
    sleep(1);
    printf("Todos %d\n", task_id);
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

    printf("PASSOU\n");

    do_MPI(num_process, task_id);

    if(task_id == 0)
        printf("PAI\n");
    else
        printf("FILHO\n");

    MPI_Finalize();
    return 0;
}
