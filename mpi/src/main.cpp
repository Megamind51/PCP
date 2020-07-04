#include <mpi.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h> 
#include <time.h>


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

void transposta(unsigned short int * dst, unsigned short int * src, int rowsI, int colsI) {
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
                    dst[((j + b)*rowsI) + i + a] = src[((i + a)*colsI) + j + b];
                }
            }
        }
    }

    // dar fix aos blocos nao processados por nao ser divisivel
    if (rows % block != 0 && cols % block != 0) {
        for (int i = rows; i < rowsI; i++) {
            for (int j = 0; j < colsI; j++) {
                dst[j*rowsI + i] = src[i*colsI + j];
            }
        }
        for (int i = 0; i < rowsI; i++) {
            for (int j = cols; j < colsI; j++) {
                dst[j*rowsI + i] = src[i*colsI + j];
            }
        }
    } else if (rows % block != 0) {
        for (int i = rows; i < rowsI; i++) {
            for (int j = 0; j < colsI; j++) {
                dst[j*rowsI + i] = src[i*colsI + j];
            }
        }
    } else if (cols % block != 0) {
        for (int i = 0; i < rowsI; i++) {
            for (int j = cols; j < colsI; j++) {
                dst[j*rowsI + i] = src[i*colsI + j];
            }
        }
    }


}


short unsigned int * my_aloc_pgm(int rows, int cols){	
	return  (short unsigned int *) malloc(rows*cols*sizeof(short unsigned int));
}

