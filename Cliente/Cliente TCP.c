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

/**Función que toma el comando ingresado y separa ip y puerto para conexión y arma un mensaje para validación en el servidor.*/
void ingresar_comando(char **cmd_name_pass, char **ip, int *puerto){
	char login[50];
	char password[50];
	char *palabras;
	char *comando;
	char *nombre;
	int j, i=0;

	*puerto=6020;
	printf("\nIngresar usuario con el formato: connect usuario@numero_ip:port"
					"  con numero_ip a.b.c.d y port de 4 dígitos");

	printf("\nUsuario: ");
	fgets(login,50,stdin);
	printf("\nContraseña: ");
	fgets(password,50,stdin);

	palabras = strtok(login, " @:");
	comando = palabras;
	while (palabras !=NULL) {
		i++;
		palabras = strtok (NULL, " @:");
		if(i==1){nombre=palabras;}
		if(i==2){
			for (j = 0; j < strlen(palabras); j++) {
				(*ip)[j] = palabras[j];
			}
		}
		if(i==3){*puerto=(atoi(palabras));}
	}
	strcat(strcat(strcat(strcat(strcat(*cmd_name_pass,comando),"@"),nombre),"@"),password);
}

/**Función que escribe un mensaje y lo envía al cliente
Función llamada reiteradas veces por otras funciones que desean enviar datos*/
void escribir_mensaje(int sockfd, char *cadena){
	int n;
	/*Llamada al sistema para enviar datos.*/
	n = write(sockfd, cadena, strlen(cadena));
	if ( n < 0 ) {
		perror( "escritura en socket" );
		exit( 1 );
	}
}

/**Función que lee un mensaje.
Función llamada reiteradas veces por otras funciones que desean leer datos*/
void leer_mensaje(int sockfd, char *cadena){
	int n;
	/*Llamada al sistema para enviar datos.*/
	n = read( sockfd, cadena, TAM );
	if ( n < 0 ) {
		perror( "lectura de socket" );
		exit( 1 );
	}
}

int main( int argc, char *argv[] ) {
	int sockfd, puerto,validation=0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int terminar = 0;
	char buffer[TAM];
	char *ip=(char*)malloc(TAM*sizeof(char));
	char *cmd_name_pass=(char*)malloc(TAM*sizeof(char));

	ingresar_comando(&cmd_name_pass, &ip, &puerto);
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}
	printf("mensaje a servidor: %s\n", cmd_name_pass);

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

		if (validation==0) {
			escribir_mensaje(sockfd,cmd_name_pass);
		}
		if (validation==1) {
			printf( "\n\nmiguel@%s:%d:",ip,puerto);
			printf( "Ingrese el mensaje a transmitir: ");
			memset( buffer, '\0', TAM );
			fgets( buffer, TAM-1, stdin );
			escribir_mensaje(sockfd,buffer);
		}

		/*Verificando si se escribió: fin*/
		buffer[strlen(buffer)-1] = '\0';
		if( !strcmp( "fin", buffer ) ) {
			terminar = 1;
		}

		memset( buffer, '\0', TAM );
		leer_mensaje(sockfd, buffer);
		if (!strcmp( "Acceso garantizado", buffer )) {
			printf("Correcto!!!!\n");
			validation=1;
		}
		if(!strcmp( "Comando, nombre y/o contraseña incorrecto", buffer )){
			escribir_mensaje(sockfd,"fin");
			terminar=1;
		}
		printf( "Respuesta: %s\n", buffer);
		if( terminar ) {
			printf( "Finalizando ejecución\n" );
			exit(0);
		}
	}
	return 0;
}
