#!/bin/bash

module load gnu/openmpi_eth/1.8.4
module load gcc/4.9.0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
export OMP_NUM_THREADS=$1
echo '>>>>> A COMPILAR'

make

echo '>>>>> A CORRER SEQUENCIAL'

./bin/sequencial 0 < bin/teste${2}.pgm

echo '>>>>> A CORRER PARALELA'


mpirun -np 2 --map-by core -mca btl self,sm,tcp bin/paralela < bin/teste${2}.pgm

make clean
