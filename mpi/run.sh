#!/bin/bash

export PATH=$PATH:/usr/local/bin
make clean
make

for nprocs in 4
do
	export OMP_NUM_THREADS=$threads
	for i in 2
	do
		mpirun -n $nprocs -display-map --map-by core --mca btl vader,self bin/omp bin/pic$i.pgm
	done 
done

