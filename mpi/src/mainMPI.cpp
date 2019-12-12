#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "pam.h"


int maxdistance=0;

//Chessboard DT F function
int CDT_f(int x,int i,int g_i)
{
    return fmax(fabs(x-i),g_i);
}

//Chessboard DT SEP function
int CDT_sep(int i,int u, int g_i,int g_u)
{
    if(g_i<=g_u)
        return fmax(i+g_u,floor((i+u)/2));
    else
        return fmin(u-g_i,floor((i+u)/2));
}


gray ** run_sequencial(gray ** matrix, int rows, int cols){
	gray ** resultado = pgm_allocarray(cols,rows);
	gray ** output    = pgm_allocarray(cols,rows);
	//Fase 1 - EZ CLAP
	for(int i=0;i<rows;i++){
		//ACHO QUE ESTA MERDA NAO È PRECISO DE MEXER NAS BORDAS

			//if border is > 0 make it 0 else 255
			/*
			if(matrix[i][0])
					resultado[i][0]=0;
			else
					resultado[i][0]=255;
			*/

			//check for obstacle in the entire row
			//Left to right pass
			for(int j=1;j<cols;j++){
					if(matrix[i][j])
							resultado[i][j]=min(255,1+resultado[i][j-1]);
					else
							resultado[i][j]=0;
			}
			//Right to left pass
			for(int j=cols-2;j>=0;j--){
					if (resultado[i][j+1]<resultado[i][j])
							resultado[i][j]=min(255,1+resultado[i][j+1]);
			}
	}

	// Fase 2 plz help


	 //Lower envelope indices
	 int s[rows];
	 // same least minimizers
	 int t[rows];
	 int q=0;
	 int w;
	 for (int j=0;j<rows;j++)
	 {
	 		//intialise variables
	 		q=0;
	 		s[0]=0;
	 		t[0]=0;
					//Top to bottom scan To compute paritions of [0,m)
					for (int u=1;u<cols;u++){
							while(q>=0 && ((CDT_f(t[q],s[q],(int)resultado[s[q]][j]))>CDT_f(t[q],u,(int)resultado[u][j])))
									q--;
							if(q<0){
									q=0;
									s[0]=u;
							}
							else{
									//Finds sub-regions
									w = 1+CDT_sep(s[q],u,(int)resultado[s[q]][j],(int)resultado[u][j]);
									if(w<cols){
											q++;
											s[q]=u;
											t[q]=w;
									}
							}
					}
					//bottom to top of image to find final DT using lower envelope
					for (int u=cols-1;u>=0;u--){
							output[u][j]= CDT_f(u,s[q],(int)resultado[s[q]][j]); // só aqui é que é alterado a matrix output
							if(u==t[q])
									q--;
								}
					}

	return output;
}

gray ** run_parallel(gray ** matrix, int rows, int cols){


	return matrix;
}

