/*
 * Testing-DSP.c
 *
 *  Created on: 25/05/2023
 *      Author: karol
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "I2CDriver.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "arm_math.h"
#include "PLLDriver.h"
#include "RTCDriver.h"
#include "AdcDriver.h"
#include "PwmDriver.h"
// Definición de los handlers necesarios
RTC_Handler_t handlerRTC = {0};
PWM_Handler_t  handlerPWM = {0};
GPIO_Handler_t hnadlerPinPWM = {0};
ADC_Config_t adcConfig = {0};
GPIO_Handler_t 	handlerLedOK  = {0};
uint16_t contador = {0};
uint8_t flag = 0;
uint8_t counter = 0;
GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t handlerUserButtonExti = {0};
BasicTimer_Handler_t handlerTimer5 = {0};
BasicTimer_Handler_t handlerTimer2 	= {0};
PLL_Handler_t handlerPLL = {0};
BasicTimer_Handler_t handlerTimerMuestreo   = {0};
char bufferMsg[128] = {0};
/* Elemento para hacer la comunicación serial */

GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal		= {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;
uint8_t 	rxData					= 0;
char bufferData[128] = {0};

char bufferReception[64] ={0};
char userMsg[64] = "Funciona";
char cmd[16];
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t counterReception = 0;
uint8_t stringComplete = 0;
/* Para utilizar la función seno */
float32_t sineValue = 0.0;
float32_t sineArgValue = 0.0;
/* Elementos para generar una señal */
#define ARRAY_Z_SIZE 256 // Tamaño del arreglo de datos
float32_t fs = 8000.0; // Frecuencia de muestreo
float32_t f0 = 250.0; // Frecuencia fundamental de la señal
float32_t dt = 0.0;   // Periodo de muestreo (será 1/fs)
float32_t stopTime = 1.0;
float32_t amplitud = 5;
float32_t sineSignal[ARRAY_Z_SIZE];
float32_t transformedSignal[ARRAY_Z_SIZE];
float32_t ptrSineSignal;

uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 128;
MCO1_Handler_t handlerMCO1 = {0};
unsigned int firstParameter;
unsigned int secondParameter;

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
// Definición de las cabeceras de las funciones del main
void initSystem(void);
void createdSignal(void);
void parseCommands(char *ptrBufferReception);
void acelerometro(void);
void testingADC(void);

#define ACCEL_ADDRESS     0x1D // 0xD2 -> Dirección del Accel con Logic_1
#define ACCEL_XOUT_L      50      // 0x3B
#define ACCEL_XOUT_H      51      // 0x3C
#define ACCEL_YOUT_L      52      // 0x3D
#define ACCEL_YOUT_H      53      // 0x3E
#define ACCEL_ZOUT_L      54      // 0x3F
#define ACCEL_ZOUT_H      55      // 0x40

#define BW_RATE          44
#define POWER_CTL        45
#define WHO_AM_I          0
float arrayX[256]={0};
float arrayY[256]={0};
float arrayZ[256]={0};
/**
 * Función principal del programa
 * Esta función es el corazón del programa !!
 *
 * */
 int main(void){

	 RCC->CR &= ~RCC_CR_HSITRIM;
	 RCC->CR |= (13 << RCC_CR_HSITRIM_Pos);


	 // Inicializamos todos los elementos del sistema
	 initSystem();
	 writeMsg(&handlerCommTerminal, "\n~Iniciando Sistema~\n");
	 /* Loop forever */
	 while(1){
		// Creamos una cadena de caracteres con los datos que llegan por el serial
		// El caracter '@' nos indica que es el final de la cadena
		if (rxData != '\0') {
			bufferReception[counterReception] = rxData;
			counterReception++;

			// if the incoming character is a newline, set a flag
			// so the main loop can do something about it:
			if (rxData == '@') {
				stringComplete = true;

				// Agrego esta línea para crear el string con el null al final
				bufferReception[counterReception] = '\0';

				counterReception = 0;
			}

			// Para que no vuelva a entrar. Solo cambia debido a la interrupción
			rxData = '\0';
		}

		// Hacemos un análisis de la cadena de datos obtenida
		if (stringComplete) {
			parseCommands(bufferReception);
			stringComplete = false;
		}

	}
	 return 0;
}


 /*
  * Función encargada de la inicialización de los elementos del sistema
  */
 void initSystem(void){

		handlerPLL.frecSpeed = FRECUENCY_100MHz;
		PLL_Config(&handlerPLL);
		/* Activación punto flotante */
		SCB->CPACR |= (0xF << 20);

		//Configurando el pin para el Led Blinky
		handlerLedOK.pGPIOx = GPIOH;
		handlerLedOK.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		handlerLedOK.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
		handlerLedOK.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		GPIO_Config(&handlerLedOK);

		//Llevamos el LED a un estado de encendido (ON o SET)
		GPIO_WritePin(&handlerLedOK, SET);
		handlerPinTX.pGPIOx = GPIOA;
		handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
		handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		handlerPinTX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;

		GPIO_Config(&handlerPinTX);

		handlerPinRX.pGPIOx = GPIOA;
		handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
		handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		handlerPinRX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;

		GPIO_Config(&handlerPinRX);

		// Configuración comunicación serial
		handlerCommTerminal.ptrUSARTx = USART1;
		handlerCommTerminal.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
		handlerCommTerminal.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
		handlerCommTerminal.USART_Config.USART_parity = USART_PARITY_NONE;
		handlerCommTerminal.USART_Config.USART_stopbits = USART_STOPBIT_1;
		handlerCommTerminal.USART_Config.USART_mode = USART_MODE_RXTX;
		handlerCommTerminal.USART_Config.USART_enableIntRX = ENABLE;
		handlerCommTerminal.USART_Config.USART_enableIntTX = DISABLE;
		USART_Config(&handlerCommTerminal);

		handlerTimer2.ptrTIMx = TIM5;
		handlerTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
		handlerTimer2.TIMx_Config.TIMx_speed = BTIMER_100MHz_SPEED_100us;
		handlerTimer2.TIMx_Config.TIMx_period = 2500;

		BasicTimer_Config(&handlerTimer2);

		/****Configuración para probar el MCO1 en el analizador de señales****/
//		handlerPinMCO1.pGPIOx = GPIOA;
//		handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
//		handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//		handlerPinMCO1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
//		handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
//		handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//		handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
//		GPIO_Config(&handlerPinMCO1);

		/*Configurando los pines sobre los que funciona el I2C1*/
		handlerI2cSCL.pGPIOx = GPIOB;
		handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
		handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
		handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
		GPIO_Config(&handlerI2cSCL);

		/*Configurando los pines sobre los que funciona el I2C1*/
		handlerI2cSDA.pGPIOx = GPIOB;
		handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
		handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
		handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
		GPIO_Config(&handlerI2cSDA);

		//Configuración Timer para 200Hz
		handlerTimerMuestreo.ptrTIMx = TIM4;
		handlerTimerMuestreo.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
		handlerTimerMuestreo.TIMx_Config.TIMx_speed = BTIMER_100MHz_SPEED_1ms;
		handlerTimerMuestreo.TIMx_Config.TIMx_period = 5; // Interrupcion cada 5 ms
		handlerTimerMuestreo.TIMx_Config.TIMx_interruptEnable = ENABLE;
		BasicTimer_Config(&handlerTimerMuestreo);

		/*Configurando el acelerómetro*/

		handlerAccelerometer.ptrI2Cx = I2C1;
		handlerAccelerometer.modeI2C = I2C_MODE_FM;
		handlerAccelerometer.slaveAddress = ACCEL_ADDRESS;
		handlerAccelerometer.mainClock = MAIN_CLOCK_100_MHz_FOR_I2C;
		handlerAccelerometer.maxI2C_FM = I2C_MAX_RISE_TIME_FM_100MHz;
		handlerAccelerometer.modeI2C_FM = I2C_MODE_FM_SPEED_400KHz_100MHz;
		i2c_config(&handlerAccelerometer);

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

 	/* Comandos acelerómetro */
	else if (strcmp(cmd, "ValoresAcelerometro") == 0) {
		acelerometro();
		writeMsg(&handlerCommTerminal, "Mostrar los datos del acelerómetro \n");
		for (int i = 0; i < 256; i++) {
			sprintf(bufferMsg, "%u arrayX = %.2f m/s2 ; arrayY = %.2f m/s2; arrayZ = %.2f m/s2 \n", i, arrayX[i], arrayY[i], arrayZ[i]);
			writeMsg(&handlerCommTerminal, bufferMsg);
		}
		rxData = '\0';
	}
	else if(strcmp(cmd, "initFFT") == 0) {
		sprintf(bufferData, "\n");
		writeMsg(&handlerCommTerminal, "Inicia la FFT");
		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fftSize);

		if(statusInitFFT == ARM_MATH_SUCCESS){
			writeMsg(&handlerCommTerminal, "FFT inicializada");
		}
		rxData = '\0';
	}
	else if(strcmp(cmd, "OnFFT") == 0){
		int k = 0;
		int d = 0;
		sprintf(bufferData, "FFT\n");
		writeMsg(&handlerCommTerminal, "Se empiezan a tomar los datos de la FFT");
		if(statusInitFFT == ARM_MATH_SUCCESS){
			float32_t transZ = 0;
			uint8_t transMax = 0;
			arm_rfft_fast_f32(&config_Rfft_fast_f32, arrayZ, transformedSignal, ifftFlag);
			arm_abs_f32(transformedSignal, arrayZ, fftSize);
			for(k= 0; k < fftSize; k++){
				if(k%2){
					if(arrayZ[k]>transMax){
						transMax = d;
						transZ = 2*arrayZ[k];
					}
					sprintf(bufferMsg, "%u: %#.6f\n", d, 2*arrayZ[k]);
					 writeMsg(&handlerCommTerminal, bufferData);
					 d++;
				}

			}
			sprintf(bufferData, " El valor maximo de z es %f con indice %u\n", transZ, transMax);
			writeMsg(&handlerCommTerminal, bufferData);
			float frecFFT = transMax * ((2*3.14))/((fftSize/2)*0.005);
			sprintf(bufferData, " La frecuencia es %f \n", frecFFT);
			writeMsg(&handlerCommTerminal, bufferData);
		}

	} else {
		// Se imprime el mensaje "wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerCommTerminal, "FFT no inicializada");
	}
	rxData = '\0';
}
 void acelerometro(void) {

 	flag = 1;
 	while (flag == 1) {

 //		i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);
 		i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x08);
 		i2c_writeSingleRegister(&handlerAccelerometer, BW_RATE, 0xF);


 		if (contador < 256) {
 			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
 			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
 			int16_t AccelX = AccelX_high << 8 | AccelX_low;

 			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
 			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
 			int16_t AccelY = AccelY_high << 8 | AccelY_low;

 			uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
 			uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
 			int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

 			arrayX[contador] = (AccelX / 256.f * 9.8);
 			arrayY[contador] = (AccelY / 256.f * 9.8);
 			arrayZ[contador] = (AccelZ / 256.f * 9.8);
 		}
// 		GPIO_WritePin(&handlerPC9, RESET);
 		while (counter) {
 			__NOP();
 		}
 		counter = 1;
 	}


}
 void BasicTimer4_Callback(void){
// 	GPIOxTooglePin(&handlerPC9);
 	if(flag == 1){
 		contador ++;
 		counter = 0;
 	}
 	if(contador == 256){
 		rxData = '\0';
 		contador = 0;
 		flag = 0;

 	}
}
void usart1Rx_Callback(void){
 	// Leemos el valor del registro DR, donde se almacena el dato que llega.
 	// Esro adempas debe bajar la bandera de la interrupción
 	rxData = getRXData();
}
 void BasicTimer5_Callback(void){
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



