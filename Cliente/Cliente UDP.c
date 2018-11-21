#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define TAM 245

/*Función utilizada para leer información recibida.*/
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/*Función utilizada para mandar información por socket.*/
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);


int main( int argc, char *argv[] ) {
	int sockfd, puerto, n, bucle=1;
	socklen_t tamano_direccion;
	struct sockaddr_in dest_addr;
	struct hostent *server;
	FILE *file;
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
	n = recvfrom( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
	printf("Nombre del archivo: %s\n", buffer);
	file=fopen(buffer,"wb");

	while(bucle){
				memset( buffer, 0, sizeof( buffer ) );

        n = recvfrom( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
        if ( n < 0 ) {
            perror( "Lectura de socket" );
            exit( 1 );
        }
				/*printf( "Respuesta: %s\n", buffer);*/

				if(!strcmp("termine",buffer)){
					n = sendto( sockfd, (void *)"ok", TAM, 0, (struct sockaddr *)&dest_addr, tamano_direccion );

					bucle=0;
				}
				else{
					fwrite(buffer, TAM, 1, file);
					/*memset( buffer, 0, sizeof( buffer ) );*/
				}
    }

		memset( buffer, 0, sizeof( buffer ) );

		n = recvfrom( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
		/*printf( "Respuesta: %s\n", buffer);*/

		fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	return 0;
}
