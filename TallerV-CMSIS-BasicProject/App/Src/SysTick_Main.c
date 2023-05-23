/*
 * SysTick_Main.c
 *
 *  Created on: 17/05/2023
 *      Author: karol
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"

#define HSI_CLOCK_CONFIGURED  0   // 16 MHz
#define HSE_CLOCK_CONFIGURED  1
#define PLL_CLOCK_CONFIGURED  2

// Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin = {0};

GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t  handlerUserButtonExti = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Elemento para hacer la comunicación serial */
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t sendMsg = 0;

// Definición de las cabeceras de las funciones del main
void initSystem(void);

int main(void){
	// Inicializamos todos los elementos del sistema
	initSystem();

	// Se configura a 16MHz
	config_SysTick_ms(0);

	// writeMsg(&usart2Comm, "Hola Mundo ! !");
	while(1){

		if(sendMsg > 4){

			writeMsg(&usart2Comm, "Hola Mundo !!");

			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(300);

			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			delay_ms(250);
			GPIOxTooglePin(&handlerBlinkyPin);
			sendMsg = 0;

		}
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
	handlerBlinkyTimer.TIMx_Config.TIMx_speed  = BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	handlerUserButton.pGPIOx              = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler   = &handlerUserButton;
	handlerUserButtonExti.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);
	// min 29:41


}

/* Callback del Timer2 - Hacemos un blinky... */
void BasicTimer2_Callback(void){

	sendMsg++;
}

/* Callback del userButton -> hacer algo */
void callback_extInt13(void){
	__NOP();
}
