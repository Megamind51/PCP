#!/bin/bash

module load gnu/openmpi_eth/1.8.4
module load gcc/4.9.0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
echo '>>>>> A COMPILAR'

make

echo '>>>>> A CORRER SEQUENCIAL'

./bin/sequencial 0 < bin/teste${4}.pgm

echo '>>>>> A CORRER PARALELA'

mpirun -n ${1} --map-by ppr:1:socket:PE=${2} --display-map -mca btl self,sm,tcp bin/paralela ${3} < bin/teste${4}.pgm
