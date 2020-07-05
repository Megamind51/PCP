#!/bin/bash

export PATH=$PATH:/usr/local/bin
make clean
make

for nprocs in 2 4 8
do
	for i in 1 2 3 4
	do
		mpirun -n $nprocs -display-map --map-by core --mca btl vader,self bin/omp bin/pic$i.pgm
	done 
done

