#!/bin/bash
file="/home/souza/Documentos/Paralela/IntroductionParallelUSP/EP2/img_sequencial"
W=500
H=480

echo "W:$W H:$H SEQUENCIAL"
for a in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done

W=720
H=576
b=0
echo "W: $W H: $H SEQUENCIAL"
for b in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done

W=1280
H=720
echo "W:$W H:$H SEQUENCIAL"
for c in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done

W=1440
H=1080
echo "W:$W H:$H SEQUENCIAL"
for d in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done

W=2048
H=1080
echo "W:$W H:$H SEQUENCIAL"
for e in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done

W=3840
H=2160
echo "W:$W H:$H SEQUENCIAL"
for f in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done

W=7680
H=4320
echo "W:$W H:$H SEQUENCIAL"
for g in [1..15]
  do
    time ./execSEQ -1.0 -1.31 0.0 0.0 $W $H C 0 $file
  done
