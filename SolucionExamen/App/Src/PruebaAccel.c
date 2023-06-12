/*
 * PruebaAccel.c
 *
 *  Created on: 11/06/2023
 *      Author: karol
 */

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
#include "RTCDriver.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerLedOK            = {0};
GPIO_Handler_t handlerPinTX            = {0};
GPIO_Handler_t handlerPinRX            = {0};

BasicTimer_Handler_t handlerStateOKTimer    = {0};
BasicTimer_Handler_t handlerTimerMuestreo   = {0};
BasicTimer_Handler_t handlerPwm = {0};
// Pin
GPIO_Handler_t handlerPC9 = {0};

PLL_Handler_t handlerPLL = {0};
/*Elemento para hacer la comunicación serial*/

USART_Handler_t handlerCommTerminal          = {0};
uint8_t rxData = 0;
char bufferData[64] = "Accel MPU-6050 testing...";

char bufferMsg[128] = {0};

/*Configuración para el I2C*/
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;
// Para el muestreo
uint16_t contador = {0};
uint8_t flag = 0;
uint8_t counter = 0;
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
// RTC
RTC_Handler_t handlerRTC = {0};
#define NUMBER_CHANNELS	2
uint8_t adcIsComplete		= 0;
uint16_t adcCounter			= 0;
uint16_t dataADC[512] = {0};
uint8_t flagADC = {0};
uint16_t data1ADC[256];
uint16_t data2ADC[256];
uint16_t dataCounter = 0;
uint16_t counterData = 0;
uint16_t freqInput = 0;
uint16_t time = 66;
uint16_t dutty = 33;

char bufferReception[64] ={0};
char userMsg[64] = "Funciona";
char cmd[16];

//ADC_Config_t channel_1				= {0};
//ADC_Config_t channel_2				= {0};
GPIO_Handler_t handlerPinMCO1               = {0};
MCO1_Handler_t handlerMCO1 = {0};

uint8_t counterReception = 0;
uint8_t stringComplete = 0;
uint8_t autoUpdateLCD = 0;

unsigned int firstParameter;
unsigned int secondParameter;

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
void parseCommands(char *ptrBufferReception);
/**
 * Función principal del programa.
 */

int main(void){



	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (13 << RCC_CR_HSITRIM_Pos);

	//Inicializamos todos los elementos del sistema
	initSystem();

//	initLCD();
	// Se cambia la velocidad de muestreo
//	i2c_writeSingleRegister(&handlerAccelerometer, BW_RATE, 0xF);

	// Se imprime el mensaje de inicio
	writeMsg(&handlerCommTerminal, bufferData);

	while (1){

//		acelerometro();
//		generarPwm();
//		accelLCD();
		// Creamos una cadena de caracteres con los datos que llegan por el serial
		// El caracter '@' nos indica que es el final de la cadena
		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			// if the incoming character is a newline, set a flag
			// so the main loop can do something about it:
			if (rxData == '@'){
				stringComplete = true;

				// Agrego esta línea para crear el string con el null al final
				bufferReception[counterReception] = '\0';

				counterReception = 0;
			}

			// Para que no vuelva a entrar. Solo cambia debido a la interrupción
			rxData = '\0';
			}

			// Hacemos un análisis de la cadena de datos obtenida
			if(stringComplete){
				parseCommands(bufferReception);
				stringComplete = false;
			}
		}
		return 0;
	}

