/*
 * PLLDriver.c
 *
 *  Created on: 21/05/2023
 *      Author: karol
 */

#include <stm32f4xx.h>
#include "PLLDriver.h"

uint8_t var = 0;

void PLL_Config(void){

	RCC->CR &= ~RCC_CR_PLLON;
	/* Se configura el voltaje PWR, dependiendo de la frecuencia con la que trabaje */

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	PWR->CR &=~(PWR_CR_VOS);
	PWR->CR |= PWR_CR_VOS;

	/* 1. Programe el nuevo número de estados de espera en los bits LATENCY en FLASH_ACR
			 * registro  */

	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
	FLASH->ACR &= ~(FLASH_ACR_LATENCY);
	FLASH->ACR |= (FLASH_ACR_LATENCY_2WS);



	/* Se configura el valor que se quiere obtener, en este caso 80MHz
	 * f(VCO clock)  = 16MHz * (PLLN/PLLM)
	 * Se toma el factor de multiplicación PLLN = 80MHz  */

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
	RCC->PLLCFGR |= (85 << RCC_PLLCFGR_PLLN_Pos);

	/* Luego se configura el factor de división PLLM = 2MHz  */

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
	RCC->PLLCFGR |= (2 << RCC_PLLCFGR_PLLM_Pos);

	/* f(Clock output) = f(VCO clock) / PLLP
	 * La frecuencia deseada es 80MHz, entonces el PLLP = 8 */

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
	RCC->PLLCFGR |= (3 << RCC_PLLCFGR_PLLP_Pos);

	RCC->CFGR &= ~ (RCC_CFGR_SW);
	RCC->CFGR &= RCC_CFGR_SW_1;


	/* 4. si es necesario, modifique el preescalador del reloj de la CPU escribiendo los bits HPRE en RCC_CFGR */
	RCC->CFGR &= ~(RCC_CFGR_HPRE);

	/* 5. Verifique que la nueva fuente de reloj de la CPU y/o el nuevo valor del preescalador del reloj de la CPU estén
	 * tenido en cuenta leyendo el estado de la fuente de reloj (bits SWS) o/y el AHB
	 * valor del preescalador (bits HPRE), respectivamente, en el registro RCC_CFGR */

	RCC->CFGR &= ~(RCC_CFGR_PPRE1); // APB1 no puede superar los 50MHz, por esta razón se debe dividir en 2
	RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);

	RCC->CFGR &= ~(RCC_CFGR_PPRE2); // APB2 no puede superar los 100MHz, es decir, no se divide por ningún factor
	RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);

	/*** Lo siguiente es para probar con el osciloscopio u analizador de señales, para eso se debe activar un prescaler  ***/
	// Se utiliza el registro MCO1: Microncontroller clock output 1

	RCC->CFGR &= ~(RCC_CFGR_MCO1);
	RCC->CFGR |= (RCC_CFGR_MCO1_0);
	RCC->CFGR |= (RCC_CFGR_MCO1_1);

	// Se va a dividir por 5 para que la frecuencia mostrada en el osciloscopio sea de 20MHz
	RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_0);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_1);
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_2);



	/* Finalmente se activa el PLL */
	RCC->CR |= RCC_CR_PLLON;

	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	/* 3. Modifique la fuente de reloj de la CPU escribiendo los bits SW en el registro RCC_CFGR */

	RCC->CFGR |= RCC_CFGR_SW_PLL;



	}

/* La siguiente función entrega el estado de la configuración del equipo para cuando está el PLL en ON
 * se entregará una frecuencia de 80MHz o de lo contrario será 16MHz */

uint32_t getConfigPLL(void){

	var = (RCC->CR & RCC_CR_PLLON) >> (RCC_CR_PLLON_Pos);

	return var;

}




