/*
 * Taller4.c
 *
 *  Created on: 7/03/2023
 *      Author: karol
 */


#include <stdint.h>

/**
 * Función principal del programa
 * Esta función es el corazón del programa! !
 *
 */

int main(void)
{
	uint16_t x1 = 0b1101100001000111;
	uint16_t x2 = 0b0000011011001110;

/* Para manipular las variables para que contengan la información de rostros masculinos se utiliza un NOT como operador lógico */
	uint16_t notx1 = ~x1;
	uint16_t notx2 = ~x2;

// Definición de máscaras

	uint16_t mask1 = 0b0;
	uint16_t mask2 = 0b0;

// Posiciones femeninas que usan lentes primera serie:
	uint16_t maskLens1 = (mask1 << 9) + 0b1;
	uint16_t lens1 = x2 & maskLens1;

// POsiciones femeninas que usan lentes, segunda serie:
	uint16_t maskLens2 = (mask1 + 0b1) << 14;
	uint16_t maskLens21 = (mask2 + 0b1) << 12;
	uint16_t lens2 = maskLens2 + maskLens21;
	uint16_t finalLens = x1 & lens2;

// Posiciones de caras masculinas que utilizan bigote:
// Serie 1:
	uint16_t maskMoustacho1 = (mask1 + 0b1) << 10;
	uint16_t maskMoustacho12 = (mask2 + 0b1) << 3;
	uint16_t moustacho1 = maskMoustacho1 + maskMoustacho12;
	uint16_t finalMoustacho1 = x1 ^ moustacho1;
// Serie 2:
	uint16_t maskMoustacho2 = (mask1 +0b1) << 8;
	uint16_t maskMoustacho21 = (mask1 + 0b1) << 4;
	uint16_t maskMoustacho22 = (mask2 + 0b1) << 0;
	uint16_t moustacho2 = maskMoustacho2 + maskMoustacho21 + maskMoustacho22;
	uint16_t finalMoustacho2 = x2 ^ moustacho2;

//  Variable de 32 bits:
	uint16_t x_32bits = x1 << 16;
	uint16_t finalSumX = x_32bits + x2;








    /* Loop forever */
	while(1){

	}

	return 0;
}
