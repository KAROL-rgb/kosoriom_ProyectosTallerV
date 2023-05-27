/*
 * TestingFpu_Main.c
 *
 *  Created on: 15/05/2023
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

// Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin = {0};

GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t handlerUserButtonExti = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

PLL_Handler_t handlerPLL = {0};
/* Elemento para hacer la comunicación serial */

GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart1Comm = {0};
uint8_t sendMsg = 0;
uint8_t usart1DataReceived = 0;

char bufferMsg[64] = {0};

float valueA = 123.4567f;
float valueB = 987.7654f;
float valueC = 0.0f;


// Definición de las cabeceras de las funciones del main
void initSystem(void);

/**
 * Función principal del programa
 * Esta función es el corazón del programa !!
 *
 * */
 int main(void){

	 /* Activamos el coprocesador matemático - FPU*/
	 SCB -> CPACR |= (0xF << 20);

	 // Inicializamos todos los elementos del sistema
	 initSystem();

	 /* Loop forever */
	 while(1){
		 /*  Realiza operación de punto flotante cuando se recibe
		  * algun caracter por el puerto serial
		  * */
		 if(usart1DataReceived != '\0'){

			 valueC = valueA * valueB;
			 sprintf(bufferMsg, "ValueC =%#.3f \n", valueC);
			 writeMsg(&usart1Comm, bufferMsg);
			 usart1DataReceived = '\0';

		 }

	 }

	 return 0;
 }

 /*
  * Función encargada de la inicialización de los elementos del sistema
  */
 void initSystem(void){

	 handlerPLL.frecSpeed = FRECUENCY_80MHz;
	 PLL_Config(&handlerPLL);
	 // Configuramos el pin para el LED_Blinky
	 handlerBlinkyPin.pGPIOx              = GPIOA;
	 handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	 handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	 handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	 handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	 handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	 GPIO_Config(&handlerBlinkyPin);

	 //Configurando el Timer2 para que funcione con el blinky
	 handlerBlinkyTimer.ptrTIMx                   = TIM2;
	 handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	 handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_80MHz_SPEED_100us;
	 handlerBlinkyTimer.TIMx_Config.TIMx_period =2500;
	 handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	 BasicTimer_Config(&handlerBlinkyTimer);

	 // El pin UserButton es una entrada simple que entregará la interrupción EXTI-13
	 // Observar que el pin seleccionado es el PIN_13, por lo tanto el callback
	 // que se debe configurar es el callback_estInt13()
	 handlerUserButton.pGPIOx                = GPIOC;
	 handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	 handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	 handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	 GPIO_Config(&handlerUserButton);

	 handlerUserButtonExti.pGPIOHandler = &handlerUserButton; // Esto es un puntero a la
	 handlerUserButtonExti.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	 extInt_Config(&handlerUserButtonExti);

	 /* Configuración de la comunicación serial */
	 handlerPinTX.pGPIOx                    =GPIOA;
	 handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_15;
	 handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	 handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	 GPIO_Config(&handlerPinTX);

	 handlerPinRX.pGPIOx       = GPIOB;
	 handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	 handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	 handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	 GPIO_Config(&handlerPinRX);

	 usart1Comm.ptrUSARTx        = USART1;
	 usart1Comm.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	 usart1Comm.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	 usart1Comm.USART_Config.USART_parity  = USART_PARITY_NONE;
	 usart1Comm.USART_Config.USART_stopbits = USART_STOPBIT_1;
	 usart1Comm.USART_Config.USART_mode = USART_MODE_RXTX;
	 usart1Comm.USART_Config.USART_enableIntRX = USART_RX_INTERRUP_ENABLE;
	 usart1Comm.USART_Config.USART_enableIntTX = USART_TX_INTERRUP_DISABLE;

	 USART_Config(&usart1Comm);



 }

 /*
  * Callback del Timer2 - Hacemos un blinky...
  */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

/*
 * Callback del userButton -> hacer algo...
 */
void callback_extInt13(void){
	__NOP();
}

/* Esta función se ejecuta cada vez que un carácter es recibido
 * por el puerto USART2
 */
void usart1Rx_Callback(void){
	usart1DataReceived = getRXData();
}




