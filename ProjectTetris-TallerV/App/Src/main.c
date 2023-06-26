/*
 * mainLibreria.c
 *
 *  Created on: 23/06/2023
 *      Author: karol
 */
/**
 ******************************************************************************
 * @file           : BasicProject_Main.c
 * @author         : Karol Osorio Monsalve - kosoriom@unal.edu.co
 * @brief          : Código principal Proyecto TallerV-Tetris

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
#include <time.h>
#include <math.h>
#include "AdcDriver.h"
#include "Tetris.h"

GPIO_Handler_t handlerSpiSCLK = { 0 };
GPIO_Handler_t handlerSpiMISO = { 0 };
GPIO_Handler_t handlerSpiMOSI = { 0 };
GPIO_Handler_t handlerSpiSS = { 0 };
BasicTimer_Handler_t handlerStateOKTimer = { 0 };
BasicTimer_Handler_t handlerTimerRandom = { 0 };
SPI_Handler_t handlerSpiPort = { 0 };
/***************************************** Definiciones para ADC *****************************************/
ADC_Config_t adcConfig = { 0 };
int joystick[2] = { 0 };
int axisX = 0;
int axisY = 0;
int SW = 0;
uint16_t adcData = 0;
uint8_t aux = 0;
uint8_t adcIsComplete = RESET;

GPIO_Handler_t handlerPinTX = { 0 };
GPIO_Handler_t handlerPinRX = { 0 };
USART_Handler_t handlerCommTerminal = { 0 };
PWM_Handler_t handlerPWM = { 0 };
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t hnadlerPinPWM = { 0 };
GPIO_Handler_t handlerSW = { 0 };
EXTI_Config_t extiButton = { 0 };

uint32_t getCounterDelay(void);
BasicTimer_Handler_t handlerDelay = { 0 };
uint32_t counterDelay = 0;
uint8_t var = { 0 };
uint16_t memoria[8];

uint8_t myVariable = 42;
uint8_t *ptr = &myVariable;
uint16_t randomSeed;
uint16_t randomNumber;

uint8_t estado;
uint8_t puntos;
uint8_t giro;
uint32_t counterButton = 0;

uint8_t col = 0;
uint8_t save = 0;
uint8_t flag = 0;
uint8_t rxData = 0; // Para valores random
char bufferData[128] = { 0 };
void tetris(int matriz[32][8], uint8_t fila);
int test = 0;
/* Definición de las funciones del programa a utilizar */
void inicio(int matriz[32][8]);
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

	adcConfig.multiChannels[0] = ADC_CHANNEL_0;
	adcConfig.multiChannels[1] = ADC_CHANNEL_1;
	adcConfig.dataAlignment = ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution = ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod = ADC_SAMPLING_PERIOD_84_CYCLES
	;
	adcConfig.adcEvent = 5;
	adcConfig.typeEvent = 1;
	//Se carga la configuración del ADC
	ADC_ConfigMultichannel(&adcConfig, 2);
	adcExternalConfig(&adcConfig);

	/* Timer para generar valores aleatorios */
	handlerTimerRandom.ptrTIMx = TIM3;
	handlerTimerRandom.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerRandom.TIMx_Config.TIMx_speed = 2;
	handlerTimerRandom.TIMx_Config.TIMx_period = 65530;
	handlerTimerRandom.TIMx_Config.TIMx_interruptEnable = 0;

	BasicTimer_Config(&handlerTimerRandom);

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

	/* Se carga la configuración del EXTI del botón */
	extiButton.pGPIOHandler = &handlerSW;
	extiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiButton);

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
	handlerCommTerminal.USART_Config.USART_parity = USART_PARITY_NONE;
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

