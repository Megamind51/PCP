
export OMP_NUM_THREADS=$2
make
rm *.pgm
./bin/executavel.o < bin/teste$1.pgm 
