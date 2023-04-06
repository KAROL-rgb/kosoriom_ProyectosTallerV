/**
 ******************************************************************************
 * @file           : main.c
 * @author         : kosoriom
 * @brief          : Main program body
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
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

// Definición de un elemento
GPIO_Handler_t handlerLed2 = {0};    // PA5

BasicTimer_Handler_t handlerTimer2 = {0};



/**
 * Función principal del programa
 */

int main(void){

	// Configurando el LED_2 -> PA5
	handlerLed2.pGPIOx = GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Seleccionar el Timer que se quiere usar
	handlerTimer2.ptrTIMx = TIM2;

	// Configuración con la cual se maneja el timer
	handlerTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;

	handlerTimer2.TIMx_Config.TIMx_period = 5000;

	handlerTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;

	// Cargamos la configuración del pinA5
	GPIO_Config(&handlerLed2);
	GPIO_WritePin(&handlerLed2, SET);

	// Cargamos la configuración del timer
	BasicTimer_Config(&handlerTimer2);

    /* Loop forever */
	while(1){

	}
}
void BasicTimerX_Callback(void){
	GPIOxTooglePin(&handlerLed2);

}
