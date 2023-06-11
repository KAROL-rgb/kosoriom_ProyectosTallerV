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

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "arm_math.h"
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
USART_Handler_t usart2Comm = {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;

char bufferMsg[64] = {0};

/* Arreglos para pruebas de librerías de CMSIS */
float32_t srcNumber[4] = {-0.987, 32.36, -45.21, -987.321};
float32_t destNumber[4] = {0};
uint32_t dataSize = 0;

/* Para utilizar la función seno */
float32_t sineValue = 0.0;
float32_t sineArgValue = 0.0;
/* Elementos para generar una señal */
#define SINE_DATA_SIZE 4096 // Tamaño del arreglo de datos
float32_t fs = 8000.0; // Frecuencia de muestreo
float32_t f0 = 250.0; // Frecuencia fundamental de la señal
float32_t dt = 0.0;   // Periodo de muestreo (será 1/fs)
float32_t stopTime = 1.0;
float32_t amplitud = 5;
float32_t sineSignal[SINE_DATA_SIZE];
float32_t transformedSignal[SINE_DATA_SIZE];
float32_t ptrSineSignal;

uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 1024;

// Definición de las cabeceras de las funciones del main
void initSystem(void);
void createdSignal(void);
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
		 if(usart2DataReceived == 'C'){
			 // Crea la señal senoidal
			 createdSignal();
			 sprintf(bufferMsg, "Creando la señal ...\n");
			 writeMsg(&usart2Comm, bufferMsg);
			 usart2DataReceived = '\0';

		 }
		 /* Para probar el seno */
		 if(usart2DataReceived == 'P'){

			 stopTime = 0.0;
			 int i = 0;

			 sprintf(bufferMsg, "Signal values: Time - Sine\n");
			 writeMsg(&usart2Comm, bufferMsg);

			 while(stopTime < 0.01){ // Imprime la señal sinoidal
				 stopTime = dt*i;
				 i++;
				 sprintf(bufferMsg, "%#.5f ; %#.6f\n", stopTime, sineSignal[i]);
				 writeMsg(&usart2Comm, bufferMsg);
			 }
			 usart2DataReceived = '\0';
		 }
		 if(usart2DataReceived == 'A'){
			 stopTime = 0.0;
			 int i = 0;

			 sprintf(bufferMsg, "Valor Absoluto \n");
			 writeMsg(&usart2Comm, bufferMsg);
			 // Valor absoluto de la señal
			 arm_abs_f32(sineSignal, transformedSignal, SINE_DATA_SIZE);

			 while(stopTime < 0.01){
				 stopTime = dt*i;
				 i++;
				 sprintf(bufferMsg, "%#.5f ; %#.6f\n", stopTime, transformedSignal[i]);
				 writeMsg(&usart2Comm, bufferMsg);
			 }
			 usart2DataReceived = '\0';
		 }
		 if(usart2DataReceived == 'I'){
			 statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fftSize);

			 if(statusInitFFT == ARM_MATH_SUCCESS){
				 sprintf(bufferMsg, "Initialization...SUCCESS! \n");
				 writeMsg(&usart2Comm, bufferMsg);
			 }
			 usart2DataReceived = '\0';
		 }
		 // Transformada de fourier
		 if(usart2DataReceived == 'F'){
			 stopTime = 0.0;
			 int i = 0;
			 int j = 0;

			 sprintf(bufferMsg, "FFT \n");
			 writeMsg(&usart2Comm, bufferMsg);

			 if(statusInitFFT == ARM_MATH_SUCCESS){
				 arm_rfft_fast_f32(&config_Rfft_fast_f32, sineSignal, transformedSignal, ifftFlag);
				 arm_abs_f32(transformedSignal, sineSignal, fftSize);

				 for(i = 1; i < fftSize; i++){
					 if(i % 2){
						 sprintf(bufferMsg, "%u: %#.6f\n", j, 2*sineSignal[i]);
						 writeMsg(&usart2Comm, bufferMsg);
						 j++;
					 }
				 }
			 }
			 else{
				 writeMsg(&usart2Comm, "FFT not initialized...");
			 }
			 usart2DataReceived = '\0';
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
	 handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
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

	usart2Comm.ptrUSARTx                                 = USART1;
	usart2Comm.USART_Config.USART_baudrate               = USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize               = USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity                 = USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits               = USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode                   = USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX            = ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX            = DISABLE;
	 USART_Config(&usart2Comm);



 }
 void createdSignal(void){
	 /*
	  * Esta es la señal creada en Matlab
	  * sineSignal = amplitud * sin(2*pi*f0*t);
	  */
	 // Creando la señal, necesitamos el periodo dt
	 dt = 1 / fs;

	 /* Llenamos el arreglo con la señal seno */
	 for(int i= 0; i <SINE_DATA_SIZE; i++){
		 sineSignal[i] = amplitud * arm_sin_f32(2*M_PI*f0*(dt*i));
	 }
 }

 /*
  * Callback del Timer2 - Hacemos un blinky...
  */
void BasicTimer2_Callback(void){
//	GPIOxTooglePin(&handlerBlinkyPin);
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
	usart2DataReceived = getRXData();
}
