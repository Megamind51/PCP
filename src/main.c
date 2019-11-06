/* Example program fragment to read a PAM or PNM image
   from stdin, add up the values of every sample in it
   (I don't know why), and write the image unchanged to
   stdout. */
#include <string.h>
#include <pam.h>
#include <omp.h>


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
  int flag = 1;
  int iterator = 1, aux;
  unsigned int col,row;
  while (flag){
      flag = 0;
      for(row = 0; row < rows; row++){
          for (col = 0; col < cols; col++) {
            if(iterator){
							aux = output[row][col];
              aux -= output[row][col] = min_vizinhos(matrix, rows, cols, row,col);
            }else{
							aux = matrix[row][col];
              matrix[row][col] = min_vizinhos(output, rows, cols, row,col);
						}
						if(aux != 0) flag = 1;

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
  int flag = 1, aux;
  int iterator = 1;
//  unsigned int col,row;

	while (flag){
      flag = 0;
      #pragma omp parallel for collapse(2) private(aux) reduction(max:flag) schedule(guided)
      for(int row = 0; row < rows; row++){
          for (int col = 0; col < cols; col++) {
            if(iterator){
							aux = output[row][col];
							aux -= output[row][col] = min_vizinhos(matrix, rows, cols, row,col);
						}
            else{
							aux = matrix[row][col];
							matrix[row][col] = min_vizinhos(output, rows, cols, row,col);
						}
							if(aux != 0) flag = 1;
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
    gray **matrix;
    gray **sequencial;
    gray **paralel;
    pm_init(argv[0], 0);
    matrix = pgm_readpgm(stdin, &cols,&rows,&maxval);
    // guardar matrix para correr na paralela ( codigo altera seq atlera variavel matrix XD)
    gray **matrix_inicial = pgm_allocarray(cols, rows);
    for(int i = 0; i < rows; i++)
      for(int j = 0; j < cols; j++)
        matrix_inicial[i][j] = matrix[i][j];
    gray **output = pgm_allocarray(cols, rows);

    double start;
    double end;
    FILE *fptr;

		for(int x = 0; x < 10; x++){
			clearCache();
	    start = omp_get_wtime();
	    sequencial = run_sequencial(matrix, output, rows, cols);
	    end = omp_get_wtime();
	    printf("sequencial %f ms to execute \n", (end - start)*1000);
			for(int i = 0; i < rows; i++)
	      for(int j = 0; j < cols; j++)
	        matrix[i][j] = matrix_inicial[i][j];
		}

    if ((fptr = fopen("sequencial.pgm","w+")) == NULL){
      printf("Erro ao abrir ficheiro");
      exit(1);
    }
    pgm_writepgm(fptr, sequencial, cols, rows, maxval, 1);
    fclose(fptr);
    // garantir que nao ha nada em cache
    clearCache();

		for(int x = 0; x < 10; x++){
			clearCache();
    	start = omp_get_wtime();
    	paralel = run_parallel(matrix, output, rows, cols);
    	end = omp_get_wtime();
    	printf("paralela %f ms to execute \n", (end - start)*1000);
			for(int i = 0; i < rows; i++)
	      for(int j = 0; j < cols; j++)
	        matrix[i][j] = matrix_inicial[i][j];

		}

    if ((fptr = fopen("paralela.pgm","w+")) == NULL){
      printf("Erro ao abrir ficheiro");
      exit(1);
    }
    pgm_writepgm(fptr, paralel, cols, rows, maxval, 1);
}