void initSystem(void){

	handlerPLL.frecSpeed = FRECUENCY_100MHz;
	PLL_Config(&handlerPLL);
//	 config_SysTick_ms(2);

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
	handlerStateOKTimer.TIMx_Config.TIMx_speed           = BTIMER_100MHz_SPEED_100us;
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

	//Configuración Timer para 200Hz
	handlerTimerMuestreo.ptrTIMx                           = TIM4;
	handlerTimerMuestreo.TIMx_Config.TIMx_mode		       = BTIMER_MODE_UP;
	handlerTimerMuestreo.TIMx_Config.TIMx_speed		       = BTIMER_100MHz_SPEED_1ms;
	handlerTimerMuestreo.TIMx_Config.TIMx_period		   = 14;                // Interrupcion cada 5 ms
	handlerTimerMuestreo.TIMx_Config.TIMx_interruptEnable  = ENABLE;
	BasicTimer_Config(&handlerTimerMuestreo);

	/*Configurando el acelerómetro*/

	handlerAccelerometer.ptrI2Cx                         = I2C1;
	handlerAccelerometer.modeI2C                         = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                    = ACCEL_ADDRESS;
	handlerAccelerometer.mainClock                       = MAIN_CLOCK_100_MHz_FOR_I2C;
	handlerAccelerometer.maxI2C_FM                       = I2C_MAX_RISE_TIME_FM_100MHz;
	handlerAccelerometer.modeI2C_FM                      = I2C_MODE_FM_SPEED_400KHz_100MHz;
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

	/****Configuración para probar el MCO1 en el analizador de señales****/
	handlerPinMCO1.pGPIOx                                = GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber         = PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode     = AF0;
	GPIO_Config(&handlerPinMCO1);

	handlerRTC.RTC_Day = 9;
	handlerRTC.RTC_Mounth = 11;
	handlerRTC.RTC_Years  = 22;
	handlerRTC.RTC_Hours = 8;
	handlerRTC.RTC_Minutes = 00;
	handlerRTC.RTC_Seconds = 00;
	RTC_Config(&handlerRTC);

	// PIN
	handlerPC9.pGPIOx = GPIOC;
	handlerPC9.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPC9.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC9.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC9.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC9.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC9.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;
	GPIO_Config(&handlerPC9);

//	// Configuración señal PWM
//	handlerPwmX.ptrTIMx          = TIM3;
//	handlerPwmX.config.channel   = PWM_CHANNEL_1;
//	handlerPwmX.config.duttyCicle = duttyValueX;
//	handlerPwmX.config.periodo   = 60000;
//	handlerPwmX.config.prescaler = 80;
//	pwm_Config(&handlerPwmX);
//	enableOutput(&handlerPwmX);
//	startPwmSignal(&handlerPwmX);
//
//	handlerPwmY.ptrTIMx          = TIM3;
//	handlerPwmY.config.channel   = PWM_CHANNEL_2;
//	handlerPwmY.config.duttyCicle = duttyValueY;
//	handlerPwmY.config.periodo   = 60000;
//	handlerPwmY.config.prescaler = 80;
//	pwm_Config(&handlerPwmY);
//	enableOutput(&handlerPwmY);
//	startPwmSignal(&handlerPwmY);
//
//	handlerPwmZ.ptrTIMx          = TIM3;
//	handlerPwmZ.config.channel   = PWM_CHANNEL_3;
//	handlerPwmZ.config.duttyCicle = duttyValueZ;
//	handlerPwmZ.config.periodo   = 60000;
//	handlerPwmZ.config.prescaler = 80;
//	pwm_Config(&handlerPwmZ);
//	enableOutput(&handlerPwmZ);
//	startPwmSignal(&handlerPwmZ);
//
//	// Configuración de los Pines del PWM
//	handlerPinCH1.pGPIOx               = GPIOC;
//	handlerPinCH1.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
//	handlerPinCH1.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_ALTFN;
//	handlerPinCH1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
//	handlerPinCH1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	handlerPinCH1.GPIO_PinConfig.GPIO_PinSpeed    =GPIO_OSPEED_FAST;
//	handlerPinCH1.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
//	GPIO_Config(&handlerPinCH1);
//
//	handlerPinCH2.pGPIOx                              = GPIOC;
//	handlerPinCH2.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
//	handlerPinCH2.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_ALTFN;
//	handlerPinCH2.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
//	handlerPinCH2.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
//	handlerPinCH2.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
//	handlerPinCH2.GPIO_PinConfig.GPIO_PinAltFunMode   = AF2;
//	GPIO_Config(&handlerPinCH2);
//
//	handlerPinCH3.pGPIOx                             = GPIOC;
//	handlerPinCH3.GPIO_PinConfig.GPIO_PinNumber      = PIN_8;
//	handlerPinCH3.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
//	handlerPinCH3.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
//	handlerPinCH3.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	handlerPinCH3.GPIO_PinConfig.GPIO_PinSpeed       =GPIO_OSPEED_FAST;
//	handlerPinCH3.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2;
//	GPIO_Config(&handlerPinCH3);
//
//	// Configuración LCD
//
//	handlerSCL_LCD.pGPIOx        					  = GPIOB;
//	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_10;
//	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinMode  	  = GPIO_MODE_ALTFN;
//	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinOPType	  = GPIO_OTYPE_OPENDRAIN;
//	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
//	handlerSCL_LCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;
//	GPIO_Config(&handlerSCL_LCD);
//
//	handlerSDA_LCD.pGPIOx        					  = GPIOB;
//	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_3;
//	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinMode  	  = GPIO_MODE_ALTFN;
//	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinOPType	  = GPIO_OTYPE_OPENDRAIN;
//	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
//	handlerSDA_LCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF9;
//	GPIO_Config(&handlerSDA_LCD);
//
////	handlerTimerLCD.ptrTIMx                          = TIM5;
////	handlerTimerLCD.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP;
////	handlerTimerLCD.TIMx_Config.TIMx_speed           = BTIMER_80MHz_SPEED_100us;
////	handlerTimerLCD.TIMx_Config.TIMx_period          = 10000;
////	handlerTimerLCD.TIMx_Config.TIMx_interruptEnable = ENABLE;
////	BasicTimer_Config(&handlerTimerLCD);
//
//	handlerLCD.ptrI2Cx      = I2C2;
//	handlerLCD.modeI2C      = I2C_MODE_FM;
//	handlerLCD.slaveAddress = 0x24;
//	handlerLCD.mainClock    = MAIN_CLOCK_80_MHz_FOR_I2C;
//	handlerLCD.maxI2C_FM    = I2C_MAX_RISE_80_TIME_FM;
//	handlerLCD.modeI2C_FM   = I2C_MODE_FM_80_SPEED_400KHz;
//	i2c_config(&handlerLCD);
//	LCD_Init(&handlerLCD);

}
void parseCommands(char *ptrBufferReception){

	// Esta función de C lee la cadena de caracteres a la que apunta el "ptr y la divide
	// y almacena en tres elementos diferentes: un string llamado "cmd", y dos numeros
	// integer llamados "firstParameter" y "secondParameter".
	// De esta forma, podemos introducir información al micro desde el puerto serial.
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	// Este primer comando imprime una lista con los otros comandos que tiene el equipo
	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerCommTerminal, "Help Menu CMDs:\n");
		writeMsg(&handlerCommTerminal, "1) help     -- Print this menu\n");
		writeMsg(&handlerCommTerminal, "2) dummy #A#B  -- dummy cmd, #A and #B are uint32_t\n");
		writeMsg(&handlerCommTerminal, "3) usermsg # # msg -- msg is a string comming from outside\n");
		writeMsg(&handlerCommTerminal, "4) initLcd -- steps to initialize the LCD\n");
		writeMsg(&handlerCommTerminal, "5) testLcd -- simple Test for the LCD\n");
		writeMsg(&handlerCommTerminal, "6) setPeriod # -- Change the Led_state period (us)\n");
		writeMsg(&handlerCommTerminal, "7) autoUpdate # -- Automatic LCD update (# -> 1/0)\n");
	}
	//	/* Comandos para elegir la señal del MCO1 */
	else if(strcmp(cmd, "selectClock") == 0){
		if(firstParameter == 0){
			handlerMCO1.clock = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg, "MCO1 configura el HSI \n");
			writeMsg(&handlerCommTerminal, bufferMsg);
		}else if(firstParameter == 1){
			handlerMCO1.clock = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg, "MCO1 configura el LSE \n");
			writeMsg(&handlerCommTerminal, bufferMsg);
		}else if(firstParameter == 2){
			handlerMCO1.clock = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg, "MCO1 configura el PLL \n");
			writeMsg(&handlerCommTerminal, bufferMsg);

		}
	}

	/* Comandos para seleccionar el prescaler de la señal en el MCO1 */
	else if(strcmp(cmd, "selectPrescaler") == 0)
	{
		if(firstParameter < 6 && firstParameter > 1){

			handlerMCO1.preescaler = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg, "MCO1 configura a %u \n", firstParameter);
			writeMsg(&handlerCommTerminal, bufferMsg);
		}
		else{
			writeMsg(&handlerCommTerminal, "Error en el comando ingresado");
		}
	}
	/* Comandos acelerómetro */
	else if(strcmp(cmd, "ValoresAcelerometro") == 0)
	{
		acelerometro();
		writeMsg(&handlerCommTerminal, "Mostrar los datos del acelerómetro \n");
			for (int i= 0; i<2000; i++){
				sprintf(bufferMsg, "%u arrayX = %.2f m/s2 ; arrayY = %.2f m/s2; arrayZ = %.2f m/s2 \n",i, arrayX[i], arrayY[i],arrayZ[i]);
				writeMsg(&handlerCommTerminal, bufferMsg);
			}
	}
	else{
		// Se imprime el mensaje "wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerCommTerminal, "Wrong CMD");
	}
}



