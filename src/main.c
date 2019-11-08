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

	while (flag){
		flag = 0;

		for(row = 0; row < rows; row++){
    		for (col = 0; col < cols; col++) {
				if(iterator){
					aux = output[row][col];
			  		aux -= output[row][col] = min_vizinhos(matrix, rows, cols, row,col);
				}
				else {
					aux = matrix[row][col];
			  		aux -= matrix[row][col] = min_vizinhos(output, rows, cols, row,col);
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

	while (flag){
    	flag = 0;
		#pragma omp parallel for collapse(2) private(aux) reduction(max:flag) schedule(static, 1024)
		for(int row = 0; row < rows; row++){
        	for (int col = 0; col < cols; col++) {

            	if(iterator){
					aux = output[row][col];
					aux -= output[row][col] = min_vizinhos(matrix, rows, cols, row, col);
				}
            	else {
					aux = matrix[row][col];
					aux -= matrix[row][col] = min_vizinhos(output, rows, cols, row, col);
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
    gray ** inicial;

	//apontadores para os resultados finais
    gray ** sequencial;
    gray ** paralel;

	//Leitura da imagem especificada por argumento
    pm_init(argv[0], 0);
    inicial = pgm_readpgm(stdin, &cols, &rows, &maxval);

    //Guardar matriz com os valores originais dos píxeis da imagem
    gray ** matrix_seq = pgm_allocarray(cols, rows);
	gray ** matrix_par  = pgm_allocarray(cols, rows);
	gray ** output_seq  = pgm_allocarray(cols, rows);
	gray ** output_par  = pgm_allocarray(cols, rows);

    for(int i = 0; i < rows; i++)
     	for(int j = 0; j < cols; j++)
        	matrix_par[i][j] = matrix_seq[i][j] = inicial[i][j];

	//Variáveis para medição temporal e apontador para os ficheiros de output
    double start;
    double end;
    FILE * fptr;


	//Inicialização da PAPI
	retval = PAPI_library_init(PAPI_VER_CURRENT);
	retval = PAPI_create_eventset(&EventSet);
	retval = PAPI_add_events(EventSet, Events, NUM_EVENTS);


	int n_iterations = 2;
	for(int y = 0; y < n_iterations; y++){
		//Limpar a cache, começar a medição do tempo e contagem dos eventos da PAPI
		clearCache();
		start = omp_get_wtime();
		retval = PAPI_start(EventSet);

		// workload sequencial
		sequencial = run_sequencial(matrix_seq, output_seq, rows, cols);

		//Medir o tempo, contadores da PAPI e reportar resultados
		end = omp_get_wtime();
		retval = PAPI_stop(EventSet, values);
		printf("\nTempo de execução sequencial: %f ms\n", (end - start)*1000);
		printf("L3MR: %f\n", values[1] / (double) values[0]);

		//Repor os valores originais da matriz
		if (y < n_iterations - 1)
			for(int i = 0; i < rows; i++)
	      		for(int j = 0; j < cols; j++) {
		        	matrix_seq[i][j] = inicial[i][j];
					//output_seq[i][j] = rand()% 256;
				}

	}

	//Abrir apontador para o ficheiro de output
    if ((fptr = fopen("sequencial.pgm","w+")) == NULL){
    	printf("Erro ao abrir ficheiro");
    	exit(1);
    }
    pgm_writepgm(fptr, sequencial, cols, rows, maxval, 1);
    fclose(fptr);


	for(int x = 0; x < n_iterations; x++){
		//Limpar a cache, começar a medição do tempo e contagem dos eventos da PAPI
		clearCache();
		start = omp_get_wtime();
		retval = PAPI_start(EventSet);

		// workload paralela
		paralel = run_parallel(matrix_par, output_par, rows, cols);

		//Medir o tempo, contadores da PAPI e reportar resultados
		end = omp_get_wtime();
		retval = PAPI_stop(EventSet, values);
		printf("\nTempo de execução paralela %f ms\n", (end - start)*1000);
		printf("L3MR: %f\n", values[1] / (double) values[0]);

		//Repor os valores iniciais da matriz
		if (x < n_iterations - 1)
			for(int i = 0; i < rows; i++)
		    	for(int j = 0; j < cols; j++) {
		        	matrix_par[i][j] = inicial[i][j];
					//output_par[i][j] = rand()% 256;
				}

	}

	//Abrir o apontador para o ficheiro de output
    if ((fptr = fopen("paralela.pgm","w+")) == NULL){
    	printf("Erro ao abrir ficheiro");
    	exit(1);
    }

	pgm_writepgm(fptr, paralel, cols, rows, maxval, 1);
}
