#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>
#define iteration_max 100
/* FLAG check error */
#define MPI_SUCCESS 0
/*These tags are used to identify the messages. TCP does not guarantee that
 *two sent messages sent will be received in the correct order.
 *define copy the Matheus Tavares and Giuliano Belinasse */
#define TAG_TASK    0
#define TAG_RESULT  1

/* Macro error, copy the Matheus Tavares nd Giuliano Belinasse */
#define DIE(...){ \
    fprintf(stderr, __VA_ARGS__);\
    exit(EXIT_FAILURE);\
}

struct task{
  int start, end, *buffer;
};

void master(int num_process, int width, int height){
    printf("Open Master\n\n");

    struct task tasks;
    int err=0;
    err |= MPI_Send(&tasks,   // Buffer to send
            sizeof(struct task), // How many elements. Note that there is a hack here.
            MPI_INT,            // Type of element. Note that there is a hack here.
            0,                 // For which process
            TAG_TASK,            // Tag of the message. Remember that TCP do not guarantee order.
            MPI_COMM_WORLD       // In the context of the entire world.
            );
    if(err != MPI_SUCCESS)
        DIE("primeiro send master erro\n");


    struct task result;

    err |= MPI_Recv(&result, // Buffer to write to. You must ensure that the message fits here.
            sizeof(struct task),          // How many elements.
            MPI_INT, // Type.
            0,        // From which process
            TAG_RESULT, // Tag of message. Again, remember that TCP do not guarantee order.
            MPI_COMM_WORLD, // In the context of the entire world.
            MPI_STATUS_IGNORE // Ignore the status return.
            );

    if(err != MPI_SUCCESS)
        DIE("erro recv master\n");

    printf("first position ==> %d\n\n", result.buffer);
}

void slave(int task_id, int width, int height, float min_real, float min_imag, float max_real, float max_imag){
    printf("Open Son's\n\n");

    struct task recv;
    int err=0;

    err |= MPI_Recv(&recv,
            sizeof(struct task),
            MPI_INT,
            0,
            TAG_TASK,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
    );

    if(err != MPI_SUCCESS)
        DIE("primeiro recv erro slave\n");

    printf("Process %d recv: %d, %d\n", task_id, recv.start, recv.end);
    for(int i=0; i< 10; i++)
       recv.buffer[i] = i+1;

    printf("first position ==> %d\n\n", recv.buffer);

    err |= MPI_Send(&recv,
            1,
            MPI_DOUBLE,
            0,
            TAG_RESULT,
            MPI_COMM_WORLD
    );

    if(err != MPI_SUCCESS)
        DIE("primeiro send erro slave\n");
}

int main(int argc, char *argv[]){
    int err = 0, task_id, num_process;

	if(argc != 10){
		printf("Please specify output file\n");
		return 1;
	}

	/* Set variables */
	float min_real = atof(argv[1]);
	float min_imag = atof(argv[2]);
	float max_real = atof(argv[3]);
	float max_imag = atof(argv[4]);
	int width = atoi(argv[5]);
	int height = atoi(argv[6]);
	char accelerator[50];
	strcpy(accelerator, argv[7]);
	int threads = atoi(argv[8]);
	char file_name[50];
	strcpy(file_name, argv[9]);
	int flag_accelerator = 0;

    /* Alocate buffer for image */
    struct task acess;
    acess.buffer = (int*)malloc(width * height * sizeof(int));
	if (acess.buffer == NULL) {
		printf("Could not create image buffer\n");
		return 1;
	}
    acess.start=10;
    acess.end=9;

    err = MPI_Init(&argc, &argv);
    if(err != MPI_SUCCESS)
        DIE("Not start MPI.\n");

    err = MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    if(err != MPI_SUCCESS)
        DIE("Err int MPI_Comm_Size.\n");

    err = MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    if(err != MPI_SUCCESS)
        DIE("Err in MPI_Comm_rank.\n");

    if(task_id == 0){
        printf("PAI\n");
        master(num_process-1, width, height);
    }else{
        printf("FILHO\n");
        slave(task_id, width, height, min_real, min_imag, max_real, max_imag);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
