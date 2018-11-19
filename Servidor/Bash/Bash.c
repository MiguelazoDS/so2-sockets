/***************************************************************************//**
@file         Bash.h
@author       Stephen Brennan
@date         Thursday,  8 January 2015
@brief        LSH (Libstephen SHell)
*******************************************************************************/
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define directorio_actual 500
#define BUFFSIZE 256

char output[BUFFSIZE];
int nbytes;

int lsh_launch(char **args){
  /*Se implementa un tubería para enviar la salida del comando execvp a un archivo*/
  pid_t pid;
  int link[2];

  /*Error*/
  if (pipe(link)==-1)
   die("pipe");

  /*Error*/
  if ((pid = fork()) == -1)
   die("fork");

  /*La salida de execvp la conecta al pipe.*/
  if (pid == 0) {
   dup2(link[1],STDOUT_FILENO);
   close(link[0]);
   close(link[1]);
   execvp(args[0], args);
   die("execl");
  }
  /*De la otra punta del pipe lee y guarda en un archivo.*/
  else {
   close(link[1]);
   nbytes=read(link[0],output, sizeof(output));
   wait(NULL);
  }

  return 0;
}

int lsh_execute(char **args){
  if (args[0] == NULL) {
   /* Se ingresó un comando vacío.*/
   return 0;
  }

  return lsh_launch(args);
}

/*Guarda en el archivo "output" el contenido de la variable "output".*/
void guardar_archivo(FILE *file){
  file=fopen("output","w");
  fprintf(file, "%s\n", output);
  fclose(file);
}

int main(int argc, char **argv){
  int status, i;
  /*Inicializamos para evitar error*/
  FILE *file=NULL;
  /*Ejecuta hasta que status no sea 1. Todos los comandos retornan 0. Se ejecuta una vez y sale.*/
  do {
    /*Guarda en argv todos los argumentos enviados, evitando la ejecución del programa*/
    for(i=0; i<argc; i++){
      argv[i]=argv[i+1];
    }
    status = lsh_execute(argv);
    /*Llamada a función.*/
    guardar_archivo(file);
  } while (status);

  return EXIT_SUCCESS;
}
