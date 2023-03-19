/**
 ******************************************************************************
 * @file           : main.c
 * @author         : kosoriom
 * @brief          : Configuración básica de un proyecto
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
/*
 * Solución primer punto:
 *
 * a) El error que se encuentra en la función GPIO_ReadPin() es que no se está limpiando
 */

#include <stdint.h>
#include "GPIOxDriver.h"
#include "stm32f411xx_hal.h"
#include <stdio.h>


/* Función principal del programa. Es acá donde se ejecuta todo */
int main(void){
	// *********************
	// Definimos el handler para el PIN que deseamos configurar. Estoy creando un objeto GPIO_Handler_t
	GPIO_Handler_t handlerUserLedPin = {0};
	printf("SI FUNCIONA");
	// Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;      // Ninguna función

// Tercer punto
// Definición de los pines y de los puertos
//Definición GPIOC
	GPIO_Handler_t handlerPB8 = {0};

	handlerPB8.pGPIOx = GPIOB;
	handlerPB8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPB8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPB8.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPB8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPB8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPB8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC9 = {0};

	handlerPC9.pGPIOx = GPIOC;
	handlerPC9.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPC9.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC9.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC9.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC9.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC9.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC7 = {0};

	handlerPC7.pGPIOx = GPIOC;
	handlerPC7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPC7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC7.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC8 = {0};

	handlerPC8.pGPIOx = GPIOC;
	handlerPC8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPC8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC8.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC6 = {0};

	handlerPC6.pGPIOx = GPIOC;
	handlerPC6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPC6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC6.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;


	GPIO_Handler_t handlerPA6 = {0};

	handlerPA6.pGPIOx = GPIOA;
	handlerPA6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPA6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPA6.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPA6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPA6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPA6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPA7 = {0};

	handlerPA7.pGPIOx = GPIOA;
	handlerPA7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPA7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPA7.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPA7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPA7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPA7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	// Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerPC9);
	GPIO_Config(&handlerPB8);
	GPIO_Config(&handlerPC7);
	GPIO_Config(&handlerPC8);
	GPIO_Config(&handlerPC6);
	GPIO_Config(&handlerPA6);
	GPIO_Config(&handlerPA7);

	// Hacemos que el PIN_A5 quede encendido
	uint32_t status = GPIO_ReadPin(&handlerUserLedPin);
	GPIO_WritePin(&handlerUserLedPin, SET);
	status= GPIO_ReadPin(&handlerUserLedPin);
	GPIO_WritePin(&handlerUserLedPin, CLEAR);


// Segundo punto


	// Este es el ciclo principal, donde se ejecuta todo el programa


	while(1){

		GPIOxTooglePin(&handlerUserLedPin);
		int s, c, con, k, estado;
		    con=58;
		    estado=1; //0 Subiendo 1 Bajando
		    for(s = 1; s <= 51; s++){
		    	for (c=6;c>=0;c--){
		    		k = con>>c;
		    		if(c==6){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPC9, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPC9, CLEAR);
		    		    }
		    		}
		    		if(c==5){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPC6, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPC6, CLEAR);
		    		    }
		    		}
		    		if(c==4){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPB8, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPB8, CLEAR);
		    		    }
		    		}
		    		if(c==3){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPA6, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPA6, CLEAR);
		    		    }
		    		}
		    		if(c==2){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPC7, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPC7, CLEAR);
		    		    }
		    		}
		    		if(c==1){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPC8, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPC8, CLEAR);
		    		    }
		    		}
		    		if(c==0){
		    		    if (k&1){
		    		    	GPIO_WritePin(&handlerPA7, SET);
		    		    }else {
		    		    	GPIO_WritePin(&handlerPA7, CLEAR);
		    		    }
		    		}
		    	}
				if(estado==0){con++;}
				else{con--;}
				if(con>60){con=1;}
				if(con<1){con=60;}
		    }

	}
	return 0;



}







