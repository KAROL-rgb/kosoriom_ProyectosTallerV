/*
 * prueba.c
 *
 *  Created on: 27/05/2023
 *      Author: karol
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "I2CDriver.h"

#include "arm_math.h"
#include <math.h>


#define HSI_CLOCK_CONFIGURED		0
#define HSE_CLOCK_CONFIGURED		1
#define PLL_CLOCK_CONFIGURED		2



//Definicion de handlers

GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t handlerUserButtonExti = {0};

GPIO_Handler_t handlerBlinkyPin = {0};	//PA5

/* Elemento para hacer la comunicacion serial */
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerUsart1 = {0};

PLL_Handler_t handlerPLL = {0};

GPIO_Handler_t handler_i2cSDA = {0};	//PB8
GPIO_Handler_t handler_i2cSCL = {0}; //PB9

I2C_Handler_t handlerAccelerometer = {0};



BasicTimer_Handler_t	handlerBlinkyTimer	= {0};  //TIM2

uint8_t rxData = 0;

char bufferData[64] = "Accel ADXL345 testing...";


uint8_t sendMsg = 0;
uint8_t counter_dummy = 0;
uint8_t usart2DataReceived = 0;
uint8_t i2cBuffer = {0};

uint16_t counterSampling = {0};
uint8_t counterFlag = {0};


#define ACCEL_ADDRESS          	 0x1D
#define ACCEL_XOUT_L             50     //DATAX0
#define ACCEL_XOUT_H             51     //DATAX1
#define ACCEL_YOUT_L             52     //DATAYO
#define ACCEL_YOUT_H             53     //DATAY1
#define ACCEL_ZOUT_L             54     //DATAZ0
#define ACCEL_ZOUT_H             55     //DATAZ1

#define POWER_CTL                45
#define WHO_AM_I                 0      //DEVID


// Definicion de las cabeceras de las funciones del main
void init_Hardware(void);
/*
 * Funcion principal del programa
 * Esta funcion es el corazon del programa!!
 */

int main(void){
	//Activamos el coprocesador matematico
	SCB->CPACR |= (0xF <<20);

	//Inicializamos todos los elementos del sistema
	init_Hardware();

	config_SysTick_ms(0);

	writeMsg(&usart6Comm, bufferData);

	while(1){
		if(rxData != '\0'){
			writeChar(&usart6Comm, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&usart6Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart6Comm, bufferData);
				rxData = '\0';
			}
			else if (rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&usart6Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, POWER_CTL);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart6Comm, bufferData);
				rxData = '\0';
			}
			else if (rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&usart6Comm, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL , 0x2D);
				rxData = '\0';
			}
			else if (rxData == 'x'){
				sprintf(bufferData, "Axis X data (r) \n");
				writeMsg(&usart6Comm, bufferData);

				uint8_t AccelX_low =  i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %.2f \n", (float) (AccelX/256.f)*9.78);
				writeMsg(&usart6Comm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&usart6Comm, bufferData);
				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer,ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %.2f \n", (AccelY/256.f)*9.78);
				writeMsg(&usart6Comm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsg(&usart6Comm, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %.2f \n",  (AccelZ/256.f)*9.78);
				writeMsg(&usart6Comm, bufferData);
				rxData = '\0';
			}
			else{
				rxData = '\0';
			}
		}

	}
	return(0);
}//Fin del Main



//Funcion que configura el hardware, timers y extis
void init_Hardware(void){

	//Configuracion del BLinky
	handlerBlinkyPin.pGPIOx											= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	handlerUserButton.pGPIOx										= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber					= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

/* ==================================== Configurando los TIMERS =============================================*/
	//Configurando el TIM2 el cual le da la tasa de encendido al LED
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_80MHz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);
/* ==================================== Configurando los EXTI =============================================*/
	handlerUserButtonExti.pGPIOHandler								= &handlerUserButton;
	handlerUserButtonExti.edgeType									= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);
/* ==================================== Configurando los USART =============================================*/
	handlerPinTX.pGPIOx												= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber						= PIN_11;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode					= AF8;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx												= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber						= PIN_12;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode					= AF8;
	GPIO_Config(&handlerPinRX);

	handlerUsart1.ptrUSARTx                                 = USART1;
	handlerUsart1.USART_Config.USART_baudrate               = USART_BAUDRATE_115200;
	handlerUsart1.USART_Config.USART_datasize               = USART_DATASIZE_8BIT;
	handlerUsart1.USART_Config.USART_parity                 = USART_PARITY_NONE;
	handlerUsart1.USART_Config.USART_stopbits               = USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_mode                   = USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_enableIntRX            = ENABLE;
	handlerUsart1.USART_Config.USART_enableIntTX            = DISABLE;
	USART_Config(&handlerUsart1);

/* ========================== PLL ======================================================================*/

	 handlerPLL.frecSpeed = FRECUENCY_80MHz;
	 PLL_Config(&handlerPLL);

	/* ========================== I2C ======================================================================*/
	handlerI2cSCL.pGPIOx                                 = GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber          = PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_HIGH;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4;
	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx                                 = GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber          = PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_HIGH;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4;
	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx                         = I2C1;
	handlerAccelerometer.modeI2C                         = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                    = ACCEL_ADDRESS;
	handlerAccelerometer.mainClock                       = MAIN_CLOCK_80_MHz_FOR_I2C;
	handlerAccelerometer.maxI2C_FM                       = I2C_MAX_RISE_80_TIME_FM;
	handlerAccelerometer.modeI2C_FM                      = I2C_MODE_SM_80_SPEED_400KHz;

	i2c_config(&handlerAccelerometer);
}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
	sendMsg++;
	counter_dummy++;
}

void callback_extInt13(void){
	__NOP();
}

/*
 * Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2
 */

void usart6Rx_Callback(void){
	rxData = getRxData();

}

