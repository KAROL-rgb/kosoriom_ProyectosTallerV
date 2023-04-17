/*
 * Usart.c
 *
 *  Created on: 15/04/2023
 *      Author: karol
 */

#include "Usart.h"

/* Variable que guarda la referencia del periférico que se esta utilizando*/
USART_TypeDef	*ptrUSART1Used = USART1;
USART_TypeDef	*ptrTUSART2Used = USART2;
USART_TypeDef	*ptrUSART6Used = USART6;

void USART_Config(USART_Handler_t *ptrUsartHandler){
	// Guardamos una referencia al periferico que estamos utilizando...
	ptrUSART1Used = ptrUsartHandler->ptrUSARTx;
	ptrUSART2Used = ptrUsartHandler->ptrUSARTx;
	ptrUSART6Used = ptrUsartHandler->ptrUSARTx;

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();
	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		// Registro del RCC que nos activa la señal de reloj para el TIM2
		RCC ->APB1ENR &=~ RCC_APB1ENR_USART1EN;
		RCC ->APB1ENR |= RCC_APB1ENR_USART1EN;
	}
	else if(ptrUsartHandler->ptrUSARTx == USART2){
		// Registro del RCC que nos activa la señal de reloj para el TIM3
		RCC ->APB1ENR &=~ RCC_APB1ENR_USART2EN;
		RCC ->APB1ENR |= RCC_APB1ENR_USART2EN;
	}
	else if(ptrUsartHandler->ptrUSARTx == USART6){
		// Registro del RCC que nos activa la señal de reloj para el TIM3
		RCC ->APB2ENR &=~ RCC_APB1ENR_USART6EN;
		RCC ->APB2ENR |= RCC_APB1ENR_USART6EN;
	}
	else{
		__NOP();
	}
	USART_CR1_M_Msk               (0x1UL << USART_CR1_M_Pos)                /*!< 0x00001000 */
	USART_CR1_M                   USART_CR1_M_Msk

}


