#!/bin/bash
file="/tmp/SOUZA/img_CUDA"
W=500
H=480
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done

W=720
H=576
b=0
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done

W=1280
H=720
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done

W=1440
H=1080
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done

W=2048
H=1080
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done

W=3840
H=2160
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done

W=7680
H=4320
var=0
threads=2
while [ $var -lt "8" ]
do
  echo "THREADS:$threads W:$W H:$H SEQUENCIAL"
  for a in [1..15]
  do
    time ./execCUDA -1.0 -1.31 0.0 0.0 $W $H C $threads $file
  done
  let threads=$threads*2
  let var=$var+1
done
