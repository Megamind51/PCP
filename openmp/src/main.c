#define  _GNU_SOURCE
#include <string.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


double clearcache [30000000];

void clearCache (void) {
	for (unsigned i = 0; i < 30000000; ++i)
		clearcache[i] = i;
}

short unsigned int * my_aloc_pgm(int rows, int cols){	
	return  (short unsigned int *) malloc(rows*cols*sizeof(short unsigned int));
}

short unsigned int * my_read_pgm(FILE * f, int * rows, int * cols){
	size_t read, len;
	char* line;
	int flag = 1, j = 0;
	short unsigned int aux;
	while (j < 4 && (read = getline(&line, &len, f)) != -1) {
		j++;
        if (flag)
			if (sscanf(line, "%d %d", rows, cols) == 2)
				flag = 0;
		free(line);		
    }
	short unsigned int * matrix = my_aloc_pgm(*rows, *cols);
	printf("%hu", matrix[0]);
	for (int i = 0; i < *rows; i++)
	{
		for (int j = 0; j < *cols; j++)
		{
			len = fscanf(f, "%hd\n", &matrix[i * (*cols) + j]);
		}
		
	}
	
	return matrix;
}

void my_write_pgm(FILE * f, short unsigned int * matrix, int rows, int cols){
	fprintf(f, "P2\n# :)\n%d %d\n255\n", rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			fprintf(f, "%hd\n", matrix[i * cols + j]);
		}
		
	}
	
}

int min, i, j;
short unsigned int * run_parallel(short unsigned int * matrix, short unsigned int * output, int rows, int cols){
	int flag = 1, aux, iterator = 1;
	while (flag){
    	flag = 0;
		#pragma omp parallel for collapse(2) private(aux, min, i, j) reduction(||:flag) schedule(static)
		for(int row = 0; row < rows; row++){
        	for (int col = 0; col < cols; col++) {
            	if(iterator){
								aux = output[row * cols + col];
								min = matrix[row * cols + col] -1;
								if (min != 255)
						    for(i = -1; i < 2; i++)
						        for(j = -1; j < 2; j++)
						            if(row + i > 0 && row + i < rows)
						                if(col + j > 0 && col + j < cols)
						                    if(matrix[(row + i) * cols + col + j] < min){
						                        min = matrix[(row + i) * cols + col + j];
						                    }
						     aux -= output[row * cols + col] = min + 1;
				}
            	else {
								aux = matrix[row * cols + col];
								min = output[row * cols + col] -1;
								if (min != 255)

						    for(i = -1; i < 2; i++)
						        for( j = -1; j < 2; j++)
						            if(row + i > 0 && row + i < rows)
						                if(col + j > 0 && col + j < cols)
						                    if(output[(row + i) * cols + col + j] < min){
						                        min = output[(row + i) * cols + col + j];
						                    }
						     aux -= matrix[row * cols + col] = min + 1;
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
    unsigned int rows, cols, row, col, min;
	//Variáveis para medição temporal e apontador para os ficheiros de output
    double start, end;
    FILE * fptr;

	short unsigned int * final;
	short unsigned int * matrix;
	short unsigned int * output;

	matrix = my_read_pgm(stdin, &cols, &rows);
	output  = my_aloc_pgm(cols, rows);
	
	final = run_parallel(matrix, output, rows, cols);

	if ((fptr = fopen("output.pgm","w")) == NULL){
			printf("Erro ao abrir ficheiro");
			exit(1);
		}

	my_write_pgm(fptr, final, cols, rows);
	fclose(fptr);
}