void acelerometro(void) {

	flag = 1;
	while (flag == 1) {

//		i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);

		GPIO_WritePin(&handlerPC9, SET);

		i2c_writeSingleRegister(&handlerAccelerometer, BW_RATE, 0xF);
		i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x08);

		if (contador < 2000) {
			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
			int16_t AccelX = AccelX_high << 8 | AccelX_low;

			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
			int16_t AccelY = AccelY_high << 8 | AccelY_low;

			uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
			uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
			int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

			arrayX[contador] = (AccelX / 256.f * 9.8);
			arrayY[contador] = (AccelY / 256.f * 9.8);
			arrayZ[contador] = (AccelZ / 256.f * 9.8);
		}
		GPIO_WritePin(&handlerPC9, RESET);
		while (counter) {
			__NOP();
		}
		counter = 1;
	}


}

//	// Hacemos un "eco con el valor que nos llega por el serial
//	if(rxData != '\0'){
//		writeChar(&handlerCommTerminal, rxData);
//
//		if(rxData == 'w'){
//			sprintf(bufferData, "WHO_AM_I? (r)\n");
//			writeMsg(&handlerCommTerminal, bufferData);
//
//			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
//			sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//			writeMsg(&handlerCommTerminal, bufferData);
//			rxData = '\0';
//		}
//		if(rxData == 'p'){
//			sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
//			writeMsg(&handlerCommTerminal, bufferData);
//
//			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, POWER_CTL);
//			sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//			writeMsg(&handlerCommTerminal, bufferData);
//			rxData = '\0';
//		}
//		if(rxData == 'r'){ //Para resetear al equipo
//			sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
//			writeMsg(&handlerCommTerminal, bufferData);
//
//			i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);
//			rxData = '\0';
//		}
//		if(rxData == 'x'){ //lecturas en x
//			sprintf(bufferData, "Axis X data (r)\n");
//			writeMsg(&handlerCommTerminal, bufferData);
//
//			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
//			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
//			int16_t AccelX = AccelX_high << 8 | AccelX_low;
//			float valorX = (AccelX/220.f)*9.8;
//			sprintf(bufferData, "AccelX = %.2f \n", valorX);
//			writeMsg(&handlerCommTerminal, bufferData);
//			rxData = '\0';
//
//		}
//		if(rxData == 'y'){ //lecturas en y
//			sprintf(bufferData, "Axis Y data (r)\n");
//			writeMsg(&handlerCommTerminal, bufferData);
//
//			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
//			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
//			int16_t AccelY = AccelY_high << 8 | AccelY_low;
//			float valorY = (AccelY/256.f)*9.8;
//			sprintf(bufferData, "AccelY = %.2f \n", valorY);
//			writeMsg(&handlerCommTerminal, bufferData);
//			rxData = '\0';
//		}
//		if(rxData == 'z'){ //lecturas en y
//			sprintf(bufferData, "Axis Z data (r)\n");
//			writeMsg(&handlerCommTerminal, bufferData);
//
//			uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
//			uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
//			int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
//			float valorZ = (AccelZ/256.f)*9.8;
//			sprintf(bufferData, "AccelZ = %.2f \n", valorZ);
//			writeMsg(&handlerCommTerminal, bufferData);
//			rxData = '\0';
//		}
//		else if(rxData == 'k'){
//			writeMsg(&handlerCommTerminal, "Mostrar los datos del acelerómetro \n");
//			for (int i= 0; i<2000; i++){
//				sprintf(bufferData, "%u arrayX = %.2f m/s2 ; arrayY = %.2f m/s2; arrayZ = %.2f m/s2 \n",i, arrayX[i], arrayY[i],arrayZ[i]);
//				writeMsg(&handlerCommTerminal, bufferData);
//			}
//		}
//		else{
//			//sprintf(bufferData, "%d \n", rxData);
////			writeChar(&handlerCommTerminal, rxData);
//			rxData = '\0';
//		}
//
//}

