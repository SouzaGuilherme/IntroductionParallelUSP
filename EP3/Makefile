CC=nvcc                                                                                   
#CCFLAGS=-Wall -O0                                                                         
LDFLAGS=-lm -lpng -O2 -Xcompiler -fopenmp                                                                      
GPUFLAGS=-Xptxas -v
FLAGCHAR=--expt-relaxed-constexpr                                                            

all:
	#nvcc -Xptxas -v union_mandel.cu -o execUNION -lm -lpng -O2 -Xcompiler -fopenmp --expt-relaxed-constexpr

	$(CC) $(GPUFLAGS) union_mandel.cu -o mbrot $(LDFLAGS) $(FLAGCHAR)    

clean:                                                                                    
	rm -f mbrot                                                                                                                                                                                                                                 
