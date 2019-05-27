#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <png.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#define MAX_ITERATION 100

int writeImage(char* filename, int width, int height, float *buffer, char* title);

static void setRGB(png_byte *ptr, float val);

float *in_CPU_parallel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration_max, float *buffer);

float *in_CPU(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration_max, float *buffer);


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

	float *buffer = (float*)malloc(width * height * sizeof(float));
    if (buffer == NULL) {
        printf("Could not create image buffer\n");
        return NULL;
    }

	/* Checks if in CPU or GPU */
	if(accelerator[0] == "c"){
	    if(threads > 1)
	    	omp_set_num_threads(threads);
	    	buffer = in_CPU_parallel(width, height, min_real, min_imag, max_real, max_imag, iteration, buffer);
	    else
	    	buffer = in_CPU(width, height, min_real, min_imag, max_real, max_imag, iteration, buffer);
	}else{ 
		/* Aloco na placa de video */
		cudaMalloc((void**)&d_buffer, width*height*sizeof(float));
	
		/* Setar os bocos para trabalhar na placa de video */
		/* CODE */		
	
		kernel<<<gred_size, block_size>>>(width, height, min_real, min_imag, max_imag, max_real, iteration, d_buffer);
	
		cudaDeviceSynchronize();

		/* Pego o buffer da Placa de video e trago para o PC */
		cudaMemcpy(buffer, d_buffer, width*height*sizeof(float), cudaMemcpyDeviceToHost);	    
	}

    printf("Creating Image\n");
    float *buffer = mandelbrot_image();
	if (buffer == NULL) {
		return 1;
	}

	printf("Saving PNG\n");
	int result = writeImage(file_name, width, height, buffer, "MANDELBROT");

	free(buffer);

	return result;
}

float *in_CPU_parallel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration_max, float *buffer){
    int x_position, y_position;
    float delt_x= (max_real - min_real) / width;
    float delt_y= (max_imag - min_imag) / height;
    float z_x, z_y;
    float z_x2, z_y2;
    int iteration = 0;
    float y_pointer;
    float x_pointer;

#pragma omp parallel for schedule(dynamic,10) private(y_pointer, x_pointer, y_position, x_position, z_x, z_y, z_y2, z_x2, modZ, mu, iteration) collapse(2)
    for (y_position = 0 ; y_position < height ; y_position++){
        for (x_position = 0 ; x_position < width ; x_position++){
            y_pointer = max_imag - delt_y * y_position;
            x_pointer = min_real + delt_x * x_position;

            #pragma omp critical (section1)
            {
	            z_x =x_pointer;
	            z_y = y_pointer;
	            z_x2 = z_x * z_x;
	            z_y2 = z_y * z_y;
	        }

            for(iteration = 0; iteration < iteration_max && ((z_x2+z_y2) < 4); iteration++){
                z_y = 2 * z_x * z_y + y_pointer;
                z_x = z_x2 - z_y2 + x_pointer;
                z_x2 = z_x * z_x;
                z_y2 = z_y * z_y;
            }

            buffer[y_position * width + x_position] = iteration;
        }
    }

    return buffer;
};


float *in_CPU(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration_max, float *buffer){
    int x_position, y_position;
    float minMu = iteration_max;
    float maxMu = 0;
    float delt_x= (max_real - min_real) / width;
    float delt_y= (max_imag - min_imag) / height;
    float z_x, z_y;
    float z_x2, z_y2;
    int iteration = 0;
    float y_pointer;
    float x_pointer;
	float modZ;
	float mu;
    for (y_position = 0 ; y_position < height ; y_position++){
        for (x_position = 0 ; x_position < width ; x_position++){
            y_pointer = max_imag - delt_y * y_position;
            x_pointer = min_real + delt_x * x_position;

	        z_x =x_pointer;
	        z_y = y_pointer;
	        z_x2 = z_x * z_x;
	        z_y2 = z_y * z_y;

            for(iteration = 0; iteration < iteration_max && ((z_x2+z_y2) < 4); iteration++){
                z_y = 2 * z_x * z_y + y_pointer;
                z_x = z_x2 - z_y2 + x_pointer;
                z_x2 = z_x * z_x;
                z_y2 = z_y * z_y;
            }

            buffer[y_position * width + x_position] = iteration;
        }
    }

    return buffer;
};

static void setRGB(png_byte *ptr, float val){

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


int writeImage(char* filename, int width, int height, float *buffer, char* title){
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
