#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "line_counter.h"
#define TAM 64

/*Función utilizada para leer información recibida.*/
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/*Función utilizada para mandar información por socket.*/
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

int main( int argc, char *argv[] ) {
	int sockfd, puerto, size, lineas;
	socklen_t tamano_direccion;
	char buffer[ TAM ];
	struct sockaddr_in serv_addr;
	int n, contador=1;
	FILE *file;

	if ( argc < 2 ) {
        	fprintf( stderr, "Uso: %s <puerto>\n", argv[0] );
		exit( 1 );
	}

	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
	if (sockfd < 0) {
		perror("ERROR en apertura de socket");
		exit( 1 );
	}

	memset( &serv_addr, 0, sizeof(serv_addr) );
	puerto = atoi( argv[1] );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( puerto );
	memset( &(serv_addr.sin_zero), '\0', 8 );

	if( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) {
		perror( "ERROR en binding" );
		exit( 1 );
	}

    printf( "Socket disponible: %d\n", ntohs(serv_addr.sin_port) );

    tamano_direccion = sizeof( struct sockaddr );

    memset( buffer, 0, TAM );
    n = recvfrom( sockfd, buffer, TAM-1, 0, (struct sockaddr *)&serv_addr, &tamano_direccion );
    if ( n < 0 ) {
        perror( "lectura de socket" );
        exit( 1 );
    }
		n = sendto( sockfd, (void *)argv[2], TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );

    /*printf( "Recibí: %s", buffer );*/
		file=fopen(argv[2],"rb");
		fseek(file,0,SEEK_END);
		size=ftell(file);
		fseek(file,0,SEEK_SET);
		lineas=line_counter(file);
		/*fread(buffer,TAM,1,file);
		printf("tamaño: %d\n", (int)(size));
		printf("archivo: %s\n", buffer);*/
		memset( buffer, 0, sizeof( buffer ) );
		printf("%d\n", lineas);
		while(contador*TAM<size/*(int)ftell(file)!=size!feof(file)fread(buffer,TAM,1,file)*/){
    		fread(buffer,TAM,1,file);
				lineas--;
				printf("posicion: %d tamaño: %d\n", (int)ftell(file), size);
				/*printf("leyendo: %s", buffer);*/
        n = sendto( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
        if ( n < 0 ) {
            perror( "escritura en socket" );
            exit( 1 );
        }
				usleep(10);
				memset( buffer, 0, sizeof( buffer ) );
				/*fread(buffer,TAM,1,file);*/
        contador++;
    }

		n = sendto( sockfd, (void *)"termine", 7, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		memset( buffer, 0, sizeof( buffer ) );
		/*sprintf(buffer,"%d",size%(int)ftell(file));*/
		fread(buffer,size%(int)ftell(file),1,file);
		printf("posicion: %d tamaño: %d\n", (int)ftell(file), size);

		n = sendto( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );

		/*n = sendto( sockfd, (void *)"termine", TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
*/
	/*	memset( buffer, 0, sizeof( buffer ) );
		fread(buffer,size%(int)ftell(file),1,file);
		printf("posicion: %d tamaño: %d\n", (int)ftell(file), size);

		n = sendto( sockfd, (void *)buffer, size%(int)ftell(file), 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		if ( n < 0 ) {
				perror( "escritura en socket" );
				exit( 1 );
		}*/
		/*memset( buffer, 0, sizeof( buffer ) );
		size=size%TAM;
		fread(buffer,size,1,file);
		printf("archivo: %s\n", buffer);
    n = sendto( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		n = sendto( sockfd, (void *)"termine", 7, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );*/
		fclose(file);
	return 0;
}
