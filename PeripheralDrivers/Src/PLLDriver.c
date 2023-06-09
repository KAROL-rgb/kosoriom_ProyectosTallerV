/*
 * PLLDriver.c
 *
 *  Created on: 21/05/2023
 *      Author: karol
 */

#include <stm32f4xx.h>
#include "PLLDriver.h"


void PLL_Config(PLL_Handler_t *ptrPLL_Handler){

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;

	/* Se configura el voltaje PWR, dependiendo de la frecuencia con la que trabaje */
	if(ptrPLL_Handler->frecSpeed == FRECUENCY_16MHz){
		RCC->CFGR &= ~RCC_CFGR_SW;
	}
	else if(ptrPLL_Handler->frecSpeed == FRECUENCY_100MHz){

		RCC->AHB1ENR |= RCC_APB1ENR_PWREN;

		PWR->CR &=~(PWR_CR_VOS);
		PWR->CR |= (1<<PWR_CR_VOS_Pos);

		/* 1. Programe el nuevo número de estados de espera en los bits LATENCY en FLASH_ACR
					 * registro  */

		FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
		FLASH->ACR &= ~(FLASH_ACR_LATENCY);
		FLASH->ACR |= (FLASH_ACR_LATENCY_3WS);

		// Limpieza
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;


		/* Se configura el valor que se quiere obtener, en este caso 100MHz
			 * f(VCO clock)  = 16MHz * (PLLN/PLLM)
			 * Se toma el factor de multiplicación PLLN = 100MHz  */

		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
		RCC->PLLCFGR |= (100 << RCC_PLLCFGR_PLLN_Pos);
		/* Luego se configura el factor de división PLLM = 2MHz  */
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
		RCC->PLLCFGR |= (2 << RCC_PLLCFGR_PLLM_Pos);

		/* f(Clock output) = f(VCO clock) / PLLP
		 * La frecuencia deseada es 100MHz, entonces el PLLP = 8 */

		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
		RCC->PLLCFGR |= (3 << RCC_PLLCFGR_PLLP_Pos);

		RCC->CFGR &= ~RCC_CFGR_SW;
		RCC->CFGR |= RCC_CFGR_SW_1;

		/* 4. si es necesario, modifique el preescalador del reloj de la CPU escribiendo los bits HPRE en RCC_CFGR */
		RCC->CFGR &= ~(RCC_CFGR_HPRE);
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* 5. Verifique que la nueva fuente de reloj de la CPU y/o el nuevo valor del preescalador del reloj de la CPU estén
			 * tenido en cuenta leyendo el estado de la fuente de reloj (bits SWS) o/y el AHB
			 * valor del preescalador (bits HPRE), respectivamente, en el registro RCC_CFGR */

		RCC->CFGR &= ~(RCC_CFGR_PPRE1); // APB1 no puede superar los 50MHz, por esta razón se debe dividir en 2
		RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);

		RCC->CFGR &= ~(RCC_CFGR_PPRE2); // APB2 no puede superar los 100MHz, es decir, no se divide por ningún factor
		RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);

		/*** Lo siguiente es para probar con el osciloscopio u analizador de señales, para eso se debe activar un prescaler  ***/
		// Se utiliza el registro MCO1: Microncontroller clock output 1

		RCC->CFGR &= ~RCC_CFGR_MCO1_0;
		RCC->CFGR &= ~RCC_CFGR_MCO1_1;

		RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
		RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
		RCC->CFGR |= RCC_CFGR_MCO1PRE_2;


		/* Finalmente se activa el PLL */
		RCC->CR |= RCC_CR_PLLON;

		while(!(RCC->CR & RCC_CR_PLLRDY)){
			__NOP();
		}

		/* 3. Modifique la fuente de reloj de la CPU escribiendo los bits SW en el registro RCC_CFGR */

		RCC->CFGR |= RCC_CFGR_SW_PLL;

	}


//	RCC->CFGR &= ~ (RCC_CFGR_SW);
//	RCC->CFGR &= RCC_CFGR_SW_1;

}
void configMCO1(MCO1_Handler_t *ptrHandlerMCO1){
	if(ptrHandlerMCO1->clock == HSI){
		RCC->CFGR &= ~RCC_CFGR_MCO1_0;
		RCC->CFGR &= ~RCC_CFGR_MCO1_1;
		if(ptrHandlerMCO1->preescaler == MCO1PRE_2){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_3){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_4){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_5){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
	}
	else if(ptrHandlerMCO1->clock == LSE){
		RCC->CFGR |= RCC_CFGR_MCO1_0;
		RCC->CFGR &= ~RCC_CFGR_MCO1_1;
		if(ptrHandlerMCO1->preescaler == MCO1PRE_2){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_3){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_4){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_5){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
	}
	else if(ptrHandlerMCO1->clock == PLL){
		RCC->CFGR |= RCC_CFGR_MCO1_0;
		RCC->CFGR |= RCC_CFGR_MCO1_1;
		if(ptrHandlerMCO1->preescaler == MCO1PRE_2){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_3){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_4){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->preescaler == MCO1PRE_5){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
	}
}


/* La siguiente función entrega el estado de la configuración del equipo para cuando está el PLL en ON
 * se entregará una frecuencia de 80MHz o de lo contrario será 16MHz */

uint32_t getConfigPLL(void){
	uint32_t var = 0;
//	uint32_t PLLN = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> (RCC_PLLCFGR_PLLN_Pos);

	if(RCC->CFGR & RCC_CR_PLLON){
		var = (FRECUENCY_100MHz);
	}
	else{
		var  = FRECUENCY_16MHz;

	}

	return var;

}




