#!/bin/bash

module load gnu/openmpi_eth/1.8.4
module load gcc/4.9.0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
export OMP_NUM_THREADS=${3}
echo '>>>>> A COMPILAR'

make

echo '>>>>> A CORRER SEQUENCIAL'

./bin/sequencial 0 < bin/teste${4}.pgm

echo '>>>>> A CORRER PARALELA'


mpirun -np ${1} --map-by ${2} -mca btl self,sm,tcp bin/paralela < bin/teste${4}.pgm

make clean
