
#include <stdio.h>
#include <stdlib.h>
#include <pam.h>

int main2(int argc, char const *argv[]) {

      if (argc < 2){
        printf("Número de parâmetros inválidos\n");
        exit(-1);
      }

      FILE *fptr;
      if ((fptr = fopen(argv[1],"r")) == NULL){
        printf("Erro ao abrir ficheiro");
        // Program exits if the file pointer returns NULL.
        exit(1);
      }
      char c;
      c = fgetc(fptr);
      while (c != EOF)
        {
        printf ("%c", c);
        c = fgetc(fptr);
        }

      fclose(fptr);

}
