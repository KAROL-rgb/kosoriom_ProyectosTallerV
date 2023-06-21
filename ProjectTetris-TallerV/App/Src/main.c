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

GPIO_Handler_t handlerSpiSCLK = { 0 };
GPIO_Handler_t handlerSpiMISO = { 0 };
GPIO_Handler_t handlerSpiMOSI = { 0 };
GPIO_Handler_t handlerSpiSS = { 0 };
BasicTimer_Handler_t handlerStateOKTimer = { 0 };
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

uint32_t getCounterDelay(void);
BasicTimer_Handler_t handlerDelay = { 0 };
uint32_t counterDelay = 0;
uint8_t var = { 0 };
uint16_t memoria[8];

uint8_t col = 0;
uint8_t save = 0;
uint8_t flag = 0;

int test = 0;
/* Definición de las funciones del programa a utilizar */
uint8_t MatrixLed_writeData(uint8_t digit, uint8_t seg);
void initSystem(void);

void traducir(int matriz[32][8]);

void mover(int punto[1][2], int matriz[32][8]);

int movimientoPalito(int cuenta, int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz1[8][8],
		int matriz2[8][8], int matriz3[8][8], int matriz4[8][8]);

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
	srand(time(NULL));

	int matriz[32][8] = { 0 };

	int punto1[1][2];
//	int punto2[1][2];
//	int punto3[1][2];
//	int punto4[1][2];

	matriz[30][1] = 1;
	matriz[30][2] = 1;
	matriz[31][1] = 1;
	matriz[31][2] = 1;

	initSystem();
	config_SysTick_ms(0);
	SPI_Configuration();
	Serial_Configuration();
	clean();
	clearMatrix();

	punto1[0][0] = -1;
	punto1[0][1] = 4;
//	punto2[0][0] = -2;
//	punto2[0][1] = 4;
//	punto3[0][0] = -3;
//	punto3[0][1] = 4;
//	punto4[0][0] = -4;
//	punto4[0][1] = 4;
	mover(punto1, matriz);
	delay_ms(1000);
	traducir(matriz);

	while(1) {
		// int test = movimientoPalito(i, punto1, punto2, punto3, punto4, matriz);
		//if (test == 1) {


//		} else {
//			break;
//		}
	}
//	int punto[1][2];
//	punto[0][0] = -1;
//	punto[0][1] = 1;
//	mover(punto, matriz1, matriz2, matriz3, matriz4);

	while (1) {
//		int figura = rand() % 7;
//		if(figura == 0){
//			//T
//			matriz1[0][0]=1;
//			matriz1[0][1]=1;
//			matriz1[0][2]=1;
//			matriz1[1][1]=1;
//			traducir(matriz1, matriz2, matriz3, matriz4);
//			delay_ms(2000);
//			matriz1[0][0]=0;
//			matriz1[0][1]=0;
//			matriz1[0][2]=0;
//			matriz1[1][1]=0;
//			traducir(matriz1, matriz2, matriz3, matriz4);
//			delay_ms(2000);
//		}
//		if(figura == 1){
//			//Cuadro
//			matriz1[4][0]=1;
//			matriz1[4][1]=1;
//			matriz1[5][0]=1;
//			matriz1[5][1]=1;
//			traducir(matriz1, matriz2, matriz3, matriz4);
//			delay_ms(2000);
//			matriz1[4][0]=0;
//			matriz1[4][1]=0;
//			matriz1[5][0]=0;
//			matriz1[5][1]=0;
//			traducir(matriz1, matriz2, matriz3, matriz4);
//			delay_ms(2000);
//		}
//		if(figura == 2){
//			//L
//			matriz1[4][3]=1;
//			matriz1[5][3]=1;
//			matriz1[6][3]=1;
//			matriz1[6][4]=1;
//			traducir(matriz1, matriz2, matriz3, matriz4);
//			delay_ms(2000);
//			matriz1[4][3]=0;
//			matriz1[5][3]=0;
//			matriz1[6][3]=0;
//			matriz1[6][4]=0;
//			traducir(matriz1, matriz2, matriz3, matriz4);
//			delay_ms(2000);
//		}
	}
	return 0;
}

