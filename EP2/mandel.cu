#include <stdio.h>
#include <math.h>
#include <png.h>
#include <string.h>
#include <stdlib.h>
#define MAX_ITERATION 100;

/* Calc Mandelbrot Set */
__global__ void kernel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration, float *buffer);

int writeImage(const char* filename, int width, int height, float *buffer, const char* title);

static void setRGB(png_byte *ptr, float val);

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
	
	//printf("Alocando effer na placa de video\n");
	cudaMalloc((void**)&d_buffer, width*height*sizeof(float));
	/* Não achei um cara pra verificar esta alocação */
	
	/* Setar os bocos para trabalhar na placa de video */
	dim3 block_size(16, 16);
	dim3 gred_size(width / block_size.x, height / block_size.y);
		
	//printf("Mandando calculo pra placa e video\n");
    	/* Se eu chamar a funcao em cuda */
	kernel<<<gred_size, block_size>>>(width, height, min_real, min_imag, max_imag, max_real, iteration, d_buffer);
	/* Passei N blocos, sendo uma thred por bloco, e por copia, nao parametro */
	
	cudaDeviceSynchronize();

	/* Pego o buffer da Placa de video e trago para o PC */
	cudaMemcpy(buffer, d_buffer, width*height*sizeof(float), cudaMemcpyDeviceToHost);
	//printf("Trouxe da placa de vieo\n");	
	
	
	/* SEQUENCIAL ->
	float *buffer = mandelbrot_image(width, height, min_real, min_imag, max_real, max_imag, 100);
	if (buffer == NULL) {
		return 1;
	}*/



	printf("Saving PNG\n");
	int result = writeImage(file_name, width, height, buffer, "MANDELBROT");
	

	/* Free no buffer na placa de video */
	cudaFree(buffer);
	/* Free no buffer no PC */
	free(buffer);

	return 0;
}

__global__ void kernel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int iteration_max, float *buffer){

	/* Isso acontece na main com copia para paca de video */
	/*float *buffer = (float*)malloc(width * height * sizeof(float));
	  if (buffer == NULL) {
	  printf("Could not create image buffer\n");
	  return NULL;
	  }*/
	int col = (blockIdx.y * blockDim.y) + threadIdx.y;
	int row = (blockIdx.x * blockDim.x) + threadIdx.x;
	int idx = row * width + col;
	int id_i = (row*width+col)/width;
	int id_j = (col*width+col)/width;
	if(col >= width || row >= height) return;

	float x0 = ((float)row / width) * (max_real - min_real);
	float y0 = ((float)col / height) * (max_imag - min_imag);


	//int x_position, y_position;
	float minMu = iteration_max;
	float maxMu = 0;
	//float x0= (max_real - min_real) / width;
	//float y0= (max_imag - min_imag) / height;
	float z_x = 0.0f, z_y = 0.0f;
	//float z_x2, z_y2;
	int iteration = 0;
	float xtemp;

	while((z_x * z_x + z_y * z_y <= 4.0f) && iteration < iteration_max){
		xtemp = z_x * z_x - z_y * z_y + x0;
		z_y = 2.0f * z_x * z_y + y0;
		z_x = xtemp;
		iteration++;
	}
	
	int color = iteration  * 3;
	if(color >= 256) color =0;
	buffer[idx] = color;
	/*if (iteration < iteration_max) {
		float modZ = sqrt(z_x*z_x + z_y*z_y);
		float mu = iteration - (log(log(modZ))) / log(2);
		if (mu > maxMu)
			maxMu = mu;
		if (mu < minMu)
			minMu = mu;
		buffer[idx] = mu;
	}
	else {
		buffer[idx] = 0;
	}*/
	/*for (y_position = 0 ; y_position < height ; y_position++){
		//float y_pointer = max_imag - delt_y * y_position;
		for (x_position = 0 ; x_position < width ; x_position++){
			float y_pointer = max_imag - delt_y * y_position;
			float x_pointer = min_real + delt_x * x_position;

			z_x = x_pointer;
			z_y = y_pointer;
			z_x2 = z_x * z_x;
			z_y2 = z_y * z_y;

			for (iteration = 0; iteration < iteration_max && ((z_x2+z_y2) < 4); iteration++){
				z_y = 2 * z_x * z_y + y_pointer;
				z_x = z_x2 - z_y2 + x_pointer;
				z_x2 = z_x * z_x;
				z_y2 = z_y * z_y;
			}

			if (iteration < iteration_max) {
				float modZ = sqrt(z_x*z_x + z_y*z_y);
				float mu = iteration - (log(log(modZ))) / log(2);
				if (mu > maxMu)
					maxMu = mu;
				if (mu < minMu)
					minMu = mu;
				buffer[y_position * width + x_position] = mu;
			}
			else {
				buffer[y_position * width + x_position] = 0;
			}
		}
	}*/

	// Scale buffer values between 0 and 1
	/*int count = width * height;
	while (count) {
		count --;
		buffer[count] = (buffer[count] - minMu) / (maxMu - minMu);
	}*/

	/* Acho que função __global__ não tem retorno, tem que ser copiado da memoria da placa */
	//return buffer;
};


__host__ static void setRGB(png_byte *ptr, float val){
	int v = (int)(val * 767);
	if (v < 0) v = 0;
	if (v > 767) v = 767;
	int offset = v % 256;

	if (v<256) {
		ptr[0] = 0; ptr[1] = 0; ptr[2] = offset;
	}
	else if (v<512) {
		ptr[0] = 0; ptr[1] = offset; ptr[2] = 255-offset;
	}
	else {
		ptr[0] = offset; ptr[1] = 255-offset; ptr[2] = 0;
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
