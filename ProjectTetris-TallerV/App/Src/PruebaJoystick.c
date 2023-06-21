/*
 * PruebaJoystick.c
 *
 *  Created on: 20/06/2023
 *      Author: karol
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "SPIxDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "string.h"
#include "SysTickDriver.h"
#include "PwmDriver.h"
#include <time.h>
#include <math.h>
#include "AdcDriver.h"

BasicTimer_Handler_t handlerStateOKTimer = { 0 };
PWM_Handler_t handlerPWM = { 0 };
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t hnadlerPinPWM = { 0 };
ADC_Config_t adcConfig                    = {0};
EXTI_Config_t extiButton = {0};     // Definición del exti el botón del encoder
uint8_t estado = 1;
GPIO_Handler_t handlerSW = {0}; // PA7 (Conecta al SW)
int joystick[2] = { 0 };
int axisX = 0;
int axisY = 0;
int SW    = 0;
uint16_t adcData = 0;
uint8_t aux = 0;
uint8_t adcIsComplete = RESET;
uint32_t counterButton = 0;

void initSystem(void) {
//	config_SysTick_ms(200);
	handlerLedOK.pGPIOx = GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLedOK);
	//Llevamos el LED a un estado de encendido (ON o SET)
	GPIO_WritePin(&handlerLedOK, SET);

	/*Configurando el timer*/
	handlerStateOKTimer.ptrTIMx = TIM5;
	handlerStateOKTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerStateOKTimer.TIMx_Config.TIMx_period = 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerStateOKTimer);

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

	hnadlerPinPWM.pGPIOx = GPIOB;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode = AF1;

	GPIO_Config(&hnadlerPinPWM);

	/* Configuración del botón, Pin PA9 (SW) */
	handlerSW.pGPIOx = GPIOA;
	handlerSW.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSW);

	handlerPWM.ptrTIMx = TIM2;
	handlerPWM.config.channel = PWM_CHANNEL_3;
	handlerPWM.config.duttyCicle = 33;
	handlerPWM.config.periodo = 60;
	handlerPWM.config.prescaler = 100;

	// Activar señal
	pwm_Config(&handlerPWM);
	enableOutput(&handlerPWM);
	startPwmSignal(&handlerPWM);

	/* Se carga la configuración del EXTI del botón */
	extiButton.pGPIOHandler = &handlerSW;
	extiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiButton);
}

int main(void){

	initSystem();

	while(1){
		if(counterButton==1){                    // Interrupción externa del exti del botón SW
			estado++;
		}
		counterButton = 0;

		axisX = joystick[0]-2030;
		axisY = joystick[1]-2024;
		if (adcIsComplete == SET){

//			sprintf(bufferData, "ADC X = %d Y = %d \n\r", (int) axisX, (int) axisY);
//			writeMsg(&handlerUsart2, bufferData);
			adcIsComplete = RESET;

	    }
	}
	return 0;
}

void adcComplete_Callback(void){
	adcData = getADC();
	aux++;
	// Leemos los valores de la conversión ADC
	if (aux == 1) {
		joystick[0] = adcData;
	}
	else if (aux == 2){
		joystick[1] = adcData;
		adcIsComplete = SET;
		aux = 0;
	}
	else{
		__NOP();
	}

}

void BasicTimer5_Callback(void) {
	//Hacemos uduttyn blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);
}

void callback_extInt9(void){                     // Exti del botón
	counterButton |= 1;	                          // Flanco de subida
}
