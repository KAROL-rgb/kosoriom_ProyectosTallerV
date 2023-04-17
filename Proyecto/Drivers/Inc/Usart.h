/*
 * Usart.h
 *
 *  Created on: 15/04/2023
 *      Author: karol
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "stm32f4xx.h"

#define USART_CR1
#define USART_BRR
#define USART_TDR
#define USART_RDR

#define USART                    USARTx
#define USART_CLK                RCC_APB1Periph_USART2
#define USART_TX_PIN             GPIO_Pin_2
#define USART_TX_GPIO_PORT       GPIOA
#define USART_TX_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define USART_TX_SOURCE          GPIO_PinSource2
#define USART_TX_AF              GPIO_AF_USART2
#define USART_RX_PIN             GPIO_Pin_3
#define USART_RX_GPIO_PORT       GPIOA
#define USART_RX_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define USART_RX_SOURCE          GPIO_PinSource3
#define USART_RX_AF              GPIO_AF_USART2
#define USART_BAUDRATE           115200
/*
 * Configuración de la estructura para el periférico USART
*/
typedef struct
{
	uint8_t USART_Mode;
	uint32_t USART_Baud;
	uint8_t USART_NoOfStopBits;
	uint8_t USART_WordLength;
	uint8_t USART_ParityControl;
	uint8_t USART_HWFlowControl;
}USART_Config_t;

/* Handler para el USART */
typedef struct
{
	USART_TypeDef              *ptrUSARTx;
	USART_Config_t              USARTx_Config;
}USART_Handler_t;



// Function prototypes
void USART_Init(void);
void USART_SendChar(uint8_t ch);
void USART_SendString(char* str);
uint8_t USART_ReceiveChar(void);
void USART_ReceiveString(char* str, uint8_t max_len);

void USART_Config(USART_Handler_t *ptrUsartHandler);



#endif /* INC_USART_H_ */
