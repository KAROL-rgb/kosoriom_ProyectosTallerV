/*
 * GPIOxDriver.h
 *
 *  Created on: 9/03/2023
 *      Author: karol
 *
 * Este programa se encarga de las definiciones relacionadas únicamente con el
 * debido manejo y control del periférico GPIOx (funciones del controlador)
 */

#ifndef INC_GPIOXDRIVER_H_
#define INC_GPIOXDRIVER_H_

// Incluyendo este archivo estamos incluyendo también el correspondiente al GPIOx
#include "stm32f411xx_hal.h"

// La siguiente es la configuración del pin:

typedef struct
{
	uint8_t GPIO_PinNumber;              // PIN con el que deseamos trabajar
	uint8_t GPIO_PinMode;                // Modo de la configuración: entrada, salida, análogo, f, alternativa
	uint8_t GPIO_PinSpeed;               // Velocidad de respuesta del PIN (solo para digital)
	uint8_t GPIO_PinPuPdControl;         // Seleccionamos si deseamos una salida Pull-up, Pull-down o "libre"
	uint8_t GPIO_PinOPType;              // Trabaja de la mano con el anterior, selecciona salida PUPD o OpenDrain
	uint8_t GPIO_PinAltFunMode;          // Selecciona cual es la función alternativa que se está configurando

}GPIO_PinConfig_t;

/* Esta es una estructura que contiene dos elementos:
 * - La dirección del puerto que se utiliza se está utilizando (la referencia al puerto)
 * - La configuración específica del PIN que se está utilizando
 */
typedef struct
{
	GPIOx_RegDef_t       *pGPIOx;              /*!< Dirección del puerto al que el PIN corresponde > */  //Descripción del periférico
	GPIO_PinConfig_t     GPIO_PinConfig;       /*!< Configuración del PIN >*/

}GPIO_Handler_t;

/* Definición de las cabeceras de las funciones del GPIOxDriver */
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin (GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);



#endif /* INC_GPIOXDRIVER_H_ */
