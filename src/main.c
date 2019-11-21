#include <string.h>
#include <pam.h>
#include <omp.h>
#include <papi.h>

// Variáveis e defines relacionados com a PAPI
#define NUM_EVENTS 2
int	Events[NUM_EVENTS] = {PAPI_L3_TCA, PAPI_L3_TCM};
int EventSet = PAPI_NULL, retval;
long long int values[NUM_EVENTS];

double clearcache [30000000];

void clearCache (void) {
	for (unsigned i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}

int min_vizinhos( gray **matrix, int rows, int cols, int row, int col){
    int i, j, min = matrix[row][col] -1;
    for(i = -1; i < 2; i++)
        for(j = -1; j < 2; j++)
            if(row + i > 0 && row + i < rows)
                if(col + j > 0 && col + j < cols)
                    if(matrix[row + i][col + j] < min){
                        min = matrix[row + i][col + j];
                    }
    return min + 1;
}


gray ** run_sequencial(gray ** matrix, gray ** output, int rows, int cols){
	int flag = 1, iterator = 1, aux;
  	unsigned int col,row;
		int i,j,min;
	while (flag){
		flag = 0;

		for(row = 0; row < rows; row++){
    		for (col = 0; col < cols; col++) {
				if(iterator){
					aux = output[row][col];
					min = matrix[row][col] -1;
			    for(i = -1; i < 2; i++)
			        for(j = -1; j < 2; j++)
			            if(row + i > 0 && row + i < rows)
			                if(col + j > 0 && col + j < cols)
			                    if(matrix[row + i][col + j] < min){
			                        min = matrix[row + i][col + j];
			                    }
			     aux -= output[row][col] = min + 1;
				}
				else {
					aux = matrix[row][col];
					min = output[row][col] -1;
			    for(i = -1; i < 2; i++)
			        for(j = -1; j < 2; j++)
			            if(row + i > 0 && row + i < rows)
			                if(col + j > 0 && col + j < cols)
			                    if(output[row + i][col + j] < min){
			                        min = output[row + i][col + j];
			                    }
			     aux -= matrix[row][col] = min + 1;
				}

				if(aux != 0)
					flag = 1;
    		}
      	}

	  	iterator = !iterator;
 	}

  	if(iterator)
    	return matrix;
  	else
    	return output;
}

gray ** run_parallel(gray ** matrix, gray ** output, int rows, int cols){
	int flag = 1, aux, iterator = 1;
	int min, i, j;
	while (flag){
    	flag = 0;
		#pragma omp parallel for collapse(2) private(aux, min, i, j) reduction(||:flag) schedule(static, 1024)
		for(int row = 0; row < rows; row++){
        	for (int col = 0; col < cols; col++) {
            	if(iterator){
								aux = output[row][col];
								min = matrix[row][col] -1;
						    for(i = -1; i < 2; i++)
						        for(j = -1; j < 2; j++)
						            if(row + i > 0 && row + i < rows)
						                if(col + j > 0 && col + j < cols)
						                    if(matrix[row + i][col + j] < min){
						                        min = matrix[row + i][col + j];
						                    }
						     aux -= output[row][col] = min + 1;
				}
            	else {
								aux = matrix[row][col];
								min = output[row][col] -1;
						    for(i = -1; i < 2; i++)
						        for( j = -1; j < 2; j++)
						            if(row + i > 0 && row + i < rows)
						                if(col + j > 0 && col + j < cols)
						                    if(output[row + i][col + j] < min){
						                        min = output[row + i][col + j];
						                    }
						     aux -= matrix[row][col] = min + 1;
				}

				if(aux != 0)
					flag = 1;
          	}
      	}

	  	iterator = !iterator;
 	}

 	if(iterator)
    	return matrix;
  	else
    	return output;
}

int main(int argc, char const *argv[]) {
    unsigned int rows, cols, maxval, row, col, min;
	//Variáveis para medição temporal e apontador para os ficheiros de output
    double start, end;
    FILE * fptr;

	//Inicialização da PAPI
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	retval = PAPI_create_eventset(&EventSet);
	retval = PAPI_add_events(EventSet, Events, NUM_EVENTS);


	pm_init(argv[0], 0);
    gray ** matrix = pgm_readpgm(stdin, &cols, &rows, &maxval);
	gray ** output  = pgm_allocarray(cols, rows);
	gray ** final;

	if(atoi(argv[1]) == 0) {
		//Limpar a cache, começar a medição do tempo e contagem dos eventos da PAPI
		clearCache();
		start = omp_get_wtime();
		retval = PAPI_start(EventSet);

		// workload sequencial
		final = run_sequencial(matrix, output, rows, cols);

		//Medir o tempo, contadores da PAPI e reportar resultados
		end = omp_get_wtime();
		retval = PAPI_stop(EventSet, values);
		printf(";%f", (end - start)*1000);

		//Abrir apontador para o ficheiro de output
		if ((fptr = fopen("sequencial.pgm","w+")) == NULL){
			printf("Erro ao abrir ficheiro");
			exit(1);
		}
	}
	else {
		//Limpar a cache, começar a medição do tempo e contagem dos eventos da PAPI
		clearCache();
		start = omp_get_wtime();
		retval = PAPI_start(EventSet);

		// workload paralela
		final = run_parallel(matrix, output, rows, cols);

		//Medir o tempo, contadores da PAPI e reportar resultados
		end = omp_get_wtime();
		retval = PAPI_stop(EventSet, values);
		printf(";%f", (end - start)*1000);

		//Abrir o apontador para o ficheiro de output
		if ((fptr = fopen("paralela.pgm","w+")) == NULL){
			printf("Erro ao abrir ficheiro");
			exit(1);
		}
	}



	pgm_writepgm(fptr, final, cols, rows, maxval, 1);
	fclose(fptr);
}
