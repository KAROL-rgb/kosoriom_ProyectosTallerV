/*
 * PruebaJoystick.c
 *
 *  Created on: 20/06/2023
 *      Author: karol
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
SPI_Handler_t handlerSpiPort = { 0 };
BasicTimer_Handler_t handlerDelay = { 0 };
uint32_t counterDelay = 0;
BasicTimer_Handler_t handlerStateOKTimer = { 0 };
PWM_Handler_t handlerPWM = { 0 };
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t hnadlerPinPWM = { 0 };
ADC_Config_t adcConfig = { 0 };
EXTI_Config_t extiButton = { 0 };    // Definición del exti el botón del encoder
uint8_t estado = 1;
GPIO_Handler_t handlerSW = { 0 }; // PA7 (Conecta al SW)
int joystick[2] = { 0 };
int axisX = 0;
int axisY = 0;
int SW = 0;
uint16_t adcData = 0;
uint8_t aux = 0;
uint8_t adcIsComplete = RESET;
uint32_t counterButton = 0;
int test = 0;
int move = 0;

uint8_t MatrixLed_writeData(uint8_t digit, uint8_t seg);

void traducir(int matriz[32][8]);

void mover(int punto[1][2], int matriz[32][8]);

int movimientoPalito(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoTGiro2(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoS(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int lInvertida(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoCubo(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoZ(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoL(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]);

int movimientoLInvertidaGiro1(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]);

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

	/* Configuración del botón, Pin PA9 (SW) */
	handlerSW.pGPIOx = GPIOA;
	handlerSW.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSW);

	handlerPWM.ptrTIMx = TIM2;
	handlerPWM.config.channel = PWM_CHANNEL_3;
	handlerPWM.config.duttyCicle = 33;
	handlerPWM.config.periodo = 60;
	handlerPWM.config.prescaler = 100;

	// Activar señal
	pwm_Config(&handlerPWM);
	enableOutput(&handlerPWM);
	startPwmSignal(&handlerPWM);

	/* Se carga la configuración del EXTI del botón */
	extiButton.pGPIOHandler = &handlerSW;
	extiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiButton);
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

	initSystem();
	config_SysTick_ms(0);
	SPI_Configuration();
	clean();
	clearMatrix();
//	adcComplete_Callback();

	int matriz[32][8] = { 0 };

	int punto1[1][2];
	int punto2[1][2];
	int punto3[1][2];
	int punto4[1][2];

	// L invertida giro 1
	punto1[0][0] = -1;
	punto1[0][1] = 3;
	punto2[0][0] = -1;
	punto2[0][1] = 4;
	punto3[0][0] = -1;
	punto3[0][1] = 5;
	punto4[0][0] = -3;
	punto4[0][1] = 3;

	while (1) {
		adcData = getADC();
		aux++;
		// Leemos los valores de la conversión ADC
		if (aux == 1) {
			joystick[0] = adcData;
			if (adcData < (3.3 * 0.07)) {

				//	 	int test = movimientoPalitoGiro(punto1, punto2, punto3, punto4, matriz);
				int test = movimientoLInvertidaGiro1(punto1, punto2, punto3,
						punto4, matriz);

				if (test == 1) {
					delay_ms(500);
					traducir(matriz);
				} else {
					break;
				}
			}
		} else if (aux == 2) {
			joystick[1] = adcData;
			adcIsComplete = SET;
			aux = 0;
		} else {
			__NOP();
		}

	}

//-1
//	while(1){
//		if(counterButton==1){                    // Interrupción externa del exti del botón SW
//			estado++;
//		}
//		counterButton = 0;
//
//		axisX = joystick[0]-2030;
//		axisY = joystick[1]-2024;
//		if (adcIsComplete == SET){
//
////			sprintf(bufferData, "ADC X = %d Y = %d \n\r", (int) axisX, (int) axisY);
////			writeMsg(&handlerUsart2, bufferData);
//			adcIsComplete = RESET;
//
//	    }
//	}
	return 0;
}

