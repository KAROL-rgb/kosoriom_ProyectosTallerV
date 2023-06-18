/**
 ******************************************************************************
 * @file           : BasicProject_Main.c
 * @author         : Karol Osorio Monsalve - kosoriom@unal.edu.co
 * @brief          : Solución básica de un proyecto con librerías externas
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

/* Definción de los elementoss del encoder*/
GPIO_Handler_t handlerSpiSCLK = { 0 };
GPIO_Handler_t handlerSpiMISO = { 0 };
GPIO_Handler_t handlerSpiMOSI = { 0 };
GPIO_Handler_t handlerSpiSS = { 0 };
BasicTimer_Handler_t handlerStateOKTimer = { 0 };
SPI_Handler_t handlerSpiPort = { 0 };

GPIO_Handler_t handlerPinTX = { 0 };
GPIO_Handler_t handlerPinRX = { 0 };
USART_Handler_t handlerCommTerminal = { 0 };
PWM_Handler_t handlerPWM = { 0 };
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t hnadlerPinPWM = { 0 };

uint32_t getCounterDelay(void);
BasicTimer_Handler_t handlerDelay = { 0 };
uint32_t counterDelay = 0;
uint8_t var = { 0 };
uint16_t memoria[8];

uint8_t col = 0;
uint8_t save = 0;
uint8_t flag = 0;

uint8_t MatrixLed_writeData(uint8_t digit, uint8_t seg);
void initSystem(void);
void traducir(int matriz1[8][8], int matriz2[8][8], int matriz3[8][8],
		int matriz4[8][8]);
void mover(int p1[1][2], int matriz1[8][8], int matriz2[8][8],
		int matriz3[8][8], int matriz4[8][8]);

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
	handlerStateOKTimer.ptrTIMx = TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerStateOKTimer.TIMx_Config.TIMx_period = 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerStateOKTimer);

	hnadlerPinPWM.pGPIOx = GPIOB;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	hnadlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode = AF1;

	GPIO_Config(&hnadlerPinPWM);

	handlerPWM.ptrTIMx = TIM2;
	handlerPWM.config.channel = PWM_CHANNEL_3;
	handlerPWM.config.duttyCicle = 33;
	handlerPWM.config.periodo = 60;
	handlerPWM.config.prescaler = 100;

	// Activar señal
	pwm_Config(&handlerPWM);
	enableOutput(&handlerPWM);
	startPwmSignal(&handlerPWM);
}

void Serial_Configuration(void) {

	/*
	 * Configurando los pines sobre los que funciona el USART
	 */
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	/*
	 * Configuramos la comunicacion serial
	 */
	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity = USART_PARITY_ODD;
	handlerCommTerminal.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX =
			USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerCommTerminal);
}

void SPI_Configuration(void) {
	// Clock
	handlerSpiSCLK.pGPIOx = GPIOB;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinAltFunMode = AF5;
	GPIO_Config(&handlerSpiSCLK);
	//MISO
	handlerSpiMISO.pGPIOx = GPIOB;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinAltFunMode = AF5;
	GPIO_Config(&handlerSpiMISO);
	//MOSI
	handlerSpiMOSI.pGPIOx = GPIOA;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinAltFunMode = AF5;
	GPIO_Config(&handlerSpiMOSI);
	//Slave slect
	handlerSpiSS.pGPIOx = GPIOA;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	GPIO_Config(&handlerSpiSS);

	//Handler SPI
	handlerSpiPort.ptrSPIx = SPI1;
	handlerSpiPort.SPI_Config.SPI_mode = SPI_MODE_0;
	handlerSpiPort.SPI_Config.SPI_fullDupplexEnable = SPI_FULL_DUPPLEX;
	handlerSpiPort.SPI_Config.SPI_datasize = SPI_DATASIZE_8_BIT;
	handlerSpiPort.SPI_Config.SPI_baudrate = SPI_BAUDRATE_FPCLK_16; // 1MHz
	handlerSpiPort.SPI_slavePin = handlerSpiSS;
	spi_config(handlerSpiPort);

	MatrixLed_writeData(0x09, 0x00); // Desactivar decodificación BCD
	MatrixLed_writeData(0x09, 0x00); // Desactivar decodificación BCD
	MatrixLed_writeData(0x09, 0x00); // Desactivar decodificación BCD
	MatrixLed_writeData(0x09, 0x00); // Desactivar decodificación BCD

	MatrixLed_writeData(0x0A, 0x00); // Brillo máximo (0x00 a 0x0F)
	MatrixLed_writeData(0x0A, 0x00); // Brillo máximo (0x00 a 0x0F)
	MatrixLed_writeData(0x0A, 0x00); // Brillo máximo (0x00 a 0x0F)
	MatrixLed_writeData(0x0A, 0x00); // Brillo máximo (0x00 a 0x0F)

	MatrixLed_writeData(0x0B, 0x07); // Configurar el número de matrices de LED (0x01 a 0x08)
	MatrixLed_writeData(0x0B, 0x07); // Configurar el número de matrices de LED (0x01 a 0x08)
	MatrixLed_writeData(0x0B, 0x07); // Configurar el número de matrices de LED (0x01 a 0x08)
	MatrixLed_writeData(0x0B, 0x07); // Configurar el número de matrices de LED (0x01 a 0x08)

	MatrixLed_writeData(0x0C, 0x01); // Encender chip
	MatrixLed_writeData(0x0C, 0x01); // Encender chip
	MatrixLed_writeData(0x0C, 0x01); // Encender chip
	MatrixLed_writeData(0x0C, 0x01); // Encender chip

	MatrixLed_writeData(0x0F, 0x00); // Desactivar prueba de visualización
	MatrixLed_writeData(0x0F, 0x00); // Desactivar prueba de visualización
	MatrixLed_writeData(0x0F, 0x00); // Desactivar prueba de visualización
	MatrixLed_writeData(0x0F, 0x00); // Desactivar prueba de visualización

}

