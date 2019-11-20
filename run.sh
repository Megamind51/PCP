#!/bin/bash

make

echo "######EXECUTANDO COM 1 THREAD######"
echo "###################################"
for j in 1 2
do
    rm *.pgm
    echo "Executando para o teste $j"
    ./bin/executavel.o 0 < bin/teste$j.pgm
done



for i in 2 4 8 12 16 20 24 32 40 48
do
    echo "######EXECUTANDO COM $i CORES######"
    echo "###################################"
    for j in 1 2
    do
        export OMP_NUM_THREADS=$i
        rm *.pgm
        echo "Executando para o teste $j"
        ./bin/executavel.o 1 < bin/teste$j.pgm
    done
done
