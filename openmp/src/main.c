#include <string.h>
#include <pam.h>
#include <omp.h>


double clearcache [30000000];

void clearCache (void) {
	for (unsigned i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}


int min, i, j;
gray ** run_parallel(gray ** matrix, gray ** output, int rows, int cols){
	int flag = 1, aux, iterator = 1;
	while (flag){
    	flag = 0;
		#pragma omp parallel for collapse(2) private(aux, min, i, j) reduction(||:flag) schedule(static,8096)
		for(int row = 0; row < rows; row++){
        	for (int col = 0; col < cols; col++) {
            	if(iterator){
								aux = output[row][col];
								min = matrix[row][col] -1;
								if (min != 255)
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
								if (min != 255)

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

	gray ** final;
	gray ** matrix;
	gray ** output;

	matrix = pgm_readpgm(stdin, &cols, &rows, &maxval);
	output  = pgm_allocarray(cols, rows);
	
	final = run_parallel(matrix, output, rows, cols);

	if ((fptr = fopen("output.pgm","w+")) == NULL){
			printf("Erro ao abrir ficheiro");
			exit(1);
		}

	pgm_writepgm(fptr, final, cols, rows, maxval, 1);
	fclose(fptr);
}
