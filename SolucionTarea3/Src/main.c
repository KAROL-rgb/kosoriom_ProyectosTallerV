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
GPIO_Handler_t handlerLED2 = {0}; //PA5
GPIO_Handler_t handlerUserButtonSW = {0}; //PC12 (Botón del encoder (SW))
GPIO_Handler_t handlerCLK = {0}; // PC10 (Conecta al CLK)
GPIO_Handler_t handlerDT = {0}; // PC11 (Conecta al DT)
GPIO_Handler_t handlerSW = {0};
GPIO_Handler_t handlerTransistor1 = {0};; // handler transistor 1
GPIO_Handler_t handlerTransistor2 = {0}; // handler transistor 2
GPIO_Handler_t handlerPC6 = {0}; // handler LED swichear

// Definción handler para display
GPIO_Handler_t handlerPinA = {0};
GPIO_Handler_t handlerPinB = {0};
GPIO_Handler_t handlerPinC = {0};
GPIO_Handler_t handlerPinD = {0};
GPIO_Handler_t handlerPinE = {0};
GPIO_Handler_t handlerPinF = {0};
GPIO_Handler_t handlerPinG = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0}; //TIM2
BasicTimer_Handler_t handlerTransistor = {0};  // Timer para el primer transistor

//Definición del exti clock
EXTI_Config_t extiCLOCK = {0};
//Definición del exti button
EXTI_Config_t extiButton = {0};


uint32_t counterExti7 = 0; //Bajo la bandera para el clock
uint32_t counterButton = 0;



uint8_t DT = 0;
uint8_t CLK = 0;
uint8_t contador = 0;
uint8_t estado = 1;
uint8_t posicion = 0;
uint8_t button = 0;
uint8_t SW = 0;
// Variables transistorers
uint8_t estadoTr1 = 0;
uint8_t estadoTr2 = 0;
uint8_t TR1 = 0;
uint8_t TR2 = 0;

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

// Unidades y decenas

/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_extInt7(void);
void callback_extInt12(void);
void BasicTimer2_Callback(void);
void BasicTimer3_Callback(void);

void delay()
{
  unsigned int i;
  for(i=0; i<600000; i++);
}

int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();

	//button = 0;
	//Leemos los pines

	contador = 0;
	posicion = 0;
	//Leemos los pines
	while(1){
		//Ciclo para controlar el giro
		//Boton
		SW = GPIO_ReadPin(&handlerSW);
		if (SW == 0){
			delay();
			estado++;
		}
		if (estado%2){
			CLK = GPIO_ReadPin(&handlerCLK);
					DT = GPIO_ReadPin(&handlerDT);
			if((counterExti7==1) && (DT==0)){
						if(contador<99){
							contador++;
						}
						counterExti7=0;

					}else if((counterExti7==1) && (DT==1)){
						if(contador<=0){

							contador=0;
						}else{
							contador--;
						}

						counterExti7=0;
					}
			TR1=GPIO_ReadPin(&handlerTransistor1);
						TR2=GPIO_ReadPin(&handlerTransistor2);
						if(TR1==0){
							GPIO_WritePin(&handlerTransistor2, 1);
							estadoTr1=TR1;
							display7(contador/10);
						}else if (TR1==1){
							GPIO_WritePin(&handlerTransistor2, 0);
							estadoTr2 = (~estadoTr1);
							display7(contador%10);
						}else{
							__NOP();
						}

		}
		else{
			CLK = GPIO_ReadPin(&handlerCLK);
					DT = GPIO_ReadPin(&handlerDT);
			if((counterExti7==1) && (DT==0)){
						if(posicion<=11){
							posicion++;
						}
						counterExti7=0;
						}else if((counterExti7==1) && (DT==1)){
							if(posicion>0){
								posicion--;
							}
							if(posicion==0){
								posicion=11;
							}
							counterExti7=0;
						}
				if(posicion>11){
					posicion=0;
				}
				if(posicion<0){
					posicion=11;
				}
				culebrita(posicion);
		}
//		if((counterButton==1) && (SW==1)){
//			button = SW;
//			counterButton = 0;
//
//		}else if ((counterButton==1) && (SW==0)){
//			button =! SW;
//			counterButton = 0;
//		}else{
//			__NOP();
//		}
//		// Configuración Transistores
//
//		TR1=GPIO_ReadPin(&handlerTransistor1);
//		TR2=GPIO_ReadPin(&handlerTransistor2);
//		if(TR1==0){
//			GPIO_WritePin(&handlerTransistor2, 1);
//			estadoTr1=TR1;
//			display7(contador/10);
//		}else if (TR1==1){
//			GPIO_WritePin(&handlerTransistor2, 0);
//			estadoTr2 = (~estadoTr1);
//			display7(contador%10);
//		}else{
//			__NOP();
//		}

	}
	return 0;
}



