#include<stdio.h>
#define iteration_max 100
#define CEIL(a, b) (((a) + (b) - 1)/(b))

// Function using of the internet
////////////////////////////////////////////////////////////////////////////////
// These are CUDA Helper functions

// This will output the proper CUDA error strings in the event that a CUDA host call returns an error
#define checkCudaErrors(err)           __checkCudaErrors (err, __FILE__, __LINE__)

inline void __checkCudaErrors( cudaError err, const char *file, const int line )
{
	if( cudaSuccess != err) {
		fprintf(stderr, "%s(%i) : CUDA Runtime API error %d: %s.\n",
				file, line, (int)err, cudaGetErrorString( err ) );
		exit(-1);

	}
}
/* Traz a GLOBAL para cá */
/* Calc_mandelbrot NVIDIA_CUDA */
__global__ void kernel(int width, int height, float min_real, float min_imag, float max_real, float max_imag, int *buffer){
	int index_vector = (blockIdx.x * blockDim.x) + threadIdx.x;
	int val_for_cal = buffer[index_vector];
	int i = val_for_cal / width;
	int j = val_for_cal % width;
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
	buffer[index_vector] = iteration;
};


extern "C" void invoke_cuda(int tam_vetor, int width, int height, float *min_real, float *min_imag, float *max_real, float *max_imag, int threads, int *vetor){
	int *d_vetor;
	
	/* Aloco na placa de video */
	checkCudaErrors(cudaMalloc(&d_vetor, tam_vetor*sizeof(int)));


	checkCudaErrors(cudaMemcpy(d_vetor, vetor, tam_vetor*sizeof(int), cudaMemcpyHostToDevice));
	
	/* Setar os bocos para trabalhar na placa de video */
	int threads_per_block = threads;
	int num_blocks = CEIL((tam_vetor), threads_per_block);
		
	kernel<<<num_blocks, threads_per_block>>>(width, height, *min_real, *min_imag, *max_real, *max_imag, d_vetor);

	checkCudaErrors(cudaDeviceSynchronize());
        
	/* Pego o buffer da Placa de video e trago para o PC */
	checkCudaErrors(cudaMemcpy(vetor, d_vetor, tam_vetor*sizeof(int), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaFree(d_vetor));
}
