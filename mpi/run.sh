#!/bin/bash

module load gcc/4.9.0
module load papi/5.5.0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

make 
echo "Cores;Imagem-1(1024x1024);Imagem-2(2048x2048);Imagem-3(4096x4096)"
for j in $(seq 1 5)
do
    echo -n "1"
    for i in $(seq 1 3)
    do
      ./bin/executavel.o 0 < bin/teste$i.pgm
    done
    echo ""
done

for i in 2 4 8 12 16 20 24 32 40 48
do
   for j in $(seq 1 10) 
   do
     echo -n "$i"
     export OMP_NUM_THREADS=$i
     for k in $(seq 1 3)
     do
        ./bin/executavel.o 1 < bin/teste$k.pgm
     done
     echo ""
   done
done


