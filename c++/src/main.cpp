#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <future>
#include <time.h>

short unsigned int * matrix;
short unsigned int * output;
int rows, cols;
int iterador = 1;
long thread_count;

double clearcache [30000000];

void clearCache (void) {
	for (unsigned i = 0; i < 30000000; ++i)
		clearcache[i] = i;
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

int run_thread(int my_rank) {
	int fatia = rows / thread_count;
	int row;
	int flag = 0;
	if(thread_count - 1 == my_rank){
		row = fatia * my_rank;
		fatia = rows - (fatia*my_rank);
	}else{
		row = fatia * my_rank;
	}
	int min, i, j, aux;
	int max_size = row + fatia;
	for(row; row < max_size; row++){
		for (int col = 0; col < cols; col++) {
			if(iterador){
				aux = output[row * cols + col];
				min = matrix[row * cols + col] -1;
				if (min != 255)
					for(i = -1; i < 2; i++)
						for(j = -1; j < 2; j++)
							if(row + i > 0 && row + i < rows)
								if(col + j > 0 && col + j < cols)
									if(matrix[(row + i) * cols + col + j] < min){
										min = matrix[(row + i) * cols + col + j];
									}
				aux -= output[row * cols + col] = min + 1;
			}
			else {
				aux = matrix[row * cols + col];
				min = output[row * cols + col] -1;
				if (min != 255)
					for(i = -1; i < 2; i++)
						for( j = -1; j < 2; j++)
							if(row + i > 0 && row + i < rows)
								if(col + j > 0 && col + j < cols)
									if(output[(row + i) * cols + col + j] < min){
										min = output[(row + i) * cols + col + j];
									}
				aux -= matrix[row * cols + col] = min + 1;
			}
			if(aux != 0)
				flag = 1;
		}
	}
   return flag;
}  

short unsigned int * run_parallel(short unsigned int * matrix, short unsigned int * output, int rows, int cols){
	int flag = 1;
	long flags[thread_count] = {0};
	std::future<int> thread_handles[thread_count];
	long thread;
	while (flag){
		for (thread = 0; thread < thread_count; thread++){
			thread_handles[thread] = std::async(&run_thread, thread);
		}  
		for (thread = 0; thread < thread_count; thread++){
			flags[thread] = thread_handles[thread].get();
		}

    	iterador = !iterador;
		flag = 0;
		for (thread = 0; thread < thread_count; thread++){
			if(flags[thread] == 1){
				flag = 1; break;
			}
		}
	}
	if(iterador)
		return matrix;
	else
		return output;
}



int main(int argc, char const *argv[]) {
    FILE * fptr;

	if (argc != 3) exit(0);
	thread_count = strtol(argv[1], NULL, 10);
	
	if ((fptr = fopen(argv[2],"r")) == NULL){
		printf("Erro ao abrir ficheiro\n");
		exit(1);
	}

	short unsigned int * final;

	matrix = my_read_pgm(fptr, &cols, &rows);
	fclose(fptr);

	output  = my_aloc_pgm(cols, rows);

	long start, end;
    struct timeval timecheck;

    gettimeofday(&timecheck, NULL);
    start = (long) timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
	
	final = run_parallel(matrix, output, rows, cols, thread_handles);

	gettimeofday(&timecheck, NULL);
    end = (long) timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
	printf("Exec time: %.3f\n", (((double) (end - start))/1000);

	if ((fptr = fopen("output.pgm","w")) == NULL){
			printf("Erro ao abrir ficheiro\n");
			exit(1);
		}

	my_write_pgm(fptr, final, cols, rows);
	fclose(fptr);
}
