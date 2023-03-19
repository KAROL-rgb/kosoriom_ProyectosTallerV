/*
 * Taller6.c
 *
 *  Created on: 16/03/2023
 *      Author: karol
 */

#include <stdint.h>

#define SUMA'+'                                   // Valor constante, no puede cambiar, y solo va a tomar el valor SUMA, toma el valor numérico de 0 a 256 del carácter suma
#define RESTA '-'
#define MULTIPLICACION   '*'
#define DIVISION     '/'

// Función calcular operación
uint16_t Operaciones (uint8_t tipoOperacion, uint16_t numero1, uint16_t numero2);

#define FACTORIAL '!'
#define FIBONACCI 5

uint32_t OPeraciones2(uint8_t tipoOperacion, uint16_t numero);




int main(void){
	resultado = OPeraciones2(FACTORIAL,5);
	resultado = OPeraciones2(FIBONACCI)

	while(1){

	}
}


uint32_t OPeraciones2(uint8_t tipoOperacion, uint16_t numero){
	switch(tipoOperación){
	case FACTORIAL:{
		//5! = 1*2*3*4*5
		uint32_t factorial = 1;
		uint16_t contador = 1;
		while (contador<=numero){
			factorial = factorial*contador;
			contador++;
			return FACTORIAL;
		}
	}	case FIBONACCI:{
			//1,1,2,3,5,8,13,21...
			uint32_t n_menos_2 = 0;
			uint32_t n_menos_1 =1;
			uint32_t n = 0;
			uint32_t contador = 0;

			while(contador < numero){
				n = n_menos_1 + n_menos_2;
				n_menos_2 = n_menos_1;
				n_menos_1 = n;
				contador++;
			}
			return n;
	}

}

