make clean
make 

for threads in 1 2 4 8
do
	for i in  1 2 3 4
	do
		rm output.pgm
		./bin/omp $threads bin/pic$i.pgm
	done
done


