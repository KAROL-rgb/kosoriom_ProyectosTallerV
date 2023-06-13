/*
 * RTCDriver.h
 *
 *  Created on: 7/06/2023
 *      Author: karol
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f4xx.h"

#define DISABLE_RTC 1
#define ENABLE_RTC	0

typedef struct
{
	uint8_t RTC_Hours;
	uint8_t RTC_Minutes;
	uint8_t RTC_Seconds;
	uint8_t RTC_Years;
	uint8_t RTC_Month;
	uint8_t RTC_Day;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t days;
	uint8_t mounths;

}RTC_Handler_t;




void RTC_Config(RTC_Handler_t *ptrHandlerRTC);

uint8_t getMinutes(RTC_Handler_t ptrHandlerRTC);
uint8_t getSeconds(RTC_Handler_t ptrHandlerRTC);
uint8_t getDate(RTC_Handler_t ptrHandlerRTC);
uint8_t getHours(RTC_Handler_t ptrHandlerRTC);
uint8_t getDays(RTC_Handler_t ptrHandlerRTC);
void DisableRTC(uint8_t disable);
void EnableRTC(uint8_t enable);

#endif /* RTCDRIVER_H_ */