int main(void) {


	initSystem();
	config_SysTick_ms(0);
	SPI_Configuration();
	Serial_Configuration();
	clean();
	adcComplete_Callback();
	srand(randomSeed);

	int matriz[32][8] = { 0 };
	inicio(matriz);
	puntos = 0;
	//delay
	int punto1[1][2];
	int punto2[1][2];
	int punto3[1][2];
	int punto4[1][2];

	matriz[30][3] = 1;
	matriz[30][4] = 1;
	matriz[31][3] = 1;
	matriz[31][4] = 1;

//	traducir(matriz);


	while (1) {
		//Tetris

		for (int i = 31; i >= 0; i--) {
			if (matriz[i][0] == 1 && matriz[i][1] == 1 && matriz[i][2] == 1
					&& matriz[i][3] == 1 && matriz[i][4] == 1
					&& matriz[i][5] == 1 && matriz[i][6] == 1
					&& matriz[i][7] == 1) {
				tetris(matriz, i);
				puntos += 1;
				traducir(matriz);
				delay_ms(500);
			}
		}

		//Caida de piezas, giros y movimientos laterales
		if (rxData != '\0') {
			writeChar(&handlerCommTerminal, rxData);
			switch (rxData) {
			case 'k': {

				sprintf(bufferData, "puntos = %u \n", puntos);
				writeMsg(&handlerCommTerminal, "score \n");

				break;
			}
			case 'r': {
				randomSeed = 0x1F & (TIM3->CNT);
				srand(randomSeed);
				(void) rand();
				randomNumber = rand();
				sprintf(bufferData, "randomNumber = %u \n", randomNumber);
				writeMsg(&handlerCommTerminal, bufferData);
				break;
			}
			}
			rxData = '\0';
		}

		int figura = rand() % 7;
		if (figura == 0) { //Cae T
			estado = 0;
			punto1[0][0] = -1;
			punto1[0][1] = 3;
			punto2[0][0] = -2;
			punto2[0][1] = 2;
			punto3[0][0] = -2;
			punto3[0][1] = 4;
			punto4[0][0] = -2;
			punto4[0][1] = 3;
			while (1) {
				if (counterButton == 1) { // Interrupción externa del exti del botón SW
					estado++;
					giro = 1;
				}
				counterButton = 0;
				if (punto1[0][0] >= 31 || punto2[0][0] >= 31
						|| punto3[0][0] >= 31 || punto4[0][0] >= 31) {
					break;
				}
				if (estado == 0 || punto1[0][0] <= 1) {
					if (joystick[1] < 100 && punto2[0][1] > 0
							&& punto1[0][0] > 1) {
						if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
								&& (matriz[punto2[0][0]][punto2[0][1] - 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] - 1;
							punto2[0][1] = punto2[0][1] - 1;
							punto3[0][1] = punto3[0][1] - 1;
							punto4[0][1] = punto4[0][1] - 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}
					}
					/*  Movimiento joystick hacia el lado derecho de la matriz */
					if (joystick[1] > 4000 && punto3[0][1] < 7
							&& punto1[0][0] > 1) {
						if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] + 1;
							punto2[0][1] = punto2[0][1] + 1;
							punto3[0][1] = punto3[0][1] + 1;
							punto4[0][1] = punto4[0][1] + 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}

					} else {
						int test = movimientoT(punto1, punto2, punto3, punto4,
								matriz);
						if (test == 1) {
							delay_ms(200);
							traducir(matriz);
						} else {
							break;
						}
					}
					estado = 0;
				} else if (estado == 1 && punto1[0][0] > 1) {
					if (giro == 1
							&& matriz[punto4[0][0] - 1][punto4[0][1]] == 0) {
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						punto3[0][0] = punto4[0][0];
						punto3[0][1] = punto4[0][1];
						punto4[0][0] = punto4[0][0] - 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto2[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto2[0][0]][punto2[0][1] - 1]
											== 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto1[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
									&& (matriz[punto3[0][0]][punto3[0][1] + 1]
											== 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoTGiro1(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 2 && punto1[0][0] > 1
						&& punto1[0][1] < 7) {
					if (giro == 1
							&& matriz[punto3[0][0]][punto3[0][1] + 1] == 0) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						punto1[0][0] = punto2[0][0];
						punto1[0][1] = punto2[0][1];
						punto2[0][0] = punto3[0][0];
						punto2[0][1] = punto3[0][1];
						punto3[0][1] = punto3[0][1] + 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto3[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoTGiro2(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 3 && punto1[0][0] > 1) {
					if (giro == 1
							&& matriz[punto2[0][0] + 1][punto2[0][1]] == 0) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						punto1[0][0] = punto1[0][0] + 1;
						punto1[0][1] = punto1[0][1] + 1;
						punto2[0][1] = punto2[0][1] + 1;
						punto3[0][1] = punto3[0][1] - 1;
						matriz[punto1[0][0]][punto1[0][1]] = 1;
						traducir(matriz);
						delay_ms(200);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto3[0][0]][punto3[0][1] - 1]
											== 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto2[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
									&& (matriz[punto2[0][0]][punto2[0][1] + 1]
											== 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoTGiro3(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 4 && punto1[0][0] > 1) {
					if (giro == 1
							&& matriz[punto3[0][0]][punto3[0][1] - 1] == 0) {
						matriz[punto4[0][0]][punto4[0][1]] = 0;
						punto2[0][1] = punto2[0][1] - 2;
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][0] = punto4[0][0] + 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						traducir(matriz);
						delay_ms(200);
						giro = 0;
					}
					if (giro == 0) {
						estado = 0;
					} else {
						estado--;
					}
				} else {
					estado--;
					giro = 0;
				}
			}
		}

		if (figura == 1) { //Cae Palito
			estado = 0;
			punto1[0][0] = -1;
			punto1[0][1] = 3;
			punto2[0][0] = -3;
			punto2[0][1] = 3;
			punto3[0][0] = -4;
			punto3[0][1] = 3;
			punto4[0][0] = -5;
			punto4[0][1] = 3;
			while (1) {
				if (counterButton == 1) { // Interrupción externa del exti del botón SW
					estado++;
					giro = 1;
				}
				counterButton = 0;
				if (punto1[0][0] >= 31 || punto2[0][0] >= 31
						|| punto3[0][0] >= 31 || punto4[0][0] >= 31) {
					break;
				} else if (estado == 0 || punto1[0][0] <= 2) {
					if (joystick[1] < 100 && punto1[0][1] > 0
							&& punto1[0][0] > 2) {
						if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
								&& (matriz[punto2[0][0]][punto2[0][1] - 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] - 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] - 1;
							punto2[0][1] = punto2[0][1] - 1;
							punto3[0][1] = punto3[0][1] - 1;
							punto4[0][1] = punto4[0][1] - 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}
					}
					/*  Movimiento joystick hacia el lado derecho de la matriz */
					if (joystick[1] > 4000 && punto4[0][1] < 7) {
						if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
								&& (matriz[punto2[0][0]][punto2[0][1] + 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] + 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] + 1;
							punto2[0][1] = punto2[0][1] + 1;
							punto3[0][1] = punto3[0][1] + 1;
							punto4[0][1] = punto4[0][1] + 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}

					} else {
						int test = movimientoPalito(punto1, punto2, punto3,
								punto4, matriz);
						if (test == 1) {
							delay_ms(200);
							traducir(matriz);
						} else {
							break;
						}
					}
					estado = 0;
				} else if (estado == 1 && punto1[0][0] > 2
						&& punto1[0][1] < 5) {
					if (giro == 1
							&& (matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
							&& (matriz[punto1[0][0]][punto1[0][1] + 2] == 0)
							&& (matriz[punto1[0][0]][punto1[0][1] + 3] == 0)) {
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto2[0][0] = punto1[0][0];
						punto2[0][1] = punto1[0][1] + 1;
						punto3[0][0] = punto1[0][0];
						punto3[0][1] = punto1[0][1] + 2;
						punto4[0][0] = punto1[0][0];
						punto4[0][1] = punto1[0][1] + 3;

						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0) {
							if (matriz[punto1[0][0]][punto1[0][1] - 1] == 0) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto4[0][1] < 7) {
							if (matriz[punto4[0][0]][punto4[0][1] + 1] == 0) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoPalitoGiro(punto1, punto2,
									punto3, punto4, matriz);
							if (test == 1) {
								delay_ms(500);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 2 && punto1[0][0] > 2) {
					if (giro == 1
							&& (matriz[punto1[0][0] - 1][punto1[0][1]] == 0)
							&& (matriz[punto1[0][0] - 2][punto1[0][1]] == 0)
							&& (matriz[punto1[0][0] - 3][punto1[0][1]] == 0)) {
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto2[0][0] = punto1[0][0] - 1;
						punto2[0][1] = punto1[0][1];
						punto3[0][0] = punto1[0][0] - 2;
						punto3[0][1] = punto1[0][1];
						punto4[0][0] = punto1[0][0] - 3;
						punto4[0][1] = punto1[0][1];

						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						estado = 0;
					} else {
						estado--;
					}
				} else {
					estado--;
				}
			}

		}
		if (figura == 2) { //Cae L invertida
			estado = 0;
			punto1[0][0] = -1;
			punto1[0][1] = 3;
			punto2[0][0] = -1;
			punto2[0][1] = 4;
			punto3[0][0] = -3;
			punto3[0][1] = 4;
			punto4[0][0] = -4;
			punto4[0][1] = 4;
			while (1) {
				if (counterButton == 1) { // Interrupción externa del exti del botón SW
					estado++;
					giro = 1;
				}
				counterButton = 0;
				if (punto1[0][0] >= 31 || punto2[0][0] >= 31
						|| punto3[0][0] >= 31 || punto4[0][0] >= 31) {
					break;
				} else if (estado == 0 || punto1[0][0] <= 1) {
					if (joystick[1] < 100 && punto1[0][1] > 0
							&& punto1[0][0] > 1) {
						if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] - 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] - 1;
							punto2[0][1] = punto2[0][1] - 1;
							punto3[0][1] = punto3[0][1] - 1;
							punto4[0][1] = punto4[0][1] - 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}
					}
					/*  Movimiento joystick hacia el lado derecho de la matriz */
					if (joystick[1] > 4000 && punto2[0][1] < 7
							&& punto1[0][0] > 1) {
						if ((matriz[punto2[0][0]][punto2[0][1] + 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] + 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] + 1;
							punto2[0][1] = punto2[0][1] + 1;
							punto3[0][1] = punto3[0][1] + 1;
							punto4[0][1] = punto4[0][1] + 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}

					} else {
						int test = movimientoLInvertida(punto1, punto2, punto3,
								punto4, matriz);
						if (test == 1) {
							delay_ms(200);
							traducir(matriz);
						} else {
							break;
						}
					}
					estado = 0;
				} else if (estado == 1 && punto1[0][0] > 1
						&& punto3[0][1] < 7) {
					if (giro == 1
							&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
							&& (matriz[punto3[0][0] - 1][punto3[0][1] - 1] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][0] = punto3[0][0];
						punto1[0][1] = punto3[0][1] - 1;
						punto2[0][0] = punto3[0][0];
						punto2[0][1] = punto3[0][1];
						punto3[0][0] = punto3[0][0];
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][0] = punto4[0][0];
						punto4[0][1] = punto4[0][1] - 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto3[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoLInvertidaGiro1(punto1, punto2,
									punto3, punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 2) {
					if (giro == 1
							&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
							&& (matriz[punto2[0][0] - 1][punto2[0][1] + 1] == 0)
							&& (matriz[punto2[0][0] - 1][punto2[0][1]] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][0] = punto1[0][0] + 1;
						punto1[0][1] = punto1[0][1] + 1;
						punto2[0][0] = punto2[0][0] - 1;
						punto2[0][1] = punto2[0][1] + 1;
						punto3[0][0] = punto3[0][0];
						punto3[0][1] = punto3[0][1] - 1;
						punto4[0][0] = punto4[0][0];
						punto4[0][1] = punto4[0][1] + 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto3[0][0]][punto3[0][1] - 1]
											== 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto2[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
									&& (matriz[punto2[0][0]][punto2[0][1] + 1]
											== 0)
									&& (matriz[punto3[0][0]][punto3[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoLInvertidaGiro2(punto1, punto2,
									punto3, punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 3 && punto1[0][0] > 1 && punto3[0][1]) {
					if (giro == 1
							&& (matriz[punto3[0][0] + 1][punto3[0][1] + 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
							&& punto3[0][1] > 0) {
						delay_ms(200);
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][1] = punto1[0][1] + 1;
						punto2[0][0] = punto2[0][0] + 1;
						punto2[0][1] = punto2[0][1] - 2;
						punto4[0][0] = punto4[0][0] + 1;
						punto4[0][1] = punto4[0][1] + 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto2[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto2[0][0]][punto2[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto1[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoLInvertidaGiro3(punto1, punto2,
									punto3, punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 4 && punto1[0][0] > 1) {
					if (giro == 1
							&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)
							&& (matriz[punto3[0][0] + 1][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0] - 1][punto3[0][1]] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][0] = punto3[0][0] + 1;
						punto1[0][1] = punto3[0][1];
						punto2[0][0] = punto3[0][0] + 1;
						punto2[0][1] = punto3[0][1] - 1;
						punto4[0][0] = punto3[0][0] - 1;
						punto4[0][1] = punto3[0][1];

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						delay_ms(200);
						giro = 0;
					}
					if (giro == 0) {
						estado = 0;
					} else {
						estado--;
					}
				} else {
					estado--;
					giro = 0;
				}
			}
		}
		if (figura == 3) { //Cae L
			estado = 0;
			punto1[0][0] = -1;
			punto1[0][1] = 3;
			punto2[0][0] = -1;
			punto2[0][1] = 4;
			punto3[0][0] = -3;
			punto3[0][1] = 3;
			punto4[0][0] = -4;
			punto4[0][1] = 3;
			while (1) {
				if (counterButton == 1) { // Interrupción externa del exti del botón SW
					estado++;
					giro = 1;
				}
				counterButton = 0;
				if (punto1[0][0] >= 31 || punto2[0][0] >= 31
						|| punto3[0][0] >= 31 || punto4[0][0] >= 31) {
					break;
				} else if (estado == 0 || punto1[0][0] <= 1) {
					if (joystick[1] < 100 && punto1[0][1] > 0
							&& punto1[0][0] > 1) {
						if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] - 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] - 1;
							punto2[0][1] = punto2[0][1] - 1;
							punto3[0][1] = punto3[0][1] - 1;
							punto4[0][1] = punto4[0][1] - 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}
					}
					/*  Movimiento joystick hacia el lado derecho de la matriz */
					if (joystick[1] > 4000 && punto2[0][1] < 7
							&& punto1[0][0] > 1) {
						if ((matriz[punto2[0][0]][punto2[0][1] + 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] + 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] + 1;
							punto2[0][1] = punto2[0][1] + 1;
							punto3[0][1] = punto3[0][1] + 1;
							punto4[0][1] = punto4[0][1] + 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}

					} else {
						int test = movimientoL(punto1, punto2, punto3, punto4,
								matriz);
						if (test == 1) {
							delay_ms(200);
							traducir(matriz);
						} else {
							break;
						}
					}
					estado = 0;
				} else if (estado == 1 && punto1[0][0] > 1 && punto3[0][1] > 0
						&& punto2[0][1] < 7) {
					if (giro == 1
							&& (matriz[punto3[0][0] + 1][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][1] = punto1[0][1] - 1;
						punto2[0][0] = punto2[0][0] - 1;
						punto2[0][1] = punto2[0][1] - 1;
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][0] = punto4[0][0] + 1;
						punto4[0][1] = punto4[0][1] - 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto3[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
									&& (matriz[punto3[0][0]][punto3[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoLGiro1(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 2 && punto1[0][0] > 1) {
					if (giro == 1
							&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)
							&& (matriz[punto3[0][0] - 1][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0] - 1][punto3[0][1]] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][0] = punto3[0][0] + 1;
						punto1[0][1] = punto3[0][1];
						punto2[0][0] = punto3[0][0] - 1;
						punto2[0][1] = punto3[0][1] - 1;
						punto4[0][0] = punto3[0][0] - 1;
						punto4[0][1] = punto3[0][1];

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto2[0][1] > 0) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto2[0][0]][punto2[0][1] - 1]
											== 0)
									&& (matriz[punto3[0][0]][punto3[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto1[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] + 1] == 0)
									&& (matriz[punto3[0][0]][punto3[0][1] + 1]
											== 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoLGiro2(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 3 && punto1[0][0] > 1
						&& punto1[0][1] < 7) {
					if (giro == 1
							&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
							&& (matriz[punto3[0][0] - 1][punto3[0][1] + 1] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][0] = punto3[0][0];
						punto1[0][1] = punto3[0][1] - 1;
						punto2[0][0] = punto3[0][0];
						punto2[0][1] = punto3[0][1];
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][0] = punto2[0][0] - 1;
						punto4[0][1] = punto2[0][1] + 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						delay_ms(200);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][0] > 1) {
							if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] - 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto3[0][1] < 7
								&& punto1[0][0] > 1) {
							if ((matriz[punto3[0][0]][punto3[0][1] + 1] == 0)
									&& (matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0)) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoLGiro3(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(200);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 4 && punto1[0][0] > 1) {
					if (giro == 1
							&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
							&& (matriz[punto2[0][0] + 1][punto2[0][1] + 1] == 0)
							&& (matriz[punto2[0][0] - 1][punto2[0][1]] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][0] = punto2[0][0] + 1;
						punto1[0][1] = punto2[0][1];
						punto2[0][0] = punto2[0][0] + 1;
						punto2[0][1] = punto2[0][1] + 1;
						punto3[0][1] = punto3[0][1] - 1;
						punto4[0][0] = punto3[0][0] - 1;
						punto4[0][1] = punto3[0][1];

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						delay_ms(200);
						giro = 0;
					}
					if (giro == 0) {
						estado = 0;
					} else {
						estado--;
					}
				} else {
					estado--;
					giro = 0;
				}
			}
		}

		if (figura == 4) { //Cae Cubo
			punto1[0][0] = -1;
			punto1[0][1] = 3;
			punto2[0][0] = -1;
			punto2[0][1] = 4;
			punto3[0][0] = -3;
			punto3[0][1] = 3;
			punto4[0][0] = -3;
			punto4[0][1] = 4;
			while (1) {
				if (joystick[1] < 100 && punto1[0][0] > 1 && punto1[0][1] > 0) {
					if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)) {
						delay_ms(200);
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][1] = punto1[0][1] - 1;
						punto2[0][1] = punto2[0][1] - 1;
						punto3[0][1] = punto3[0][1] - 1;
						punto4[0][1] = punto4[0][1] - 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
					}
				}
				/*  Movimiento joystick hacia el lado derecho de la matriz */
				if (joystick[1] > 4000 && punto2[0][1] < 7
						&& punto1[0][0] > 1) {
					if ((matriz[punto2[0][0]][punto2[0][1] + 1] == 0)
							&& (matriz[punto4[0][0]][punto4[0][1] + 1] == 0)) {
						delay_ms(200);
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][1] = punto1[0][1] + 1;
						punto2[0][1] = punto2[0][1] + 1;
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][1] = punto4[0][1] + 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
					}

				} else {
					int test = movimientoCubo(punto1, punto2, punto3, punto4,
							matriz);
					if (test == 1) {
						delay_ms(200);
						traducir(matriz);
					} else {
						break;
					}
				}
				estado = 0;
			}
		}

		if (figura == 5) { //Cae S
			estado = 0;
			punto1[0][0] = -1;
			punto1[0][1] = 3;
			punto2[0][0] = -1;
			punto2[0][1] = 4;
			punto3[0][0] = -2;
			punto3[0][1] = 5;
			punto4[0][0] = -2;
			punto4[0][1] = 4;
			while (1) {
				if (counterButton == 1) { // Interrupción externa del exti del botón SW
					estado++;
					giro = 1;
				}
				counterButton = 0;
				if (punto1[0][0] >= 31 || punto2[0][0] >= 31
						|| punto3[0][0] >= 31 || punto4[0][0] >= 31) {
					break;
				} else if (estado == 0 || punto1[0][0] <= 1) {
					if (joystick[1] < 100 && punto1[0][1] > 0
							&& punto1[0][0] > 1) {
						if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] - 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] - 1;
							punto2[0][1] = punto2[0][1] - 1;
							punto3[0][1] = punto3[0][1] - 1;
							punto4[0][1] = punto4[0][1] - 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}
					}
					/*  Movimiento joystick hacia el lado derecho de la matriz */
					if (joystick[1] > 4000 && punto3[0][1] < 7
							&& punto1[0][0] > 1) {
						if ((matriz[punto2[0][0]][punto2[0][1] + 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] + 1;
							punto2[0][1] = punto2[0][1] + 1;
							punto3[0][1] = punto3[0][1] + 1;
							punto4[0][1] = punto4[0][1] + 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}

					} else {
						int test = movimientoS(punto1, punto2, punto3, punto4,
								matriz);
						if (test == 1) {
							delay_ms(200);
							traducir(matriz);
						} else {
							break;
						}
					}
					estado = 0;
				} else if (estado == 1 && punto1[0][0] > 1) {
					if (giro == 1
							&& (matriz[punto4[0][0]][punto4[0][1] - 1] == 0)
							&& (matriz[punto4[0][0] - 1][punto4[0][1] - 1] == 0)) {
						matriz[punto1[0][0]][punto1[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;

						punto1[0][1] = punto1[0][1] + 1;
						punto2[0][0] = punto2[0][0] - 1;
						punto2[0][1] = punto2[0][1] - 1;
						punto3[0][1] = punto3[0][1] - 1;
						punto4[0][0] = punto4[0][0] - 1;
						punto4[0][1] = punto4[0][1] - 1;

						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto2[0][1] > 0) {
							if (matriz[punto2[0][0]][punto2[0][1] - 1] == 0) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto1[0][1] < 7) {
							if (matriz[punto1[0][0]][punto1[0][1] + 1] == 0) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoSGiro1(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(500);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 2 && punto1[0][0] > 1 && punto1[0][1] > 0
						&& punto1[0][1] < 7) {
					if (giro == 1
							&& (matriz[punto3[0][0] + 1][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0]][punto3[0][1] + 1] == 0)) {
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto1[0][1] = punto1[0][1] - 1;
						punto2[0][0] = punto2[0][0] + 1;
						punto2[0][1] = punto2[0][1] + 1;
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][0] = punto4[0][0] + 1;
						punto4[0][1] = punto4[0][1] + 1;

						matriz[punto1[0][0]][punto1[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						estado = 0;
					} else {
						estado--;
					}
				} else {
					estado--;
					giro = 0;
				}
			}

		}

		if (figura == 6) { //Cae Z
			estado = 0;
			punto1[0][0] = -1;
			punto1[0][1] = 4;
			punto2[0][0] = -1;
			punto2[0][1] = 5;
			punto3[0][0] = -2;
			punto3[0][1] = 3;
			punto4[0][0] = -2;
			punto4[0][1] = 4;
			while (1) {
				if (counterButton == 1) { // Interrupción externa del exti del botón SW
					estado++;
					giro = 1;
				}
				counterButton = 0;
				if (punto1[0][0] >= 31 || punto2[0][0] >= 31
						|| punto3[0][0] >= 31 || punto4[0][0] >= 31) {
					break;
				} else if (estado == 0 || punto1[0][0] <= 1) {
					if (joystick[1] < 100 && punto3[0][1] > 0
							&& punto1[0][0] > 1) {
						if ((matriz[punto1[0][0]][punto1[0][1] - 1] == 0)
								&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] - 1;
							punto2[0][1] = punto2[0][1] - 1;
							punto3[0][1] = punto3[0][1] - 1;
							punto4[0][1] = punto4[0][1] - 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}
					}
					/*  Movimiento joystick hacia el lado derecho de la matriz */
					if (joystick[1] > 4000 && punto2[0][1] < 7
							&& punto1[0][0] > 1) {
						if ((matriz[punto2[0][0]][punto2[0][1] + 1] == 0)
								&& (matriz[punto4[0][0]][punto4[0][1] + 1] == 0)) {
							delay_ms(200);
							matriz[punto1[0][0]][punto1[0][1]] = 0;
							matriz[punto2[0][0]][punto2[0][1]] = 0;
							matriz[punto3[0][0]][punto3[0][1]] = 0;
							matriz[punto4[0][0]][punto4[0][1]] = 0;

							punto1[0][1] = punto1[0][1] + 1;
							punto2[0][1] = punto2[0][1] + 1;
							punto3[0][1] = punto3[0][1] + 1;
							punto4[0][1] = punto4[0][1] + 1;

							matriz[punto1[0][0]][punto1[0][1]] = 1;
							matriz[punto2[0][0]][punto2[0][1]] = 1;
							matriz[punto3[0][0]][punto3[0][1]] = 1;
							matriz[punto4[0][0]][punto4[0][1]] = 1;
						}

					} else {
						int test = movimientoZ(punto1, punto2, punto3, punto4,
								matriz);
						if (test == 1) {
							delay_ms(200);
							traducir(matriz);
						} else {
							break;
						}
					}
					estado = 0;
				} else if (estado == 1 && punto1[0][0] > 1) {
					if (giro == 1
							&& (matriz[punto4[0][0]][punto4[0][1] + 1] == 0)
							&& (matriz[punto4[0][0] - 1][punto4[0][1] + 1] == 0)) {
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto3[0][0]][punto3[0][1]] = 0;

						punto2[0][0] = punto2[0][0] - 1;
						punto3[0][1] = punto3[0][1] + 1;
						punto4[0][0] = punto4[0][0] - 1;
						punto4[0][1] = punto4[0][1] + 1;

						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto4[0][0]][punto4[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						if (joystick[1] < 100 && punto1[0][1] > 0
								&& punto1[0][1] > 0) {
							if (matriz[punto1[0][0]][punto1[0][1] - 1] == 0
									&& matriz[punto3[0][0]][punto3[0][1] - 1]
											== 0) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] - 1;
								punto2[0][1] = punto2[0][1] - 1;
								punto3[0][1] = punto3[0][1] - 1;
								punto4[0][1] = punto4[0][1] - 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}
						}
						/*  Movimiento joystick hacia el lado derecho de la matriz */
						if (joystick[1] > 4000 && punto2[0][1] < 7) {
							if (matriz[punto2[0][0]][punto2[0][1] + 1] == 0
									&& matriz[punto4[0][0]][punto4[0][1] + 1]
											== 0) {
								delay_ms(200);
								matriz[punto1[0][0]][punto1[0][1]] = 0;
								matriz[punto2[0][0]][punto2[0][1]] = 0;
								matriz[punto3[0][0]][punto3[0][1]] = 0;
								matriz[punto4[0][0]][punto4[0][1]] = 0;

								punto1[0][1] = punto1[0][1] + 1;
								punto2[0][1] = punto2[0][1] + 1;
								punto3[0][1] = punto3[0][1] + 1;
								punto4[0][1] = punto4[0][1] + 1;

								matriz[punto1[0][0]][punto1[0][1]] = 1;
								matriz[punto2[0][0]][punto2[0][1]] = 1;
								matriz[punto3[0][0]][punto3[0][1]] = 1;
								matriz[punto4[0][0]][punto4[0][1]] = 1;
							}

						} else {
							int test = movimientoSGiro1(punto1, punto2, punto3,
									punto4, matriz);
							if (test == 1) {
								delay_ms(500);
								traducir(matriz);
							} else {
								break;
							}
						}
					} else {
						estado--;
					}
				} else if (estado == 2 && punto1[0][0] > 1
						&& punto1[0][1] > 0) {
					if (giro == 1
							&& (matriz[punto3[0][0]][punto3[0][1] - 1] == 0)
							&& (matriz[punto3[0][0] + 1][punto3[0][1] + 1] == 0)) {
						matriz[punto2[0][0]][punto2[0][1]] = 0;
						matriz[punto4[0][0]][punto4[0][1]] = 0;

						punto2[0][0] = punto2[0][0] + 1;
						punto3[0][1] = punto3[0][1] - 1;
						punto4[0][0] = punto4[0][0] + 1;
						punto4[0][1] = punto4[0][1] - 1;

						matriz[punto2[0][0]][punto2[0][1]] = 1;
						matriz[punto3[0][0]][punto3[0][1]] = 1;
						traducir(matriz);
						giro = 0;
					}
					if (giro == 0) {
						estado = 0;
					} else {
						estado--;
					}
				} else {
					estado--;
					giro = 0;
				}
			}

		}
		//Fin del juego
		if (matriz[0][0] || matriz[0][1] || matriz[0][2] || matriz[0][3]
				|| matriz[0][4] || matriz[0][5] || matriz[0][6]
				|| matriz[0][7]) {
			end(matriz);
			traducir(matriz);
			break;
		}
	}
	return 0;
}

void tetris(int matriz[32][8], uint8_t fila) {
	for (int i = 0; i < 8; i++) {
		matriz[fila][i] = 0;
	}
	traducir(matriz);
	delay_ms(1000);
	for (int i = 32; i > 0; i--) {
		if (i <= fila) {
			for (int j = 0; j < 8; j++) {
				matriz[i][j] = matriz[i - 1][j];
			}
		}

	}
}
void inicio(int matriz[32][8]){
	matriz[1][6] = 1;
	matriz[2][6] = 1;
	matriz[3][6] = 1;
	matriz[4][6] = 1;
	matriz[5][6] = 1;

	matriz[3][1] = 1;
	matriz[3][2] = 1;
	matriz[3][3] = 1;
	matriz[3][4] = 1;
	matriz[3][5] = 1;
	matriz[2][6] = 1;

	matriz[7][1] = 1;
	matriz[7][2] = 1;
	matriz[7][3] = 1;
	matriz[7][4] = 1;
	matriz[7][5] = 1;
	matriz[7][6] = 1;

	matriz[8][1] = 1;
	matriz[9][1] = 1;
	matriz[10][1] = 1;
	matriz[8][4] = 1;
	matriz[9][4] = 1;
	matriz[10][4] = 1;
	matriz[8][6] = 1;
	matriz[9][6] = 1;
	matriz[10][6] = 1;

	matriz[12][6] = 1;
	matriz[13][6] = 1;
	matriz[14][6] = 1;
	matriz[15][6] = 1;
	matriz[16][6] = 1;

	matriz[14][1] = 1;
	matriz[14][2] = 1;
	matriz[14][3] = 1;
	matriz[14][4] = 1;
	matriz[14][5] = 1;

	matriz[18][1] = 1;
	matriz[18][2] = 1;
	matriz[18][3] = 1;
	matriz[18][4] = 1;
	matriz[18][5] = 1;
	matriz[18][6] = 1;

	matriz[19][6] = 1;
	matriz[20][6] = 1;
	matriz[21][5] = 1;
	matriz[21][4] = 1;
	matriz[20][3] = 1;
	matriz[19][3] = 1;
	matriz[20][2] = 1;
	matriz[21][1] = 1;

	matriz[23][6] = 1;
	matriz[23][1] = 1;
	matriz[25][6] = 1;
	matriz[25][1] = 1;

	matriz[24][1] = 1;
	matriz[24][2] = 1;
	matriz[24][3] = 1;
	matriz[24][4] = 1;
	matriz[24][5] = 1;
	matriz[24][6] = 1;

	matriz[27][1] = 1;
	matriz[27][3] = 1;
	matriz[27][4] = 1;
	matriz[27][5] = 1;
	matriz[27][6] = 1;

	matriz[28][1] = 1;
	matriz[28][3] = 1;
	matriz[28][6] = 1;

	matriz[29][1] = 1;
	matriz[29][3] = 1;
	matriz[29][6] = 1;

	matriz[30][1] = 1;
	matriz[28][1] = 1;
	matriz[30][2] = 1;
	matriz[30][3] = 1;
	matriz[30][6] = 1;

	traducir(matriz);
	delay_ms(3000);
	for(int i=0; i<32;i++){
		for(int j=0;j<8;j++){
			matriz[i][j] = 0;
		}
	}
}
void traducir(int matriz[32][8]) {
	int matriz1[8][8] = { 0 };
	int matriz2[8][8] = { 0 };
	int matriz3[8][8] = { 0 };
	int matriz4[8][8] = { 0 };
	for (int i = 0; i < 32; i++) {
		if (i >= 0 && i < 8) {
			for (int j = 0; j < 8; j++) {
				matriz1[i][j] = matriz[i][j];
			}
		}
		if (i >= 8 && i < 16) {
			for (int j = 0; j < 8; j++) {
				matriz2[i - 8][j] = matriz[i][j];
			}
		}
		if (i >= 16 && i < 24) {
			for (int j = 0; j < 8; j++) {
				matriz3[i - 16][j] = matriz[i][j];
			}
		}
		if (i >= 24 && i < 32) {
			for (int j = 0; j < 8; j++) {
				matriz4[i - 24][j] = matriz[i][j];
			}
		}
	}

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
void adcComplete_Callback(void) {
	adcData = getADC();
	aux++;
// Leemos los valores de la conversión ADC
	if (aux == 1) {
		joystick[0] = adcData;
	} else if (aux == 2) {
		joystick[1] = adcData;
		adcIsComplete = SET;
		aux = 0;
	} else {
		__NOP();
	}

}

void BasicTimer5_Callback(void) {
//Hacemos uduttyn blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);

}

void callback_extInt9(void) {                     // Exti del botón
	counterButton |= 1;	                          // Flanco de subida
}
void usart2Rx_Callback(void) {
//Leemos el valor del registro DR, donde se almacena el dato que llega.
//Esto además debe bajar la bandera de la interrupción
	rxData = getRXData();
}

