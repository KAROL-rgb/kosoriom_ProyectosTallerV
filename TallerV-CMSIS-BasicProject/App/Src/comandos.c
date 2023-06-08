/*
 * comandos.c
 *
 *  Created on: 3/06/2023
 *      Author: karol
 */

/* Defines for the USART and BLINKY pheripheral */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"

#include "AdcDriver.h"
#include "PLLDriver.h"

BasicTimer_Handler_t handlerTimer2 	= {0};	// Timer2

USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2

uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX

void initSystem(void);
void parseCommands(char *ptrBufferReception);

GPIO_Handler_t 	handlerLedOK 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
PLL_Handler_t handlerPLL = {0};

ADC_Config_t channel_1				= {0};
ADC_Config_t channel_2				= {0};

char bufferData[128] = {0};

uint8_t counterReception = 0;
char bufferReception[64] ={0};
uint8_t stringComplete = 0;
uint8_t autoUpdateLCD = 0;
char userMsg[64] = "Funciona";
char cmd[16];
unsigned int firstParameter;
unsigned int secondParameter;

// Definición para prescalers
#define MCO1PRE_2    4
#define MCO1PRE_3    5
#define MCO1PRE_4    6
#define MCO1PRE_5    7

#define numberOfChannels	2
uint8_t adcIsComplete		= 0;
uint8_t adcCounter			= 0;
uint16_t dataADC[numberOfChannels] = {0};

