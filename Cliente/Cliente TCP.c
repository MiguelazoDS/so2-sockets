#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define TAM 256

/*Función utilizada para mandar información por socket.*/
ssize_t write (int fd, const void *buf, size_t count);
/*Función utilizada para leer información recibida.*/
ssize_t read(int fd, void *buf, size_t count);
/*Función que recibe una línea desde un archivo.*/
char *fgets(char *str, int n, FILE *stream);
/*Función que produce una demora de tiempo especificada por parámetro.*/
void sleep(int);

/*Función que verifica que el usuario y contraseña sea correcto*/
void validar(int *correcto, char *usuario, char *password, char *ip, int *puerto){
	char *comando;
	char *nombre;
	char *palabras;
	int i=0;
	int j;

	/*Divide el comando usando como separadores un "espacio", "@" y ":" y guarda esas divisiones en comando, nombre, ip y puerto*/
	palabras = strtok(usuario," @:");
	comando = palabras;
	while (palabras !=NULL) {
		i++;
		palabras = strtok (NULL, " @:");
		if(i==1){nombre=palabras;}
		if(i==2){
			for (j = 0; j < strlen(palabras); j++) {
				*(ip+j)=*(palabras+j);
			}
		}
		if(i==3){*puerto=(atoi(palabras));}
	}

	/*Verifica que comando, nombre, y puerto sean correctos y si lo son modifica la variable entera pasada por referencia "correcto"*/
	password[strlen(password)-1] = '\0';
	if(!strcmp("connect",comando))
		if(!strcmp("miguel",nombre))
			if(*puerto==6020)
				if(!strcmp("miguel",password))
					*correcto=1;
}

int main( int argc, char *argv[] ) {
	int sockfd, n, puerto, correcto=0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int terminar = 0;
	char buffer[TAM];
	/*Los siguientes arreglos guardan la información que se ingresa para el login*/
	char usuario[50];
	char password[50];
	char *ip=(char*)malloc(TAM*sizeof(char));

	printf("\nIngresar usuario con el formato: connect usuario@numero_ip:port"
					"  con numero_ip a.b.c.d donde a,b,c,d son valores de 3 dígitos y port es de 4 dígitos");
	/*Ciclo que dura hasta que el comando y la contraseña sean correctas*/
	do {
		printf("\nUsuario: ");
		fgets(usuario,50,stdin);
		printf("\nContraseña: ");
		fgets(password,50,stdin);
		validar(&correcto, usuario, password, ip, &puerto);
		if(!correcto) printf("\nnombre de usuario y/o contraseña incorrecto\n");
	} while(!correcto);

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}

	server = gethostbyname(ip);
	if (server == NULL) {
		fprintf( stderr,"Error, no existe el host\n" );
		exit( 0 );
	}
	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons( puerto);
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit( 1 );
	}

	while(1) {
		printf( "Ingrese el mensaje a transmitir: " );
		memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );

		n = write( sockfd, buffer, strlen(buffer) );
		if ( n < 0 ) {
			perror( "escritura de socket" );
			exit( 1 );
		}

		/*Verificando si se escribió: fin*/
		buffer[strlen(buffer)-1] = '\0';
		if( !strcmp( "fin", buffer ) ) {
			terminar = 1;
		}

		memset( buffer, '\0', TAM );
		n = read( sockfd, buffer, TAM );
		if ( n < 0 ) {
			perror( "lectura de socket" );
			exit( 1 );
		}
		printf( "Respuesta: %s\n", buffer );
		if( terminar ) {
			printf( "Finalizando ejecución\n" );
			exit(0);
		}
	}
	return 0;
}
