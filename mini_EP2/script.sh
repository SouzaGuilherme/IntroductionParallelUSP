#!/bin/bash

vetorSize=8192													# Start in 1k
a=0
d=0

while [ $d -lt "1" ]									# 30 execucoes para chegar na curva
do
	a=0
#	echo "0 -> "$d
#	echo "tamanho do vetor"$vetorSize "Algortmo 0" >> text.txt
#	while [ $a -lt "10" ]
#	do
#		./main --matrix-size $vetorSize --algorithm 0 >> text.txt
#		let a=$a+1
#	done
	a=0
	echo "1 -> "$d
	echo "tamanho do vetor"$vetorSize "Algortmo 1" >> text.txt
	while [ $a -lt "10" ]
	do
		./main --matrix-size $vetorSize --algorithm 1 >> text.txt
		let a=$a+1
	done
	a=0
	echo "2 -> "$d
	echo "tamanho do vetor"$vetorSize "Algortmo 2" >> text.txt
	while [ $a -lt "10" ]
	do
		./main --matrix-size $vetorSize --algorithm 2 >> text.txt
		let a=$a+1
	done
	echo "FIM"
	let vetorSize=$vetorSize*2
	let d=$d+1
done
