/*
 * Tetris.h
 *
 *  Created on: 23/06/2023
 *      Author: karol
 */

#ifndef TETRIS_H_
#define TETRIS_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "SPIxDriver.h"
/* Definiciones de las funciones de las figuras a utilzar */

uint8_t MatrixLed_writeData(uint8_t digit, uint8_t seg);

void initSystem(void);

void clean(void);

void traducir(int matriz[32][8]);

void mover(int punto[1][2], int matriz[32][8]);

int movimientoT(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoTGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoTGiro2(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoTGiro3(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoPalito(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoPalitoGiro(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoLInvertida(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoLInvertidaGiro1(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]);

int movimientoLInvertidaGiro2(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]);

int movimientoLInvertidaGiro3(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]);

int movimientoL(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoLGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoLGiro2(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoLGiro3(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoCubo(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoS(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoSGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoZ(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoZGiro1(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

#endif /* TETRIS_H_ */