int movimientoPalito(int cuenta, int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz1[8][8],
		int matriz2[8][8], int matriz3[8][8], int matriz4[8][8]) {
	if (cuenta >= 0 && cuenta <= 7) {
		// Entra en matriz 1
		if (matriz1[punto1[0][0] + 1][punto1[0][1]] == 0) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.

			if (punto1[0][0] >= 0) {
				// Movimiento dentro de la matriz
				matriz1[punto1[0][0]][punto1[0][1]] = 0;// Se apaga la luz donde estoy
				matriz1[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
				punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto
				if (punto2[0][0] >= 0) {
					matriz1[punto2[0][0]][punto2[0][1]] = 0;
					matriz1[punto2[0][0] + 1][punto2[0][1]] = 1;
					punto2[0][0] = punto2[0][0] + 1;
				}
				if (punto3[0][0] >= 0) {
					matriz1[punto3[0][0]][punto3[0][1]] = 0;
					matriz1[punto3[0][0] + 1][punto3[0][1]] = 1;
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] >= 0) {
					matriz1[punto4[0][0]][punto4[0][1]] = 0;
					matriz1[punto4[0][0] + 1][punto4[0][1]] = 1;
					punto4[0][0] = punto4[0][0] + 1;
				}
				if (punto2[0][0] < 0) {
					punto2[0][0] = punto2[0][0] + 1;
				}
				if (punto3[0][0] < 0) {
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] < 0) {
					punto4[0][0] = punto4[0][0] + 1;
				}
				if (punto2[0][0] == -1) { // Va a entra a a la matriz el punto
					matriz1[punto2[0][0] + 1][punto2[0][1]] = 1;
					punto2[0][0] = punto2[0][0] + 1;
				}
				if (punto3[0][0] == -1) {
					matriz1[punto3[0][0] + 1][punto3[0][1]] = 1;
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] == -1) {
					matriz1[punto4[0][0] + 1][punto4[0][1]] = 1;
					punto4[0][0] = punto4[0][0] + 1;
				}
				cuenta += 1;
				return 1;
			} else {
				// Movimiento de la base por fuera de la matriz
				matriz1[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
				punto1[0][0] = punto1[0][0] + 1;
				if (punto2[0][0] == -1) {
					matriz1[punto2[0][0] + 1][punto2[0][1]] = 1;
					punto2[0][0] = punto2[0][0] + 1;
				}
				if (punto3[0][0] == -1) {
					matriz1[punto3[0][0] + 1][punto3[0][1]] = 1;
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] == -1) {
					matriz1[punto4[0][0] + 1][punto4[0][1]] = 1;
					punto4[0][0] = punto4[0][0] + 1;
				}
				cuenta += 1;
				return 1;
			}

		} else {
			return 0;
		}
	}
	if (cuenta >= 8 && cuenta <= 15) { //Entra en la segunda matriz
		if (matriz2[punto1[0][0] - 7][punto1[0][1]] == 0) { //Se verifica que este vacio donde va a caer
			if (punto1[0][0] - 7 >= 0) {
				matriz2[punto1[0][0] - 8][punto1[0][1]] = 0;
				matriz2[punto1[0][0] - 7][punto1[0][1]] = 1;
				punto1[0][0] = punto1[0][0] + 1;

				if (punto2[0][0] - 7 >= 0) {
					matriz2[punto2[0][0] - 8][punto2[0][1]] = 0;
					matriz2[punto2[0][0] - 7][punto2[0][1]] = 1;
					punto2[0][0] = punto2[0][0] + 1;

				}
				if (punto3[0][0] - 7 >= 0) {
					matriz2[punto3[0][0] - 8][punto3[0][1]] = 0;
					matriz2[punto3[0][0] - 7][punto3[0][1]] = 1;
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] - 7 >= 0) {
					matriz2[punto4[0][0] - 8][punto4[0][1]] = 0;
					matriz2[punto4[0][0] - 7][punto4[0][1]] = 1;
					punto4[0][0] = punto4[0][0] + 1;
				}
				if (punto2[0][0] - 8 < 0) {
					punto2[0][0] = punto2[0][0] + 1;
					matriz2[punto2[0][0]][punto2[0][1]] = 0;
				}
				if (punto3[0][0] - 8 < 0) {
					punto3[0][0] = punto3[0][0] + 1;
					matriz2[punto3[0][0]][punto3[0][1]] = 0;
				}
				if (punto4[0][0] - 8 < 0) {
					punto4[0][0] = punto4[0][0] + 1;
					matriz2[punto3[0][0]][punto3[0][1]] = 0;
				}
				cuenta += 1;
				return 1;

			} else {
				matriz2[punto1[0][0] - 7][punto1[0][1]] = 1;
				punto1[0][0] = punto1[0][0] + 1;
				matriz1[punto1[0][0]][punto1[0][1]] = 0;
				if (punto2[0][0] - 8 < 0) {
					punto2[0][0] = punto2[0][0] + 1;
					matriz1[punto2[0][0]][punto2[0][1]] = 0;
				}
				if (punto3[0][0] - 8 < 0) {
					punto3[0][0] = punto3[0][0] + 1;
					matriz1[punto3[0][0]][punto3[0][1]] = 0;
				}
				if (punto4[0][0] - 8 < 0) {
					punto4[0][0] = punto4[0][0] + 1;
					matriz1[punto3[0][0]][punto3[0][1]] = 0;
				}
			}
			cuenta += 1;
			return 1;
		} else {
			return 0;
		}
	}
	if (cuenta >= 16 && cuenta <= 23) {
		if (matriz3[punto1[0][0] - 15][punto1[0][1]] == 0) { //Se verifica que este vacio donde va a caer
			if (punto1[0][0] - 15 >= 0) {
				matriz3[punto1[0][0] - 16][punto1[0][1]] = 0;
				matriz3[punto1[0][0] - 15][punto1[0][1]] = 1;
				punto1[0][0] = punto1[0][0] + 1;

				if (punto2[0][0] - 15 >= 0) {
					matriz3[punto2[0][0] - 16][punto2[0][1]] = 0;
					matriz3[punto2[0][0] - 15][punto2[0][1]] = 1;
					punto2[0][0] = punto2[0][0] + 1;
				}
				if (punto3[0][0] - 15 >= 0) {
					matriz3[punto3[0][0] - 16][punto3[0][1]] = 0;
					matriz3[punto3[0][0] - 15][punto3[0][1]] = 1;
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] - 15 >= 0) {
					matriz3[punto4[0][0] - 16][punto4[0][1]] = 0;
					matriz3[punto4[0][0] - 15][punto4[0][1]] = 1;
					punto4[0][0] = punto4[0][0] + 1;
				}
				if (punto2[0][0] - 16 < 0) {
					punto2[0][0] = punto2[0][0] + 1;
					matriz3[punto2[0][0]][punto2[0][1]] = 0;
				}
				if (punto3[0][0] - 16 < 0) {
					punto3[0][0] = punto3[0][0] + 1;
					matriz3[punto3[0][0]][punto3[0][1]] = 0;
				}
				if (punto4[0][0] - 16 < 0) {
					punto4[0][0] = punto4[0][0] + 1;
					matriz3[punto3[0][0]][punto3[0][1]] = 0;
				}
				cuenta += 1;
				return 1;
			} else {
				matriz3[punto1[0][0] - 15][punto1[0][1]] = 1;
				punto1[0][0] = punto1[0][0] + 1;
				matriz3[punto1[0][0]][punto1[0][1]] = 0;
				if (punto2[0][0] - 16 < 0) {
					punto2[0][0] = punto2[0][0] + 1;
					matriz3[punto2[0][0]][punto2[0][1]] = 0;
				}
				if (punto3[0][0] - 16 < 0) {
					punto3[0][0] = punto3[0][0] + 1;
					matriz3[punto3[0][0]][punto3[0][1]] = 0;
				}
				if (punto4[0][0] - 16 < 0) {
					punto4[0][0] = punto4[0][0] + 1;
					matriz3[punto3[0][0]][punto3[0][1]] = 0;
				}
			}
			cuenta += 1;
			return 1;

		} else {
			return 0;
		}
	}
	if (cuenta >= 24 && cuenta <= 31) {
		if (matriz4[punto1[0][0] - 23][punto1[0][1]] == 0) { //Se verifica que este vacio donde va a caer
			if (punto1[0][0] - 23 >= 0) {
				matriz4[punto1[0][0] - 24][punto1[0][1]] = 0;
				matriz4[punto1[0][0] - 23][punto1[0][1]] = 1;
				punto1[0][0] = punto1[0][0] + 1;
				if (punto2[0][0] - 23 >= 0) {
					matriz4[punto2[0][0] - 24][punto2[0][1]] = 0;
					matriz4[punto2[0][0] - 23][punto2[0][1]] = 1;
					punto2[0][0] = punto2[0][0] + 1;
				}
				if (punto3[0][0] - 23 >= 0) {
					matriz4[punto3[0][0] - 24][punto3[0][1]] = 0;
					matriz4[punto3[0][0] - 23][punto3[0][1]] = 1;
					punto3[0][0] = punto3[0][0] + 1;
				}
				if (punto4[0][0] - 23 >= 0) {
					matriz4[punto4[0][0] - 24][punto4[0][1]] = 0;
					matriz4[punto4[0][0] - 23][punto4[0][1]] = 1;
					punto4[0][0] = punto4[0][0] + 1;
				}
				if (punto2[0][0] - 24 < 0) {
					punto2[0][0] = punto2[0][0] + 1;
					matriz4[punto2[0][0]][punto2[0][1]] = 0;
				}
				if (punto3[0][0] - 24 < 0) {
					punto3[0][0] = punto3[0][0] + 1;
					matriz4[punto3[0][0]][punto3[0][1]] = 0;
				}
				if (punto4[0][0] - 24 < 0) {
					punto4[0][0] = punto4[0][0] + 1;
					matriz4[punto3[0][0]][punto3[0][1]] = 0;
				}
				cuenta += 1;
				return 1;
			} else {
				matriz4[punto1[0][0] - 23][punto1[0][1]] = 1;
				punto1[0][0] = punto1[0][0] + 1;
				matriz4[punto1[0][0]][punto1[0][1]] = 0;
				if (punto2[0][0] - 24 < 0) {
					punto2[0][0] = punto2[0][0] + 1;
					matriz4[punto2[0][0]][punto2[0][1]] = 0;
				}
				if (punto3[0][0] - 24 < 0) {
					punto3[0][0] = punto3[0][0] + 1;
					matriz4[punto3[0][0]][punto3[0][1]] = 0;
				}
				if (punto4[0][0] - 24 < 0) {
					punto4[0][0] = punto4[0][0] + 1;
					matriz4[punto3[0][0]][punto3[0][1]] = 0;
				}
			}
			cuenta += 1;
			return 1;
		}

		else {
			return 0;
		}
	} else {
		return 0;
	}
}

