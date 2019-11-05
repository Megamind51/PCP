/* Example program fragment to read a PAM or PNM image
   from stdin, add up the values of every sample in it
   (I don't know why), and write the image unchanged to
   stdout. */
#include <string.h>
#include <pam.h>

int min_vizinhos(unsigned int **matrix, int cols, int rows, int col, int row){
  int min = 254;
  if (row < rows-1 && col < cols-1 && col > 0 && row > 0){
    if (matrix[row+1][col] < min)
      min = matrix[row+1][col];
    if (matrix[row+1][col+1] < min)
      min = matrix[row+1][col+1];
    if (matrix[row][col+1] < min)
      min = matrix[row][col+1];
    if (matrix[row-1][col] < min)
      min = matrix[row-1][col];
    if (matrix[row-1][col+1] < min)
      min = matrix[row-1][col+1];
    if (matrix[row+1][col-1] < min)
      min = matrix[row+1][col-1];
    if (matrix[row-1][col-1] < min)
      min = matrix[row-1][col-1];
    if (matrix[row][col-1] < min)
      min = matrix[row][col-1];
    }else if (row == 0 && col == 0){
      if (matrix[row][col+1] < min)
        min = matrix[row][col+1];
      if (matrix[row+1][col+1] < min)
        min = matrix[row+1][col+1];
      if (matrix[row+1][col] < min)
        min = matrix[row+1][col];
    }else if (row == rows-1 && col == cols-1){
      if (matrix[row][col-1] < min)
        min = matrix[row][col-1];
      if (matrix[row-1][col-1] < min)
        min = matrix[row-1][col-1];
      if (matrix[row-1][col] < min)
        min = matrix[row-1][col];
    }else if (row == rows-1 && col == 0){
      if (matrix[row][col+1] < min)
        min = matrix[row][col+1];
      if (matrix[row-1][col+1] < min)
        min = matrix[row-1][col+1];
      if (matrix[row-1][col] < min)
        min = matrix[row-1][col];
    }else if (row == 0 && col == cols-1){
      if (matrix[row][col-1] < min)
        min = matrix[row][col-1];
      if (matrix[row+1][col-1] < min)
        min = matrix[row+1][col-1];
      if (matrix[row+1][col] < min)
        min = matrix[row+1][col];
    }else if (row == 0){
      if (matrix[row][col-1] < min)
        min = matrix[row][col-1];
      if (matrix[row][col+1] < min)
        min = matrix[row][col+1];
      if (matrix[row+1][col] < min)
        min = matrix[row+1][col];
      if (matrix[row+1][col+1] < min)
        min = matrix[row+1][col+1];
      if (matrix[row+1][col-1] < min)
        min = matrix[row+1][col-1];
    }else if (col == 0){
      if (matrix[row+1][col] < min)
        min = matrix[row+1][col];
      if (matrix[row-1][col] < min)
        min = matrix[row-1][col];
      if (matrix[row][col+1] < min)
        min = matrix[row][col+1];
      if (matrix[row+1][col+1] < min)
        min = matrix[row+1][col+1];
      if (matrix[row-1][col+1] < min)
        min = matrix[row-1][col+1];
    }else if (col == cols-1){
      if (matrix[row+1][col] < min)
        min = matrix[row+1][col];
      if (matrix[row-1][col] < min)
        min = matrix[row-1][col];
      if (matrix[row][col-1] < min)
        min = matrix[row][col-1];
      if (matrix[row+1][col-1] < min)
        min = matrix[row+1][col-1];
      if (matrix[row-1][col-1] < min)
        min = matrix[row-1][col-1];

    }else{
      if (matrix[row][col-1] < min)
        min = matrix[row][col-1];
      if (matrix[row][col+1] < min)
        min = matrix[row][col+1];
      if (matrix[row-1][col] < min)
        min = matrix[row-1][col];
      if (matrix[row-1][col+1] < min)
        min = matrix[row-1][col+1];
      if (matrix[row-1][col-1] < min)
        min = matrix[row-1][col-1];
    }
    return min +1;
}


