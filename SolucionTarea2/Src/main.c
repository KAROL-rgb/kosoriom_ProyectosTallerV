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

#include <stdint.h>
#include "GPIOxDriver.h"
#include "stm32f411xx_hal.h"

// Declaramos las variables a utilizar

uint16_t USER_BUTTON = 0;
uint16_t status = 0;
uint16_t status2 = 0;

/*
 * Solución primer punto:
 *
 * a) La función GPIO_ReadPin(), es la encargada de leer el valor del registro IDR, lo hace mediante un shift a la derecha tantas veces como la ubicación del PIN lo indique.
 * El error que se encuentra en la función es que no se está limpiando el registro IDR de los bits que quedan después de realizar el shift. Al realizar del debugger se observó
 * que el pin 5 cambiaba de valor pero al pedir el valor del IDR, éste se inicializaba con un valor aleatorio (0 o 1) y estaba realizando un shift de 5 unidades hacia la derecha
 * al valor que tenía pero no estaba devolviendo el valor del pin. Esto estaba pasando porque no se estaba limpiando la posición antes de cargar la nueva configuración en la variable
 * pinValue.
 *
 * b) Para solucionarlo se propuso una máscara de un SET y una operación AND, con las cuales se garantizara la limpieza de la posición y que al hacer el shift se mantuviera el valor
 * de la posición del registro, de esta manera al cargar la configuración en la variable pinValue ésta arroja un 1 o un 0 dependiendo del valor de la posición que se puede leer en el IDR
 * al momento de hacer el debugger. La corrección realizada en el archivo GPIOxDriver.c fue:
 *
 * c) pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler ->GPIO_PinConfig.GPIO_PinNumber);
 *    pinValue &= 0b1;

*/

/*La siguiente es la función del delay, la cual será usada en el punto 3
 * La función delay es la encargada de dar el tiempo en el cual se apagaran o encenderán los LEDs
 */
void delay()
{
  unsigned int i;
  for(i=0; i<1600000; i++);
}

/* Función principal del programa. Es acá donde se ejecuta todo */

