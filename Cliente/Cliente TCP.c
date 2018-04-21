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
	char **aux=NULL;
	char *comando;
	char *nombre;
	char *palabras;
	int i=0;
	int j;
	*aux=usuario;
	/*Divide el comando usando como separadores un "espacio", "@" y ":" y guarda esas divisiones en comando, nombre, ip y puerto*/
	palabras = strtok(usuario," @:");
	comando = palabras;
	while (palabras !=NULL) {
		i++;
		palabras = strtok (NULL, " @:");
		if(i==1){nombre=palabras;}
		if(i==2){
			for (j = 0; j < strlen(palabras); j++) {
				/**(ip+j)=*(palabras+j);*/
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

	usuario=*aux;
}

void ingresar_comando(char **cmd_name_pass, char **ip, int *puerto){
	/*int correcto=1;*/
	char login[50];
	char password[50];
	char *palabras;
	char *comando;
	char *nombre;
	int j, i=0;
	/**cmd_name_pass="hola";*/
	/**ip="192.168.0.7";*/

	*puerto=6020;
	printf("\nIngresar usuario con el formato: connect usuario@numero_ip:port"
					"  con numero_ip a.b.c.d y port de 4 dígitos");
	/*Ciclo que dura hasta que el comando y la contraseña sean correctas*/
	/*do {*/
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
				/**(ip+j)=*(palabras+j);*/
				/**ip[j] = palabras[0];*/
				(*ip)[j] = palabras[j];
				printf("%c %c\n", (*ip)[j], palabras[j]);

			}
		}
		if(i==3){*puerto=(atoi(palabras));}
	}
	printf("%s %s %s %d", nombre, comando, *ip, *puerto);
	strcat(strcat(strcat(strcat(strcat(*cmd_name_pass,comando),"@"),nombre),"@"),password);
		/*validar(cmd_name_pass, comando, password, ip, puerto);*/
		/*if(!correcto) printf("\nnombre de usuario y/o contraseña incorrecto\n");*/
	/*} while(!correcto);*/
}

/**Función que escribe un mensaje y lo envía al cliente
Función llamada reiteradas veces por otras funciones que desean enviar datos*/
void escribir_mensaje(int sockfd, int n, char *cadena){
	/*Llamada al sistema para enviar datos.*/
	n = write(sockfd, cadena, strlen(cadena));
	if ( n < 0 ) {
		perror( "escritura en socket" );
		exit( 1 );
	}
}

/**Función que lee un mensaje.
Función llamada reiteradas veces por otras funciones que desean leer datos*/
void leer_mensaje(int sockfd, int n, char *cadena){
	/*Llamada al sistema para enviar datos.*/
	n = read( sockfd, cadena, TAM );
	if ( n < 0 ) {
		perror( "lectura de socket" );
		exit( 1 );
	}
}

int main( int argc, char *argv[] ) {
	int sockfd, n, puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int terminar = 0;
	char buffer[TAM];
	char *ip=(char*)malloc(TAM*sizeof(char));
	char *cmd_name_pass=(char*)malloc(TAM*sizeof(char));

	ingresar_comando(&cmd_name_pass, &ip, &puerto);
	printf("cmd_name_pass: %s ip: %s puerto: %d\n", cmd_name_pass, ip, puerto);
	printf("IP: %s\n", ip);
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
		escribir_mensaje(sockfd,n,cmd_name_pass);
		memset( buffer, '\0', TAM );
		leer_mensaje(sockfd,n,buffer);
		printf( "\n\nmiguel@%s:%d:",ip,puerto);

		printf( "Ingrese el mensaje a transmitir: ");
		memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );

		/*n = write( sockfd, buffer, strlen(buffer) );
		if ( n < 0 ) {
			perror( "escritura de socket" );
			exit( 1 );
		}*/
		escribir_mensaje(sockfd,n,buffer);

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
