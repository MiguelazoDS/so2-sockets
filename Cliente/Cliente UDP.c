#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define TAM 256

/*Función utilizada para leer información recibida.*/
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/*Función utilizada para mandar información por socket.*/
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);


int main( int argc, char *argv[] ) {
	int sockfd, puerto, n, contador=3;
	socklen_t tamano_direccion;
	struct sockaddr_in dest_addr;
	struct hostent *server;

	char buffer[TAM];
	if (argc < 3) {
		fprintf( stderr, "Uso %s host puerto\n", argv[0] );
		exit(0);
	}

	server = gethostbyname( argv[1] );
	if ( server == NULL ) {
		fprintf( stderr, "ERROR, no existe el host\n");
		exit(0);
	}

	puerto = atoi( argv[2] );
	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
	if (sockfd < 0) {
		perror( "apertura de socket" );
		exit( 1 );
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons( puerto );
	dest_addr.sin_addr = *( (struct in_addr *)server->h_addr );
	memset( &(dest_addr.sin_zero), '\0', 8 );

	/*printf( "Ingrese el mensaje a transmitir: " );
	memset( buffer, 0, TAM );
	fgets( buffer, TAM, stdin );*/

	tamano_direccion = sizeof( dest_addr );
	n = sendto( sockfd, (void *)"ok", TAM, 0, (struct sockaddr *)&dest_addr, tamano_direccion );
	if ( n < 0 ) {
		perror( "Escritura en socket" );
		exit( 1 );
	}
	while(contador){
        memset( buffer, 0, sizeof( buffer ) );
        n = recvfrom( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
        if ( n < 0 ) {
            perror( "Lectura de socket" );
            exit( 1 );
        }
        printf( "Respuesta: %s\n", buffer );
        contador--;
    }
    memset( buffer, 0, sizeof( buffer ) );
    n = recvfrom( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
    printf( "Respuesta: %s\n", buffer );

	return 0;
}