int movimientoTGiro2(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
			&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0 && punto3[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoPalito(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (matriz[punto1[0][0] + 1][punto1[0][1]] == 0) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto
			if (punto2[0][0] >= 0) {
				matriz[punto2[0][0]][punto2[0][1]] = 0;
				matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
				punto2[0][0] = punto2[0][0] + 1;
			}
			if (punto3[0][0] >= 0) {
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] >= 0) {
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}
			if (punto2[0][0] < -1) {
				punto2[0][0] = punto2[0][0] + 1;
			}
			if (punto3[0][0] < -1) {
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) {
				punto4[0][0] = punto4[0][0] + 1;
			}
			if (punto2[0][0] == -1) { // Va a entra a a la matriz el punto
				matriz[punto2[0][0] + 1][punto2[0][1]] = 1;
				punto2[0][0] = punto2[0][0] + 1;
			}
			if (punto3[0][0] == -1) {
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] == -1) {
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}
			return 1;
		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		}
	} else {
		return 0;
	}
}
int lInvertida(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto3[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoL(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto3[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoCubo(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto3[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto3[0][0]][punto3[0][1]] = 0;
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto3[0][0] = punto3[0][0] + 1;
			}
			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto3[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
				punto3[0][0] = punto3[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoS(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1 && punto2[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0 && punto2[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
int movimientoLInvertidaGiro1(int punto1[1][2], int punto2[1][2],
		int punto3[1][2], int punto4[1][2], int matriz[32][8]) {
	if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
			&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
			&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.
		if (punto1[0][0] >= 0 && punto2[0][0] >= 0 && punto3[0][0] >= 0) {
			// Movimiento dentro de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			if (punto4[0][0] >= 0) { //Si se mueve dentro de la matriz, hara lo mismo que las bases
				matriz[punto4[0][0]][punto4[0][1]] = 0;
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] < -1) { //Si esta por fuera, solo caera, sin iluminar
				punto4[0][0] = punto4[0][0] + 1;
			}

			if (punto4[0][0] == -1) { //Si esta a punto de entrar en la matriz
				matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
				punto4[0][0] = punto4[0][0] + 1;
			}

			return 1; //SI SE PUDO BAJAR

		} else {
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe

			punto1[0][0] = punto1[0][0] + 1; //Se suma +1 en las filas de cada punto
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SE PUDO BAJAR
		}
	} else {
		return 0; // NO SE PUDO BAJAR
	}
}
int movimientoZ(int punto1[1][2], int punto2[1][2], int punto3[1][2],
		int punto4[1][2], int matriz[32][8]) {
	if (punto1[0][0] == -1 && punto2[0][0] == -1) {	//Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Prendo luz siguiente, sin apagar la anterior porque no existe
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1;

			punto1[0][0] = punto1[0][0] + 1;
			punto2[0][0] = punto2[0][0] + 1;
			punto3[0][0] = punto3[0][0] + 1;
			punto4[0][0] = punto4[0][0] + 1;
			return 1;
		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	}
	if (punto1[0][0] == 0 && punto2[0][0] == 0) { //Solo se verifica la base en el primer paso
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) {	// Si la matriz en la fila siguiente, misma columna, esta desocupada.
			// Movimiento de la base por fuera de la matriz
			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0] + 1][punto3[0][1]] = 1;
			punto3[0][0] = punto3[0][0] + 1;

			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1;

		} else {
			return 0; //Se perdio el juego, pues no se puede ingresar una nueva figura
		}
	} else {
		if ((matriz[punto1[0][0] + 1][punto1[0][1]] == 0)
				&& (matriz[punto2[0][0] + 1][punto2[0][1]] == 0)
				&& (matriz[punto3[0][0] + 1][punto3[0][1]] == 0)) { // Si la matriz en la fila siguiente, misma columna, esta desocupada.

			matriz[punto1[0][0]][punto1[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto1[0][0] + 1][punto1[0][1]] = 1; // Se prende la siguiente
			punto1[0][0] = punto1[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto2[0][0]][punto2[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto2[0][0] + 1][punto2[0][1]] = 1; // Se prende la siguiente
			punto2[0][0] = punto2[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto3[0][0]][punto3[0][1]] = 0;	// Se apaga la luz donde estoy
			matriz[punto3[0][0] + 1][punto3[0][1]] = 1; // Se prende la siguiente
			punto3[0][0] = punto3[0][0] + 1; // Le sumo 1 a la fila del punto

			matriz[punto4[0][0]][punto4[0][1]] = 0;
			matriz[punto4[0][0] + 1][punto4[0][1]] = 1;
			punto4[0][0] = punto4[0][0] + 1;

			return 1; //SI SE PUDO BAJAR
		} else {
			return 0;
		}
	}
	return 0;
}
//int movimientoCubo(int punto1[1][2], int punto2[1][2], int punto3[1][2],
//		int punto4[1][2], int matriz[32][8]){
//	if (punto1[0][0] == 0 && punto2[0][0] == 0){
//
//	}
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
//		if(joystick[0] > 1.76 * 0.8){
//			int matriz[32][8] = { 0 };
//
//			int punto1[1][2];
//			int punto2[1][2];
//			int punto3[1][2];
//			int punto4[1][2];
//
//			// L invertida giro 1
//			punto1[0][0] = -1;
//			punto1[0][1] = 3;
//			punto2[0][0] = -1;
//			punto2[0][1] = 4;
//			punto3[0][0] = -1;
//			punto3[0][1] = 5;
//			punto4[0][0] = -3;
//			punto4[0][1] = 3;
//
//			while (1) {
//				//	 	int test = movimientoPalitoGiro(punto1, punto2, punto3, punto4, matriz);
//				int test = movimientoLInvertidaGiro1(punto1, punto2, punto3,
//						punto4, matriz);
//
//				if (test == 1) {
//					delay_ms(500);
//					traducir(matriz);
//				} else {
//					break;
//				}
//			}
//		}
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
