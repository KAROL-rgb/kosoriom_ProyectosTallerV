/**
 ******************************************************************************
 * @file           : main.c
 * @author         : kosoriom
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* Definicion de los elementos */
GPIO_Handler_t handlerLED2 = {0}; //PA5 (LED del micro)

/* Definción de los elementoss del encoder*/
GPIO_Handler_t handlerCLK = {0}; // PC7 (Conecta al CLK)
GPIO_Handler_t handlerDT = {0}; // PB6 (Conecta al DT)
GPIO_Handler_t handlerSW = {0}; // PA7 (Conecta al SW)

/* Definición de los handler de los transistores */
GPIO_Handler_t handlerTransistor1 = {0};; // handler transistor 1
GPIO_Handler_t handlerTransistor2 = {0}; // handler transistor 2

/* Definción handler para display*/
GPIO_Handler_t handlerPinA = {0}; // Segmento a
GPIO_Handler_t handlerPinB = {0}; // Segmento b
GPIO_Handler_t handlerPinC = {0}; // Segmento c
GPIO_Handler_t handlerPinD = {0}; // Segmento d
GPIO_Handler_t handlerPinE = {0}; // Segmento e
GPIO_Handler_t handlerPinF = {0}; // Segmento f
GPIO_Handler_t handlerPinG = {0}; // Segmento g

/* Definición de los Timers*/
BasicTimer_Handler_t handlerBlinkyTimer = {0}; // Timer para el LED de estado TIM2
BasicTimer_Handler_t handlerTransistor = {0};  // Timer para el transistor 1 TIM3

/* Definición de los Exti */
EXTI_Config_t extiClock = {0};      // Definición del exti para clock del encoder
EXTI_Config_t extiButton = {0};     // Definición del exti el botón del encoder

uint32_t counterExti = 0;          // Bajo la bandera para el clock
uint32_t counterButton = 0;        // Bajo la bander para el botón

/* Definición de las variables*/
uint8_t DT = 0;
uint8_t CLK = 0;
uint8_t contador = 0;
uint8_t estado = 1;
uint8_t posicion = 0;
uint8_t SW = 0;

/* Definición de las variables transistores*/
uint8_t estadoTr1 = 0;
uint8_t estadoTr2 = 0;
uint8_t TR1 = 0;
uint8_t TR2 = 0;

/* Función para mostrar los números del 0 al 9 */
/* NOTA: Dado que el display a utilizar es de ánodo común se energiza con RESET (0 lógico) como se observa a continuación */

void display7(uint8_t numero){
	switch(numero){
	// Casos para encender los LEDs del segmento
	case 0:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, RESET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 1:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 2:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, RESET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	case 3:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	case 4:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	case 5:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	case 6:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, RESET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	case 7:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 8:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, RESET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	case 9:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, RESET);
		break;
	}

}

/* Función para mostrar las posiciones de la culebrita */
void culebrita(uint8_t numero){
	switch(numero){
	case 0:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 1:
		GPIO_WritePin(&handlerPinA, RESET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 2:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 3:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, RESET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 4:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 5:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, RESET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 6:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, RESET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 7:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 8:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 9:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, RESET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 10:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, SET);
		GPIO_WritePin(&handlerPinC, RESET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;
	case 11:
		GPIO_WritePin(&handlerPinA, SET);
		GPIO_WritePin(&handlerPinB, RESET);
		GPIO_WritePin(&handlerPinC, SET);
		GPIO_WritePin(&handlerPinD, SET);
		GPIO_WritePin(&handlerPinE, SET);
		GPIO_WritePin(&handlerPinF, SET);
		GPIO_WritePin(&handlerPinG, SET);
		break;

	}
}

/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_extInt7(void);
void callback_extInt12(void);
void BasicTimer2_Callback(void);
void BasicTimer3_Callback(void);

/* Función de retardo para el cambio de estado (de cotador a culebrita) */
void delay()
{
  unsigned int i;
  for(i=0; i<600000; i++);
}

int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();

	contador = 0;
	posicion = 0;

	while(1){

		SW = GPIO_ReadPin(&handlerSW);           // Lectura del botón
		if (SW == 0){                            // Condición para cuando el botón está presionado (valor de 0)
			delay();                             // Para controlar que se cuente alrededor de 1s
			estado++;                            // Instrucción para que el estado aumente cuando se cumpla lo anterior
		}
		if ((estado%2) == 1){                    // Se verifica que si el módulo es 1, es un número impar, por ende entra a modo contador
			CLK = GPIO_ReadPin(&handlerCLK);
			DT = GPIO_ReadPin(&handlerDT);

			// Se verifica si el giro es clockwise o counterwise
			if((counterExti == 1) && (DT == 0)){          // Clockwise
				if(contador < 99){
					contador++;
				}
				counterExti = 0;

			}else if((counterExti == 1) && (DT == 1)){    // Counterwise
				if(contador <= 0){
					contador = 0;
				}else{
					contador--;
				}

				counterExti = 0;
			}
			// "Suicheo" de los transistores para el contador
			TR1=GPIO_ReadPin(&handlerTransistor1);
			TR2=GPIO_ReadPin(&handlerTransistor2);
			if(TR1 == 0){                                   // Se muestran las decenas (lado izquierdo)
				GPIO_WritePin(&handlerTransistor2, 1);
				estadoTr1 = TR1;
				display7(contador/10);
			}else if (TR1 == 1){                           // Se muestran las unidades (lado derecho)
				GPIO_WritePin(&handlerTransistor2, 0);
				estadoTr2 = (~estadoTr1);
				display7(contador%10);
			}else{
				__NOP();
			}
		}
		else{
			// Se verifica que si el módulo es 0, es un número par, por ende entra a modo culebrita
			CLK = GPIO_ReadPin(&handlerCLK);
			DT = GPIO_ReadPin(&handlerDT);
			if((counterExti == 1) && (DT == 0)){
				if(posicion <= 11){
					posicion++;
				}
				counterExti = 0;
			}else if((counterExti == 1) && (DT == 1)){
				if(posicion == 0){
					posicion = 11;
				}else{
					posicion--;
				}
				counterExti = 0;
			}
			if(posicion > 11){
				posicion = 0;
			}
			if(posicion < 0){
				posicion = 11;
			}
			// "Suicheo" de los transistores para la culebrita
			TR1=GPIO_ReadPin(&handlerTransistor1);
			TR2=GPIO_ReadPin(&handlerTransistor2);

			/* Se generan condiciones lógicas de manera tal que se abarquen todas las posiciones de los segmentos, tanto de izquiera como de derecha
			 * Esta condición permite cambiar de posición en el modo culebrita y "suichear" a la vez ambos segmentos */
			if(((posicion >= 1) && (posicion <= 4)) || ((posicion >= 7) && (posicion <= 8))){                            // Posiciones segmento de la izquierda
				if (TR1 == 1){
					GPIO_WritePin(&handlerTransistor1, 0);
					GPIO_WritePin(&handlerTransistor2, 1);
					estadoTr2 = TR1;
					culebrita(posicion);
				}
			}
			if((posicion == 0) || ((posicion >= 5) && (posicion <= 6)) || ((posicion >= 9) && (posicion <= 11))){       // Posiciones segmento de la derecha
				if(TR1 == 0){
					GPIO_WritePin(&handlerTransistor1, 1);
					GPIO_WritePin(&handlerTransistor2, 0);
					estadoTr1 = (~estadoTr2);
					culebrita(posicion);
				}

			}
			else{
				__NOP();
			}

		}

	}
	return 0;
}