short unsigned int * my_read_pgm(FILE * f, int * rows, int * cols){
	char line[128];
	int flag = 1, j = 0;
	while (j < 4 ) {
		fgets(line, 128, f);
		j++;
        if (flag)
			if (sscanf(line, "%d %d", rows, cols) == 2)
				flag = 0;	
    }
	short unsigned int * matrix = my_aloc_pgm(*rows, *cols);

	for (int i = 0; i < *rows; i++)
	{
		for (int j = 0; j < *cols; j++)
		{
			fscanf(f, "%hd\n", &matrix[i * (*cols) + j]);
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

        unsigned short int * final;
        unsigned short int * matrix;
        unsigned short int * output;
        unsigned short int * resultado;
        unsigned short int * transpose;

        if ((fptr = fopen(argv[1],"r")) == NULL){
		    printf("Erro ao abrir ficheiro\n");
		    exit(1);
	    }


        matrix = my_read_pgm(fptr, &cols, &rows);
        fclose(fptr);

        int aux[2] = {rows, cols};
        start = MPI_Wtime();

        MPI_Bcast(aux, 2, MPI_INT, 0, MPI_COMM_WORLD);
        output = my_aloc_pgm(rows, cols);
        resultado = my_aloc_pgm(cols, rows);
        transpose = my_aloc_pgm(rows, cols);



        // Determinar número de linhas a enviar a cada processo
        // Arrays com as dimensões dos dados a ser enviados aos processos
        int partition = rows / (nprocesses - 1);
        int tamanho_ultimo_processo = rows - (partition * (nprocesses - 2));
        int i = 1;

        double t  = time();

        // Enviar número de linhas e colunas aos processos
        for (; i < nprocesses - 1; i++) {
            MPI_Send(&(matrix[(i - 1) * partition]), cols * partition, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);

        }
        // Enviar dimensão dos dados para o último processo
        MPI_Send(&(matrix[(i - 1) * partition]), cols * tamanho_ultimo_processo, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);

        //receber linhas  processadas
        for (i = 1; i < nprocesses - 1; i++) {
            MPI_Recv(&(resultado[(i - 1) * partition]), cols * partition, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD,
                     &status);
        }
        // receber last chunk
        MPI_Recv(&(resultado[(i - 1) * partition]), cols * tamanho_ultimo_processo, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD, &status);

        // fazer transposta para enviar para processos linhas à mesma

        transposta(transpose, resultado, rows, cols);


        // redefinir variaveis apos transposta
        partition = cols / (nprocesses - 1);
        tamanho_ultimo_processo = cols - (partition * (nprocesses - 2));
        i = 1;

        // Enviar número de linhas e colunas aos processos apos transposta
        for (; i < nprocesses - 1; i++) {
            MPI_Send(&(transpose[(i - 1) * partition]), rows * partition, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        }
        // Enviar dimensão dos dados para o último processo apos transposta
        MPI_Send(&(transpose[(i - 1) * partition]), rows * tamanho_ultimo_processo, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);


        //receber linhas  processadas apos fase 2
        for (i = 1; i < nprocesses - 1; i++) {
            MPI_Recv(&(output[(i - 1) * partition]), rows * partition, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD, &status);
        }
        // receber last chunk apos fase 2
        MPI_Recv(&(output[(i - 1) * partition]), rows * tamanho_ultimo_processo, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD, &status);

        // transposta para voltar ao formato inicial
        transposta(matrix, output, cols, rows);
        
        t = time() - t;
        printf("Exec time: %.3f\n", t);



        //Abrir o apontador para o ficheiro de output
        if ((fptr = fopen("output.pgm", "w+")) == NULL) {
            printf("Erro ao abrir ficheiro");
            exit(1);
        }

        my_write_pgm(fptr, matrix, cols, rows);
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

        unsigned short int * processar = my_aloc_pgm(initial_data[1], partition );
        unsigned short int * resultado = my_aloc_pgm(initial_data[1], partition );

        MPI_Recv(&(processar[0]), partition * initial_data[1], MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD,
                 &status); // receber linhas

        //Fazer fase 1
        for (int i = 0; i < partition; i++) {
            //check for obstacle in the entire row
            //Left to right pass

            if (resultado[i * initial_data[1]])
                resultado[i * initial_data[1]] = 0;
            else
                resultado[i * initial_data[1]] = 255;

            //check for obstacle in the entire row
            //Left to right pass
            for (int j = 1; j < initial_data[1]; j++) {
                if (processar[i * initial_data[1] + j]){
                    resultado[i * initial_data[1] + j]= std::min(255, 1 + resultado[i * initial_data[1] + j - 1]);
                }
                else
                    resultado[i * initial_data[1] + j] = 0;
            }
            //Right to left pass
            for (int j = initial_data[1] - 2; j >= 0; j--) {
                if (resultado[i * initial_data[1] + j + 1] < resultado[i * initial_data[1] + j ] )
                    resultado[i * initial_data[1] + j ] = std::min(255, 1 + resultado[i * initial_data[1] + j + 1]);
            }
        }

        //enviar linhas para processo sicronizador
        MPI_Send(&(resultado[0]), partition * initial_data[1], MPI_UNSIGNED_SHORT, 0, 0,
                 MPI_COMM_WORLD); 

         partition = initial_data[1] / (nprocesses - 1);
         if ( myrank == nprocesses - 1 ){
             partition = initial_data[1] - (partition * (nprocesses - 2));
         }

        processar = my_aloc_pgm( initial_data[1], partition );
        resultado = my_aloc_pgm( initial_data[1], partition );


        MPI_Recv(&(processar[0]), initial_data[0] * partition, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD,
                 &status); 

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
                       ((CDT_f(t[q], s[q], (int) processar[j*initial_data[0] + s[q]])) > CDT_f(t[q], u, (int) processar[j*initial_data[0] + u])))
                    q--;
                if (q < 0) {
                    q = 0;
                    s[0] = u;
                } else {
                    //Finds sub-regions
                    w = 1 + CDT_sep(s[q], u, (int) processar[j*initial_data[0] + s[q]], (int) processar[j*initial_data[0] + u]);
                    if (w < initial_data[0]) {
                        q++;
                        s[q] = u;
                        t[q] = w;
                    }
                }

            }
            //bottom to top of image to find final DT using lower envelope
            for (int u = initial_data[0] - 1; u >= 0; u--) {
                resultado[j*initial_data[0] + u] = CDT_f(u, s[q], (int) processar[j*initial_data[0] + s[q]]); // só aqui é que é alterado a matrix output
                if (u == t[q])
                    q--;
            }

        }
        //enviar linhas para processo sicronizador

        MPI_Send(&(resultado[0]), initial_data[0] * partition, MPI_UNSIGNED_SHORT, 0, 0,
                 MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return (0);

}
