make clean
make 

rm output.pgm

for threads in 1 2 3 4
do
	export OMP_NUM_THREADS=$threads
	for i in  1 2 3 4
	do
		./omp bin/pic$i.pgm
	done
done

