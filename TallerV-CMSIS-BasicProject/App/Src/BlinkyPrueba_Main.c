/*
 * BlinkyPrueba_Main.c
 *
 *  Created on: 5/06/2023
 *      Author: karol
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PLLDriver.h"
#include "AdcDriver.h"

/*NOTA IMPORTANTE: En el driver del timer, se añadió la configuración el TIMER 9, 10 y 11
 *Ya que este se encuentra en el bus APB2 que va a una frecuencia máxima de 100MHz
 * */
//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin             = {0};
GPIO_Handler_t handlerPinMCO1               = {0};
BasicTimer_Handler_t handlerBlinkyTimer     = {0};
PLL_Handler_t handlerPLL = {0};

//Definición de las cabeceras de las funciones del main
void initSystem(void);


/**
 * Función principal del programa.
 */

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

	while (1){

	}
	return 0;
}


void initSystem(void){
	handlerPLL.frecSpeed = FRECUENCY_100MHz;
	PLL_Config(&handlerPLL);
	//Configurando el pin para el Led de estado Blinky
	handlerBlinkyPin.pGPIOx                              = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	//Configurando el Timer2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx                           = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode             = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed            = BTIMER_100MHz_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period           = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable  = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/****Configuración para probar el MCO1 en el analizador de señales****/
	handlerPinMCO1.pGPIOx                                = GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber         = PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode     = AF0;
	GPIO_Config(&handlerPinMCO1);
}




/*
 * Callback del Timer2 -Hacemos un blinky
 */

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);

}
