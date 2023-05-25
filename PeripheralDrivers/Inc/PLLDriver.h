/*
 * PLLDriver.h
 *
 *  Created on: 23/05/2023
 *      Author: karol
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#define FRECUENCY_16MHz   0
#define FRECUENCY_80MHz   1

// Definicion de cabeceras para funciones del PLL
void PLL_Config(void);
uint32_t getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