uint8_t MatrixLed_writeData(uint8_t digit, uint8_t seg) {

	uint8_t data[2];

	// Columna de la matriz que deseamos seleccionar
	data[0] = digit;

	// Fila de la matriz que deseamos seleccionar
	data[1] = seg;
	// Activamos el esclavo
	spi_selectSlave(&handlerSpiPort);

	// Enviamos los datos
	spi_transmit(handlerSpiPort, data, 2);

	// Desactivamos el esclavo
	spi_unSelectSlave(&handlerSpiPort);
	return seg;
}

uint8_t MatrixLedMod1(uint8_t digit, uint8_t seg) {

	uint8_t data[2];
	uint8_t zero[2];

	zero[0] = 0;
	zero[1] = 0;

	// Columna de la matriz que deseamos seleccionar
	data[0] = digit;

	// Fila de la matriz que deseamos seleccionar
	data[1] = seg;
	// Activamos el esclavo
	spi_selectSlave(&handlerSpiPort);

	// Enviamos los datos NO-OP
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, data, 2);

	// Desactivamos el esclavo
	spi_unSelectSlave(&handlerSpiPort);
	return seg;
}

uint8_t MatrixLedMod2(uint8_t digit, uint8_t seg) {

	uint8_t data[2];
	uint8_t zero[2];

	zero[0] = 0;
	zero[1] = 0;

	// Columna de la matriz que deseamos seleccionar
	data[0] = digit;

	// Fila de la matriz que deseamos seleccionar
	data[1] = seg;
	// Activamos el esclavo
	spi_selectSlave(&handlerSpiPort);

	// Enviamos los datos NO-OP
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, data, 2);
	spi_transmit(handlerSpiPort, zero, 2);

	// Desactivamos el esclavo
	spi_unSelectSlave(&handlerSpiPort);
	return seg;
}

uint8_t MatrixLedMod3(uint8_t digit, uint8_t seg) {

	uint8_t data[2];
	uint8_t zero[2];

	zero[0] = 0;
	zero[1] = 0;

	// Columna de la matriz que deseamos seleccionar
	data[0] = digit;

	// Fila de la matriz que deseamos seleccionar
	data[1] = seg;
	// Activamos el esclavo
	spi_selectSlave(&handlerSpiPort);

	// Enviamos los datos NO-OP
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, data, 2);
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, zero, 2);

	// Desactivamos el esclavo
	spi_unSelectSlave(&handlerSpiPort);
	return seg;
}

uint8_t MatrixLedMod4(uint8_t digit, uint8_t seg) {

	uint8_t data[2];
	uint8_t zero[2];

	zero[0] = 0;
	zero[1] = 0;

	// Columna de la matriz que deseamos seleccionar
	data[0] = digit;

	// Fila de la matriz que deseamos seleccionar
	data[1] = seg;
	// Activamos el esclavo
	spi_selectSlave(&handlerSpiPort);

	// Enviamos los datos NO-OP
	spi_transmit(handlerSpiPort, data, 2);
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, zero, 2);
	spi_transmit(handlerSpiPort, zero, 2);

	// Desactivamos el esclavo
	spi_unSelectSlave(&handlerSpiPort);
	return seg;
}

//void delay()
//{
//  unsigned int i;
//  for(i=0; i<320000; i++);
//}

void timerDelay_Configuration(void) {

	/*
	 * Configuramos el timer
	 */
	handlerDelay.ptrTIMx = TIM5;
	handlerDelay.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerDelay.TIMx_Config.TIMx_speed = BTIMER_SPEED_10us;
	handlerDelay.TIMx_Config.TIMx_period = 100;
	BasicTimer_Config(&handlerDelay);
}
uint32_t getCounterDelay(void) {

	return counterDelay;
}

