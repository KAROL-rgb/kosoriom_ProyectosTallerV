/*
 * AccelTest_Main.c
 *
 *  Created on: 24/05/2023
 *      Author: karol
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PwmDriver.h"
#include "I2CDriver.h"
#include "arm_math.h"
#include "PLLDriver.h"
#include "LCDDriver.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerLedOK            = {0};
GPIO_Handler_t handlerPinTX            = {0};
GPIO_Handler_t handlerPinRX            = {0};

BasicTimer_Handler_t handlerStateOKTimer    = {0};
BasicTimer_Handler_t handlerTimerMuestreo   = {0};
BasicTimer_Handler_t handlerPwm = {0};

PLL_Handler_t handlerPLL = {0};
/*Elemento para hacer la comunicación serial*/

USART_Handler_t handlerCommTerminal          = {0};
uint8_t rxData = 0;
char bufferData[64] = "Accel MPU-6050 testing...";

/*Configuración para el I2C*/
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;
// Para el muestreo
uint16_t contador = {0};
uint8_t flag = 0;
// Handler PWM
PWM_Handler_t handlerPwmX = {0};
PWM_Handler_t handlerPwmY = {0};
PWM_Handler_t handlerPwmZ = {0};
// Pines PWM
GPIO_Handler_t handlerPinCH1 = {0};
GPIO_Handler_t handlerPinCH2 = {0};
GPIO_Handler_t handlerPinCH3 = {0};

uint16_t duttyValueX = 30000;
uint16_t duttyValueY = 30000;
uint16_t duttyValueZ = 30000;

uint8_t flagPwm = 0;


// Handler para la LCD
GPIO_Handler_t handlerSDA_LCD  = {0};
GPIO_Handler_t handlerSCL_LCD = {0};
BasicTimer_Handler_t handlerTimerLCD = {0};
I2C_Handler_t handlerLCD = {0};
uint8_t flagLCD = {0};


float arrayX[2000]={0};
float arrayY[2000]={0};
float arrayZ[2000]={0};

#define ACCEL_ADDRESS     0x1D // 0xD2 -> Dirección del Accel con Logic_1
#define ACCEL_XOUT_L      50      // 0x3B
#define ACCEL_XOUT_H      51      // 0x3C
#define ACCEL_YOUT_L      52      // 0x3D
#define ACCEL_YOUT_H      53      // 0x3E
#define ACCEL_ZOUT_L      54      // 0x3F
#define ACCEL_ZOUT_H      55      // 0x40

#define BW_RATE          44
#define POWER_CTL        45
#define WHO_AM_I          0


//Definición de las cabeceras de las funciones del main
void initSystem(void);
void acelerometro(void);
void generarPwm(void);
void initLCD(void);
void accelLCD(void);
/**
 * Función principal del programa.
 */

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

	initLCD();
	// Se cambia la velocidad de muestreo
	i2c_writeSingleRegister(&handlerAccelerometer, BW_RATE, 0xF);

	// Se imprime el mensaje de inicio
	writeMsg(&handlerCommTerminal, bufferData);

	while (1){
		acelerometro();
		generarPwm();
		accelLCD();
		}
	}

