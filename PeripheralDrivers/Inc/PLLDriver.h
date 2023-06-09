/*
 * PLLDriver.h
 *
 *  Created on: 23/05/2023
 *      Author: karol
 */
#include "stm32f4xx.h"

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#define FRECUENCY_16MHz   0
//#define FRECUENCY_80MHz 1
#define FRECUENCY_100MHz  1

#define HSI     0
#define LSE     1
#define HSE     2
#define PLL     3

// Definición para prescalers
#define MCO1PRE_2    2
#define MCO1PRE_3    3
#define MCO1PRE_4    4
#define MCO1PRE_5    5

typedef struct{
	uint8_t    frecSpeed;
	uint8_t    MCO1PRE;

}PLL_Handler_t;

typedef struct{
	uint8_t clock;
	uint8_t preescaler;
}MCO1_Handler_t;

// Definicion de cabeceras para funciones del PLL
void PLL_Config(PLL_Handler_t *ptrPLL_Handler);
uint32_t getConfigPLL(void);
void signalPrescaler(PLL_Handler_t *ptrfrecSpeed, uint8_t preScaler );
void signalClock(PLL_Handler_t *ptrfrecSpeed, uint8_t clock);
void configMCO1(MCO1_Handler_t *ptrHandlerMCO1);
#endif /* PLLDRIVER_H_ */
