#!/bin/bash

module load gnu/openmpi_eth/1.8.4
module load gcc/4.9.0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

echo "Cores;Imagem1;Imagem2;Imagem3;"
echo -n "1"
for j in 1 2 3
do
	./bin/executavelSeq 0 < bin/teste$j.pgm
done


for i in 2 4 8 12 16 20 24 28 32 40 48
do
	echo -n "$i"
	for j in 1 2 3
	do
		mpirun -np $i -mca btl self,sm,tcp bin/executavel.o < bin/teste$j.pgm
	done
done