int vizinhos_branco(unsigned int **matrix, int cols, int rows, int col, int row){
    if (row < rows-1 && col < cols-1 && col > 0 && row > 0){
      if(matrix[row+1][col] != 255 ||
         matrix[row-1][col] != 255 ||
         matrix[row+1][col+1] != 255 ||
         matrix[row-1][col+1] != 255 ||
         matrix[row+1][col-1] != 255 ||
         matrix[row-1][col-1] != 255 ||
         matrix[row][col+1] != 255 ||
         matrix[row][col-1] != 255 )
          return 1;
    }else if(row == 0 && col == 0){
      if(matrix[row][col+1] != 255 ||
         matrix[row+1][col+1] != 255 ||
         matrix[row+1][col] != 255)
          return 1;
    }else if(row == rows-1 && col == cols-1){
      if(matrix[row][col-1] != 255 ||
         matrix[row-1][col-1] != 255 ||
         matrix[row-1][col] != 255)
          return 1;

    }else if(row == rows-1 && col == 0){
      if(matrix[row][col+1] != 255 ||
         matrix[row-1][col+1] != 255 ||
         matrix[row-1][col] != 255)
          return 1;

    }else if(row == 0 && col == cols-1){
      if(matrix[row][col-1] != 255 ||
         matrix[row+1][col-1] != 255 ||
         matrix[row+1][col] != 255)
          return 1;

    }else if(row == 0){
      if(matrix[row][col-1] != 255 ||
         matrix[row][col+1] != 255 ||
         matrix[row+1][col] != 255 ||
         matrix[row+1][col+1] != 255 ||
         matrix[row+1][col-1] != 255)
      return 1;

    }else if(col == 0){
      if(matrix[row+1][col] != 255 ||
         matrix[row-1][col] != 255 ||
         matrix[row][col+1] != 255 ||
         matrix[row+1][col+1] != 255 ||
         matrix[row-1][col+1] != 255)
      return 1;

    }else if(col == cols-1){
      if(matrix[row+1][col] != 255 ||
         matrix[row-1][col] != 255 ||
         matrix[row][col-1] != 255 ||
         matrix[row+1][col-1] != 255 ||
         matrix[row-1][col-1] != 255)
      return 1;

    }else {
      if(matrix[row][col-1] != 255 ||
         matrix[row][col+1] != 255 ||
         matrix[row-1][col] != 255 ||
         matrix[row-1][col-1] != 255 ||
         matrix[row-1][col+1] != 255)
      return 1;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
      unsigned int row;
      unsigned int col;
      unsigned int rows, cols, maxval;
      // array input
      gray **matrix;
      pm_init(argv[0], 0);
      matrix = pgm_readpgm(stdin, &cols,&rows,&maxval);
      gray **output = pgm_allocarray(cols, rows);
        for(row = 0; row < rows; row++)
          for (col = 0; col < cols; col++)
            output[row][col] = matrix[row][col];

        for(row = 0; row < rows; row++){
          for (col = 0; col < cols; col++) {
            // 255 -> Branco
            if (matrix[row][col] == 255){
              if(vizinhos_branco(matrix,cols,rows,col,row) ){
                 output[row][col] = min_vizinhos(matrix, cols, rows, col,row);
               }
          }
        }
        }
        for (int flag = 1; flag;){
          flag = 0;
        for(row = 0; row < rows; row++){
          for (col = 0; col < cols; col++) {
            // 255 -> Branco
            if (matrix[row][col] == 255){
              if(vizinhos_branco(matrix,cols,rows,col,row) ){
                flag = 1;
                 output[row][col] = min_vizinhos(matrix, cols, rows, col,row);
               }
          }
          }
        }
          for(row = 0; row < rows; row++)
            for (col = 0; col < cols; col++)
              matrix[row][col] = output[row][col];

        }
        FILE *fptr;
        if ((fptr = fopen("testeoutput.pgm","w+")) == NULL){
          printf("Erro ao abrir ficheiro");
          exit(1);
        }
      pgm_writepgm(fptr, output, cols, rows, maxval, 1);


}