void initSystem(void){

	 handlerPLL.frecSpeed = FRECUENCY_80MHz;
	 PLL_Config(&handlerPLL);
	 config_SysTick_ms(2);

	//Configurando el pin para el Led Blinky
	SCB->CPACR |= (0xF << 20);

	handlerLedOK.pGPIOx                              = GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLedOK);
	//Llevamos el LED a un estado de encendido (ON o SET)
	GPIO_WritePin(&handlerLedOK, SET);

	/*Configurando el timer*/
	handlerStateOKTimer.ptrTIMx                          = TIM2;
	handlerStateOKTimer.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP;
	handlerStateOKTimer.TIMx_Config.TIMx_speed           = BTIMER_80MHz_SPEED_100us;
	handlerStateOKTimer.TIMx_Config.TIMx_period          = 2500;
	handlerStateOKTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerStateOKTimer);

	/*Configurando los pines sobre los que funciona el I2C1*/
	handlerI2cSCL.pGPIOx                                 = GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber          = PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4;
	GPIO_Config(&handlerI2cSCL);

	/*Configurando los pines sobre los que funciona el I2C1*/
	handlerI2cSDA.pGPIOx                                 = GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber          = PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4;
	GPIO_Config(&handlerI2cSDA);

	//Configuración Timer para 1kHz
	handlerTimerMuestreo.ptrTIMx                           = TIM4;
	handlerTimerMuestreo.TIMx_Config.TIMx_mode		       = BTIMER_MODE_UP;
	handlerTimerMuestreo.TIMx_Config.TIMx_speed		       = BTIMER_80MHz_SPEED_100us;
	handlerTimerMuestreo.TIMx_Config.TIMx_period		   = 10;                // Interrupcion cada 10 ms
	handlerTimerMuestreo.TIMx_Config.TIMx_interruptEnable  = ENABLE;
	BasicTimer_Config(&handlerTimerMuestreo);

	/*Configurando el acelerómetro*/

	handlerAccelerometer.ptrI2Cx                         = I2C1;
	handlerAccelerometer.modeI2C                         = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                    = ACCEL_ADDRESS;
	handlerAccelerometer.mainClock                       = MAIN_CLOCK_80_MHz_FOR_I2C;
	handlerAccelerometer.maxI2C_FM                       = I2C_MAX_RISE_80_TIME_FM;
	handlerAccelerometer.modeI2C_FM                      = I2C_MODE_FM_80_SPEED_400KHz;
	i2c_config(&handlerAccelerometer);

	/*Configurando la comunicación serial*/
	//Pin de transmisión
	handlerPinTX.pGPIOx                                  = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber           = PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinTX);

	//Pin de recepción
	handlerPinRX.pGPIOx                                  = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber           = PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinRX);

	//USART 1
	handlerCommTerminal.ptrUSARTx                                 = USART1;
	handlerCommTerminal.USART_Config.USART_baudrate               = USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize               = USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity                 = USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits               = USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode                   = USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX            = ENABLE;
	handlerCommTerminal.USART_Config.USART_enableIntTX            = DISABLE;
	USART_Config(&handlerCommTerminal);

	// Configuración señal PWM
	handlerPwmX.ptrTIMx          = TIM3;
	handlerPwmX.config.channel   = PWM_CHANNEL_1;
	handlerPwmX.config.duttyCicle = duttyValueX;
	handlerPwmX.config.periodo   = 60000;
	handlerPwmX.config.prescaler = 80;
	pwm_Config(&handlerPwmX);
	enableOutput(&handlerPwmX);
	startPwmSignal(&handlerPwmX);

	handlerPwmY.ptrTIMx          = TIM3;
	handlerPwmY.config.channel   = PWM_CHANNEL_2;
	handlerPwmY.config.duttyCicle = duttyValueY;
	handlerPwmY.config.periodo   = 60000;
	handlerPwmY.config.prescaler = 80;
	pwm_Config(&handlerPwmY);
	enableOutput(&handlerPwmY);
	startPwmSignal(&handlerPwmY);

	handlerPwmZ.ptrTIMx          = TIM3;
	handlerPwmZ.config.channel   = PWM_CHANNEL_3;
	handlerPwmZ.config.duttyCicle = duttyValueZ;
	handlerPwmZ.config.periodo   = 60000;
	handlerPwmZ.config.prescaler = 80;
	pwm_Config(&handlerPwmZ);
	enableOutput(&handlerPwmZ);
	startPwmSignal(&handlerPwmZ);

	// Configuración de los Pines del PWM
	handlerPinCH1.pGPIOx               = GPIOC;
	handlerPinCH1.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	handlerPinCH1.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_ALTFN;
	handlerPinCH1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinCH1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinCH1.GPIO_PinConfig.GPIO_PinSpeed    =GPIO_OSPEED_FAST;
	handlerPinCH1.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&handlerPinCH1);

	handlerPinCH2.pGPIOx                              = GPIOC;
	handlerPinCH2.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPinCH2.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_ALTFN;
	handlerPinCH2.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPinCH2.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPinCH2.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
	handlerPinCH2.GPIO_PinConfig.GPIO_PinAltFunMode   = AF2;
	GPIO_Config(&handlerPinCH2);

	handlerPinCH3.pGPIOx                             = GPIOC;
	handlerPinCH3.GPIO_PinConfig.GPIO_PinNumber      = PIN_8;
	handlerPinCH3.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	handlerPinCH3.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPinCH3.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinCH3.GPIO_PinConfig.GPIO_PinSpeed       =GPIO_OSPEED_FAST;
	handlerPinCH3.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2;
	GPIO_Config(&handlerPinCH3);

	// Configuración LCD

	handlerSCL_LCD.pGPIOx        					  = GPIOB;
	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_10;
	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinMode  	  = GPIO_MODE_ALTFN;
	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinOPType	  = GPIO_OTYPE_OPENDRAIN;
	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;
	GPIO_Config(&handlerSCL_LCD);

	handlerSDA_LCD.pGPIOx        					  = GPIOB;
	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_3;
	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinMode  	  = GPIO_MODE_ALTFN;
	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinOPType	  = GPIO_OTYPE_OPENDRAIN;
	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF9;
	GPIO_Config(&handlerSDA_LCD);

	handlerTimerLCD.ptrTIMx                          = TIM5;
	handlerTimerLCD.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP;
	handlerTimerLCD.TIMx_Config.TIMx_speed           = BTIMER_80MHz_SPEED_100us;
	handlerTimerLCD.TIMx_Config.TIMx_period          = 10000;
	handlerTimerLCD.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerTimerLCD);

	handlerLCD.ptrI2Cx      = I2C2;
	handlerLCD.modeI2C      = I2C_MODE_FM;
	handlerLCD.slaveAddress = 0x24;
	handlerLCD.mainClock    = MAIN_CLOCK_80_MHz_FOR_I2C;
	handlerLCD.maxI2C_FM    = I2C_MAX_RISE_80_TIME_FM;
	handlerLCD.modeI2C_FM   = I2C_MODE_FM_80_SPEED_400KHz;
	i2c_config(&handlerLCD);
	LCD_Init(&handlerLCD);



}
void acelerometro(void){

	flag = 1;
	i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);

	if(contador < 2000){
		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

		arrayX[contador] = (AccelX/256.f)*9.8;
		arrayY[contador] = (AccelY/256.f)*9.8;
		arrayZ[contador] = (AccelZ/256.f)*9.8;

	// Hacemos un "eco con el valor que nos llega por el serial
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
		if(rxData == 'p'){
			sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
			writeMsg(&handlerCommTerminal, bufferData);

			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, POWER_CTL);
			sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';
		}
		if(rxData == 'r'){ //Para resetear al equipo
			sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
			writeMsg(&handlerCommTerminal, bufferData);

			i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);
			rxData = '\0';
		}
		if(rxData == 'x'){ //lecturas en x
			sprintf(bufferData, "Axis X data (r)\n");
			writeMsg(&handlerCommTerminal, bufferData);

			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
			int16_t AccelX = AccelX_high << 8 | AccelX_low;
			float valorX = (AccelX/220.f)*9.8;
			sprintf(bufferData, "AccelX = %.2f \n", valorX);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';

		}
		if(rxData == 'y'){ //lecturas en y
			sprintf(bufferData, "Axis Y data (r)\n");
			writeMsg(&handlerCommTerminal, bufferData);

			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
			int16_t AccelY = AccelY_high << 8 | AccelY_low;
			float valorY = (AccelY/256.f)*9.8;
			sprintf(bufferData, "AccelY = %.2f \n", valorY);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';
		}
		if(rxData == 'z'){ //lecturas en y
			sprintf(bufferData, "Axis Z data (r)\n");
			writeMsg(&handlerCommTerminal, bufferData);

			uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
			uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
			int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
			float valorZ = (AccelZ/256.f)*9.8;
			sprintf(bufferData, "AccelZ = %.2f \n", valorZ);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'k'){
			writeMsg(&handlerCommTerminal, "Mostrar los datos del acelerómetro \n");
			for (int i= 0; i<2000; i++){
				sprintf(bufferData, "%u arrayX = %.2f m/s2 ; arrayY = %.2f m/s2; arrayZ = %.2f m/s2 \n",i, arrayX[i], arrayY[i],arrayZ[i]);
				writeMsg(&handlerCommTerminal, bufferData);
			}
		}
		else{
			//sprintf(bufferData, "%d \n", rxData);
//			writeChar(&handlerCommTerminal, rxData);
			rxData = '\0';
		}

	}
	}

}

