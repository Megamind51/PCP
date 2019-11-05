/* Example program fragment to read a PAM or PNM image
   from stdin, add up the values of every sample in it
   (I don't know why), and write the image unchanged to
   stdout. */
#include <string.h>
#include <pam.h>
#include <omp.h>

int min_vizinhos( gray **matrix, int rows, int cols, int row, int col, int* flag){
    int i, j, min = matrix[row][col] -1;
  //  printf("%d %d\n",row,col );
    for(i = -1; i < 2; i++)
        for(j = -1; j < 2; j++)
            if(row + i > 0 && row + i < rows)
                if(col + j > 0 && col + j < cols)
                    if(matrix[row + i][col + j] < min){
                        if(!(*flag) )
                            *flag = 1;
                        min = matrix[row + i][col + j];
                    }

    return min + 1;
}

gray ** run_sequencial(gray ** matrix, gray ** output, int rows, int cols){
  int flag = 0;
  int iterator = 1;
  unsigned int col,row;
  for(row = 0; row < rows; row++){
      for (col = 0; col < cols; col++) {
          output[row][col] = min_vizinhos(matrix, rows, cols, row,col, &flag);
      }
  }
  while (flag){
      flag = 0;
      for(row = 0; row < rows; row++){
          for (col = 0; col < cols; col++) {
            if(iterator)
              output[row][col] = min_vizinhos(matrix, rows, cols, row,col,&flag);
            else
              matrix[row][col] = min_vizinhos(output, rows, cols, row,col,&flag);

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
  int flag = 0;
  int iterator = 1;
  unsigned int col,row;
  for(row = 0; row < rows; row++){
    #pragma omp parallel for
      for (col = 0; col < cols; col++) {
          output[row][col] = min_vizinhos(matrix, rows, cols, row,col, &flag);
      }
  }
  while (flag){
      flag = 0;
      for(row = 0; row < rows; row++){
        #pragma omp parallel for
          for (col = 0; col < cols; col++) {
            if(iterator)
              output[row][col] = min_vizinhos(matrix, rows, cols, row,col,&flag);
            else
              matrix[row][col] = min_vizinhos(output, rows, cols, row,col,&flag);
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
    gray **output = pgm_allocarray(cols, rows);

    double start;
    double end;

    start = omp_get_wtime();
    sequencial = run_sequencial(matrix, output, rows, cols);
    end = omp_get_wtime();

    printf("sequencial %f ms to execute \n", (end - start)*1000);

    start = omp_get_wtime();
    paralel = run_parallel(matrix, output, rows, cols);
    end = omp_get_wtime();
    printf("paralela %f ms to execute \n", (end - start)*1000);

    FILE *fptr;
    if ((fptr = fopen("sequencial.pgm","w+")) == NULL){
        printf("Erro ao abrir ficheiro");
        exit(1);
    }
    pgm_writepgm(fptr, sequencial, cols, rows, maxval, 1);
    fclose(fptr);
    if ((fptr = fopen("paralela.pgm","w+")) == NULL){
        printf("Erro ao abrir ficheiro");
        exit(1);
    }
    pgm_writepgm(fptr, paralel, cols, rows, maxval, 1);

}
