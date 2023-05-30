/*
 * SysTickDriver.c
 *
 *  Created on: 17/05/2023
 *      Author: karol
 */

#include <stm32f4xx.h>
#include "SysTickDriver.h"

uint64_t ticks = 0;
uint64_t ticks_start = 0;
uint64_t ticks_counting = 0;

void config_SysTick_ms (uint8_t systemClock){
	//Reiniciamos el valor de la variable que cuenta el timepo;
	ticks = 0;

	switch(systemClock){

		//caso para el reloj HSI -> 16MHz
		case SYSTEMCLOCK_HSI:

			SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
			break;

		//Caso para el reloj HSE
		case SYSTEMCLOCK_HSE:

			SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
			break;

		//Caso para el reloj PLL a 100MHz
		case SYSTEMCLOCK_PLL_100MHz:

			SysTick->LOAD = SYSTICK_LOAD_VALUE_100MHz_1ms;
			break;
		//Caso para el reloj PLL a 80 MHZ
		case SYSTEMCLOCK_PLL_80MHz:

			SysTick->LOAD = SYSTICK_LOAD_VALUE_80MHz_1ms;


		//En caso de que se ingrese un valor diferente
		default:
			SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
			break;
		}


	//Limpiamos el valor de systick
	SysTick->VAL = 0;

	//Configuramos el reloj in terno como reloj para el timer
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	//Desactivamos las interupciones globales
	__disable_irq();

	//matriculamos la interrupcion en el NVIC
	NVIC_EnableIRQ(SysTick_IRQn);

	//activamos la interrupcion debida al conteo a cero del systick
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	//activamos el timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	//Activamos las interupciones globales
	__enable_irq();

}

uint64_t getTicks_ms(void){
	return ticks;
}


void delay_ms(uint32_t wait_time_ms){
	//captura el primer valor de timepo para comprar
	ticks_start = getTicks_ms();

	//captura el segundo valor de timepo para comprar
	ticks_counting = getTicks_ms();

	//Compara si el valor del tiempo es menor que el stert+wait
	//actualiza el valor del counting
	//Repite la operacion hast que el counting sea mayor
	while(ticks_counting < (ticks_start + (uint64_t)wait_time_ms)){

		//actualizar el valor
		ticks_counting = getTicks_ms();

		}
	}

void SysTick_Handler (void){
	// verificamos la interrupcion
	if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){

		//limpiamos la bandera
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		//incrementamos el contador
		ticks ++;

		}
	}
