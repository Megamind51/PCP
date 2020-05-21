#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/bin
make clean
make

mpirun -n 8 bin/executavel.o bin/pic.pgm

