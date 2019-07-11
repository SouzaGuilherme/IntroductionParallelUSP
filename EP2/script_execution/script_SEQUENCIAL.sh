#!/bin/bash
a=0
b=0
c=0
d=0
e=0
f=0
g=0
threads=1
file="/home/souza/Documentos/Paralela/IntroductionParallelUSP/EP2/img_Opem_MP"
W=500
H=480
echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for a in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done

W=720
H=576
b=0

echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for b in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done
W=1280
H=720

echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for c in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done
W=1440
H=1080

echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for d in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done

W=2048
H=1080

echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for e in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done
W=3840
H=2160

echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for f in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done
W=7680
H=4320

echo "THREADS:"$threads" W:"$W "H:"$H "SEQUENCIAL" >> resultados_SEQUENCIAL.txt
for g in {1..15}
  do
    (time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H 0 $threads $file) &>> resultados_SEQUENCIAL.txt
  done
