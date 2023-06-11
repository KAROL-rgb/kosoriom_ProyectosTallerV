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
#include "PwmDriver.h"
#include "AdcDriver.h"
#include "PLLDriver.h"
#include "RTCDriver.h"

BasicTimer_Handler_t handlerTimer2 	= {0};	// Timer2
USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2
uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX

GPIO_Handler_t 	handlerLedOK 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
PLL_Handler_t handlerPLL = {0};
RTC_Handler_t handlerRTC = {0};
// PWM y ADC
PWM_Handler_t  handlerPWM = {0};
GPIO_Handler_t hnadlerPinPWM = {0};
ADC_Config_t adcConfig = {0};

char bufferData[128] = {0};

#define NUMBER_CHANNELS	2
uint8_t adcIsComplete		= 0;
uint16_t adcCounter			= 0;
uint16_t dataADC[512] = {0};
uint8_t flagADC = {0};
uint16_t data1ADC[256];
uint16_t data2ADC[256];
uint16_t dataCounter = 0;
uint16_t counterData = 0;
uint16_t freqInput = 0;
uint16_t time = 66;
uint16_t dutty = 33;

//ADC_Config_t channel_1				= {0};
//ADC_Config_t channel_2				= {0};
GPIO_Handler_t handlerPinMCO1               = {0};
MCO1_Handler_t handlerMCO1 = {0};

char bufferReception[64] ={0};
char userMsg[64] = "Funciona";
char cmd[16];

uint8_t counterReception = 0;
uint8_t stringComplete = 0;
uint8_t autoUpdateLCD = 0;

unsigned int firstParameter;
unsigned int secondParameter;

void initSystem(void);
void parseCommands(char *ptrBufferReception);
void testingADC(void);
int main(void){

	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (13 << RCC_CR_HSITRIM_Pos);

	initSystem();
	writeMsg(&handlerCommTerminal, "\n~Iniciando Sistema~\n");

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
//		if(adcIsComplete == 1){
//		// Enviamos los datos por consola
//		sprintf(bufferData,"%u\t%u\n",dataADC[0],dataADC[1]);
//		writeMsg(&handlerCommTerminal, bufferData);
//		// Bajamos la bandera del ADC
//		adcIsComplete = 0;
//	}

	}
	return 0;
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

