#include <stdio.h>
#include <math.h>
#include <png.h>
#include <string.h>
#include <stdlib.h>
#define MAX_ITERATION 100
#define CEIL(a, b) (((a) + (b) - 1)/(b)) 

__global__ void kernel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration, float *buffer);

int writeImage(const char* filename, int width, int height, float *buffer, const char* title);

static void setRGB(png_byte *ptr, float val);

#define CEIL(a, b) (((a) + (b) - 1)/(b)) 

/* ./exec <min_real> <min_imag> <max_real> <max_imag> <W> <H> <CPU/GPU> <Treadhs> <Saida> */
int main(int argc, char *argv[]){

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
	int iteration = MAX_ITERATION;
	float *buffer;
	float *d_buffer;


	/* Aloco no PC */
	buffer = (float*)malloc(width*height*sizeof(float));
	
	/* Aloco na placa de video */
	cudaMalloc(&d_buffer, width*height*sizeof(float));
	
	/* Setar os bocos para trabalhar na placa de video */
	int threads_per_block = threads;
	int num_blocks = CEIL((width*height), threads_per_block);
		
    	/* Se eu chamar a funcao em cuda */
	kernel<<<num_blocks, threads_per_block>>>(width, height, min_real, min_imag, max_imag, max_real, iteration, d_buffer);
	/* Passei N blocos, sendo uma x threads por bloco */
	
	cudaDeviceSynchronize();

	/* Pego o buffer da Placa de video e trago para o PC */
	cudaMemcpy(buffer, d_buffer, width*height*sizeof(float), cudaMemcpyDeviceToHost);
	
	/* So printa a imagem */
	printf("Saving PNG\n");
	int result = writeImage(file_name, width, height, buffer, "MANDELBROT");
	

	/* Free no buffer na placa de video */
	cudaFree(buffer);
	/* Free no buffer no PC */
	free(buffer);

	return 0;
}

__global__ void kernel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration_max, float *buffer){

	int index_vector = (blockIdx.x * blockDim.x) + threadIdx.x;
	int i = index_vector / width;
	int j = index_vector % width;
	float del_x = (max_real - min_real)/width;
	float del_y= (max_imag - min_imag)/height;
	float z_y = 0.0;
	float z_x = 0.0; 
	int iteration = 0;
	float xtemp;
	float x_point = min_real + del_x * j;
	float y_point = max_imag - del_y * i;
	
	while((((z_x * z_x) + (z_y * z_y)) <= 4) && (iteration < iteration_max)){
		xtemp = ((z_x * z_x) - (z_y * z_y)+x_point);
		z_y = (2.0 * z_x * z_y)+ y_point;
		z_x = xtemp;
		iteration++;
	}
	
	buffer[index_vector] = iteration;
};

__host__ static void setRGB(png_byte *ptr, float val){

	int v = 255 - (int)(val/MAX_ITERATION) * 255;
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


__host__ int writeImage(const char* filename, int width, int height, float *buffer, const char* title){
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
