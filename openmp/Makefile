default: omp

dtrans.h: dtrans.d 
	dtrace -h -s src/dtrans.d 

main.o: src/main.c dtrans.h 
	g++ -c src/main.c dtrans.h 

omp.o: main.o dtrans.h
	dtrace -G -s src/dtrans.d main.o -o omp.o 

omp: omp.o 
	g++ -o omp omp.o main.o dtrans.h -lm -O0 -g -fopenmp

clean:
	rm -f *.o
	rm -f dtrans.h
	rm -f omp
