#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "pam.h"
#include <omp.h>

int OMP_NUM_THREADS;

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
    int block = 64;
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
      #pragma omp parallel for collapse(2)
      for (int i = 0; i < rows; i += block) {
        for (int j = 0; j < cols; j += block) {
            // 2 loops interiores nao vetorizados para mander propriedades de caching
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
    double start2, end2, tempo_total2;
    double tempo_total3 = 0;

    MPI_Status status;
    int aux;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &aux);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    double start, end, tempo_total;
    tempo_total = 0;
    if( argc == 2){
      omp_set_num_threads(atoi(argv[1]));
    }
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
        int initial_data[2] = {rows, cols};
        MPI_Bcast(initial_data, 2, MPI_INT, 0, MPI_COMM_WORLD);
        output = pgm_allocarray(cols, rows);
        resultado = pgm_allocarray(cols, rows);
        transpose = pgm_allocarray(cols, rows);

        // Determinar número de linhas a enviar a cada processo
        // Arrays com as dimensões dos dados a ser enviados aos processos
        int partition = rows / (nprocesses);
        int tamanho_ultimo_processo = rows - (partition * (nprocesses - 1));
        int i = 1;

        start = MPI_Wtime();
        start2 = MPI_Wtime();
        // Enviar número de linhas e colunas aos processos
        for (; i < nprocesses - 1; i++) {
            MPI_Send(&(matrix[(i) * partition][0]), cols * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

        }
        // Enviar dimensão dos dados para o último processo
        if (nprocesses > 1)
            MPI_Send(&(matrix[(i) * partition][0]), cols * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

        end2 = MPI_Wtime();
        tempo_total3 += (end2 - start2);

        //FAZER TRABALHO
        #pragma omp parallel for
        for (int k = 0; k < partition; k++) {
            //check for obstacle in the entire row
            //Left to right pass

            if (resultado[k][0])
                resultado[k][0] = 0;
            else
                resultado[k][0] = 255;

            //check for obstacle in the entire row
            //Left to right pass
            for (int l = 1; l < initial_data[1]; l++) {
                if (matrix[k][l])
                    resultado[k][l] = min(255, 1 + resultado[k][l - 1]);
                else
                    resultado[k][l] = 0;
            }
            //Right to left pass
            for (int l = initial_data[1] - 2; l >= 0; l--) {
                if (resultado[k][l + 1] < resultado[k][l])
                    resultado[k][l] = min(255, 1 + resultado[k][l + 1]);
            }
        }


        //FAZER TRABALHO


        start2 = MPI_Wtime();
        //receber linhas  processadas
        for (i = 1; i < nprocesses - 1; i++) {
            MPI_Recv(&(resultado[(i) * partition][0]), cols * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD,
                     &status);
        }
        // receber last chunk
        if (nprocesses > 1)
            MPI_Recv(&(resultado[(i) * partition][0]), cols * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);

        end2 = MPI_Wtime();
        tempo_total3 += (end2 - start2);

        // fazer transposta para enviar para processos linhas à mesma
        start2 = MPI_Wtime();
        transposta(transpose, resultado, rows, cols);

        end2 = MPI_Wtime();

        tempo_total2 = (end2 - start2);



        // redefinir variaveis apos transposta
        partition = cols / (nprocesses);
        tamanho_ultimo_processo = cols - (partition * (nprocesses - 1));
        i = 1;

        start2 = MPI_Wtime();
        // Enviar número de linhas e colunas aos processos apos transposta
        for (; i < nprocesses - 1; i++) {
            MPI_Send(&(transpose[(i) * partition][0]), rows * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
        }
        // Enviar dimensão dos dados para o último processo apos transposta
        if (nprocesses > 1)
            MPI_Send(&(transpose[(i) * partition][0]), rows * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

        end2 = MPI_Wtime();
        tempo_total3 += (end2 - start2);



        //FAZER TRABALHO

        //Lower envelope indices
        int s[initial_data[0]];
        // same least minimizers
        int t[initial_data[0]];
        int q = 0;
        int w;

        #pragma omp parallel for private(q, w, t, s)
        for (int j = 0; j < partition; j++) // Linhas
        {
            //intialise variables
            q = 0;
            s[0] = 0;
            t[0] = 0;
            //Top to bottom scan To compute paritions of [0,m)
            for (int u = 1; u < initial_data[0]; u++) {

                while (q >= 0 &&
                       ((CDT_f(t[q], s[q], (int) transpose[j][s[q]])) > CDT_f(t[q], u, (int) transpose[j][u])))
                    q--;
                if (q < 0) {
                    q = 0;
                    s[0] = u;
                } else {
                    //Finds sub-regions
                    w = 1 + CDT_sep(s[q], u, (int) transpose[j][s[q]], (int) transpose[j][u]);
                    if (w < initial_data[0]) {
                        q++;
                        s[q] = u;
                        t[q] = w;
                    }
                }

            }
            //bottom to top of image to find final DT using lower envelope
            for (int u = initial_data[0] - 1; u >= 0; u--) {
                output[j][u] = CDT_f(u, s[q], (int) transpose[j][s[q]]); // só aqui é que é alterado a matrix output
                if (u == t[q])
                    q--;
            }

        }


        //FAZER TRABALHO

        start2 = MPI_Wtime();
        //receber linhas  processadas apos fase 2
        for (i = 1; i < nprocesses - 1; i++) {
            MPI_Recv(&(output[(i) * partition][0]), rows * partition, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
        }
        // receber last chunk apos fase 2
        if (nprocesses > 1)
            MPI_Recv(&(output[(i) * partition][0]), rows * tamanho_ultimo_processo, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);

        end2 = MPI_Wtime();
        tempo_total3 += (end2 - start2);
        // transposta para voltar ao formato inicial
        start2 = MPI_Wtime();
        transposta(transpose, output, rows, cols);
        end2 = MPI_Wtime();
        end = MPI_Wtime();
        tempo_total2 += (end2 - start2);

        tempo_total += (end - start);
        printf("Tempo transposta %f;  \n", tempo_total2);
        printf("Tempo COM %f;  \n", tempo_total3);
        printf("%f; %d \n", tempo_total, myrank);

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



        int partition = initial_data[0] / (nprocesses );
        if ( myrank == nprocesses - 1 ){
            partition = initial_data[0] - (partition * (nprocesses - 1));
        }

        int partition2 = initial_data[1] / (nprocesses);
         if ( myrank == nprocesses - 1 ){
             partition2 = initial_data[1] - (partition2 * (nprocesses - 1));
         }

        gray** processar2 = pgm_allocarray( initial_data[0], partition2 );
        gray** resultado2 = pgm_allocarray( initial_data[0], partition2 );

        gray **processar = pgm_allocarray(initial_data[1], partition );
        gray **resultado = pgm_allocarray(initial_data[1], partition );

        MPI_Recv(&(processar[0][0]), partition * initial_data[1], MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD,
                 &status); // receber linhas

        start2 = MPI_Wtime();

        //Fazer fase 1
        #pragma omp parallel for
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

        end2 = MPI_Wtime();
        tempo_total2 = (end2 - start2);

        //enviar linhas para processo sicronizador
        MPI_Send(&(resultado[0][0]), partition * initial_data[1], MPI_UNSIGNED, 0, 0,
                 MPI_COMM_WORLD); // receber o tamanho de cada linha e numero de linhas que vai receber





        MPI_Recv(&(processar2[0][0]), initial_data[0] * partition2, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD,
                 &status); // receber linhas

        //Lower envelope indices
        int s[initial_data[0]];
        // same least minimizers
        int t[initial_data[0]];
        int q = 0;
        int w;
        start2 = MPI_Wtime();
        #pragma omp parallel for private(q, w, t, s)
        for (int j = 0; j < partition2; j++) // Linhas
        {

            //intialise variables
            q = 0;
            s[0] = 0;
            t[0] = 0;
            //Top to bottom scan To compute paritions of [0,m)
            for (int u = 1; u < initial_data[0]; u++) { //Colunas

                while (q >= 0 &&
                       ((CDT_f(t[q], s[q], (int) processar2[j][s[q]])) > CDT_f(t[q], u, (int) processar2[j][u])))
                    q--;
                if (q < 0) {
                    q = 0;
                    s[0] = u;
                } else {
                    //Finds sub-regions
                    w = 1 + CDT_sep(s[q], u, (int) processar2[j][s[q]], (int) processar2[j][u]);
                    if (w < initial_data[0]) {
                        q++;
                        s[q] = u;
                        t[q] = w;
                    }
                }

            }
            //bottom to top of image to find final DT using lower envelope
            for (int u = initial_data[0] - 1; u >= 0; u--) {
                resultado2[j][u] = CDT_f(u, s[q], (int) processar2[j][s[q]]); // só aqui é que é alterado a matrix output
                if (u == t[q])
                    q--;
            }

        }
        //enviar linhas para processo sicronizador


        end2 = MPI_Wtime();
        tempo_total2+= (end2 - start2);

        MPI_Send(&(resultado2[0][0]), initial_data[0] * partition2, MPI_UNSIGNED, 0, 0,
                 MPI_COMM_WORLD); // receber o tamanho de cada linha e numero de linhas que vai receber

        printf("Tempo paralelo %f; %d \n", tempo_total2, myrank);

    }

    MPI_Finalize();
    return (0);

}