int main(int argc, char *argv[]) {

	// Cenas MPI
	int nprocesses, nprocess_total;
	int myrank;
	MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &nprocesses);
	MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

		// processo que le imagens, escreve e afins
	if(myrank == 0){

			unsigned int  maxval, row, col, min;
		  int rows, cols;
			//Variáveis para medição temporal e apontador para os ficheiros de output
		  double start, end;
		  FILE * fptr;
			//Inicialização da PAPI
			gray ** final;
			gray ** matrix;
			gray ** output;
			gray ** resultado;

			pm_init(argv[0], 0);
			matrix = pgm_readpgm(stdin, &cols, &rows, &maxval);
			output = pgm_allocarray(cols, rows);
			resultado = pgm_allocarray(cols, rows);
      gray ** transpose   = pgm_allocarray(cols,rows);

      int partition = rows / (nprocesses-1); // Elementos para cada processo (-1 pq processo 0 nao processa imagens so sicroniza)
			int aux[2] = {cols,partition};
			int i = 1;
			//enviar tamanho de linhas e colunas para processos alugarem memoria necessaria
			for(; i < nprocesses - 1; i++){
			//	printf("Enviar para %d\n",i );
				MPI_Send(aux, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			// enviar last chunk
	//		printf("Enviar para %d\n",i );
			int aux_2[2] = {cols,rows - (partition*(i-1))}; // alterar o aux[2] está a dar bad trip
			//aux[2] = rows - (partition*i);  // <- Isto da erro se alterar esta nao sei pq, bruxaria
			MPI_Send(aux_2, 2, MPI_INT, i, 0, MPI_COMM_WORLD);

	//		printf("Teste Send Matrix" );
			//enviar linhas para processar
			for(i = 1; i < nprocesses - 1; i++){
				for( int j = 0; j < partition; j++){
					MPI_Send(matrix[(i-1)*partition+j], cols, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
			//		printf("Enviar linha %d para %d\n",i*partition+j, i  );
				}
			}
			// enviar last chunk

			for( int j = 0; j < rows - (partition*(i-1)); j++){
				MPI_Send(matrix[(i-1)*partition+j], cols, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
			//	printf("Enviar linha %d para %d\n",i*partition+j, i  );
			}

			//receber linhas  processadas
			for(i = 1; i < nprocesses - 1; i++){
				for( int j = 0; j < partition; j++){
					MPI_Recv(resultado[(i-1)*partition+j], cols, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
			//		printf("Enviar linha %d para %d\n",i*partition+j, i  );
				}
			}
			// enviar last chunk

			for( int j = 0; j < rows - (partition*(i-1)); j++){
				MPI_Recv(resultado[(i-1)*partition+j], cols, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
			//	printf("Enviar linha %d para %d\n",i*partition+j, i  );
			}
			printf("Chegou aqui\n"  );
			//Lower envelope indices
			int s[rows];
			// same least minimizers
			int t[rows];
			int q=0;
			int w;
			for (int j=0;j<rows;j++)
			{
				 //intialise variables
				 q=0;
				 s[0]=0;
				 t[0]=0;
						 //Top to bottom scan To compute paritions of [0,m)
						 for (int u=1;u<cols;u++){
								 while(q>=0 && ((CDT_f(t[q],s[q],(int)resultado[s[q]][j]))>CDT_f(t[q],u,(int)resultado[u][j])))
										 q--;
								 if(q<0){
										 q=0;
										 s[0]=u;
								 }
								 else{
										 //Finds sub-regions
										 w = 1+CDT_sep(s[q],u,(int)resultado[s[q]][j],(int)resultado[u][j]);
										 if(w<cols){
												 q++;
												 s[q]=u;
												 t[q]=w;
										 }
								 }
						 }
						 //bottom to top of image to find final DT using lower envelope
						 for (int u=cols-1;u>=0;u--){
								 output[u][j]= CDT_f(u,s[q],(int)resultado[s[q]][j]); // só aqui é que é alterado a matrix output
								 if(u==t[q])
										 q--;
									 }
						 }




			//Abrir o apontador para o ficheiro de output
			if ((fptr = fopen("paralela.pgm","w+")) == NULL){
				printf("Erro ao abrir ficheiro");
				exit(1);
			}

			pgm_writepgm(fptr, output, cols, rows, maxval, 1);
			fclose(fptr);

			}
	// Processos trabalhadores
	else{
    int initial_data[2] = {0,0};
		MPI_Recv( initial_data, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status ); // receber o tamanho de cada linha e numero de linhas que vai receber
		printf("Este recebeu  = %d , COL = %d ; ROW = %d \n", myrank, initial_data[0], initial_data[1]);


		gray ** processar    = pgm_allocarray(initial_data[0], initial_data[1]);
		gray ** resultado    = pgm_allocarray(initial_data[0], initial_data[1]);
		for (int i = 0; i <  initial_data[1]; i++ ){
			MPI_Recv( processar[i], initial_data[0], MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, &status  ); // receber linhas
 		//	printf("Eu %d Recebi linha %d\n",myrank,i );
		}

		//Fazer fase 1
		for(int i=0;i<initial_data[1];i++){
				//check for obstacle in the entire row
				//Left to right pass

        if(resultado[i][0])
  					resultado[i][0]=0;
  			else
  					resultado[i][0]=255;


  			//check for obstacle in the entire row
  			//Left to right pass
  			for(int j=1;j<initial_data[0];j++){
  					if(processar[i][j])
  							resultado[i][j]=min(255,1+resultado[i][j-1]);
  					else
  							resultado[i][j]=0;
  			}
  			//Right to left pass
  			for(int j=initial_data[0]-2;j>=0;j--){
  					if (resultado[i][j+1]<resultado[i][j])
  							resultado[i][j]=min(255,1+resultado[i][j+1]);
  			}
		}

		//enviar linhas para processo sicronizador

		for (int i = 0; i <  initial_data[1]; i++ ){
			MPI_Send( resultado[i], initial_data[0], MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD); // receber o tamanho de cada linha e numero de linhas que vai receber
 		//	printf("Eu %d enviar linha %d\n",myrank,i );
		}
	}

  MPI_Finalize ();
  return(0);

}