int main(void){
	// *********************
	// Definimos el handler para el PIN que deseamos configurar. Estoy creando un objeto GPIO_Handler_t
	GPIO_Handler_t handlerUserLedPin = {0};

	// Para probar el primer punto se procede a configurar el PIN_A5. Deseamos trabajar con el puerto GPIOA.

	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;      // Ninguna función

	// Lo siguiente es cargar la configuración del Pin
	GPIO_Config(&handlerUserLedPin);
	//Encender PIN_A5
	GPIO_WritePin(&handlerUserLedPin, SET);
	// Prueba de lectura
	status = GPIO_ReadPin(&handlerUserLedPin);

/*
* Segundo punto:
*
* La función TooglePin permite cambiar el estado de un PIN. Lo primero que se realizó fue escribir la configuración para direccionar el pin y escojer el registro a utilizar, para este caso
* fue con el registro ODR, debido a que este permite configurar el PIN como salida. Se aplica un SET (0b1) y se desplaza a la izquierda tantas veces como el número del pin que se desea cambiar
* lo indique. Lo siquiente es aplicar la operación XOR al valor cargado en el registro, esta operación es utilizada ya que conserva el valor cuando se aplica con cero y cambia el valor cuando
* se aplica con 1.
* La función TooglePin fue creada en el archivo GPIOxDriver.h
*/
	// Prueba de la función TooglePin para cambiar el estado del LED2
	GPIOxTooglePin(&handlerUserLedPin);
	status = GPIO_ReadPin(&handlerUserLedPin);
	GPIOxTooglePin(&handlerUserLedPin);
	status2 =  GPIO_ReadPin(&handlerUserLedPin);

/*
 * Tercer punto:
 *
 */
	// Lo primero es definir el handler para cada uno de los PINES que se desean utilizar:

	GPIO_Handler_t handlerPC9 = {0};
	GPIO_Handler_t handlerPB8 = {0};
	GPIO_Handler_t handlerPC7 = {0};
	GPIO_Handler_t handlerPC8 = {0};
	GPIO_Handler_t handlerPC6 = {0};
	GPIO_Handler_t handlerPA6 = {0};
	GPIO_Handler_t handlerPA7 = {0};
	GPIO_Handler_t handlerPC13 = {0}; // Este handler corresponde al PIN del USER BUTTON

	// Se configuran los puertos con los cuales se va a trabajar.

	handlerPB8.pGPIOx = GPIOB;
	handlerPB8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPB8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPB8.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPB8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPB8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPB8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	handlerPC9.pGPIOx = GPIOC;
	handlerPC9.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPC9.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC9.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC9.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC9.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC9.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	handlerPC7.pGPIOx = GPIOC;
	handlerPC7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPC7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC7.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	handlerPC8.pGPIOx = GPIOC;
	handlerPC8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPC8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC8.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	handlerPC6.pGPIOx = GPIOC;
	handlerPC6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPC6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC6.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	handlerPA6.pGPIOx = GPIOA;
	handlerPA6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPA6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPA6.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPA6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPA6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPA6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	handlerPA7.pGPIOx = GPIOA;
	handlerPA7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPA7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPA7.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPA7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPA7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPA7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	// Configuración del puerto del USER_BUTTON:

	handlerPC13.pGPIOx = GPIOC;
	handlerPC13.GPIO_PinConfig.GPIO_PinNumber       = PIN_13;
	handlerPC13.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IN;
	handlerPC13.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC13.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;

	// Cargamos la configuración de cada uno de los PINES

	GPIO_Config(&handlerPC9);
	GPIO_Config(&handlerPB8);
	GPIO_Config(&handlerPC7);
	GPIO_Config(&handlerPC8);
	GPIO_Config(&handlerPC6);
	GPIO_Config(&handlerPA6);
	GPIO_Config(&handlerPA7);

	//Configuración del User Button
	GPIO_Config(&handlerPC13);

	// Se inicializan las variables c, con, k, y estado

	int c, con, k, estado;
	con = 1;                                                 // Este es el contador para los LEDS y me va a indicar el número del LED que se va a alumnbrar
	estado = 0;                                              // Este es el estado que debe cambiar el USER BUTTON donde 0 es subiendo y 1 bajando
	while(1){

		for (c = 6;c >= 0;c--){                              // Este ciclo entra en el contador para pasar la representación del contador a binario
			k = con >> c;                                    // Shift de c posiciones y el for los resta
			if(c == 6){                                      // Comenzamos el condicional desde la posición menos significativa, PC9

/*El shift se está guardando en la variable k, de esta manera se está haciendo el shift c cantidad de veces hacia la izquierda al contador,
el cual va a ser un número de 7 bits siempre, con el resultado del shif se realiza una operación AND con el número 1, es decir, se evalúa
esa primera posición. Si el resultado es 1 el AND arroja un True (se enciende el LED) de lo contrario será un False (LED apagado).*/

				if (k&1){
					GPIO_WritePin(&handlerPC9, SET);
			    }else {
			    	GPIO_WritePin(&handlerPC9, CLEAR);
			    }
			}
			if(c == 5){
				if (k&1){
			    	GPIO_WritePin(&handlerPC6, SET);
			    }else {
			    	GPIO_WritePin(&handlerPC6, CLEAR);
			    }
			}
			if(c == 4){
				if (k&1){
			    	GPIO_WritePin(&handlerPB8, SET);
			    }else {
			    	GPIO_WritePin(&handlerPB8, CLEAR);
			    }
			}
			if(c == 3){
				if (k&1){
					GPIO_WritePin(&handlerPA6, SET);
				}else {
					GPIO_WritePin(&handlerPA6, CLEAR);
			    }
			}
			if(c == 2){
				if (k&1){
					GPIO_WritePin(&handlerPC7, SET);
				}else {
					GPIO_WritePin(&handlerPC7, CLEAR);
			    }
			}
			if(c == 1){
				if (k&1){
					GPIO_WritePin(&handlerPC8, SET);
			    }else {
			    	GPIO_WritePin(&handlerPC8, CLEAR);
			    }
			}
			if(c == 0){                                             // Posición más significativa
				if (k&1){
					GPIO_WritePin(&handlerPA7, SET);
			   }else {
				   GPIO_WritePin(&handlerPA7, CLEAR);
			   }
			}
		}

		if(con > 60){
			con = 1;                      // Condicional de restricción, no puede ser más de 60 ni menos de 1
		}
		if(con < 1){
			con = 60;
		}
		delay();

		USER_BUTTON = GPIO_ReadPin(&handlerPC13); // Se hace uso de la variable USER_BUTTON para leer el estado del botón y proceder a evaluar si está o no presionado

		if(USER_BUTTON == 0){                    // Dirección del conteo, evaluado con la función estado definida anteriormente.
			estado=1;
		}else {
			estado=0;
		}

		if(estado == 0){con++;}                  // Indica que si el estado es 0 hay un aumento en el contador y de lo contrario hay un decremento
		else{con--;}
	}
	return 0;

}









