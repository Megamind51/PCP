#!/bin/bash

export PATH=$PATH:/usr/local/bin
make clean
make

mpirun -n 8 -display-map --map-by core --mca btl vader,self bin/executavel.o bin/pic.pgm

