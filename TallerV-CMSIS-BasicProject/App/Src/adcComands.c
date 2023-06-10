/*
 * adcComands.c
 *
 *  Created on: 10/06/2023
 *      Author: karol
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PwmDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "AdcDriver.h"

/**Handler para todos los periféricos que se van a implementar en el desarrollo del proyecto***/
GPIO_Handler_t handlerStateLED = {0}; // StateLED

GPIO_Handler_t handlerPinTX    = {0}; // handlerPinTX
GPIO_Handler_t handlerPinRX    = {0}; // handlerPinRX
USART_Handler_t handlerCommTerminal = {0}; // Usart para la terminal en USART 6

GPIO_Handler_t handlerMCOPin   = {0}; //  Para probar el clock en que está trabajando el microcontrolador
PLL_Handler_t handlerPLL = {0};
//BasicTimer_Handler_t handlerTimer5 = {0}; // Timer2 Para el startSingleADC
BasicTimer_Handler_t handlerTimer5 = {0}; // Timer5 Para el led de estado

ADC_Config_t adcConfig = {0}; //Configuración del ADC

/*Elementos para el PWM*///BasicTimer_Handler_t handlerTimer5 = {0}; // Timer2 Para el startSingleADC
GPIO_Handler_t handlerPinPwmChanel          = {0};
PWM_Handler_t handlerSignalPWM              = {0};



/**Variables implemetadas en el proyecto****/
char bufferReception[64] = { 0 };

char userMsg[64] = "Holi, ya me funciona";
char cmd[16];
unsigned int firstParameter;
unsigned int secondParameter;

uint8_t stringComplete = 0;
uint8_t counterReception = 0;
uint8_t rxData = 0; // Variable donde se guardarán los datos obtenidos por el RX
int i = 0;
uint16_t freqInput = 0;
uint16_t time= 66;
uint16_t dutty = 33;

void initSystem(void);
void parseCommands (char *ptrBufferReception);
void testingADC(void);
void calibrationCLOCK(void);



char bufferData[128] = { 0 };

/*Para el ADC*/

#define NUMBER_CHANNELS 2
uint8_t adcIsComplete = 0;
uint16_t adcCounter = 0;
uint16_t dataADC[512] = { 0 };



int main(void) {

	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (13 << RCC_CR_HSITRIM_Pos);

	initSystem();
	writeMsg(&handlerCommTerminal, "\n~Iniciando Sistema~\n");

	while (1) {
		/**Creación de comandos**/
		if (rxData != '\0') {
			//Creamos una cadena de caracteres con los datos que llegan
			//El caracter '@' nos indica que es final de la cadena
			bufferReception[counterReception] = rxData;
			counterReception++;

			//If the incomming character is a newline, set a flag
			//So the main llop can do somenthing about it:
			if (rxData == '@') {
				stringComplete = true;

				//Agrego esta linea para crear el string con el null al final
				bufferReception[counterReception - 1] = '\0';

				counterReception = 0;
			}

			//Para que no vuelva a entrar. Solo cambia debido a la interrupción
			rxData = '\0';

		}
		//Hacemos el analisis  de la cadena de datos obtenida
		if (stringComplete) {
			parseCommands(bufferReception);
			stringComplete = false;

		}


	}
	return 0;
}