void init_Hardware(void){

	/* Config Led de estado PA5 */
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 	= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLED2);

	/* Configuración del Pin PC7 (CLK) */
	handlerCLK.pGPIOx = GPIOC;
	handlerCLK.GPIO_PinConfig.GPIO_PinNumber 	= PIN_7;
	handlerCLK.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_IN;
	handlerCLK.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerCLK);

	/* Configuración del Pin PB6 (DT) */
	handlerDT.pGPIOx = GPIOB;
	handlerDT.GPIO_PinConfig.GPIO_PinNumber 	= PIN_6;
	handlerDT.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_IN;
	handlerDT.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerDT.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDT);


	/* Configuración del botón, Pin PA7 (SW) */
	handlerSW.pGPIOx = GPIOA;
	handlerSW.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSW);

	/* Configuración Pin al transistor1 */
	handlerTransistor1.pGPIOx = GPIOA;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerTransistor1);

	/* Configuración pin al transistor2 */
	handlerTransistor2.pGPIOx = GPIOC;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerTransistor2);

	/* Configuración pines para los segmentos de LEDS del display */

	/* Configuración segmento de LED A*/
	handlerPinA.pGPIOx = GPIOC;
	handlerPinA.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerPinA.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinA.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinA.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinA.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinA.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinA);

	/* Configuración segmento de LED B */
	handlerPinB.pGPIOx = GPIOB;
	handlerPinB.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPinB.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinB.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinB.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinB.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinB.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinB);

	/* Configuración segmento de LED C */
	handlerPinC.pGPIOx = GPIOC;
	handlerPinC.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPinC.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinC.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinC.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinC.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinC.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinC);


	/*Configuración segmento de LED D*/
	handlerPinD.pGPIOx = GPIOB;
	handlerPinD.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPinD.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinD.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinD.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinD.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinD.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinD);

	/* Configuración segmento de LED E */
	handlerPinE.pGPIOx = GPIOC;
	handlerPinE.GPIO_PinConfig.GPIO_PinNumber       = PIN_12;
	handlerPinE.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinE.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinE.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinE.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinE.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinE);

	/* Connfiguración segmento de LED F*/
	handlerPinF.pGPIOx = GPIOC;
	handlerPinF.GPIO_PinConfig.GPIO_PinNumber       = PIN_10;
	handlerPinF.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinF.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinF.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinF.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinF.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinF);

	/*Configuración segmento de LED G*/
	handlerPinG.pGPIOx = GPIOC;
	handlerPinG.GPIO_PinConfig.GPIO_PinNumber       = PIN_11;
	handlerPinG.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinG.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinG.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinG.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinG.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinG);

	/* Config TIM2 para encender el LED2 (LED de estado) cada 250 ms */
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 250;                // Interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración TIM3 para Transistores */
	handlerTransistor.ptrTIMx = TIM3;
	handlerTransistor.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerTransistor.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerTransistor.TIMx_Config.TIMx_period		= 12;                // Interrupcion cada 12 ms
	handlerTransistor.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerTransistor);

	/* Se Carga la configuración del EXTI del Clock */
	extiClock.pGPIOHandler = &handlerCLK;
	extiClock.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiClock);

	/* Se carga la configuración del EXTI del botón */
	extiButton.pGPIOHandler = &handlerSW;
	extiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiButton);

}

void callback_extInt7(void){                      // Exti del Clock
	counterExti = 1;                              // Flanco de subida

}

void callback_extInt12(void){                     // Exti del botón
	counterButton = 1;	                          // Flanco de subida

}


void BasicTimer2_Callback(void){                  // Timer para el LED de estado
	GPIOxTooglePin(&handlerLED2);

}

void BasicTimer3_Callback(void){                  // Timer para los transistores
	GPIOxTooglePin(&handlerTransistor1);
	GPIOxTooglePin(&handlerTransistor2);
}

