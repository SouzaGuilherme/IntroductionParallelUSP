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
    int start, end, *vector;
};

static void master(int num_process, int width, int height){
    /* To distribute the services for process/machine */
    /* This father */
    printf("VEIO PAI\n");
    struct task tasks[num_process];
    for(int a = 0; a< num_process; a++){
        tasks[a].vector = malloc(width*height*sizeof(int));
        if(tasks[a].vector == NULL)
            DIE("Err, Malloc to buffer\n");
    }
    int err = 0;

    printf("ALOCOU VETOR\n");

    // The work is being splitted as evenly as possible between processes.
    int process_with_one_more_work = (width * height) % num_process;
    printf("process_with_one_more_work==> %d\n", process_with_one_more_work);
    for (int i = 0; i < num_process; ++i){
        int work_size = (width * height) / num_process;
        printf("work_size ==> %d\n", work_size);
        if (i < process_with_one_more_work)
            work_size += 1;

        tasks[i].start = i * work_size;
        tasks[i].end = (i + 1) * work_size;
        printf("start ==> %d\n", tasks[i].start);
        printf("end ==> %d\n", tasks[i].end);
        err |= MPI_Send(&tasks[i],   // Buffer to send
                sizeof(struct task), // How many elements. Note that there is a hack here.
                MPI_INT,            // Type of element. Note that there is a hack here.
                i+1,                 // For which process
                TAG_TASK,            // Tag of the message. Remember that TCP do not guarantee order.
                MPI_COMM_WORLD       // In the context of the entire world.
                );
        printf("DANDO OS TRABALHOS\n");
    }
    // Gather the results
    printf("NEM CHEGOU AQUI\n");
    struct task *acess_buffer;
    for(int i = 0; i < num_process; i++){
        if(i == 0){
            err |= MPI_Recv(&acess_buffer, // Buffer to write to. You must ensure that the message fits here.
                    1,          // How many elements.
                    MPI_INT, // Type.
                    i+1,        // From which process
                    TAG_RESULT, // Tag of message. Again, remember that TCP do not guarantee order.
                    MPI_COMM_WORLD, // In the context of the entire world.
                    MPI_STATUS_IGNORE // Ignore the status return.
                    );
            printf("PASS 1\n");
            if (err)
                DIE("There was an MPI error in the master.\n");
        }else{
            struct task *fodase;
            err |= MPI_Recv(&fodase, // Buffer to write to. You must ensure that the message fits here.
                    1,          // How many elements.
                    MPI_INT, // Type.
                    i+1,        // From which process
                    TAG_RESULT, // Tag of message. Again, remember that TCP do not guarantee order.
                    MPI_COMM_WORLD, // In the context of the entire world.
                    MPI_STATUS_IGNORE // Ignore the status return.
                    );
                printf("PASS resto\n");
            if (err)
                DIE("There was an MPI error in the master.\n");
        }
    }
    /* Print color image */
    printf("printa imagem\n");
    for(int d = 0; d < (width*height); d++){
        int i = d / width;
        int j = d % width;

        printf("vetor[%d][%d] ==> %d\n", i, j, acess_buffer->vector[width*i + j]);
    }

    for(int a = 0; a< num_process; a++){
        free(tasks[a].vector);
    }
}


void calc_mandelbrot(struct task *acess, struct task *acess_buffer, int width, int height, float min_real, float min_imag, float max_real, float max_imag){
    printf("MANDELBROT\n");
    for(int i = acess->start; i < acess->end; i++){
        int d = i / width;
        int j = i % width;

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
        printf("ITERATION ==> %d\n", iteration);
        printf("ANTES -> BUFFER[%d] ==> %d \n", (width*d+j), acess->vector[width*d+j]);
        acess->vector[width*d + j] = iteration;
        printf("DEPOIS -> BUFFER[%d] ==> %d \n", (width*d+j), acess->vector[width*d+j]);

    }
    printf("ACABOU MANDEL\n");
}

static void slave(int task_id, int width, int height, float min_real, float min_imag, float max_real, float max_imag){
    /* Son perform service */
    printf("VEIO FILHO\n");
    struct task recv;
    struct task acess_buffer;
    int err = 0;

   printf("VAI PRO MPI_RECV\n"); 
    err |= MPI_Recv(&recv,
            sizeof(struct task),
            MPI_INT,
            0,
            TAG_TASK,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
    );
   printf("PASSOU O MPI_RECV\n"); 

    printf("Process %d recv: %d, %d\n", task_id, recv.start, recv.end);
    calc_mandelbrot(&recv, &acess_buffer, width, height, min_real, min_imag, max_real, max_imag);


    err |= MPI_Send(&acess_buffer,
            sizeof(struct task),
            MPI_INT,
            1,
            TAG_RESULT,
            MPI_COMM_WORLD
    );

    if (err)
        DIE("There was an MPI error in one slave.\n");
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
    int *buffer = (int*)malloc(width * height * sizeof(int));
	if (buffer == NULL) {
		printf("Could not create image buffer\n");
		return 1;
	}

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
