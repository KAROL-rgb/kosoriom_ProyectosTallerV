/* @file           : BasicProject_Main.c
 * @author         : Karol Osorio Monsalve - kosoriom@unal.edu.co
 * @brief          : Solución básica de un proyecto con librerías externas */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"


/* Definicion de los elementos */
GPIO_Handler_t handlerLED2 = {0}; //PA5 (LED del micro)
GPIO_Handler_t handlerSW = {0}; // Botón
EXTI_Config_t extiSW = {0};

// Sistema de comunicación USART:
USART_Handler_t usart2comm = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

// Variables
uint8_t sendMsg = 0;



/* Prototipos de funciones del main */
void initSystem(void);
void callback_extInt13(void);
void BasicTimer2_Callback(void);
void usart2Rx_Callback(void);


int main(void){

	/* Inicializacion de elementos*/
	initSystem();

	// writeMsg(&usart2comm, "Hola mundo ! !");
		/* Loop forever */
		while(1){
			if(sendMsg == 1){
				writeMsg(&usart2comm, "Hola Mundo !!");
				sendMsg = 0;
			}
		}

	return 0;
}

void initSystem(void){

	/* Configuración del LED, Pin PA5 */
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLED2);

	/* Config TIM2 para encender el LED2 (LED de estado) cada 250 ms */
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 250;                // Interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	// El pin UserButton es una entrada simple que entregará la interrupción EXTI-13
	// Observar que el pin seleccionado es el PIN_13, por lo tanto el callback
	// que se debe configurar es el callback_ExtInt13()

	/* Configuración del botón, Pin PC13 (SW) */
	handlerSW.pGPIOx = GPIOC;
	handlerSW.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSW);

	/* Se carga la configuración del EXTI del botón */
	extiSW.pGPIOHandler = &handlerSW;
	extiSW.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiSW);

	/* Configuración de la comunicación serial */

	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	usart2comm.ptrUSARTx = USART2;
	usart2comm.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	usart2comm.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	usart2comm.USART_Config.USART_parity = USART_PARITY_NONE;
	usart2comm.USART_Config.USART_stopbits = USART_STOPBIT_1;
	usart2comm.USART_Config.USART_mode = USART_MODE_RXTX;
	usart2comm.USART_Config.USART_enableIntRX = USART_RX_INTERRUP_DISABLE;
	usart2comm.USART_Config.USART_enableIntTX = USART_TX_INTERRUP_DISABLE;

	USART_Config(&usart2comm);
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
	sendMsg++;

}
/* Callback SW (User Button) -> hacer algo... */
void callback_extInt13(void){
	__NOP();

}