void generarPwm(void){
//	if(flagPwm == 1)

		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;
		float valorX = (AccelX/256.f)*9.8;

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;
		float valorY = (AccelY/256.f)*9.8;

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
		float valorZ = (AccelZ/256.f)*9.8;

		duttyValueX = 1020*(valorX) + 30000;
		duttyValueY = 1020*(valorY) + 30000;
		duttyValueZ = 1020*(valorZ) + 30000;

		updateDuttyCycle(&handlerPwmX, duttyValueX);
		updateDuttyCycle(&handlerPwmY, duttyValueY);
		updateDuttyCycle(&handlerPwmZ, duttyValueZ);
//		flagPwm =! flagPwm;


}
void initLCD(void){

	LCD_setCursor(&handlerLCD, 0, 0);
	LCD_sendSTR(&handlerLCD, "Eje X = ");
	LCD_setCursor(&handlerLCD, 0, 16);
	LCD_sendSTR(&handlerLCD, "m/s2");
	LCD_setCursor(&handlerLCD, 1, 0);
	LCD_sendSTR(&handlerLCD, "Eje Y = ");
	LCD_setCursor(&handlerLCD, 1, 16);
	LCD_sendSTR(&handlerLCD, "m/s2");
	LCD_setCursor(&handlerLCD, 2, 0);
	LCD_sendSTR(&handlerLCD, "Eje Z = ");
	LCD_setCursor(&handlerLCD, 2, 16);
	LCD_sendSTR(&handlerLCD, "m/s2");
	LCD_setCursor(&handlerLCD, 3, 0);
	LCD_sendSTR(&handlerLCD, "OFSZ MAX 250mg");

}

