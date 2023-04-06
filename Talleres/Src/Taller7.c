/*
 * Taller7.c
 *
 *  Created on: 23/03/2023
 *      Author: karol
 */

#include <stdint.h>
int main(void){
	uint8_t dato = 10;

	uint8_t *pDato = &dato;

	uint32_t addrDato = (uint32_t)pDato;

	*pDato = 20;   // Valor

	pDato ++;  // Apunta a una posicion de memoria diferente a la de pDato

	*pDato = 0x1234;
	;

uint32_t miPrimerArreglo = {2, 0xABD, 0b111110, 10 }; // Puntero

uint32_T variable1 = *miPrimerArreglo;  //Primer valor del arreglo 12

miPrimerArreglo ++;

for (uinnt8_t i = 0; i<5; i++){   // Va guardado la posición del arreglo- Accede a la primera posición
	uint32_t variableAux = miPrimerArreglo[i];
for (uinnt8_t i = 0; i<5; i++){   // Va guardado la posición del arreglo

	uint32_t *auxPunter = miPrimerArreglo + i; // Arreglos de estructuras
	variableAux = (*miPrimerArreglo + i);  //*(miPrimerArreglo[i])
}



}



	*pDato = 50;

	pDato ++;

	*pDato = 100;

	uint16_t dato_16 = (uint16_t) dato;

	(void) dato_16;
	(void) addrDato;



	typedef struct{

		uint8_t edad;
		uint16_t estatura;
		uint32_t arreglo[10];


	}Estudiante_t;   // Tipo de estructura

	Estudiante_t estudiante1 = {};
	Estudiante_t estudiante2 = {0};
	Estudiante_t estudiante3 = {0};
	Estudiante_t estudiante4 = {0};

	estudiante1.edad = 22;
	estudiante.estatura = 173;
	*(estudiante1.notas) = 45;
	*(estudiante1.notas + 1) = 38;

Estudiante_t *pEstudiante1 =  &estudiante1;          //estamos apuntando a la estructura que ya creamos. Puntero que se dirije a cualquier posición de memoria
pEstudiante1->edad = 21;
pEstudiante1->estatura = 173;

// String
// comillas simples para elementos solos
// Comillas dobles para strings
char miString[] = "Linux lo mejor";  // Siempre guardan el elemnto NULL
//Las funciones creadas en el DRIVER se les pasa las posiciones de memoria


	while(1){

	}
}



// Acceder a estructura