//void generarPwm(void){
////	if(flagPwm == 1)
//
//		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
//		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
//		int16_t AccelX = AccelX_high << 8 | AccelX_low;
//		float valorX = (AccelX/256.f)*9.8;
//
//		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
//		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
//		int16_t AccelY = AccelY_high << 8 | AccelY_low;
//		float valorY = (AccelY/256.f)*9.8;
//
//		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
//		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
//		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
//		float valorZ = (AccelZ/256.f)*9.8;
//
//		duttyValueX = 1020*(valorX) + 30000;
//		duttyValueY = 1020*(valorY) + 30000;
//		duttyValueZ = 1020*(valorZ) + 30000;
//
//		updateDuttyCycle(&handlerPwmX, duttyValueX);
//		updateDuttyCycle(&handlerPwmY, duttyValueY);
//		updateDuttyCycle(&handlerPwmZ, duttyValueZ);
////		flagPwm =! flagPwm;
//
//
//}
//void initLCD(void){
//
//	LCD_setCursor(&handlerLCD, 0, 0);
//	LCD_sendSTR(&handlerLCD, "Eje X = ");
//	LCD_setCursor(&handlerLCD, 0, 16);
//	LCD_sendSTR(&handlerLCD, "m/s2");
//	LCD_setCursor(&handlerLCD, 1, 0);
//	LCD_sendSTR(&handlerLCD, "Eje Y = ");
//	LCD_setCursor(&handlerLCD, 1, 16);
//	LCD_sendSTR(&handlerLCD, "m/s2");
//	LCD_setCursor(&handlerLCD, 2, 0);
//	LCD_sendSTR(&handlerLCD, "Eje Z = ");
//	LCD_setCursor(&handlerLCD, 2, 16);
//	LCD_sendSTR(&handlerLCD, "m/s2");
//	LCD_setCursor(&handlerLCD, 3, 0);
//	LCD_sendSTR(&handlerLCD, "OFSZMAX250mg");
//
//}