//void movimiento(int segmento, int punto1[1][2], int punto2[1][2],
//		int punto3[1][2], int punto4[1][2], int matriz1[8][8],
//		int matriz2[8][8], int matriz3[8][8], int matriz4[8][8]) {
//	//Ver si p1 es una base
//	if (punto1[0][0] >= punto2[0][0] && punto1[0][0] >= punto3[0][0]
//			&& punto1[0][0] >= punto4[0][0]) {
//		if (punto1[0][0] == punto2[0][0] && punto1[0][0] == punto3[0][0]
//				&& punto1[0][0] == punto4[0][0]) {
////Base4
//		}
//		if (punto1[0][0] == punto2[0][0] && punto1[0][0] == punto3[0][0]
//				&& punto1[0][0] != punto4[0][0]) {
////Base3
//		}
//		if (punto1[0][0] == punto2[0][0] && punto1[0][0] != punto3[0][0]
//				&& punto1[0][0] == punto4[0][0]) {
////Base3
//		}
//		if (punto1[0][0] != punto2[0][0] && punto1[0][0] == punto3[0][0]
//				&& punto1[0][0] == punto4[0][0]) {
////Base3
//		}
//		if (punto1[0][0] == punto2[0][0] && punto1[0][0] != punto3[0][0]
//				&& punto1[0][0] != punto4[0][0]) {
////Base2
//		}
//		if (punto1[0][0] != punto2[0][0] && punto1[0][0] == punto3[0][0]
//				&& punto1[0][0] != punto4[0][0]) {
////Base2
//		}
//		if (punto1[0][0] != punto2[0][0] && punto1[0][0] != punto3[0][0]
//				&& punto1[0][0] == punto4[0][0]) {
////Base2
//		}
//		if (punto1[0][0] != punto2[0][0] && punto1[0][0] != punto3[0][0]
//				&& punto1[0][0] != punto4[0][0]) {
//			base1(segmento, punto1, punto2, punto3, punto4, matriz1, matriz2,
//					matriz3, matriz4);
//		}
//	} else {
//		//Si p1 no es una base, entonces hay que encontrarla en el resto, ver si p2 es una base
//		if (punto2[0][0] >= punto3[0][0] && punto2[0][0] >= punto4[0][0]) {
//			if (punto2[0][0] == punto3[0][0] && punto2[0][0] == punto4[0][0]) {
////Base3
//			}
//			if (punto2[0][0] == punto3[0][0] && punto2[0][0] != punto4[0][0]) {
////Base2
//			}
//			if (punto2[0][0] != punto3[0][0] && punto2[0][0] == punto4[0][0]) {
////Base2
//			}
//			if (punto2[0][0] != punto3[0][0] && punto2[0][0] != punto4[0][0]) {
////Base1
//				base1(segmento, punto2, punto1, punto3, punto4, matriz1,
//						matriz2, matriz3, matriz4);
//			}
//		} else {
////Si p2 no es una base, entonces hay que encontrarla en el resto, ver si p3 es una base
//			if (punto3[0][0] >= punto4[0][0]) {
//				if (punto3[0][0] == punto4[0][0]) {
//					//Base2
//				}
//				if (punto3[0][0] != punto4[0][0]) {
//					//Base1
//					base1(segmento, punto3, punto1, punto2, punto4, matriz1,
//							matriz2, matriz3, matriz4);
//				}
//			} else {
////Solo hay una base de 1 y es p4
//				base1(segmento, punto4, punto1, punto2, punto3, matriz1,
//						matriz2, matriz3, matriz4);
//			}
//		}
//	}
//}

