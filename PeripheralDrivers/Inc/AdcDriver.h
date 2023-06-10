/*
 * AdcDriver.h
 *
 *  Created on: 31/05/2023
 *      Author: karol
 */

#ifndef INC_ADCDRIVER_H_
#define INC_ADCDRIVER_H_

#include "stm32f4xx.h"

#define ADC_CHANNEL_0		0
#define ADC_CHANNEL_1		1
#define ADC_CHANNEL_2		2
#define ADC_CHANNEL_3		3
#define ADC_CHANNEL_4		4
#define ADC_CHANNEL_5		5
#define ADC_CHANNEL_6		6
#define ADC_CHANNEL_7		7
#define ADC_CHANNEL_8		8
#define ADC_CHANNEL_9		9
#define ADC_CHANNEL_10		10
#define ADC_CHANNEL_11		11
#define ADC_CHANNEL_12		12
#define ADC_CHANNEL_13		13
#define ADC_CHANNEL_14		14
#define ADC_CHANNEL_15		15
#define ADC_CHANNEL_16		16

#define ADC_RESOLUTION_12_BIT	0
#define ADC_RESOLUTION_10_BIT	1
#define ADC_RESOLUTION_8_BIT	2
#define ADC_RESOLUTION_6_BIT	3

#define ADC_ALIGNMENT_RIGHT		0
#define ADC_ALIGNMENT_LEFT		1

#define ADC_SAMPLING_PERIOD_3_CYCLES	0b000;
#define ADC_SAMPLING_PERIOD_15_CYCLES	0b001;
#define ADC_SAMPLING_PERIOD_28_CYCLES	0b010;
#define ADC_SAMPLING_PERIOD_56_CYCLES	0b011;
#define ADC_SAMPLING_PERIOD_84_CYCLES	0b100;
#define ADC_SAMPLING_PERIOD_112_CYCLES	0b101;
#define ADC_SAMPLING_PERIOD_144_CYCLES	0b110;
#define ADC_SAMPLING_PERIOD_480_CYCLES	0b111;

#define ADC_EVENT_DISABLE    0
#define ADC_EVENT_ENABLE     1
// Para utilizar más de un canal
#define NUMBER_CHANNELS	2

#define TIM1_CC1    1
#define TIM1_CC2    2
#define TIM1_CC3    3
#define TIM2_CC2    4
#define TIM2_CC3    5
#define TIM2_CC4    6
#define TIM2_TRGO   7
#define TIM3_CC1    8
#define TIM3_TRGO   9
#define TIM4_CC4    10
#define TIM5_CC1    11
#define TIM5_CC2    12
#define TIM5_CC3    13
#define EXTI_LINE   15

typedef struct
{
	uint8_t		channel;		    // Canal ADC que será utilizado para la conversión ADC
	uint8_t		resolution;		    // Precisión con la que el ADC hace la adquisición del dato
	uint8_t     numeroDeCanales;
	uint16_t	samplingPeriod[16];	    // Tiempo deseado para hacer la adquisición del dato
	uint8_t		dataAlignment;	    // Alineación a la izquierda o a la derecha
	uint16_t	adcData;			//Dato de la conversión
	uint16_t    adcEvent;
	uint8_t     typeEvent;
	uint8_t     multiChannels[16];
}ADC_Config_t;

void adc_Config(ADC_Config_t *adcConfig);
void configAnalogPin(uint8_t adcChannel);
void adcComplete_Callback(void);
void startSingleADC(void);
void startContinousADC(void);
uint16_t getADC(void);
void adcExternalEXTIConfig(void);
void adcTimerEventConfig(void);
// Función para ADC multicanal
void ADC_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales);
void adcExternalConfig(ADC_Config_t *adcConfig);
#endif /* INC_ADCDRIVER_H_ */

