make clean
make 

for threads in 1 2 3 4
do
	export OMP_NUM_THREADS=$threads
	for i in  1 2 3 4
	do
		rm output.pgm
		./bin/omp  bin/pic.pgm
	done
done