//void base1(int segmento, int base1[1][2], int punto2[1][2], int punto3[1][2],
//		int punto4[1][2], int matriz1[8][8], int matriz2[8][8],
//		int matriz3[8][8], int matriz4[8][8]) {
//
//	if (segmento == 0) {
//		//Entra en matriz 1
//		if (matriz1[base1[0][0] + 1][base1[0][1]] == 0) {
////Si la matriz en la fila siguiente, misma columna, esta desocupada.
//			if (base1[0][0] >= 0) {
////Movimiento dentro de la matriz
//				matriz1[base1[0][0]][base1[0][1]] = 0; //Se apaga la luz donde estoy
//				matriz1[base1[0][0] + 1][base1[0][1]] = 1; //Se prende la siguiente
//				base1[0][0] = base1[0][0] + 1; //Le sumo 1 a la fila del punto
//				if (punto2[0][0] >= 0) {
//					matriz1[punto2[0][0]][punto2[0][1]] = 0;
//					matriz1[punto2[0][0] + 1][punto2[0][1]] = 1;
//					punto2[0][0] = punto2[0][0] + 1;
//				}
//				if (punto3[0][0] >= 0) {
//					matriz1[punto3[0][0]][punto3[0][1]] = 0;
//					matriz1[punto3[0][0] + 1][punto3[0][1]] = 1;
//					punto3[0][0] = punto3[0][0] + 1;
//				}
//				if (punto4[0][0] >= 0) {
//					matriz1[punto4[0][0]][punto4[0][1]] = 0;
//					matriz1[punto4[0][0] + 1][punto4[0][1]] = 1;
//					punto4[0][0] = punto4[0][0] + 1;
//				}
//				if (punto2[0][0] < 0) {
//					punto2[0][0] = punto2[0][0] + 1;
//				}
//				if (punto3[0][0] < 0) {
//					punto3[0][0] = punto3[0][0] + 1;
//				}
//				if (punto4[0][0] < 0) {
//					punto4[0][0] = punto4[0][0] + 1;
//				}
//				if (punto2[0][0] == -1) { //Va a entra a a la matriz el punto
//					matriz1[punto2[0][0] + 1][punto2[0][1]] = 1;
//					punto2[0][0] = punto2[0][0] + 1;
//				}
//				if (punto3[0][0] == -1) {
//					matriz1[punto3[0][0] + 1][punto3[0][1]] = 1;
//					punto3[0][0] = punto3[0][0] + 1;
//				}
//				if (punto4[0][0] == -1) {
//					matriz1[punto4[0][0] + 1][punto4[0][1]] = 1;
//					punto4[0][0] = punto4[0][0] + 1;
//				}
//
//				delay_ms(1000);
//				traducir(matriz1, matriz2, matriz3, matriz4);
//			} else {
////Movimiento de la base por fuera de la matriz
//				matriz1[base1[0][0] + 1][base1[0][1]] = 1; //Prendo luz siguiente, sin apagar la anterior porque no existe
//				base1[0][0] = base1[0][0] + 1;
//				if (punto2[0][0] == -1) {
//					matriz1[punto2[0][0] + 1][punto2[0][1]] = 1;
//					punto2[0][0] = punto2[0][0] + 1;
//				}
//				if (punto3[0][0] == -1) {
//					matriz1[punto3[0][0] + 1][punto3[0][1]] = 1;
//					punto3[0][0] = punto3[0][0] + 1;
//				}
//				if (punto4[0][0] == -1) {
//					matriz1[punto4[0][0] + 1][punto4[0][1]] = 1;
//					punto4[0][0] = punto4[0][0] + 1;
//				}
//
//				delay_ms(1000);
//				traducir(matriz1, matriz2, matriz3, matriz4);
//			}
//		}
//	}
//	if (segmento == 1) {
//		if (matriz2[base1[0][0] - 7][base1[0][1]] == 0) {
//			if (base1[0][0] - 8 >= 0) {
//				matriz2[base1[0][0] - 8][base1[0][1]] = 0;
//				matriz2[base1[0][0] - 7][base1[0][1]] = 1;
//				base1[0][0] = base1[0][0] + 1;
//				if (punto2[0][0] - 8 >= 0) {
//					matriz2[punto2[0][0] - 8][punto2[0][1]] = 0;
//					matriz2[punto2[0][0] - 7][punto2[0][1]] = 1;
//					punto2[0][0] = punto2[0][0] + 1;
//				}
//				if (punto3[0][0] - 8 >= 0) {
//					matriz2[punto3[0][0] - 8][punto3[0][1]] = 0;
//					matriz2[punto3[0][0] - 7][punto3[0][1]] = 1;
//					punto3[0][0] = punto3[0][0] + 1;
//				}
//				if (punto4[0][0] - 8 >= 0) {
//					matriz2[punto4[0][0] - 8][punto4[0][1]] = 0;
//					matriz2[punto4[0][0] - 7][punto4[0][1]] = 1;
//					punto4[0][0] = punto4[0][0] + 1;
//				}
//				if (punto2[0][0] - 8 < 0) {
//					punto2[0][0] = punto2[0][0] + 1;
//					matriz1[punto2[0][0]][punto2[0][1]] = 0;
//				}
//				if (punto3[0][0] - 8 < 0) {
//					punto3[0][0] = punto3[0][0] + 1;
//					matriz1[punto3[0][0]][punto3[0][1]] = 0;
//				}
//				if (punto4[0][0] - 8 < 0) {
//					punto4[0][0] = punto4[0][0] + 1;
//					matriz1[punto3[0][0]][punto3[0][1]] = 0;
//				}
//
//				delay_ms(1000);
//				traducir(matriz1, matriz2, matriz3, matriz4);
//			} else {
//				matriz2[base1[0][0] - 7][base1[0][1]] = 1;
//				base1[0][0] = base1[0][0] + 1;
//				matriz1[base1[0][0]][base1[0][1]] = 0;
//				if (punto2[0][0] - 8 < 0) {
//					punto2[0][0] = punto2[0][0] + 1;
//					matriz1[punto2[0][0]][punto2[0][1]] = 0;
//				}
//				if (punto3[0][0] - 8 < 0) {
//					punto3[0][0] = punto3[0][0] + 1;
//					matriz1[punto3[0][0]][punto3[0][1]] = 0;
//				}
//				if (punto4[0][0] - 8 < 0) {
//					punto4[0][0] = punto4[0][0] + 1;
//					matriz1[punto3[0][0]][punto3[0][1]] = 0;
//				}
//
//				delay_ms(1000);
//				traducir(matriz1, matriz2, matriz3, matriz4);
//			}
//		}
//	}
//}
//void base2(int base1[1][2], int base2[1][2], int punto3[1][2], int punto4[1][2],
//		int matriz1[8][8], int matriz2[8][8], int matriz3[8][8],
//		int matriz4[8][8]) {
//
//}
//void base3(int base1[1][2], int base2[1][2], int base3[1][2], int punto4[1][2],
//		int matriz1[8][8], int matriz2[8][8], int matriz3[8][8],
//		int matriz4[8][8]) {
//
//}
//void base4(int base1[1][2], int punto2[1][2], int punto3[1][2],
//		int punto4[1][2], int matriz1[8][8], int matriz2[8][8],
//		int matriz3[8][8], int matriz4[8][8]) {
//
//}
void mover(int punto[1][2], int matriz[32][8]) {
	if (matriz[punto[0][0] + 1][punto[0][1]] == 0) {
		if (punto[0][0] >= 0) {
			matriz[punto[0][0]][punto[0][1]] = 0;
			matriz[punto[0][0] + 1][punto[0][1]] = 1;
			punto[0][0] = punto[0][0] + 1;
		} else {
			matriz[punto[0][0] + 1][punto[0][1]] = 1;
			punto[0][0] = punto[0][0] + 1;
		}
	}

}
void traducir(int matriz[32][8]) {
		for (int i = 0; i < 8; i++) {
			int aux = 0;
			for (int j = 0; j < 8; j++) {
				aux |= (matriz[j][i] << j);
			}
			MatrixLedMod1(i + 1, aux);
		}
		for (int k = 7; k < 16; k++) {
			int aux =0;
			for (int j = 0; j < 8; j++) {
				aux |= (matriz[j][k] << j);
			}
			MatrixLedMod2(k + 1, aux);
		}
		for (int l = 16; l < 24; l++) {
			int aux = 0;
			for (int j = 0; j < 8; j++) {
				aux |= (matriz[j][l] << j);
			}
			MatrixLedMod3(l + 1, aux);
		}
		for (int m = 24; m < 32; m++) {
			int aux = 0;
			for (int j = 0; j < 8; j++) {
				aux |= (matriz[j][m] << j);
			}
			MatrixLedMod4(m + 1, aux);
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

