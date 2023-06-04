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

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"

#include "AdcDriver.h"

BasicTimer_Handler_t handlerStateOKTimer 			= {0};	// Timer2

USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2

uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX

void initSystem(void);

ADC_Config_t channel_1				= {0};
ADC_Config_t channel_2				= {0};

char bufferData[128] = {0};
uint8_t counterReception = 0;
char bufferReception[128] ={0};
uint8_t stringComplete = 0;
uint8_t autoUpdateLCD = 0;
uint16_t userMsg = 0;
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
		if(makeUpdateLCD){
			updateLCD();
			makeUpdateLCD = false;
		}

	}
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
	else if (strcmp(cmd, "initLcd") == 0){
		writeMsg(&handlerCommTerminal, "CMD: initLcd\n");
		lcd_init();

	}
	else if(strcmp(cmd, "clearLCD") == 0){
		writeMsg(&handlerCommTerminal, "CMD: clearLcd\n");
		lcd_clear();
	}
	else if(strcmp(cmd, "testLcd") == 0){
		writeMsg(&handlerCommTerminal, "CMD: testLcd\n");

		updateLCD();
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
		handlerStateOKTimer.TIMx_Config.TIMx_period = firstParameter;
		BasicTimer_Config(&handlerStateOKTimer);
	}
	else{
		// Se imprime el mensaje "wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerCommTerminal, "Wrong CMD");
	}
}

void updateLCD(void){
	if(updateValues){
		lcd_moveCursor(1, 2);
		lcd_write_msg("Testing LCD");
		lcd_moveCursor(2, 0);
		lcd_write_msg("ADC ch10 = ");

		lcd_moveCUrsor(3, 0);
		lcd_write_msg("ADC ch11 = ");

		lcd_moveCursor(4, 0);
		lcd_write_msg("ADC ch12 = ");
	}
}
void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esro adempas debe bajar la bandera de la interrupción
	rxData = getRXData();
}
void BasicTimer2_Callback(void){
	// Hacemos un blinky, para indicar que el equipo está funcionando correctamente
}