void init_Hardware(void){

	/* Config Led2 - PA5 */
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

	/* Configuración del Pin PB8 (DT) */
	handlerDT.pGPIOx = GPIOB;
	handlerDT.GPIO_PinConfig.GPIO_PinNumber 	= PIN_8;
	handlerDT.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_IN;
	handlerDT.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;
	handlerDT.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDT);
	/* Config botón SW*/

	handlerSW.pGPIOx = GPIOC;
	handlerSW.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	handlerSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSW);

	// COnfiguración pin al transistor1
	handlerTransistor1.pGPIOx = GPIOC;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerTransistor1);
	//Configuración pin al transistor2
	handlerTransistor2.pGPIOx = GPIOC;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerTransistor2);



	// Configuración pines para LEDS display

	// Configuración LED A
	handlerPinA.pGPIOx = GPIOB;
	handlerPinA.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPinA.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinA.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinA.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinA.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinA.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinA);
	// COnfiguración LED B
	handlerPinB.pGPIOx = GPIOC;
	handlerPinB.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPinB.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinB.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinB.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinB.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinB.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinB);

	// Configuración LED C
	handlerPinC.pGPIOx = GPIOA;
	handlerPinC.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPinC.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinC.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinC.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinC.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinC.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinC);
	// Configuración LED D

	handlerPinD.pGPIOx = GPIOA;
	handlerPinD.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPinD.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinD.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinD.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinD.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinD.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinD);
	// Configuración LED E
	handlerPinE.pGPIOx = GPIOB;
	handlerPinE.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPinE.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinE.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinE.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinE.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinE.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinE);
	// COnfiguración LED F
	handlerPinF.pGPIOx = GPIOC;
	handlerPinF.GPIO_PinConfig.GPIO_PinNumber       = PIN_10;
	handlerPinF.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinF.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinF.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinF.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinF.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinF);
	// Configuración LED G
	handlerPinG.pGPIOx = GPIOC;
	handlerPinG.GPIO_PinConfig.GPIO_PinNumber       = PIN_11;
	handlerPinG.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPinG.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinG.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinG.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPinG.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPinG);
	// Configuración LED
	/* Config TIM2 para encender cada 300 ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 250; // Interrupcion cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//Config TIM para Transistores
	handlerTransistor.ptrTIMx = TIM3;
	handlerTransistor.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerTransistor.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerTransistor.TIMx_Config.TIMx_period		= 13; // Interrupcion cada 300 ms
	handlerTransistor.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerTransistor);

	//Cargamos la configuración del EXTI del CLOCK
	extiCLOCK.pGPIOHandler = &handlerCLK;
	extiCLOCK.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiCLOCK);

	//Cargamos la configuración del EXTI del botón
	extiButton.pGPIOHandler = &handlerSW;
	extiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiButton);



}
void callback_extInt7(void){ //exti DEL CLOCK
	counterExti7 = 1; //Flanco de subida


}

void callback_extInt12(void){
	counterButton = 1;

}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);



}

void BasicTimer3_Callback(void){
	GPIOxTooglePin(&handlerTransistor1);
	GPIOxTooglePin(&handlerTransistor2);
}