void displayNumber(uint8_t matrix, uint8_t number) {
	MatrixLed_writeData(matrix, number);
}

void clean(void) {
	MatrixLedMod1(0x01, 0b00000000);
	MatrixLedMod1(0x02, 0b00000000);
	MatrixLedMod1(0x03, 0b00000000);
	MatrixLedMod1(0x04, 0b00000000);
	MatrixLedMod1(0x05, 0b00000000);
	MatrixLedMod1(0x06, 0b00000000);
	MatrixLedMod1(0x07, 0b00000000);
	MatrixLedMod1(0x08, 0b00000000);

	MatrixLedMod2(0x01, 0b00000000);
	MatrixLedMod2(0x02, 0b00000000);
	MatrixLedMod2(0x03, 0b00000000);
	MatrixLedMod2(0x04, 0b00000000);
	MatrixLedMod2(0x05, 0b00000000);
	MatrixLedMod2(0x06, 0b00000000);
	MatrixLedMod2(0x07, 0b00000000);
	MatrixLedMod2(0x08, 0b00000000);

	MatrixLedMod3(0x01, 0b00000000);
	MatrixLedMod3(0x02, 0b00000000);
	MatrixLedMod3(0x03, 0b00000000);
	MatrixLedMod3(0x04, 0b00000000);
	MatrixLedMod3(0x05, 0b00000000);
	MatrixLedMod3(0x06, 0b00000000);
	MatrixLedMod3(0x07, 0b00000000);
	MatrixLedMod3(0x08, 0b00000000);

	MatrixLedMod4(0x01, 0b00000000);
	MatrixLedMod4(0x02, 0b00000000);
	MatrixLedMod4(0x03, 0b00000000);
	MatrixLedMod4(0x04, 0b00000000);
	MatrixLedMod4(0x05, 0b00000000);
	MatrixLedMod4(0x06, 0b00000000);
	MatrixLedMod4(0x07, 0b00000000);
	MatrixLedMod4(0x08, 0b00000000);

}
void clearMatrix() {
	// Apaga todos los LEDs de la matriz
	for (int i = 0; i < 8; i++) {
		MatrixLedMod1(0x02, 0x00);
		MatrixLedMod2(0x02, 0x00);
		MatrixLedMod3(0x02, 0x00);
		MatrixLedMod4(0x02, 0x00);
		delay_ms(100);
	}
}

