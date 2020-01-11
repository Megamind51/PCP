#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "pam.h"


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

int main(int argc, char *argv[]) {

    // Cenas MPI
    int nprocesses, nprocess_total;
    int myrank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    double start, end, tempo_total;
    tempo_total = 0;

    // processo que le imagens, escreve e afins
    if (myrank == 0) {

        unsigned int row, col, maxval;
        int rows, cols;
        //Variáveis para medição temporal e apontador para os ficheiros de output
        FILE *fptr;

        gray **final;
        gray **matrix;
        gray **output;
        gray **resultado;
        gray **transpose;

        pm_init(argv[0], 0);
        matrix = pgm_readpgm(stdin, &cols, &rows, &maxval);
        int aux[2] = {rows, cols};
        start = MPI_Wtime();

        MPI_Bcast(aux, 2, MPI_INT, 0, MPI_COMM_WORLD);
        output = pgm_allocarray(cols, rows);
        resultado = pgm_allocarray(cols, rows);
        transpose = pgm_allocarray(cols, rows);

        // Determinar número de linhas a enviar a cada processo
        // Arrays com as dimensões dos dados a ser enviados aos processos
        int partition = rows / (nprocesses - 1);
        int tamanho_ultimo_processo = rows - (partition * (nprocesses - 2));
        int i = 1;


        // Enviar número de linhas e colunas aos processos
        for (; i < nprocesses - 1; i++) {
            MPI_Send(&(matrix[(i - 1) * partition][0]), cols * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

        }
        // Enviar dimensão dos dados para o último processo
        MPI_Send(&(matrix[(i - 1) * partition][0]), cols * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);


        //receber linhas  processadas
        for (i = 1; i < nprocesses - 1; i++) {
            MPI_Recv(&(resultado[(i - 1) * partition][0]), cols * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD,
                     &status);
        }
        // receber last chunk

        MPI_Recv(&(resultado[(i - 1) * partition][0]), cols * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
        // fazer transposta para enviar para processos linhas à mesma
        transposta(transpose, resultado, rows, cols);

        // redefinir variaveis apos transposta
        partition = cols / (nprocesses - 1);
        tamanho_ultimo_processo = cols - (partition * (nprocesses - 2));
        i = 1;

        // Enviar número de linhas e colunas aos processos apos transposta
        for (; i < nprocesses - 1; i++) {
            MPI_Send(&(transpose[(i - 1) * partition][0]), rows * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
        }
        // Enviar dimensão dos dados para o último processo apos transposta
        MPI_Send(&(transpose[(i - 1) * partition][0]), rows * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

        //receber linhas  processadas apos fase 2
        for (i = 1; i < nprocesses - 1; i++) {
            MPI_Recv(&(output[(i - 1) * partition][0]), rows * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
        }
        // receber last chunk apos fase 2
        MPI_Recv(&(output[(i - 1) * partition][0]), rows * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);

        // transposta para voltar ao formato inicial
        transposta(transpose, output, rows, cols);
        end = MPI_Wtime();
        tempo_total = (end - start);
        printf(";%f", tempo_total);

        //Abrir o apontador para o ficheiro de output
        if ((fptr = fopen("paralela.pgm", "w+")) == NULL) {
            printf("Erro ao abrir ficheiro");
            exit(1);
        }

        pgm_writepgm(fptr, transpose, cols, rows, maxval, 1);
        fclose(fptr);
    }
        // Processos trabalhadores
    else {
        int initial_data[2] = {0, 0};
        // receber o tamanho de cada linha e numero de linhas que vai receber
        MPI_Bcast(initial_data, 2, MPI_INT, 0, MPI_COMM_WORLD);

        int partition = initial_data[0] / (nprocesses - 1);
        if ( myrank == nprocesses - 1 ){
            partition = initial_data[0] - (partition * (nprocesses - 2));
        }

        gray **processar = pgm_allocarray(initial_data[1], partition );
        gray **resultado = pgm_allocarray(initial_data[1], partition );

        MPI_Recv(&(processar[0][0]), partition * initial_data[1], MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD,
                 &status); // receber linhas
        //Fazer fase 1
        for (int i = 0; i < partition; i++) {
            //check for obstacle in the entire row
            //Left to right pass

            if (resultado[i][0])
                resultado[i][0] = 0;
            else
                resultado[i][0] = 255;

            //check for obstacle in the entire row
            //Left to right pass
            for (int j = 1; j < initial_data[1]; j++) {
                if (processar[i][j])
                    resultado[i][j] = min(255, 1 + resultado[i][j - 1]);
                else
                    resultado[i][j] = 0;
            }
            //Right to left pass
            for (int j = initial_data[1] - 2; j >= 0; j--) {
                if (resultado[i][j + 1] < resultado[i][j])
                    resultado[i][j] = min(255, 1 + resultado[i][j + 1]);
            }
        }

        //enviar linhas para processo sicronizador
        MPI_Send(&(resultado[0][0]), partition * initial_data[1], MPI_UNSIGNED, 0, 0,
                 MPI_COMM_WORLD); // receber o tamanho de cada linha e numero de linhas que vai receber

         partition = initial_data[1] / (nprocesses - 1);
         if ( myrank == nprocesses - 1 ){
             partition = initial_data[1] - (partition * (nprocesses - 2));
         }

        processar = pgm_allocarray( initial_data[1], partition );
        resultado = pgm_allocarray( initial_data[1], partition );


        MPI_Recv(&(processar[0][0]), initial_data[0] * partition, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD,
                 &status); // receber linhas

        //Lower envelope indices
        int s[initial_data[0]];
        // same least minimizers
        int t[initial_data[0]];
        int q = 0;
        int w;
        for (int j = 0; j < partition; j++) // Linhas
        {

            //intialise variables
            q = 0;
            s[0] = 0;
            t[0] = 0;
            //Top to bottom scan To compute paritions of [0,m)
            for (int u = 1; u < initial_data[0]; u++) { //Colunas

                while (q >= 0 &&
                       ((CDT_f(t[q], s[q], (int) processar[j][s[q]])) > CDT_f(t[q], u, (int) processar[j][u])))
                    q--;
                if (q < 0) {
                    q = 0;
                    s[0] = u;
                } else {
                    //Finds sub-regions
                    w = 1 + CDT_sep(s[q], u, (int) processar[j][s[q]], (int) processar[j][u]);
                    if (w < initial_data[0]) {
                        q++;
                        s[q] = u;
                        t[q] = w;
                    }
                }

            }
            //bottom to top of image to find final DT using lower envelope
            for (int u = initial_data[0] - 1; u >= 0; u--) {
                resultado[j][u] = CDT_f(u, s[q], (int) processar[j][s[q]]); // só aqui é que é alterado a matrix output
                if (u == t[q])
                    q--;
            }

        }
        //enviar linhas para processo sicronizador

        MPI_Send(&(resultado[0][0]), initial_data[0] * partition, MPI_UNSIGNED, 0, 0,
                 MPI_COMM_WORLD); // receber o tamanho de cada linha e numero de linhas que vai receber

    }

    MPI_Finalize();
    return (0);

}