//		handlerTimer2.ptrTIMx								= TIM2;
//		handlerTimer2.TIMx_Config.TIMx_mode				    = BTIMER_MODE_UP;
//		handlerTimer2.TIMx_Config.TIMx_speed				= BTIMER_100MHz_SPEED_100us;
//		handlerTimer2.TIMx_Config.TIMx_period 				= 2500;
//
//		BasicTimer_Config(&handlerTimer2);

		/****Configuración para probar el MCO1 en el analizador de señales****/
		handlerPinMCO1.pGPIOx                                = GPIOA;
		handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber         = PIN_8;
		handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
		handlerPinMCO1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
		handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
		handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
		handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode     = AF0;
		GPIO_Config(&handlerPinMCO1);

		/*** Configuración del canal ADC ***/
		adcConfig.multiChannels[0]     = ADC_CHANNEL_0;
		adcConfig.multiChannels[1]     = ADC_CHANNEL_1;
		adcConfig.dataAlignment 	   = ADC_ALIGNMENT_RIGHT;
		adcConfig.resolution 		   = ADC_RESOLUTION_12_BIT;
		adcConfig.samplingPeriod	   = ADC_SAMPLING_PERIOD_84_CYCLES;
		adcConfig.adcEvent = 5;
		adcConfig.typeEvent = 1;
		//Se carga la configuración del ADC
		ADC_ConfigMultichannel(&adcConfig,2);
		adcExternalConfig(&adcConfig);

		hnadlerPinPWM.pGPIOx                           = GPIOB;
		hnadlerPinPWM.GPIO_PinConfig.GPIO_PinNumber    = PIN_10;
		hnadlerPinPWM.GPIO_PinConfig.GPIO_PinMode      = GPIO_MODE_ALTFN;
		hnadlerPinPWM.GPIO_PinConfig.GPIO_PinOPType    = GPIO_OTYPE_PUSHPULL;
		hnadlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		hnadlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed     = GPIO_OSPEED_FAST;
		hnadlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode  = AF1;

		/*Cargamos la configuración de los registros del MCU*/
		GPIO_Config(&hnadlerPinPWM);

		handlerPWM.ptrTIMx    = TIM2;
		handlerPWM.config.channel = PWM_CHANNEL_3;
		handlerPWM.config.duttyCicle = 33;
		handlerPWM.config.periodo = 60;
		handlerPWM.config.prescaler = 100;



		// Activar señal
		pwm_Config(&handlerPWM);
		enableOutput(&handlerPWM);
		startPwmSignal(&handlerPWM);

		handlerRTC.RTC_Day = 9;
		handlerRTC.RTC_Mounth = 11;
		handlerRTC.RTC_Years  = 22;
		handlerRTC.RTC_Hours = 8;
		handlerRTC.RTC_Minutes = 00;
		handlerRTC.RTC_Seconds = 00;
		RTC_Config(&handlerRTC);


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
//	/* Comandos para elegir la señal del MCO1 */
	else if(strcmp(cmd, "selectClock") == 0){
		if(firstParameter == 0){
			handlerMCO1.clock = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferData, "MCO1 configura el HSI \n");
			writeMsg(&handlerCommTerminal, bufferData);
		}else if(firstParameter == 1){
			handlerMCO1.clock = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferData, "MCO1 configura el LSE \n");
			writeMsg(&handlerCommTerminal, bufferData);
		}else if(firstParameter == 2){
			handlerMCO1.clock = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferData, "MCO1 configura el PLL \n");
			writeMsg(&handlerCommTerminal, bufferData);

		}
	}

	/* Comandos para seleccionar el prescaler de la señal en el MCO1 */
	else if(strcmp(cmd, "selectPrescaler") == 0)
	{
		if(firstParameter < 6 && firstParameter > 1){
			handlerMCO1.preescaler = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferData, "MCO1 configura a %u \n", firstParameter);
			writeMsg(&handlerCommTerminal, bufferData);
		}
		else{
			writeMsg(&handlerCommTerminal, "Error en el comando ingresado");
		}
	}


	/* Comandos para el RTC */
	// Configuración de la hora inicial
	else if(strcmp(cmd, "initialHours") == 0)
	{
		writeMsg(&handlerCommTerminal, " Inicializacion time \n");
		handlerRTC.RTC_Hours = firstParameter;
		handlerRTC.RTC_Minutes = secondParameter;
		handlerRTC.RTC_Seconds = 00;
		RTC_Config(&handlerRTC);

	}
	/* Configuración fecha inicial */
	else if(strcmp(cmd, "initalMonths") == 0){
		writeMsg(&handlerCommTerminal, " Inicializacion date \n");
		handlerRTC.RTC_Day = firstParameter;
		handlerRTC.RTC_Mounth = secondParameter;
		handlerRTC.RTC_Years = 23;
		RTC_Config(&handlerRTC);
	}
	/* configuración hora actual */
	else if(strcmp(cmd, "getHours") == 0){
		writeMsg(&handlerCommTerminal, " Actual Hour \n");
		uint8_t hours = getHours(handlerRTC);
		uint8_t minutes = getMinutes(handlerRTC);
		uint8_t seconds = getSeconds(handlerRTC);
		sprintf(bufferData, "La hora es: %u : %u : %u \n", hours, minutes, seconds);
		writeMsg(&handlerCommTerminal, bufferData);

	}
	else if(strcmp(cmd, "getMounths") == 0){
		writeMsg(&handlerCommTerminal, " Actual date \n");
		uint8_t days = getDays(handlerRTC);
		uint8_t mounths = getMounths(handlerRTC);
		sprintf(bufferData, "La fecha es: %u / %u / 2023 \n", days, mounths);
		writeMsg(&handlerCommTerminal, bufferData);

	}
	// Comandos para ADC
	else if(strcmp(cmd, "PwmConfig") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Ingrese la frecuencia\n");

		if(firstParameter >= 800 && firstParameter <= 1500){
		freqInput = firstParameter * 10;
		time = (1000000/freqInput); //Aquí el periodo queda

		dutty = time/2;
		updateFrequency(&handlerPWM, time);
		updateDuttyCycle(&handlerPWM, dutty);
		writeMsg(&handlerCommTerminal, "Frecuencia de muestreo actualizada");
		}

	}else if(strcmp(cmd, "ADC") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Activando conversión ADC\n");

		testingADC();
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
void adcComplete_Callback(void){
	dataADC[adcCounter] = getADC();
	if(adcCounter < (512 - 1)){
		adcCounter++;
	}
	else{
		adcIsComplete = 1;
		adcCounter = 0;
	}
}

void testingADC(void){
	if(adcIsComplete == 1){
		stopPwmSignal(&handlerPWM);
		for(int i= 0; i < 512; i+=2){
			sprintf(bufferData, "%u\t%u \n", dataADC[i], dataADC[i-1]);
			writeMsg(&handlerCommTerminal, bufferData);
		}
		startPwmSignal(&handlerPWM);
		adcIsComplete = 0;
	}
}
