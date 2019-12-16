#!/bin/bash

module load gnu/openmpi_eth/1.8.4
module load gcc/4.9.0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

make 

mpirun -np ${1} -mca btl self,sm,tcp bin/executavel.o < bin/teste${2}.pgm



