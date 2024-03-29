/*
 * RTCDriver.c
 *
 *  Created on: 7/06/2023
 *      Author: karol
 */

#include "RTCDriver.h"

uint8_t data = 0;
uint8_t dataTens = 0;
uint8_t dataUnits = 0;

void RTC_Config(RTC_Handler_t *ptrHandlerRTC){
	/* Activar PWR para quitar los dominios de respaldo */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;            // Deshabilitar la protección de escritura del dominio de respaldo y acceso a RTC Backup registers

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
	RTC->PRER |= (127 << RTC_PRER_PREDIV_A_Pos);
	RTC->PRER |= (255 << RTC_PRER_PREDIV_S_Pos);
	RCC->BDCR &= ~RCC_BDCR_RTCEN;

	RTC->CR |= RTC_CR_BYPSHAD;

	RTC->DR = 0;      // Calendar date shadow register
	RTC->TR = 0;      // Calendar time shadow register.

	/* Configuración para la hora */
	if(ptrHandlerRTC->RTC_Hours <= 23){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Hours)/10) << RTC_TR_HT_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Hours)%10) << RTC_TR_HU_Pos);
	}

	if(ptrHandlerRTC->RTC_Minutes <= 59){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Minutes)/10) << RTC_TR_MNT_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Minutes)%10) << RTC_TR_MNU_Pos);

	}

	if(ptrHandlerRTC->RTC_Seconds <= 59){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Seconds)/10) << RTC_TR_ST_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Seconds)%10) << RTC_TR_SU_Pos);
	}

	/* Configuración para la fecha */
	RTC-> DR |= (((ptrHandlerRTC->RTC_Years)/10) << RTC_DR_YT_Pos);
	RTC-> DR |= (((ptrHandlerRTC->RTC_Years)%10) << RTC_DR_YU_Pos);

	if(ptrHandlerRTC->RTC_Month <= 12){
		RTC->DR |= (((ptrHandlerRTC->RTC_Month)/10) << RTC_DR_MT_Pos);
		RTC->DR |= (((ptrHandlerRTC->RTC_Month)%10) << RTC_DR_MU_Pos);
	}

	if(ptrHandlerRTC->RTC_Day <= 31){
		RTC->DR |= (((ptrHandlerRTC->RTC_Day)/10) << RTC_DR_DT_Pos);
		RTC->DR |= (((ptrHandlerRTC->RTC_Day)%10) << RTC_DR_DU_Pos);
	}

	/* Salimos del modo de inicialización */
	RCC->BDCR |= RCC_BDCR_RTCEN;
	RTC->ISR &= ~RTC_ISR_INIT;
	PWR->CR &= ~PWR_CR_DBP;

	RTC->CR &= ~RTC_CR_BYPSHAD;
	RTC->WPR = (0xFF);
}
uint8_t getDays(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint32_t dataTens = 0;
	uint32_t dataUnits = 0;

	dataTens = RTC->DR & RTC_DR_DT_Msk;
	dataTens >>= RTC_DR_DT_Pos;

	dataUnits = RTC->DR & RTC_DR_DU_Msk;
	dataUnits >>= RTC_DR_DU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;

}
uint8_t getHours(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint32_t dataTens = 0;
	uint32_t dataUnits = 0;

	dataTens = RTC->TR & RTC_TR_HT_Msk;
	dataTens >>= RTC_TR_HT_Pos;

	dataUnits = RTC->TR & RTC_TR_HU_Msk;
	dataUnits >>= RTC_TR_HU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}
uint8_t getDate(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint32_t dataTens = 0;
	uint32_t dataUnits = 0;

	dataTens = RTC->DR & RTC_DR_MT_Msk;
	dataTens >>= RTC_DR_MT_Pos;

	dataUnits = RTC->DR & RTC_DR_MU_Msk;
	dataUnits >>= RTC_DR_MU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}
uint8_t getSeconds(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint8_t dataTens = 0;
	uint8_t dataUnits = 0;

	dataTens = RTC->TR & RTC_TR_ST_Msk;
	dataTens >>= RTC_TR_ST_Pos;

	dataUnits = RTC->TR & RTC_TR_SU_Msk;
	dataUnits >>= RTC_TR_SU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}
uint8_t getMinutes(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint32_t dataTens = 0;
	uint32_t dataUnits = 0;

	dataTens = RTC->TR & RTC_TR_MNT_Msk;
	dataTens >>= RTC_TR_MNT_Pos;

	dataUnits = RTC->TR & RTC_TR_MNU_Msk;
	dataUnits >>= RTC_TR_MNU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}








