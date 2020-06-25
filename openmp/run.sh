make clean
make 

export OMP_NUM_THREADS=8

rm output.pgm
./omp bin/pic.pgm


