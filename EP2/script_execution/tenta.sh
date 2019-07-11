#!/bin/bash

file="/home/souza/Documentos/Paralela/IntroductionParallelUSP/EP2/img_Opem_MP"
W=500
H=480
var=0
threads=2
a=0
i=0
for i in 1 2 3 4 5 6 7 8
do
  echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> opa.txt
  for a in {1..15}
    do
      (time ./execMP -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> opa.txt
    done
  let threads=$threads*2
done
