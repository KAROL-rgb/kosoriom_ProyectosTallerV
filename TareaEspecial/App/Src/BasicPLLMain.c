/* @file           : BasicProject_Main.c
 * @author         : Karol Osorio Monsalve - kosoriom@unal.edu.co
 * @brief          : Solución básica de un proyecto con librerías externas */


#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
//#include "SysTickDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"

#include <math.h>

/* Definición de variables */
GPIO_Handler_t handlerLedOK = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerStateOKTimer = {0};

PLL_Handler_t handlerPLL = {0};

USART_Handler_t  handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferData[64] = "Accel MPU-6050 TESTING...";

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint32_t systemTicksEnd = 0;

/* Configuración para el I2C */
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

#define ACCEL_ADDRESS     0b1101001   // 0xD2 -> Dirección del Accel con Logic_1
#define ACCEL_XOUT_H      59      // 0x3B
#define ACCEL_XOUT_L      60      // 0x3C
#define ACCEL_YOUT_H      61      // 0x3D
#define ACCEL_YOUT_L      62      // 0x3E
#define ACCEL_ZOUT_H      63      // 0x3F
#define ACCEL_ZOUT_L      64      // 0x40

#define PWR_MGMT_1        107
#define WHO_AM_I          117

/* Definición de prototipos de funciones */
void initSystem(void);

int main(void)
{
	SCB->CPACR |= (0xF <<20);
	// Llamamos a la función que nos inicializa el hardware del sistema
	initSystem();

	/* Imprimir un mensaje de inicio */
	writeMsg(&handlerCommTerminal, bufferData);

	while(1){

		// Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&handlerCommTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'p'){
				sprintf(bufferData, "PWRR_MGMT_1 state (r)\n");
				writeMsg(&handlerCommTerminal, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'r'){ //Para resetear al equipo
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&handlerCommTerminal, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}
			else if(rxData == 'x'){ //lecturas en x
				sprintf(bufferData, "Axis X data (r)\n");

				writeMsg(&handlerCommTerminal, bufferData);

				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;

				sprintf(bufferData, "AccelX = %.2f \n", (int) (AccelX/256.f)*9.78);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){ //lecturas en y
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&handlerCommTerminal, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;

				sprintf(bufferData, "AccelY = %.2f \n", (int) (AccelY/256.f)*9.78);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){ //lecturas en y
				sprintf(bufferData, "Axis Z data (r)\n");

				writeMsg(&handlerCommTerminal, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

				sprintf(bufferData, "AccelZ = %.2f \n", (int) (AccelZ/256.f)*9.78);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else{
				rxData = '\0';
			}
		}
	}
	return 0;
}


void initSystem(void){


	 handlerPLL.frecSpeed = FRECUENCY_80MHz;
	 PLL_Config(&handlerPLL);
	//Configurando el pin para el Led Blinky
	handlerLedOK.pGPIOx                              = GPIOH;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber       = PIN_1;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLedOK);

	//Llevamos el LED a un estado de encendido (ON o SET)
	GPIO_WritePin(&handlerLedOK, SET);

	/*Configurando la comunicación serial*/
	//Pin de transmisión
	handlerPinTX.pGPIOx                                  = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber           = PIN_15;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinTX);

	//Pin de recepción
	handlerPinRX.pGPIOx                                  = GPIOB;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber           = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinRX);

	// Configuración comunicación serial
	handlerCommTerminal.ptrUSARTx                                 = USART1;
	handlerCommTerminal.USART_Config.USART_baudrate               = USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize               = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity                 = USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits               = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode                   = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX            = ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX            = DISABLE;
	USART_Config(&handlerCommTerminal);

	/*Configurando el timer*/
	handlerStateOKTimer.ptrTIMx                          = TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed           = BTIMER_80MHz_SPEED_100us;
	handlerStateOKTimer.TIMx_Config.TIMx_period          = 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerStateOKTimer);




	/*Configurando los pines sobre los que funciona el I2C1*/
	handlerI2cSCL.pGPIOx                                 = GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber          = PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_HIGH;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4;
	GPIO_Config(&handlerI2cSCL);

	/*Configurando los pines sobre los que funciona el I2C1*/
	handlerI2cSDA.pGPIOx                                 = GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber          = PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_HIGH;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4;
	GPIO_Config(&handlerI2cSDA);


	/*Configurando el acelerómetro*/

	handlerAccelerometer.ptrI2Cx                         = I2C1;
	handlerAccelerometer.modeI2C                         = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                    = ACCEL_ADDRESS;
	handlerAccelerometer.mainClock                       = MAIN_CLOCK_80_MHz_FOR_I2C;
	handlerAccelerometer.maxI2C_FM                       = I2C_MAX_RISE_80_TIME_FM;
	handlerAccelerometer.modeI2C_FM                      = I2C_MODE_SM_80_SPEED_400KHz;



	i2c_config(&handlerAccelerometer);


}

/* Callback relacionado con la recepción del USART2
 * Debo leer el puerto para bajar la bandera de la interrupción
 * */
void usart1Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción
	rxData = getRXData();
}

/*
 * Callback del Timer2
 */

void BasicTimer2_Callback(void){
	//Hacemos un blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);
}