//void accelLCD(void){
//
//	if(flagLCD == 1){
//		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
//		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
//		int16_t AccelX = AccelX_high << 8 | AccelX_low;
//		float valorX = (AccelX/220.f)*9.8;
//		sprintf(bufferData, "%.2f \n", valorX);
//		LCD_setCursor(&handlerLCD, 0, 9);
//		LCD_sendSTR(&handlerLCD, bufferData);
//
//		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
//		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
//		int16_t AccelY = AccelY_high << 8 | AccelY_low;
//		float valorY = (AccelY/256.f)*9.8;
//		sprintf(bufferData, "%.2f \n", valorY);
//		LCD_setCursor(&handlerLCD, 1, 9);
//		LCD_sendSTR(&handlerLCD, bufferData);
//
//		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
//		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
//		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
//		float valorZ = (AccelZ/256.f)*9.8;
//		sprintf(bufferData, " %.2f \n", valorZ);
//		LCD_setCursor(&handlerLCD, 2, 9);
//		LCD_sendSTR(&handlerLCD, bufferData);
//		LCD_setCursor(&handlerLCD, 3, 0);
//
//		flagLCD =! flagLCD;
//	}
//
//}

/*
 * Callback del Timer2 -Hacemos un blinky
 */

void BasicTimer2_Callback(void){
	//Hacemos uduttyn blinky, para indicar que el equipo está funcionando correctamente
	GPIOxTooglePin(&handlerLedOK);

}
/* Callback relacionado con el Timer para 1kHz */
void BasicTimer4_Callback(void){
	GPIOxTooglePin(&handlerPC9);
	if(flag == 1){
		contador ++;
		counter = 0;
	}
	if(contador == 2000){
		rxData = '\0';
		contador = 0;
		flag = 0;

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

