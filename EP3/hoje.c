#include<omp.h>
#include<png.h>
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
/* Macro for divison correct for cuda */
#define CEIL(a, b) (((a) + (b) - 1)/(b))

struct task{
    int start, end;
};

static void setRGB(png_byte *ptr, float val);

int writeImage(const char* filename, int width, int height, int *buffer);

void master(int num_process, int width, int height){
    printf("Sou o PAI e estou no MASTER\n");
    int err = 0;
    struct task *acess = malloc(num_process*sizeof(struct task));
    if(acess == NULL)
        DIE("Error in malloc acess MASTER\n");


    /* move result gather in slave for buffer */
    int *buffer = malloc(width * height * sizeof(int));
    if(buffer == NULL)
        DIE("Error in malloc Buffer MASTER\n");


    /* To distribute the services for process/machine */
    int process_with_one_more_work = (width * height) % num_process;
    printf("SOBRA=>>> %d\n\n", process_with_one_more_work);
    int work_size = (width * height) / num_process;
    for(int i=0; i < num_process; i++){
        if(i == (num_process-1)){
            printf("\n\n\nTRABALHO a MAIS\n\n\n");
            (acess+i)->start = i * work_size;
            (acess+i)->end = (((i + 1) * (work_size)) + process_with_one_more_work);
       }else{
            (acess+i)->start = i * work_size;
            (acess+i)->end = (i + 1) * work_size;
        }
        /* For testing */
        printf("Start-MASTER= %d --- End-MASTER= %d ---",
                (acess+i)->start, (acess+i)->end);

        err |= MPI_Send((acess+i),
                sizeof(struct task),
                MPI_CHAR,
                i+1,
                TAG_TASK,
                MPI_COMM_WORLD);

        if(err != MPI_SUCCESS)
            DIE("Err Send -- MASTER.\n");
    }

    /* Gather the results */
    printf("NUM_PROCESS ==> %d\n\n", num_process);
    for(int i = 0; i < num_process; i++){
        int tam_vetor = ((acess+i)->end - (acess+i)->start);
        int *vetor = malloc(tam_vetor * sizeof(int));
        printf("TAMANHO VETOR PEDACO MASTER => %d\n", ((acess+i)->end - (acess+i)->start));
        if(vetor == NULL)
            DIE("vector malloc in master error\n");

        err |= MPI_Recv(vetor,
            (sizeof(int) * tam_vetor),
            MPI_CHAR,
            i+1,
            TAG_RESULT,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        if(err != MPI_SUCCESS)
            DIE("Err recv -- MASTER\n");

        printf("IMPRIMI NA MASTER\n");
        int correct_vetor = 0;
        for(int a = (acess+i)->start; a < (acess+i)->end; a++){
            *(buffer+a) = *(vetor+correct_vetor);
            //printf("MASTER -- vetor[%d] = %d\n", a, *(buffer+a));
            correct_vetor++;
        }
        free(vetor);
        printf("FOI\n\n\n");
    }

	printf("Saving PNG\n");
	int result = writeImage("IMAGEM_MANDEL", width, height, buffer);
    free(buffer);
}

/* Calc_mandelbrot sequential */
/*void calc_mandelbrot(int start, int end, int *vetor, int width, int height, float min_real, float min_imag, float max_real, float max_imag){
    int correct_vetor = 0;
    for(int index_vector = start; index_vector < end; index_vector++){
         int i = index_vector / width;
         int j = index_vector % width;
         float del_x = (max_real - min_real)/width;
         float del_y= (max_imag - min_imag)/height;
         int iteration = 0;
         float x_point = min_real + del_x * i;
         float y_point = max_imag - del_y * j;
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
         printf("BOA MANDEL\n");
         *(vetor+correct_vetor) = iteration;
         correct_vetor++;

     }
 }*/


/* Calc_mandelbrot Parallel OPEN_MP */
void calc_mandelbrot(int start, int end, int *vetor, int width, int height, float min_real, float min_imag, float max_real, float max_imag){
    int index_vector, correct_vetor = 0, i, j, iteration = 0;
    float del_x, del_y, x_point, y_point, z_y, z_x, z_y2, z_x2;

#pragma omp parallel for schedule(dynamic,1) private(i,j,index_vector, del_x, del_y, x_point, y_point, z_y2, z_x2, z_y, z_x, iteration)
    // private(index_vector, x_point, y_point, z_y, z_x, z_x2, z_y2, iteration, i, j, correct_vetor) 
    //i, j, del_x, del_y, x_point, y_point, z_y, z_x, z_x2, z_y2, iteration, correct_vetor)
    for(index_vector = start; index_vector < end; index_vector++){
            i = index_vector / width;
            j = index_vector % width;
            del_x = (max_real - min_real)/width;
            del_y= (max_imag - min_imag)/height;
            x_point = min_real + del_x * i;
            y_point = max_imag - del_y * j;
            z_y = y_point;
            z_x = x_point;
            z_x2 = z_x * z_x;
            z_y2 = z_y * z_y;
            for(iteration = 0; iteration < iteration_max && ((z_x2 + z_y2) <= 4); iteration++){
                z_y = (2.0 * z_x * z_y)+ y_point;
                z_x = z_x2 - z_y2 + x_point;
                z_x2 = z_x * z_x;
                z_y2 = z_y * z_y;
            }
#pragma omp critical (section1)
      {
        printf("BOA MANDEL\n");
        *(vetor+correct_vetor) = iteration;
        correct_vetor++;
        }
    }
}

void slave(int task_id, int width, int height, float min_real, float min_imag, float max_real, float max_imag){
    printf("Sou Processo %d e Estou no SLAVE\n", task_id);
    int err = 0;
    struct task *recv;
    err |= MPI_Recv(recv,
            sizeof(struct task),
            MPI_CHAR,
            0,
            TAG_TASK,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        if(err != MPI_SUCCESS)
            DIE("Err recv -- SLAVE.\n");

        int tam_vetor = (recv->end - recv->start);

        /* For testing */
        printf("Start-SLAVE= %d --- End-SLAVE= %d ---",
                recv->start, recv->end);

        int *vetor = malloc(tam_vetor * sizeof(int));
        if(vetor == NULL)
            DIE("vector malloc in slave error\n");

        printf("TAMANHO VETOR PEDACO SLAVE => %d\n", (recv->end - recv->start));

        /*for(int i = recv.start; i < recv.end; i++)
            vetor[i] = i+1;*/
        calc_mandelbrot(recv->start, recv->end, vetor, width, height, min_real, min_imag, max_real, max_imag);
        //for(int i = 0; i < tam_vetor; i++)
          //  printf("SLAVE-PROCESS %d -- vetor[%d] = %d\n", task_id, i,*(vetor+i));

        printf("PASSOU ----------------------------------\n\n");

        err |= MPI_Send(vetor,
                (sizeof(int) * tam_vetor),
                MPI_CHAR,
                0,
                TAG_RESULT,
                MPI_COMM_WORLD);

        if(err != MPI_SUCCESS)
            DIE("Err Send -- SLAVE.\n");

        free(vetor);
        printf("ACABOU SLAVE\n\n");
}

int main(int argc, char *argv[]){
    int err = 0, task_id, num_process;
    int height = 300, width = 300;
    float min_real = -2, max_real = -1;
    float min_imag = 0, max_imag = 1;

    err = MPI_Init(&argc, &argv);
    if(err != MPI_SUCCESS)
        DIE("Not start MPI.\n");

    err = MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    if(err != MPI_SUCCESS)
        DIE("Err int MPI_Comm_Size.\n");

    err = MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    if(err != MPI_SUCCESS)
        DIE("Err in MPI_Comm_rank.\n");

    /* I guarantee that num_process is not larger than the image */
    if((num_process-1) > (width*height))
        num_process = ( width * height)+1;

    if(task_id == 0){
        printf("PAI\n");

        master((num_process-1), width, height);
    }else if(task_id <= (num_process-1)){
        printf("FILHO\n");
        slave(task_id, width, height, min_real, min_imag, max_real, max_imag);
    }

    printf("ANTES BARRIER\n");
    MPI_Barrier(MPI_COMM_WORLD);
    printf("DPS BARRIER\n");
    printf("ANTES FINALIZE\n");
    MPI_Finalize();
    printf("DPS FINALIZE\n");
    printf("MORRE\n\n");
    return 0;
}


static void setRGB(png_byte *ptr, float val){

	int v = 255 - (int)(val/iteration_max) * 255;
	if(v == 0){
		ptr[0] = v;
		ptr[1] = v;
		ptr[2] = v;
	}else{
		if(val < 10){
			ptr[0] = 192;
			ptr[1] = 217;
			ptr[2] = 217;
		}else if(val < 15){
			ptr[0] = 95;
			ptr[1] = 159;
			ptr[2] = 159;
		}else if(val < 25){
			ptr[0] = 0;
			ptr[1] = 255;
			ptr[2] = 255;
		}else if(val < 50){
			ptr[0] = 255;
			ptr[1] = 0;
			ptr[2] = 255;
		}else if(val < 75){
			ptr[0] = 234;
			ptr[1] = 173;
			ptr[2] = 234;
		}else{
			ptr[0] = 79;
			ptr[1] = 47;
			ptr[2] = 79;
		}
	}
};


int writeImage(const char* filename, int width, int height, int *buffer){
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;

	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		printf( "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		printf( "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		printf( "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf( "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y=0 ; y<height ; y++) {
		for (x=0 ; x<width ; x++) {
			setRGB(&(row[x*3]), buffer[y*width + x]);
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return code;
};