int main(void) {
	int matriz1[8][8] = { 0 };
	int matriz2[8][8] = { 0 };
	int matriz3[8][8] = { 0 };
	int matriz4[8][8] = { 0 };
	matriz4[6][1] = 1;
	matriz4[6][2] = 1;
	matriz4[7][1] = 1;
	matriz4[7][2] = 1;
	initSystem();
	config_SysTick_ms(0);
	SPI_Configuration();
	Serial_Configuration();
	clean();
	clearMatrix();

//	const char* binary = "11011000";
//	char* hex = binaryToHex(binary);
//    free(hex);  // Remember to free the allocated memory

//	writeModule3(0x02,0xFF);
//	MatrixLedMod3(0x02, 0x01);
//	MatrixLedMod4(0x02, 0x01);
//	MatrixLedMod2(0x02, 0x01);
	//+++++++++++++++++++++
//	MatrixLedMod1(0x01, 0x31);
//	MatrixLedMod1(0x02, 0x33);
//	MatrixLedMod1(0x03, 0x01);
//	MatrixLedMod1(0x04, 0x74);
//	MatrixLedMod1(0x05, 0x47);
//	MatrixLedMod1(0x06, 0x88);
//	MatrixLedMod1(0x07, 0xD8);
//	MatrixLedMod1(0x08, 0x50);
	// Para mostrar la T
//	MatrixLedMod1(0x03, 0x01);
//	MatrixLedMod1(0x04, 0x03);
//	MatrixLedMod1(0x05, 0x01);

	// Para mostrar la L izquierda
//	MatrixLedMod1(0x03, 0x04);
//	MatrixLedMod1(0x04, 0x07);

	// Para mostrar la L derecha
//	MatrixLedMod1(0x04, 0x04);
//	MatrixLedMod1(0x03, 0x07);

//	// Para mostrar la S hacia la izquierda
//	MatrixLedMod1(0x02, 0x01);
//	MatrixLedMod1(0x03, 0x03);
//	MatrixLedMod1(0x04, 0x02);

	// Para mostrar S hacia la derecha
//	MatrixLedMod1(0x02, 0x02);
//	MatrixLedMod1(0x03, 0x03);
//	MatrixLedMod1(0x04, 0x01);

	// Para mostrar cuadrado
//	MatrixLedMod1(0x02, 0x03);
//	MatrixLedMod1(0x03, 0x03);

//	// Para mostrar |
//	MatrixLedMod1(0x02, 0x0F<<1);

	// Para mostrar un punto
//	MatrixLedMod1(0x03, 0x01<<7);

//	MatrixLedMod1(0x02, 0x01<<7);
//	uint8_t col =  0x01<<7;
//	uint8_t a = MatrixLedMod1(0x02, 0x01<<7);
	int punto[1][2];
	punto[0][0] = -1;
	punto[0][1] = 1;
//	for (int i; i < 8; i++) {
		//save = MatrixLedMod4(0x02, (uint8_t)col);
		mover(punto, matriz1, matriz2, matriz3, matriz4);

//		i++;
//	}

//	}

	while (1) {

	}
	return 0;
}
void mover(int punto[1][2], int matriz1[8][8], int matriz2[8][8],
	int matriz3[8][8], int matriz4[8][8]) {
	int p1[1][2];
	p1[0][0] = punto[0][0];
	p1[0][1] = punto[0][1];
	int p2[1][2];
	p2[0][0] = punto[0][0];
	p2[0][1] = punto[0][1];
	int p3[1][2];
	p3[0][0] = punto[0][0];
	p3[0][1] = punto[0][1];
	int p4[1][2];
	p4[0][0] = punto[0][0];
	p4[0][1] = punto[0][1];
	for (int i = 0; i < 32; i++) {
		if (i < 8) {
			if (matriz1[p1[0][0] + 1][p1[0][1]] == 0) {
				if (p1[0][0] >= 0) {
					matriz1[p1[0][0]][p1[0][1]] = 0;
					matriz1[p1[0][0] + 1][p1[0][1]] = 1;
					p1[0][0] = p1[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				} else {
					matriz1[p1[0][0] + 1][p1[0][1]] = 1;
					p1[0][0] = p1[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				}
			}
		}
		if (i >= 8 && i < 16) {
			matriz1[p1[0][0]][p1[0][1]] = 0;
			if (matriz2[p2[0][0] + 1][p2[0][1]] == 0) {
				if (p2[0][0] >= 0) {
					matriz2[p2[0][0]][p2[0][1]] = 0;
					matriz2[p2[0][0] + 1][p2[0][1]] = 1;
					p2[0][0] = p2[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				} else {
					matriz2[p2[0][0] + 1][p2[0][1]] = 1;
					p2[0][0] = p2[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				}
			}
		}
		if (i >= 16 && i < 24) {
			matriz2[p2[0][0]][p2[0][1]] = 0;
			if (matriz3[p3[0][0] + 1][p3[0][1]] == 0) {
				if (p3[0][0] >= 0) {
					matriz3[p3[0][0]][p3[0][1]] = 0;
					matriz3[p3[0][0] + 1][p3[0][1]] = 1;
					p3[0][0] = p3[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				} else {
					matriz3[p3[0][0] + 1][p3[0][1]] = 1;
					p3[0][0] = p3[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				}
			}
		}
		if (i >= 24 && i < 32) {
			matriz3[p3[0][0]][p3[0][1]] = 0;
			if (matriz4[p4[0][0] + 1][p4[0][1]] == 0) {
				if (p4[0][0] >= 0) {
					matriz4[p4[0][0]][p4[0][1]] = 0;
					matriz4[p4[0][0] + 1][p4[0][1]] = 1;
					p4[0][0] = p4[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				} else {
					matriz4[p4[0][0] + 1][p4[0][1]] = 1;
					p4[0][0] = p4[0][0] + 1;
					delay_ms(1000);
					traducir(matriz1, matriz2, matriz3, matriz4);
				}
			}

		}
	}

}
void traducir(int matriz1[8][8], int matriz2[8][8], int matriz3[8][8],
		int matriz4[8][8]) {

	for (int i = 0; i < 8; i++) {
		uint8_t aux = 0;
		for (int j = 0; j < 8; j++) {
			aux |= (matriz1[j][i] << j);
		}
		MatrixLedMod1(i + 1, aux);
	}
	for (int i = 0; i < 8; i++) {
		uint8_t aux = 0;
		for (int j = 0; j < 8; j++) {
			aux |= (matriz2[j][i] << j);
		}
		MatrixLedMod2(i + 1, aux);
	}
	for (int i = 0; i < 8; i++) {
		uint8_t aux = 0;
		for (int j = 0; j < 8; j++) {
			aux |= (matriz3[j][i] << j);
		}
		MatrixLedMod3(i + 1, aux);
	}
	for (int i = 0; i < 8; i++) {
		uint8_t aux = 0;
		for (int j = 0; j < 8; j++) {
			aux |= (matriz4[j][i] << j);
		}
		MatrixLedMod4(i + 1, aux);
	}
}
void BasicTimer2_Callback(void) {
	//Hacemos uduttyn blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);

}

