#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define TAM 1024

/*Función utilizada para leer información recibida.*/
ssize_t write (int fd, const void *buf, size_t count);
/*Función utilizada para mandar información por socket.*/
ssize_t read(int fd, void *buf, size_t count);
/*Función que permite atender múltiples clientes*/
pid_t fork(void);
/*Función que obtiene el id de un proceso.*/
pid_t getpid(void);
/*Función que cierra un socket*/
/*void close(int newsockfd);*/

void pwd(char *output, char *path){
	FILE *file;
	if( access(path, F_OK ) != -1){
  	printf("Existe el archivo\n");
		file=fopen("pwd","r");
		fgets(output,TAM,file);
		/*Necesario para borrar una nueva línea que se agrega al final.*/
		output[strlen(output)-1] = '\0';
		fclose(file);
	}
	else {
		printf("No existe el archivo\n");
		file=fopen("pwd","w");
		getcwd(output,TAM);
		fprintf(file, "%s\n", output);
		fclose(file);
	}
}

void cd(char *cadena){
	char *palabras;
	char *comando;
	char *directorio;
	char *final;
	int i=0;
	char cwd[TAM];

	palabras = strtok(cadena, " ");
	comando = palabras;
	while (palabras != NULL){
		i++;
		palabras = strtok(NULL, " ");
		if(i==1) {directorio = palabras;}
		if(i==2) {final = palabras;}
	}
	printf("P: %s\n", comando);
	if (final == NULL) {
		if (directorio == NULL) {
			chdir(getenv("HOME"));
		}
		else{
			chdir(directorio);
		}
	}

	getcwd(cwd,TAM);
	printf("working directory: %s\n", cwd);
}

void bash(char *output, char *comando){
	system(comando);
	output="salida";
}

/**Función que utiliza el mensaje con información de comando, usuario y contraseña del cliente
y verifica que coincidan con los que tiene guardados.*/
int verificar(char *buffer){
	char *palabras;
	char *comando;
	char *nombre;
	char *password;
	int i=0;

	palabras = strtok(buffer,"@");
	comando=palabras;
	while(palabras != NULL){
		i++;
		palabras = strtok (NULL, "@");
		if(i==1){nombre=palabras;}
		if(i==2){password=palabras;}
	}
	password[strlen(password)-1]='\0';

	if(!strcmp(comando,"connect") && !strcmp(nombre,"miguel") && !strcmp(password,"root")){
		return 1;
	}
	return 0;
}

/**Función que escribe un mensaje y lo envía al cliente
Función llamada reiteradas veces por otras funciones que desean enviar datos*/
void escribir_mensaje(int newsockfd, char *cadena){
int n;
	/*Llamada al sistema para enviar datos.*/
	n = write(newsockfd, cadena, strlen(cadena));
	if ( n < 0 ) {
		perror( "escritura en socket" );
		exit( 1 );
	}
}

/**Función que lee un mensaje.
Función llamada reiteradas veces por otras funciones que desean leer datos*/
void leer_mensaje(int newsockfd,  char *cadena){
	int n;
	/*Llamada al sistema para enviar datos.*/
	n = read( newsockfd, cadena, TAM-1);
	if ( n < 0 ) {
		perror( "lectura de socket" );
		exit( 1 );
	}
}

int main( int argc, char *argv[] ) {
	int sockfd, newsockfd, pid, puerto=6020;
	socklen_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	char directorio_inicial[TAM];
	char path[TAM];
	char output[TAM];
	char *intermedio=malloc(TAM*sizeof(char));

	char *final=malloc(TAM*sizeof(char));
	getcwd(directorio_inicial,TAM);

	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0 ) {
		perror( " apertura de socket ");
		exit( 1 );
	}

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( puerto );

	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit( 1 );
	}

  printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen( sockfd, 5 );
	clilen = sizeof( cli_addr );

	while( 1 ) {
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
		if ( newsockfd < 0 ) {
			perror( "accept" );
			exit( 1 );
		}

		pid = fork();
		if ( pid < 0 ) {
			perror( "fork" );
			exit( 1 );
		}

		if ( pid == 0 ) {  /*Proceso hijo*/
			close( sockfd );

			/****************Validación******************/
			memset( buffer, 0, TAM );
			leer_mensaje(newsockfd, buffer);
			if (verificar(buffer)){
				escribir_mensaje(newsockfd, "valido");
			}
			else{
				escribir_mensaje(newsockfd, "invalido");
			}
			/*********************************************/
			while ( 1 ) {
				memset( buffer, 0, TAM );
				leer_mensaje(newsockfd, buffer);

				printf( "PROCESO %d. ", getpid() );
				printf( "Recibí: %s", buffer );


				/*Verificación de si hay que terminar*/
				buffer[strlen(buffer)-1] = '\0';
				if( !strcmp( "fin", buffer ) ) {
					printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
					exit(0);
				}
				else if (!strcmp("?",buffer)) {
					escribir_mensaje(newsockfd, "\n\nComandos válidos:\n"
																			"-\"?\": Devuelve este mensaje\n"
																			"-\"descargar archivo\": Descarga (si existe) el archivo\n"
																			"-\"cd path\": Cambia de directorio (si el destino existe)\n"
																			"-\"pwd\": Devuelve el directorio actual\n"
																			"-Cualquier otro comando es interpretado por el Bash del servidor\n");
				}
				else if (!strcmp("pwdf",buffer)){
					memset(path, '\0', TAM);
					memset(output, '\0', TAM);
					strcat(path,directorio_inicial);
					strcat(path,"/pwd");
					pwd(output, path);
					escribir_mensaje(newsockfd, output);
				}
				else if(!strncmp("cd ",buffer,3)){
					escribir_mensaje(newsockfd, "Las dos primeras letras son cd más un espacio");
					cd(buffer);
				}
				else{
					memset(path, '\0', TAM);
					memset(output, '\0', TAM);
					strcat(path,directorio_inicial);
					strcat(path,"/Bash/Bash");
					sprintf(intermedio, "\"%s\"", path);
					sprintf(final, "%s %s", intermedio, buffer);
					system(final);
					escribir_mensaje(newsockfd, "salida");
				}

			}
		}
		else {
			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close( newsockfd );
		}
	}
	return 0;
}