void accelLCD(void){

	if(flagLCD == 1){
		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;
		float valorX = (AccelX/220.f)*9.8;
		sprintf(bufferData, "%.2f \n", valorX);
		LCD_setCursor(&handlerLCD, 0, 9);
		LCD_sendSTR(&handlerLCD, bufferData);

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;
		float valorY = (AccelY/256.f)*9.8;
		sprintf(bufferData, "%.2f \n", valorY);
		LCD_setCursor(&handlerLCD, 1, 9);
		LCD_sendSTR(&handlerLCD, bufferData);

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
		float valorZ = (AccelZ/256.f)*9.8;
		sprintf(bufferData, " %.2f \n", valorZ);
		LCD_setCursor(&handlerLCD, 2, 9);
		LCD_sendSTR(&handlerLCD, bufferData);
		LCD_setCursor(&handlerLCD, 3, 0);

		flagLCD =! flagLCD;
	}

}

/*
 * Callback del Timer2 -Hacemos un blinky
 */

void BasicTimer2_Callback(void){
	//Hacemos uduttyn blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);

}
/* Callback relacionado con el Timer para 1kHz */
void BasicTimer4_Callback(void){
	if(flag == 1){
		contador ++;
	}
	if(contador == 2000){
		rxData = '\0';
		contador = 0;

	}
}
void BasicTimer5_Callback(void){

	flagLCD =! flagLCD;
}
//void BasicTimer3_Callback(void){
//	flagPwm =! flagPwm;
//}

/* Callback relacionado con la recepción del USART2
 * Debo leer el puerto para bajar la bandera de la interrupción
 * */
void usart1Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción
	rxData = getRXData();
}
