/* @file           : BasicProject_Main.c
 * @author         : Karol Osorio Monsalve - kosoriom@unal.edu.co
 * @brief          : Solución básica de un proyecto con librerías externas */


#include <stdint.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"

GPIO_Handler_t handlerBlinkyPin = {0};

GPIO_Handler_t handlerPinMCO1 = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

// Definición de las cabeceras de las funciones del main
void initSystem(void);

int main(void){

	initSystem();
	PLL_Config();
	getConfigPLL();

	while(1){

	}

	return 0;
}

void initSystem(void){
	// Configurando el pin para el Led_Blinky
	handlerBlinkyPin.pGPIOx           = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber  = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	// Configurando el Timer2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx           = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed  = BTIMER_80MHz_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/** Configurando para probar el MCO1 en el analizador de señales ***/
	handlerPinMCO1.pGPIOx                             = GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber      = PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;
	GPIO_Config(&handlerPinMCO1);
}

/* Callback del Timer2 - Hacemos un blinky... */
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerBlinkyPin);
}


