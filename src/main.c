/* Example program fragment to read a PAM or PNM image
   from stdin, add up the values of every sample in it
   (I don't know why), and write the image unchanged to
   stdout. */

#include <pam.h>

int min_vizinhos(unsigned int **matrix, int rows, int cols, int row, int col){
    int i, j, min = 254;
    for(i = -1; i < 2; i++)
        for(j = -1; j < 2; j++)
            if(row + i > 0 && row + i < rows)
                if(col + j > 0 && col + j < cols)
                    if(matrix[row + i][col + j] < min)
                        min = matrix[row + i][col + j];

    return min + 1;
}


int main(int argc, char const *argv[]) {
    unsigned int row;
    unsigned int col;
    unsigned int rows, cols, maxval;
    unsigned int **matrix;
    pm_init(argv[0], 0);
    matrix = pgm_readpgm(stdin, &cols,&rows,&maxval);
    for(row = 0; row < rows; row++){
        for (col = 0; col < cols; col++) {
            matrix[row][col] = min_vizinhos(matrix, rows, cols, row, col);
        }
    }
    
    FILE *fptr;
    if ((fptr = fopen("testeoutput.pgm","w+")) == NULL){
        printf("Erro ao abrir ficheiro");
        exit(1);
    }
    pgm_writepgm(fptr, matrix, cols, rows, maxval, 1);
}
