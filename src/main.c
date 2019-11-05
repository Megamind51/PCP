/* Example program fragment to read a PAM or PNM image
   from stdin, add up the values of every sample in it
   (I don't know why), and write the image unchanged to
   stdout. */
#include <string.h>
#include <pam.h>

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


int main(int argc, char const *argv[]) {
    unsigned int rows, cols, maxval, row, col, min;
    int flag = 0;
    gray **matrix;
    pm_init(argv[0], 0);
    matrix = pgm_readpgm(stdin, &cols,&rows,&maxval);
    gray **output = pgm_allocarray(cols, rows);
    int iterator = 1;

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


    FILE *fptr;
    if ((fptr = fopen("testeoutput.pgm","w+")) == NULL){
        printf("Erro ao abrir ficheiro");
        exit(1);
    }
    if(iterator)
        pgm_writepgm(fptr, matrix, cols, rows, maxval, 1);
    else
        pgm_writepgm(fptr, output, cols, rows, maxval, 1);

}
