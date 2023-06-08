/*
 * PruebaADC.c
 *
 *  Created on: 31/05/2023
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


/* Defines for the USART and BLINKY pheripheral */

// Handler para todos los periféricos que se van a implementar en el desarrollo del proyecto
GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
PLL_Handler_t handlerPLL = {0};
BasicTimer_Handler_t handlerTimer2 			= {0};	// Timer2

USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2

uint8_t 	rxData					= 0;		// Variable donde se guardarán los datos obtenidos por el RX

void initSystem(void);

//ADC_Config_t channel_1				= {0};
//ADC_Config_t channel_2				= {0};
ADC_Config_t adcConfig = {0};

char bufferData[128] = {0};

#define NUMBER_CHANNELS	2
uint8_t adcIsComplete		= 0;
uint8_t adcCounter			= 0;
uint16_t dataADC[NUMBER_CHANNELS] = {0};

int main(void)
{
	initSystem();
	writeMsg(&handlerCommTerminal, "\n~Iniciando Sistema~\n");

    while(1){

    	if(rxData != '\0'){

			if(rxData == 'a'){
				// Iniciamos una conversión ADC
				startSingleADC();

				}

			// Limpiamos el dato de RX
			rxData = '\0';

    	}

		if(adcIsComplete == 1){
			// Enviamos los datos por consola
			sprintf(bufferData,"%u\t%u\n",dataADC[0],dataADC[1]);
			writeMsg(&handlerCommTerminal, bufferData);
			// Bajamos la bandera del ADC
			adcIsComplete = 0;
		}
    }
    return 0;
}

void initSystem(void){

	 handlerPLL.frecSpeed = FRECUENCY_100MHz;
	 PLL_Config(&handlerPLL);
//	FPU_init();

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Cargamos la configuración

	GPIO_Config(&handlerStateLED);

	/* Configurando los pines sobre los que funciona el USART */
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

	handlerCommTerminal.ptrUSARTx							= USART1;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_enableIntRX		= ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX		= DISABLE;

	USART_Config(&handlerCommTerminal);

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed				= BTIMER_100MHz_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period 				= 50;

	BasicTimer_Config(&handlerTimer2);
//	startCounterTimer(&handlerTimer2);

	// Configuración del ADC
//	channel_1.channel 			= ADC_CHANNEL_0;
//	channel_1.dataAlignment 	= ADC_ALIGNMENT_RIGHT;
//	channel_1.samplingPeriod 	= ADC_SAMPLING_PERIOD_84_CYCLES;
//	channel_1.resolution		= ADC_RESOLUTION_12_BIT;
//
//	adc_Config(&channel_1);
//
//	channel_2.channel 			= ADC_CHANNEL_1;
//	channel_2.dataAlignment 	= ADC_ALIGNMENT_RIGHT;
//	channel_2.samplingPeriod 	= ADC_SAMPLING_PERIOD_84_CYCLES;
//	channel_2.resolution		= ADC_RESOLUTION_12_BIT;
//
//	// Cargamos la configuración
//	ADC_ConfigBiChannel(&channel_1,&channel_2);
	adcConfig.multiChannels[0]   = ADC_CHANNEL_0;
	adcConfig.multiChannels[1]   = ADC_CHANNEL_1;
	adcConfig.dataAlignment 	   = ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution 		   = ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod	   = ADC_SAMPLING_PERIOD_84_CYCLES;


	//Se carga la configuración del ADC
	ADC_ConfigMultichannel(&adcConfig,2);

}

// Timer encargado del StateLED
void BasicTimer2_Callback(void){
//	startSingleADC();
	GPIOxTooglePin(&handlerStateLED);
}

void usart1Rx_Callback(void){
	// Lectura de los datos recibidos
	rxData = getRXData();
}

void adcComplete_Callback(void){
	dataADC[adcCounter] = getADC();
	if(adcCounter < (NUMBER_CHANNELS-1)){
		adcCounter++;
	}
	else{
		adcIsComplete = 1;
		adcCounter = 0;
	}
}

