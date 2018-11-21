/**@file line_counter.h
@author Cazajous Miguel A.
@date 26 Abr 2017.
@brief Contador de líneas.*/

#include <stdio.h>
#include <stdlib.h>

/**Cuenta las líneas que tiene un archivo.
En este header se define una función que cuenta las líneas de un archivo
para ello verifica que cada caracter sea igual a un salto de línea "\n"

@param FILE *file
Recibe un puntero a archivo (el cual debe haber recibido antes la devolución de una función de apertura)*/
int line_counter(FILE *file){
	int counter=0;
	char c;
	/*Se va leyendo un caracter por vez hasta que se lee el final del archivo
	y cada vez que coincide con un salto de línea "\n" se aumenta la variable contador*/
	do{
		c=fgetc(file);
		if(c=='\n'){
			counter++;;
		}
	}
	while(!feof(file));
	/*Vuelve el puntero al incio del archivo*/
	fseek(file,0,SEEK_SET);
	return counter;
}
