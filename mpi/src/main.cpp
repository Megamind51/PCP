#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "pam.h"


double clearcache [30000000];
void clearCache (void) {
	for (unsigned i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}



gray ** run_sequencial(gray ** matrix, int rows, int cols){

	return matrix;
}

gray ** run_parallel(gray ** matrix, int rows, int cols){


	return matrix;
}

int main(int argc, char const *argv[]) {
    unsigned int  maxval, row, col, min;
    int rows, cols;
	//Variáveis para medição temporal e apontador para os ficheiros de output
    double start, end;
    FILE * fptr;
	//Inicialização da PAPI
	gray ** final;
	gray ** matrix;
	gray ** output;
	if(atoi(argv[1]) == 2) {
		matrix  = pgm_allocarray(16384, 16384);
		output  = pgm_allocarray(16384, 16384);
		for(int i = 0; i < 16384; i++){
			for(int j = 0; j < 16384; j++){
				matrix[i][j] = (gray) rand()%256;
			}
		}
	} else{
		pm_init(argv[0], 0);
  	matrix = pgm_readpgm(stdin, &cols, &rows, &maxval);
		output  = pgm_allocarray(cols, rows);
	}
	if(atoi(argv[1]) == 0) {
		//Limpar a cache, começar a medição do tempo e contagem dos eventos da PAPI
		clearCache();

		start = MPI_Wtime();

		// workload sequencial
		final = run_sequencial(matrix, rows, cols);

		//Medir o tempo, contadores da PAPI e reportar resultados
		end = MPI_Wtime();
		printf(";%f", (end - start)*1000);

		//Abrir apontador para o ficheiro de output
		if ((fptr = fopen("sequencial.pgm","w+")) == NULL){
			printf("Erro ao abrir ficheiro");
			exit(1);
		}
	} else if(atoi(argv[1]) == 2) {
		clearCache();
		start = MPI_Wtime();
		final = run_parallel(matrix, 16384, 16384);
		end = MPI_Wtime();
		printf(";%f", (end - start)*1000);

	}
	else {
		clearCache();
		start = MPI_Wtime();

		// workload paralela
		final = run_parallel(matrix, rows, cols);

		//Medir o tempo, contadores da PAPI e reportar resultados
		end = MPI_Wtime();
		printf(";%f", (end - start)*1000);

		//Abrir o apontador para o ficheiro de output
		if ((fptr = fopen("paralela.pgm","w+")) == NULL){
			printf("Erro ao abrir ficheiro");
			exit(1);
		}
	}


 if(atoi(argv[1]) != 2) {
		pgm_writepgm(fptr, final, cols, rows, maxval, 1);
		fclose(fptr);
	}

}
