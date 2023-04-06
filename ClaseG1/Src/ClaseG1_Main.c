/*
 * ClaseG1_Main.c
 *
 *  Created on: 23/03/2023
 *      Author: karol
 */


#include <stm32f4xx.h>
#include<stdint.h>

uint32_t counter = 0;
uint32_t *ptr_Counter;
uint32_t auxCounter = 0;

uint8_t byteVariable;
uint8_t *ptr_ByteVariable;

int main(void){
	/*Trabajando con la variablr de 32 bits*/
	counter = 12345; //Asignando un valor a esta variable

	auxCounter = counter; // C es pasado por valor, auxcounter recibe el valor de counter

	/* Hacer que el puntero apunte a una variable */
	ptr_Counter = &counter;

	// Modificar el valor almacenado en la memoria a donde apunta*/
	*ptr_Counter = 43658709;

	ptr_Counter++;  // Tomamos el puntero como una variable
	*ptr_Counter = 43658709;

	byteVariable = 32;
	ptr_ByteVariable = &byteVariable;
	*ptr_ByteVariable = 123;

	/* Intentandp mezclar punteros. Punteo es variable de 32 bits */
	auxCounter = (uint32_t)&counter; // Haciendo un cast para obtener el valor. Es pasar por un molde que es uint32 entonces ya mo es un puntero
	ptr_ByteVariable = (uint8_ t *)auxCounter;  // Vuelve a ser un puntero
	*ptr_ByteVariable = 1;

	while(1){

	}
	return 0;
}
