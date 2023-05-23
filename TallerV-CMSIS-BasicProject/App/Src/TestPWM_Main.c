/*
 * TestPWM_Main.c
 *
 *  Created on: 19/05/2023
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
#include "PwmDriver.h"

// Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin       = {0};

GPIO_Handler_t handlerUserButton   = {0};
EXTI_Config_t handlerUserButtonExti   = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Elemento para hacer la comunicación serial */
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;

/* Elementos para el PWM */
GPIO_Handler_t handlerPinPwmChannel = {0};
PWM_Handler_t handlerSignalPWM = {0};

uint16_t duttyValue = 1500;

char bufferMsg[64] = {0};

// Definición de las cabeceras de las funciones del main
void initSystem(void);

int main(void){

	/* Activamos el coprocesador Matemático - FPU */
	SCB->CPACR |= (0xF << 20);

	// Inicializamos todos los elementos del sistema
	initSystem();

	while(1){
		// Verificando el PWM
		if(usart2DataReceived != '\0'){

			if(usart2DataReceived == 'D'){

				// Down
				duttyValue -= 10;
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}

			/* Para probar el Seno...*/
			if(usart2DataReceived == 'U'){

				// Up...
				duttyValue += 10;
				// Lanzamos un nuevo ciclo de adquisiciones
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}

			/* Imprimimos el mensaje */
			sprintf(bufferMsg, "dutty = %u \n", (unsigned int)duttyValue);
			writeMsg(&usart2Comm, bufferMsg);

			/* Cambiamos el estado del elemento que controla la entrada */
			usart2DataReceived = '\0';
		}
	}

	return 0;
}

/*
 * Función encargada de la inicialización de los elementos del sistema
 */
void initSystem(void){

	// Configurando el pin para el Led_Blinky
	handlerBlinkyPin.pGPIOx          = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	// Configurando el Timer2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx   = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	// El pin UserButton es una entrada simple que entregará la interrpción EXTI-13
	// Observar que el pin seleccionado es el PIN_13, por lo tanto el callback
	// que se debe configurar es el callback_extInt13()
	handlerUserButton.pGPIOx        = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler     = &handlerUserButton; // Esto es un puntero
	handlerUserButtonExti.edgeType         = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);

	/* Configuración de la comunicación serial */
	handlerPinTX.pGPIOx               = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx                  = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx            = USART2;
	usart2Comm.USART_Config.USART_baudrate     = USART_BAUDRATE_19200;
	usart2Comm.USART_Config.USART_datasize     = USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity       = USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits     = USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode         = USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX  = USART_RX_INTERRUP_ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX  = USART_TX_INTERRUP_DISABLE;
	USART_Config(&usart2Comm);

	/*  COnfiguramos el PWM */
	handlerPinPwmChannel.pGPIOx               = GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed    =GPIO_OSPEED_FAST;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;

	/* Cargamos la configuración em los registros del MCU */
	GPIO_Config(&handlerPinPwmChannel);

	/* Configurando el Timer para que genere la señal PWM */
	handlerSignalPWM.ptrTIMx          = TIM3;
	handlerSignalPWM.config.channel   = PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle = duttyValue;
	handlerSignalPWM.config.periodo   = 20000;
	handlerSignalPWM.config.prescaler = 16;

	/* Cargamos la configuración en los registros del MCU */
	pwm_Config(&handlerSignalPWM);

	/* Activamos la señal */
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);
}

/*
 * Callback del Timer2 - Hacemos un blinky...
 */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}
/*
 * Callback del userButton-> hacer algo...
 */
void callback_extInt13(void){
	__NOP();

}

/* Esta función se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2
 */
void usart2Rx_Callback(void){

	usart2DataReceived = getRXData();
}


