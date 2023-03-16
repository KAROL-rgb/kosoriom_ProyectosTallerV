/**
 ******************************************************************************
 * @file           : main.c
 * @author         : kosoriom
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>

#include "stm32f411xx_hal.h"


/**
 * Función principal del programa
 * Esta función es el corazón del programa! !
 *
 */

int main(void)
{
	//COnfiguración inicial del MCU
	//RCC -> AHB1ENR = 0x0001;
	RCC ->AHB1ENR &= ~(1 << 0);  // Borrar la posición 0 del registro AHB1ENR
	RCC -> AHB1ENR |= (1 << 0);    // Activando la señal de reloj del GPIOA

	// Configurar MODER
	GPIOA ->MODER &= ~(0b11 << 10); // EStamos limpiando la posición correspondiente al pin5
	GPIOA ->MODER |= (0b01 << 10); // Configurando el pin5 como salida general

	// Configurar el OTYPE
	GPIOA ->OTYPER &= ~(0b1 << 5); // pinC configurado como salida push-pull

	// Configurar el OSPEEDR
	GPIOA ->OSPEEDR &= ~(0b11 << 10); // Limpiando las posiciones correspndientes al pinC
	GPIOA ->OSPEEDR |= (0b01 << 10);  // Configurando la velocidad como fast

	GPIOA ->ODR &= ~(0b1 << 5); // Limpiando posición 5 -LED apagado
	GPIOA ->ODR |= (0b1 << 5); // Escribiendo 1 en posición 5 -LED encendido


    /* Loop forever */
	while(1){

	}

	return 0;
}