int main(void){

	initSystem();

	while(1){
		// Creamos una cadena de caracteres con los datos que llegan por el serial
		// El caracter '@' nos indica que es el final de la cadena
		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			// if the incoming character is a newline, set a flag
			// so the main loop can do something about it:
			if (rxData == '@'){
				stringComplete = true;

				// Agrego esta línea para crear el string con el null al final
				bufferReception[counterReception] = '\0';

				counterReception = 0;
			}

			// Para que no vuelva a entrar. Solo cambia debido a la interrupción
			rxData = '\0';
		}

		// Hacemos un análisis de la cadena de datos obtenida
		if(stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}
//		if(makeUpdateLCD){
//			updateLCD();
//			makeUpdateLCD = false;
//		}

	}
}
void initSystem(void){
		handlerPLL.frecSpeed = FRECUENCY_100MHz;
		PLL_Config(&handlerPLL);
		//Configurando el pin para el Led Blinky
		handlerLedOK.pGPIOx                              = GPIOH;
		handlerLedOK.GPIO_PinConfig.GPIO_PinNumber       = PIN_1;
		handlerLedOK.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		handlerLedOK.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
		handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
		handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerLedOK);

		//Llevamos el LED a un estado de encendido (ON o SET)
		GPIO_WritePin(&handlerLedOK, SET);
		handlerPinTX.pGPIOx									= GPIOA;
		handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
		handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerPinTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
		handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

		GPIO_Config(&handlerPinTX);

		handlerPinRX.pGPIOx									= GPIOA;
		handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
		handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerPinRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
		handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
		handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

		GPIO_Config(&handlerPinRX);

		// Configuración comunicación serial
		handlerCommTerminal.ptrUSARTx                                 = USART1;
		handlerCommTerminal.USART_Config.USART_baudrate               = USART_BAUDRATE_115200;
		handlerCommTerminal.USART_Config.USART_datasize               = USART_DATASIZE_8BIT;
		handlerCommTerminal.USART_Config.USART_parity                 = USART_PARITY_NONE;
		handlerCommTerminal.USART_Config.USART_stopbits               = USART_STOPBIT_1;
		handlerCommTerminal.USART_Config.USART_mode                   = USART_MODE_RXTX;
		handlerCommTerminal.USART_Config.USART_enableIntRX            = ENABLE;
		handlerCommTerminal.USART_Config.USART_enableIntTX            = DISABLE;
		USART_Config(&handlerCommTerminal);

		handlerTimer2.ptrTIMx								= TIM2;
		handlerTimer2.TIMx_Config.TIMx_mode				    = BTIMER_MODE_UP;
		handlerTimer2.TIMx_Config.TIMx_speed				= BTIMER_100MHz_SPEED_100us;
		handlerTimer2.TIMx_Config.TIMx_period 				= 50;

		BasicTimer_Config(&handlerTimer2);

}
void parseCommands(char *ptrBufferReception){

	// Esta función de C lee la cadena de caracteres a la que apunta el "ptr y la divide
	// y almacena en tres elementos diferentes: un string llamado "cmd", y dos numeros
	// integer llamados "firstParameter" y "secondParameter".
	// De esta forma, podemos introducir información al micro desde el puerto serial.
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	// Este primer comando imprime una lista con los otros comandos que tiene el equipo
	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerCommTerminal, "Help Menu CMDs:\n");
		writeMsg(&handlerCommTerminal, "1) help     -- Print this menu\n");
		writeMsg(&handlerCommTerminal, "2) dummy #A#B  -- dummy cmd, #A and #B are uint32_t\n");
		writeMsg(&handlerCommTerminal, "3) usermsg # # msg -- msg is a string comming from outside\n");
		writeMsg(&handlerCommTerminal, "4) initLcd -- steps to initialize the LCD\n");
		writeMsg(&handlerCommTerminal, "5) testLcd -- simple Test for the LCD\n");
		writeMsg(&handlerCommTerminal, "6) setPeriod # -- Change the Led_state period (us)\n");
		writeMsg(&handlerCommTerminal, "7) autoUpdate # -- Automatic LCD update (# -> 1/0)\n");
	}
	/* Comandos para elegir la señal del MCO1 */
	else if(strcmp(cmd, "selectClock") == 0){
		if(firstParameter == 0){
			signalClock(&handlerPLL, firstParameter);
			writeMsg(&handlerCommTerminal, "HSI \n");
		}else if(firstParameter == 1){
			signalClock(&handlerPLL, firstParameter);
			writeMsg(&handlerCommTerminal, "LSE \n");
		}else if(firstParameter == 3){
			signalClock(&handlerPLL, firstParameter);
			writeMsg(&handlerCommTerminal, "PLL \n");
		}else{
			writeMsg(&handlerCommTerminal, "Wrong \n");
		}
	}
	/* Comandos para seleccionar el prescaler de la señal en el MCO1 */
	else if(strcmp(cmd, "selectPrescaler") == 0)
	{
		if(firstParameter ==  2){
			signalPrescaler(&handlerPLL, MCO1PRE_2);
			writeMsg(&handlerCommTerminal, "Division by 2 \n");
		}else if(firstParameter == 3){
			signalPrescaler(&handlerPLL, MCO1PRE_3);
			writeMsg(&handlerCommTerminal, "Division by 3 \n");
		}else if(firstParameter == 4){
			signalPrescaler(&handlerPLL, MCO1PRE_4);
			writeMsg(&handlerCommTerminal, "Division by 4 \n");
		}else if(firstParameter == 5){
			signalPrescaler(&handlerPLL, MCO1PRE_5);
			writeMsg(&handlerCommTerminal, "Division by 5 \n");
		}
	}

	/* Comandos para el RTC */
	else if(strcmp(cmd, "selectDays") == 0)
	{
		if(RTC_Day <= 31){
			writeMsg(&handlerCommTerminal, "Division by 3 \n");
		}

	}

	// El comando dummy sirve para entender como funciona la recepción de números enviados
	// desde la consola
	else if (strcmp(cmd, "dummy")== 0){
		writeMsg(&handlerCommTerminal, "CMD: dummy\n");
		// Cambiando el formato para presenta el número por el puerto serial
		sprintf(bufferData, "number A = %u\n", firstParameter);
		writeMsg(&handlerCommTerminal, bufferData);

		// Cambiando el formato para presentar el número por el puerto serial
		sprintf(bufferData, "number B = %u \n", secondParameter);
		writeMsg(&handlerCommTerminal, bufferData);

	}

	// El comando usermsg sirve para entender como funciona la recepción de strings enviados
	// desde la consola
	else if (strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerCommTerminal, "CMD: usermsg\n");
		writeMsg(&handlerCommTerminal, userMsg);
		writeMsg(&handlerCommTerminal, "\n");
	}
	else if(strcmp(cmd, "autoUpdate") == 0){
		writeMsg(&handlerCommTerminal, "CMD: autoUpdate\n");
		if(firstParameter != 0){
			autoUpdateLCD = true;
		}else{
			autoUpdateLCD = false;
		}

	}
	else if(strcmp(cmd, "setPeriod") == 0){
		writeMsg(&handlerCommTerminal, "CMD: setPeriod\n");
		if(firstParameter < 100){
			firstParameter = 100;
		}
		if(firstParameter > 10000){
			firstParameter = 10000;
		}
		handlerTimer2.TIMx_Config.TIMx_period = firstParameter;
		BasicTimer_Config(&handlerTimer2);
	}
	else{
		// Se imprime el mensaje "wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerCommTerminal, "Wrong CMD");
	}
}


//void updateLCD(void){
//	if(updateValues){
//		lcd_moveCursor(1, 2);
//		lcd_write_msg("Testing LCD");
//		lcd_moveCursor(2, 0);
//		lcd_write_msg("ADC ch10 = ");
//
//		lcd_moveCUrsor(3, 0);
//		lcd_write_msg("ADC ch11 = ");
//
//		lcd_moveCursor(4, 0);
//		lcd_write_msg("ADC ch12 = ");
//	}
//}
void usart1Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esro adempas debe bajar la bandera de la interrupción
	rxData = getRXData();
}
void BasicTimer2_Callback(void){
	// Hacemos un blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);
}
