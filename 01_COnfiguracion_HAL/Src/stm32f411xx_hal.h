/**************************************************************************************************
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 2, 2023
 *      Author: karol
 *
 * Este archivo contiene la información más básica del micro:
 * 	- Valores del reloj principal
 * 	- Distribución básica de la memoria (descrito en la figura 14 del micro)
 * 	- Posiciones de memoria de los periféricos disponibles en el micro descrito en la
 * 	  tabla 1 (Memory Map)
 * 	- Incluye los demás registros de los periféricos
 * 	- Definiciones de las constantes más básicas.
 *
 * 	NOTA: La diferencia del NVIC será realizada al momento de describir el uso de las
 * 	      interrupciones
 *
 ********************************************************************************************************/

#ifndef INC_STM32F411XX_HAL_H_
#define INC_STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED     16000000      // Value for the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED     4000000       // Value for the main clock signal (HSE -> High Speed External)

#define NOP()       asm("NOP")
#define __weak       __attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * Remember, 1KByte = 1024 bytes
 */
#define FLASH_BASE_ADDR        0x08000000U      // Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR         0x20000000U      // Esta es la memoria RAM, 128KB.

/*NOTA: Observar que existen unos registros específicos del Cortex M4 en la región 0xE0000000U
 * Los controladores de las interrupciones se encuentran allí, por ejemplo.
 * Esto se verá a su debido tiempo.
 */

/*
 * NOTA:
 * Ahora agrergamos la dirección de memoria base para cada uno de los periféricos que posee el micro
 * EN el "datasheet" del micro, Figura 14 (Memory Map) encontramos el mapa de los buses !
 * 	- APB1 (Advance Peripherical BUs)
 * 	-APB2
 * 	-AHB1 (Advance High - performance Bus)
 * 	-AHB2
 */

/**
 * AHBx and APBx Bus Peripherals base addresses
 */
 */

#endif /* STM32F411XX_HAL_H_ */
