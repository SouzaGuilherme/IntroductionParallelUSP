COMPILADOR_CUDA=nvcc
NOME_ARQUIVO_CUDA=so_cuda.cu

COMPILADOR_MPI=mpicc
NOME_ARQUIVO_MPI=so_mpi.c

#CCFLAGS=-Wall -O0                                                                         
LDFLAGS=-lpng -Xcompiler -fopenmp -lmpi                                                                     
GPUFLAGS=-lcudart -lcublas

all:
	$(COMPILADOR_CUDA) -c $(NOME_ARQUIVO_CUDA) -o tmp_CUDA.o
	$(COMPILADOR_MPI) -c $(NOME_ARQUIVO_MPI) -o tmp_MPI.o
	$(COMPILADOR_CUDA) $(GPUFLAGS) $(LDFLAGS) tmp_MPI.o tmp_CUDA.o -o MPICUDA

#nvcc -Xptxas -v union_mandel.cu -o execUNION -lm -lpng -O2 -Xcompiler -fopenmp --expt-relaxed-constexpr

clean:                                                                                    
	rm -f tmp_CUDA.o tmp_MPI.0 MPICUDA                                                                                                                                                                                                                                
