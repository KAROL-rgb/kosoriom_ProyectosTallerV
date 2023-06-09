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
	RCC->CR |= PWR_CR_DBP;            // Deshabilitar la protección de escritura del dominio de respaldo y acceso a RTC Backup registers

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
	if(ptrHandlerRTC->RTC_Hours < 24){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Hours)/10) << RTC_TR_HT_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Hours)%10) << RTC_TR_HU_Pos);
	}

	if(ptrHandlerRTC->RTC_Minutes < 60){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Minutes)/10) << RTC_TR_MNT_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Minutes)%10) << RTC_TR_MNU_Pos);

	}

	if(ptrHandlerRTC->RTC_Seconds < 60){
		RTC -> TR |= (((ptrHandlerRTC->RTC_Seconds)/10) << RTC_TR_ST_Pos);
		RTC -> TR |= (((ptrHandlerRTC->RTC_Seconds)%10) << RTC_TR_SU_Pos);
	}

	/* Configuración para la fecha */
	RTC-> DR |= (((ptrHandlerRTC->RTC_Years)/10) << RTC_DR_YT_Pos);
	RTC-> DR |= (((ptrHandlerRTC->RTC_Years)%10) << RTC_DR_YU_Pos);

	if(ptrHandlerRTC->RTC_Mounth <= 12 && ptrHandlerRTC->RTC_Mounth >= 1){
		RTC->DR |= (((ptrHandlerRTC->RTC_Mounth)/10) << RTC_DR_MT_Pos);
		RTC->DR |= (((ptrHandlerRTC->RTC_Mounth)%10) << RTC_DR_MU_Pos);
	}

	if(ptrHandlerRTC->RTC_Day <= 31 && ptrHandlerRTC->RTC_Day >=1){
		RTC->DR |= (((ptrHandlerRTC->RTC_Day)/10) << RTC_DR_DT_Pos);
		RTC->DR |= (((ptrHandlerRTC->RTC_Day)%10) << RTC_DR_DU_Pos);
	}

	/* Salimos del modo de inicialización */
	RCC->BDCR |= RCC_BDCR_RTCEN;
	RTC->ISR &= ~RTC_ISR_INIT;
	PWR->CR &= ~PWR_CR_DBP;
}
uint8_t getDays(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint8_t dataTens = 0;
	uint8_t dataUnits = 0;

	dataTens = RTC->DR & RTC_DR_DT;
	dataTens >>= RTC_DR_DT_Pos;

	dataUnits = RTC->DR & RTC_DR_DU;
	dataUnits >>= RTC_DR_DU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;

}
uint8_t getHours(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint8_t dataTens = 0;
	uint8_t dataUnits = 0;

	dataTens = RTC->TR & RTC_TR_HT;
	dataTens >>= RTC_TR_HT_Pos;

	dataUnits = RTC->TR & RTC_TR_HU;
	dataUnits >>= RTC_TR_HU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}
uint8_t getMounths(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint8_t dataTens = 0;
	uint8_t dataUnits = 0;

	dataTens = RTC->DR & RTC_DR_MT;
	dataTens >>= RTC_DR_MT_Pos;

	dataUnits = RTC->DR & RTC_DR_MU;
	dataUnits >>= RTC_DR_MU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}
uint8_t getSeconds(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint8_t dataTens = 0;
	uint8_t dataUnits = 0;

	dataTens = RTC->TR & RTC_TR_ST;
	dataTens >>= RTC_TR_ST_Pos;

	dataUnits = RTC->TR & RTC_TR_SU;
	dataUnits >>= RTC_TR_SU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}
uint8_t getMinutes(RTC_Handler_t ptrHandlerRTC){
	uint8_t data = 0;
	uint8_t dataTens = 0;
	uint8_t dataUnits = 0;

	dataTens = RTC->TR & RTC_TR_MNT;
	dataTens >>= RTC_TR_MNT_Pos;

	dataUnits = RTC->TR & RTC_TR_MNU;
	dataUnits >>= RTC_TR_MNU_Pos;

	data = (dataTens*10) + dataUnits;

	return data;
}