void initSystem(void) {

	 handlerPLL.frecSpeed = FRECUENCY_100MHz;
	 PLL_Config(&handlerPLL);


/*****Cargamos la configuración de todos los pines******/

	/***********************LED DE ESTADO************************************/
	handlerStateLED.pGPIOx = GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerStateLED);

	// Configuración del Timer 5 para el Led de estado
	handlerTimer5.ptrTIMx = TIM5;
	handlerTimer5.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimer5.TIMx_Config.TIMx_speed = BTIMER_100MHz_SPEED_100us;
	handlerTimer5.TIMx_Config.TIMx_period = 2500;
	BasicTimer_Config(&handlerTimer5);

	/*********************CONFIGURACIÓN DEL USART 6 **************************/
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF8;
	GPIO_Config(&handlerPinRX);

	handlerCommTerminal.ptrUSARTx = USART6;
	handlerCommTerminal.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_enableIntRX = ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX = DISABLE;
	USART_Config(&handlerCommTerminal);

	/***********************CONFIGURACIÓN DEL ADC*******************************/


	//Configuración del multicanal
	adcConfig.multiChannels[0]    = ADC_CHANNEL_0;
	adcConfig.multiChannels[1]    = ADC_CHANNEL_1;
	adcConfig.dataAlignment         = ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution            = ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod        = ADC_SAMPLING_PERIOD_112_CYCLES;
	adcConfig.typeEvent				= ADC_EVENT_ENABLE;
	adcConfig.adcEvent              = TIM2_CC3;



	//Se carga la configuración del ADC
	/*Nota: En caso de que sea un solo canal el segundo argumento del ADC_CongigMultichannel se pondrá en 1 */
	ADC_ConfigMultichannel(&adcConfig, 2);
	adcExternalConfig(&adcConfig);


	/* Configurando el pin que funciona el MCO para el PLL*/
	handlerMCOPin.pGPIOx 							= GPIOA;
	handlerMCOPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerMCOPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerMCOPin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerMCOPin.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	handlerMCOPin.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerMCOPin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;
	GPIO_Config(&handlerMCOPin);

	//Configuramos el PWM
	handlerPinPwmChanel.pGPIOx                           = GPIOB;
	handlerPinPwmChanel.GPIO_PinConfig.GPIO_PinNumber    = PIN_10;
	handlerPinPwmChanel.GPIO_PinConfig.GPIO_PinMode      = GPIO_MODE_ALTFN;
	handlerPinPwmChanel.GPIO_PinConfig.GPIO_PinOPType    = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChanel.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPwmChanel.GPIO_PinConfig.GPIO_PinSpeed     = GPIO_OSPEED_FAST;
	handlerPinPwmChanel.GPIO_PinConfig.GPIO_PinAltFunMode  = AF1;

	/*Cargamos la configuración de los registros del MCU*/
	GPIO_Config(&handlerPinPwmChanel);

	/*Configurando el Timer para que genere la señal PWM*/
	handlerSignalPWM.ptrTIMx                            = TIM2;
	handlerSignalPWM.config.channel                     = PWM_CHANNEL_3;
	handlerSignalPWM.config.duttyCicle                  = dutty;
	handlerSignalPWM.config.periodo                     = time;
	handlerSignalPWM.config.prescaler                   = 100;

	pwm_Config(&handlerSignalPWM);

	/*Activamos la señal*/
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);

}

void BasicTimer5_Callback(void) {
	GPIOxTooglePin(&handlerStateLED);
}

void usart6Rx_Callback(void) {
// Lectura de los datos recibidos
	rxData = getRXData();
}

void adcComplete_Callback(void) {
	dataADC[adcCounter] = getADC();
	if(adcCounter < 512 - 1){
		adcCounter++;
	}else{
		adcIsComplete = 1;
		adcCounter = 0;
	}

}

void testingADC(void) {
	if(adcIsComplete == 1){
		stopPwmSignal(&handlerSignalPWM);
		for(int i = 0; i < 512; i+=2){
			sprintf(bufferData, " %u\t%u \n",  dataADC[i], dataADC[i-1]);
			writeMsg(&handlerCommTerminal, bufferData);
		}
		adcIsComplete = 0;
	}

}

void calibrationCLOCK(void){
	RCC->CR &= ~(RCC_CR_HSITRIM); //Limpiamos el registro
	RCC->CR |= (11 << RCC_CR_HSITRIM_Pos);
}

void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);
	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerCommTerminal, "Help Menu CMDs:ADC\n");
		writeMsg(&handlerCommTerminal, "1) help -- Print this menu\n");
		writeMsg(&handlerCommTerminal, "2) clock -- Selección del MCO1\n");
		writeMsg(&handlerCommTerminal, "3) MCO preescaler\n");
		writeMsg(&handlerCommTerminal, "4) test--Testing ADC\n");
		writeMsg(&handlerCommTerminal, "5) PwmConfig--Frecuencia de muestreo\n");
		writeMsg(&handlerCommTerminal, "6) ADC - Inicialización del ADC\n");

	}else if(strcmp(cmd, "clock")==0){
		writeMsg(&handlerCommTerminal, "CMD: clock\n");


	}else if(strcmp(cmd,"MCOpreescaler")==0){
		writeMsg(&handlerCommTerminal, "CMD: MCO\n");

	}else if(strcmp(cmd,"test")==0){
		writeMsg(&handlerCommTerminal, "CMD: Testing adc\n");

	}else if(strcmp(cmd, "PwmConfig") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Ingrese la frecuencia\n");
		if(firstParameter >= 800 && firstParameter <= 1500){
			freqInput = firstParameter * 10;
			time = (1000000/freqInput); //Aquí el periodo queda

			dutty = time/2;
			updateFrequency(&handlerSignalPWM, time);
			updateDuttyCycle(&handlerSignalPWM, dutty);
		writeMsg(&handlerCommTerminal, "Frecuencia de muestreo actualizada");
		}



	}else if(strcmp(cmd, "ADC") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Activando conversión ADC\n");

		testingADC();

	}else{
		//Si está malo
		writeMsg(&handlerCommTerminal, "Wrong CMD");
	}


}
