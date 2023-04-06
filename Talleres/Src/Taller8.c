/*
 * Taller8.c
 *
 *  Created on: 30/03/2023
 *      Author: karol
 */


// Taller 6 - Paso por valor y paso por referencia

#include <stdint.h>

// Creacion de una funcion que duplique el valor de un numero

// 1.0 Paso por valor Básico

void duplicarNumero(uint8_t numero);

// 1.1 Paso por referencia

void duplicarNumeroRef(uint8_t *numero);

// 1.2 Paso por valor reasignando variables.

uint8_t duplicarNumeroReturn(uint8_t numero);

// 1.3 Arreglos por Referencia

void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas);

// ***** // SOLUCION EJERCICIO // ***** //

void stringCaseConverter(uint8_t *string);

int main(void){

	uint8_t variable = 10;
	duplicarNumeroRef(&variable);                //variable = duplicarNumero(variable); // Reasignar la variable
	uint16_t deudas[]={10000;50000;2500;4000};
	abonarDeudas(deudas,4);

	stringCaseConverter("HoLa");

	// 1.5 EJERCICIO:

	// Crear una función llamada stringCaseConverter que no retorne ningún
	// valor, y reciba una string.

	// Esta función deberá cambiar las minúsculas por mayúsculas y viceversa
	// del string original.

}
void duplicarNumero(uint8_t numero){
	numero = numero * 2;
}
void duplicarNumeroRef(uint8_t *numero){
	*numero = (*numero) * 2;
}

void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas){
	for(uint8_t i=0; i<cantidadDeudas;i++){
		// misDeudas [i] = misDeudas[i]/2
		*misDeudas =*misDeudas/2;
		*(misDeudas+i) = *(misDeudas+i)/2; // Para acceder a la siguiente posición
	}
	void stringCaseConverter(uint8_t *string){


	}




}
void stringCaseConventer(uint8_t *string){
		for(uint8_t i=0;i<4;i++){
			if(65<*(string+i)<90){
				*(string+i) = *(string+i)+32;
			}
			else{
				*(string+i) = *(string+i)-32;
			}
		}

}
void stringCaseConverter(uint8_t *string)
{
	while(*(string+i)!= '\0'){

	}
}


