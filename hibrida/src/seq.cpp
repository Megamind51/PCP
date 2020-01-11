#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "pam.h"


int maxdistance = 0;


double clearcache[30000000];

void clearCache(void) {
    for (unsigned i = 0; i < 30000000; ++i)
        clearcache[i] = i;
}


//Chessboard DT F function
int CDT_f(int x, int i, int g_i) {
    return fmax(fabs(x - i), g_i);
}

//Chessboard DT SEP function
int CDT_sep(int i, int u, int g_i, int g_u) {
    if (g_i <= g_u)
        return fmax(i + g_u, floor((i + u) / 2));
    else
        return fmin(u - g_i, floor((i + u) / 2));
}



void transposta(gray **dst, gray **src, int rowsI, int colsI) {
    int block = 32;
    int rows = rowsI;
    int cols = colsI;
    // garantir que nao existe seg fault ao tentar aceder a matriz com
    // tamanhos nao divisiveis pelo block usado
    if (rows % block != 0 && cols % block != 0) {
        rows -= block;
        cols -= block;
    } else if (rows % block != 0) {
        rows -= block;
    } else if (cols % block != 0) {

        cols -= block;
    }

    for (int i = 0; i < rows; i += block) {
        for (int j = 0; j < cols; j += block) {
            for (int a = 0; a < block; a++) {
                for (int b = 0; b < block; b++) {
                    dst[j + b][i + a] = src[i + a][j + b];
                }
            }
        }
    }

    // dar fix aos blocos nao processados por nao ser divisivel
    if (rows % block != 0 && cols % block != 0) {
        for (int i = rows; i < rowsI; i++) {
            for (int j = 0; j < colsI; j++) {
                dst[j][i] = src[i][j];
            }
        }
        for (int i = 0; i < rowsI; i++) {
            for (int j = cols; j < colsI; j++) {
                dst[j][i] = src[i][j];
            }
        }
    } else if (rows % block != 0) {
        for (int i = rows; i < rowsI; i++) {
            for (int j = 0; j < colsI; j++) {
                dst[j][i] = src[i][j];
            }
        }
    } else if (cols % block != 0) {
        for (int i = 0; i < rowsI; i++) {
            for (int j = cols; j < colsI; j++) {
                dst[j][i] = src[i][j];
            }
        }
    }


}

gray **run_sequencial(gray **matrix, int rows, int cols) {
    gray **resultado = pgm_allocarray(cols, rows);
    gray **output = pgm_allocarray(cols, rows);
    gray **transpose = pgm_allocarray(rows, cols);
    gray **last = pgm_allocarray(rows, cols);
    //Fase 1 - EZ CLAP
    for (int i = 0; i < rows; i++) {
        //if border is > 0 make it 0 else 255

        if (resultado[i][0])
            resultado[i][0] = 0;
        else
            resultado[i][0] = 255;


        //check for obstacle in the entire row
        //Left to right pass
        for (int j = 1; j < cols; j++) {
            if (matrix[i][j])
                resultado[i][j] = min(255, 1 + resultado[i][j - 1]);
            else
                resultado[i][j] = 0;
        }
        //Right to left pass
        for (int j = cols - 2; j >= 0; j--) {
            if (resultado[i][j + 1] < resultado[i][j])
                resultado[i][j] = min(255, 1 + resultado[i][j + 1]);
        }
    }
    // Fase 2 plz help

    transposta(transpose, resultado, rows, cols);


    //Lower envelope indices
    int s[cols];
    // same least minimizers
    int t[cols];
    int q = 0;
    int w;
    for (int j = 0; j < cols; j++) {
        //intialise variables
        q = 0;
        s[0] = 0;
        t[0] = 0;
        //Top to bottom scan To compute paritions of [0,m)
        for (int u = 1; u < rows; u++) {
            while (q >= 0 && ((CDT_f(t[q], s[q], (int) transpose[j][s[q]])) > CDT_f(t[q], u, (int) transpose[j][u])))
                q--;
            if (q < 0) {
                q = 0;
                s[0] = u;
            } else {
                //Finds sub-regions
                w = 1 + CDT_sep(s[q], u, (int) transpose[j][s[q]], (int) transpose[j][u]);
                if (w < rows) {
                    q++;
                    s[q] = u;
                    t[q] = w;
                }
            }
        }
        //bottom to top of image to find final DT using lower envelope
        for (int u = rows - 1; u >= 0; u--) {
            last[j][u] = CDT_f(u, s[q], (int) transpose[j][s[q]]); // só aqui é que é alterado a matrix output
            if (u == t[q])
                q--;
        }
    }

    transposta(output, last, cols, rows);

    return output;
}

gray **run_parallel(gray **matrix, int rows, int cols) {


    return matrix;
}

int main(int argc, char const *argv[]) {
    unsigned int maxval, row, col, min;
    int rows, cols;
    //Variáveis para medição temporal e apontador para os ficheiros de output
    double start, end;
    FILE *fptr;
    //Inicialização da PAPI
    gray **final;
    gray **matrix;
    gray **output;
    if (atoi(argv[1]) == 2) {
        matrix = pgm_allocarray(16384, 16384);
        output = pgm_allocarray(16384, 16384);
        for (int i = 0; i < 16384; i++) {
            for (int j = 0; j < 16384; j++) {
                matrix[i][j] = (gray) rand() % 256;
            }
        }
    } else {
        pm_init(argv[0], 0);
        matrix = pgm_readpgm(stdin, &cols, &rows, &maxval);
        output = pgm_allocarray(cols, rows);
    }
    if (atoi(argv[1]) == 0) {
        //Limpar a cache, começar a medição do tempo e contagem dos eventos da PAPI
        clearCache();

        start = MPI_Wtime();

        // workload sequencial

        final = run_sequencial(matrix, rows, cols);

        //Medir o tempo, contadores da PAPI e reportar resultados
        end = MPI_Wtime();
        printf("%f\n", (end - start));

        //Abrir apontador para o ficheiro de output
        if ((fptr = fopen("sequencial.pgm", "w+")) == NULL) {
            printf("Erro ao abrir ficheiro");
            exit(1);
        }
    } else if (atoi(argv[1]) == 2) {
        clearCache();
        start = MPI_Wtime();
        final = run_parallel(matrix, 16384, 16384);
        end = MPI_Wtime();
        printf("%f", (end - start));

    } else {
        clearCache();
        start = MPI_Wtime();

        // workload paralela
        final = run_parallel(matrix, rows, cols);

        //Medir o tempo, contadores da PAPI e reportar resultados
        end = MPI_Wtime();
        printf(";%f", (end - start));

        //Abrir o apontador para o ficheiro de output
        if ((fptr = fopen("paralela.pgm", "w+")) == NULL) {
            printf("Erro ao abrir ficheiro");
            exit(1);
        }
    }


    if (atoi(argv[1]) != 2) {
        pgm_writepgm(fptr, final, cols, rows, maxval, 1);
        fclose(fptr);
    }

}
