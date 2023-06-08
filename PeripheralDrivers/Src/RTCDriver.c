/*
 * RTCDriver.c
 *
 *  Created on: 7/06/2023
 *      Author: karol
 */

#include "RTCDriver.h"


void RTC_Config(RTC_Handler_t *ptrHandlerRTC){
	/* Activar PWR para quitar los dominios de respaldo */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->CR |= PWR_CR_DSP;            // Deshabilitar la protección de escritura del dominio de respaldo y acceso a RTC Backup registers

	/* Activar el LSE */
	RCC->BDCR |= RCC_BDCR_LSEON;
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP();
	}
	RCC->BDCR |= (0b01 << RCC_BDCR_RTCSEL_Pos);
	RCC->BDCR |= RCC_BDCR_RTCEN; // Activamos el RTC Clock

	/* Habilitamos el registro de escritura escribiendo una clave en el registro de protección de escritura */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	/* Inicialización y configuración del calendario */
	RTC->ISR |= RTC_ISR_INIT;

	// Activamos las actualizaciones del calendario
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}
	RTC->CR |= RTC_CR_BYPSHAD;

	RTC->DR = 0;      // Calendar date shadow register
	RTC->TR = 0;      // Calendar time shadow register.

	/* Configuración para la hora */
	if(ptrHandlerRTC->RTC_Hours <= 24){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Hours)/10) << RTC_TR_HT_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Hours)%10) << RTC_TR_HU_Pos);
	}

	if(ptrHandlerRTC->RTC_Minutes <= 60){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Minutes)/10) << RTC_TR_MNT_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Minutes)%10) << RTC_TR_MNU_Pos);

	}

	if(ptrHandlerRTC->RTC_Seconds <= 60){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Seconds)/10) << RTC_TR_ST_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Seconds)%10) << RTC_TR_SU_Pos);
	}

	/* Configuración para la fecha */
	RTC-> DR |= (((ptrHandlerRTC->RTC_Years)/10) << RTC_DR_YT_Pos);
	RTC-> DR |= (((ptrHandlerRTC->RTC_Years)%10) << RTC_DR_YU_Pos);

	if(ptrHandlerRTC->RTC_Mounth <= 12){
		RTC->DR |= (((ptrHandlerRTC->RTC_Mounth)/10) << RTC_DR_MT_Pos);
		RTC->DR |= (((ptrHandlerRTC->RTC_Mounth)%10) << RTC_DR_MU_Pos);
	}

	if(ptrHandlerRTC->RTC_Day <= 31){
		RTC->DR |= (((ptrHandlerRTC->RTC_Day)/10) << RTC_DR_DT_Pos);
		RTC->DR |= (((ptrHandlerRTC->RTC_Day)%10) << RTC_DR_DU_Pos);
	}

	/* Salimos del modo de inicialización */
	RCC->BDCR |= RCC_BDCR_RTCEN;
	RCC->ISR &= ~RTC_ISR_INIT;
	PWR->CR &= ~PWR_CR_DBP;

}
