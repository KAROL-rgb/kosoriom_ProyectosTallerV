/*
 * configExti_Main.c
 *
 *  Created on: 23/03/2023
 *      Author: karol
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

BasicTimer_Handler_t handlerBlinkyTimer = {0}; //TIM2
//Definición del exti
EXTI_Config_t extiCLOCK = {0};

uint32_t counterExti7 = 0; //Bajo la bandera
uint8_t DT = 0;
uint8_t CLK = 0;
uint8_t contador = 0;
uint8_t j = 0;


/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_extInt7(void);


int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();


	//Leemos los pines

	contador = 0;
	//Leemos los pines
	while(1){
		CLK = GPIO_ReadPin(&handlerCLK);
		DT = GPIO_ReadPin(&handlerDT);
		if(counterExti7==1 && DT==0 ){
			contador++;
			counterExti7=0;
		}else if(counterExti7==1 && DT==0){

			contador--;
			counterExti7=0;

		//}else{
		//	contador = 10;
		}


	}


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


	/* Config TIM2 para encender cada 300 ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 300; // Interrupcion cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Config botón SW*/

//	handlerUserButtonSW.pGPIOx = GPIOC;
//	handlerUserButtonSW.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
//	handlerUserButtonSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
//	handlerUserButtonSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerUserButtonSW);

	//Cargamos la configuración del EXTI
	extiCLOCK.pGPIOHandler = &handlerCLK;
	extiCLOCK.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiCLOCK);





}
void callback_extInt7(void){ //cLOCK
	counterExti7 = 1; //Flanco de subida

	j++;
}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}



