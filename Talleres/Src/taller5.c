/*
 * taller5.c
 *
 *  Created on: 9/03/2023
 *      Author: karol
 */

// Librería para los tipos de variables
#include <stdint.h>

// Librería para variables booleanas
#include <stdbool.h>

// Librería para usar funciones matemáticas
#include <math.h>

#define CONSTANTE 100
#define UNSIGNED  0
#define SIGNED    1

bool variableBooleana = true;
uint8_t parametro1 = 100;
uint16_t parametro2= -173;


// Header fundamental funciones

void clearGlobal(void);
uint8_t getMaxChar(void);   // tamaño de Char es 255
uint16_t getMaxValue(uint16_t x, uint16_t y, uint16_t z);

int main(void){

	clearGlobal();
	parametro1 = getMaxChar;
	uint16_t parametro3 = getMaxValue(100, 200, 50);
}
// Función que no recibe ni devuelve parámetros
void clearGlobal(void){
	variableBooleana = false;
	 parametro1 = 0;
	 parametro2 = 0;

}
uint8_t getMaxChar(void){
	uint8_t maxChar = pow (2,8) -1;        //tamaño máximo de Char

	return maxChar;
}
uint16_t getMaxValue(uint16_t x, uint16_t y, uint16_t z){
	if ((x>y) && (x>z)){
		return x;
	}else if ((y>x) && (y>z)){
		return y;
	}else{
		return z;
	}
}

