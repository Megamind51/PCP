#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib


mpirun -np 8 -mca btl self,sm,tcp bin/executavel.o < bin/pic.pgm

