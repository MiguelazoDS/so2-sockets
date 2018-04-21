#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TAM 256

/*Función utilizada para leer información recibida.*/
ssize_t write (int fd, const void *buf, size_t count);
/*Función utilizada para mandar información por socket.*/
ssize_t read(int fd, void *buf, size_t count);
/*Función que permite atender múltiples clientes*/
pid_t fork(void);
/*Función que obtiene el id de un proceso.*/
pid_t getpid(void);
/*Función que cierra un socket*/
void close(int newsockfd);

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

	if(!strcmp(comando,"connect") && !strcmp(nombre,"miguel") && !strcmp(password,"root")){
		printf("El usuario es correcto\n");
	}
	printf("Mensaje recibido desde el cliente: c: %s n: %s p: %s\n", comando, nombre, password);
	printf("Longitudes: c: %d n: %d p: %d\n", (int)strlen(comando), (int)strlen(nombre), (int)strlen(password));

	return 0;
}

int main( int argc, char *argv[] ) {
	int sockfd, newsockfd, pid, puerto=6020;
	socklen_t clilen;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

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

			while ( 1 ) {
				memset( buffer, 0, TAM );

				n = read( newsockfd, buffer, TAM-1 );
				if ( n < 0 ) {
					perror( "lectura de socket" );
					exit(1);
				}
				verificar(buffer);
				printf( "PROCESO %d. ", getpid() );
				printf( "Recibí: %s", buffer );

				n = write( newsockfd, "Obtuve su mensaje", 18 );
				if ( n < 0 ) {
					perror( "escritura en socket" );
					exit( 1 );
				}
				/*Verificación de si hay que terminar*/
				buffer[strlen(buffer)-1] = '\0';
				if( !strcmp( "fin", buffer ) ) {
					printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
					exit(0);
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
