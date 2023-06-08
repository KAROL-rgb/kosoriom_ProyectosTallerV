/*
 * RTCDriver.h
 *
 *  Created on: 7/06/2023
 *      Author: karol
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f4xx.h"


typedef struct
{
	uint8_t RTC_Hours;
	uint8_t RTC_Minutes;
	uint8_t RTC_Seconds;
	uint8_t RTC_Years;
	uint8_t RTC_Mounth;
	uint8_t RTC_Day;
}RTC_Handler_t;




void RTC_Config(RTC_Handler_t *ptrHandlerRTC);

#endif /* RTCDRIVER_H_ */
