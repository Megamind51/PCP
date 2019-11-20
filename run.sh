#!/bin/bash

make

for i in 1 2 4 8 16 24 32 48 64 96
do
    echo "######EXECUTANDO COM $i CORES######"
    echo "###################################"
    for j in 1 2 3
    do
        export OMP_NUM_THREADS=$i
        rm *.pgm
        echo "Executando para o teste $j"
        ./bin/executavel.o < bin/teste$j.pgm
    done
done
