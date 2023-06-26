/*
 * Tetris.c
 *
 *  Created on: 23/06/2023
 *      Author: karol
 */

#include <stm32f4xx.h>
#include "Tetris.h"

int movimientoT(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}

int movimientoTGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 1) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoTGiro2(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
			&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0 && punto3[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoTGiro3(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 1) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoPalito(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto
			if (punto2[0][0] >= 0) {
				matriz[punto2[0][0]][punto2[0][1]] = 0;
				matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
				punto2[0][0] = punto2[0][0] + 1;
			}
			if (punto3[0][0] >= 0) {
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] >= 0) {
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}
			if (punto2[0][0] < -1) {
				punto2[0][0] = punto2[0][0] + 1;
			}
			if (punto3[0][0] < -1) {
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) {
				punto4[0][0] = punto4[0][0] + 1;
			}
			if (punto2[0][0] == -1) { // Va a entra a a la matriz el punto
				matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
				punto2[0][0] = punto2[0][0] + 1;
			}
			if (punto3[0][0] == -1) {
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] == -1) {
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}
			return 1;
		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		}
	} else {
		return 0;
	}
}
int movimientoPalitoGiro(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
			&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)
			&& (matriz[punto4[0][0] + 1][punto4[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0 && punto3[0][0] >= 0
				&& punto4[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1; // Se prende la siguiente
			punto4[0][0] = punto4[0][0] + 1; // Le sumo 1 a la fila del punto

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoLInvertida(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto3[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoLInvertidaGiro1(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
			&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0 && punto3[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoLInvertidaGiro2(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			punto2[0][0] = punto2[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 1) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoLInvertidaGiro3(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoL(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto3[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoLGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoLGiro2(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			punto2[0][0] = punto2[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 1) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoLGiro3(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
			&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0 && punto3[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoCubo(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto3[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoS(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1 && punto2[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0 && punto2[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoSGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 1) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoZ(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1 && punto2[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0 && punto2[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoZGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) {// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
			punto2[0][0] = punto2[0][0] + 1;

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 1) { //Solo se verifica la base en el primer paso
		if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0
				&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
				// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
void end(int matriz[32][8]){
	for(int i=0;i<32;i++){
		for(int j = 0; j<8;j++){
			matriz[i][j]=0;
		}
	}
	//END
	//	E
	matriz[1][1]=1;
	matriz[1][2]=1;
	matriz[1][3]=1;
	matriz[1][4]=1;
	matriz[1][5]=1;
	matriz[1][6]=1;

	matriz[2][1]=1;
	matriz[3][1]=1;

	matriz[4][1]=1;
	matriz[4][2]=1;
	matriz[4][3]=1;
	matriz[4][4]=1;
	matriz[4][5]=1;
	matriz[4][6]=1;

	matriz[5][1]=1;
	matriz[6][1]=1;
	matriz[6][2]=1;
	matriz[6][3]=1;
	matriz[6][4]=1;
	matriz[6][5]=1;
	matriz[6][6]=1;

	// N
	matriz[9][1]=1;
	matriz[10][2]=1;
	matriz[11][3]=1;
	matriz[12][4]=1;
	matriz[13][5]=1;
	matriz[14][6]=1;

	matriz[10][2]=1;
	matriz[11][3]=1;
	matriz[12][4]=1;
	matriz[13][5]=1;
	matriz[14][6]=1;

	matriz[10][1]=1;
	matriz[11][1]=1;
	matriz[12][1]=1;
	matriz[13][1]=1;
	matriz[14][1]=1;

	matriz[9][6]=1;
	matriz[10][6]=1;
	matriz[11][6]=1;
	matriz[12][6]=1;
	matriz[13][6]=1;
	matriz[14][6]=1;

	// 	D
	matriz[17][1]=1;
	matriz[17][2]=1;
	matriz[17][4]=1;
	matriz[18][1]=1;
	matriz[19][1]=1;
	matriz[20][1]=1;
	matriz[21][1]=1;
	matriz[22][1]=1;
	matriz[17][3]=1;
	matriz[18][5]=1;

	matriz[22][2]=1;
	matriz[22][3]=1;
	matriz[22][4]=1;
	matriz[21][5]=1;
	matriz[20][6]=1;
	matriz[19][6]=1;

	// ?
	matriz[26][1]=1;
	matriz[25][2]=1;
	matriz[25][3]=1;
	matriz[25][4]=1;
	matriz[25][5]=1;
	matriz[26][6]=1;
	matriz[27][6]=1;
	matriz[28][5]=1;
	matriz[29][4]=1;
	matriz[31][4]=1;

}
