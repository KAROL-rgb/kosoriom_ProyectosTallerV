/*
 * LCDDriver.h
 *
 *  Created on: 29/05/2023
 *      Author: karol
 */

#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"
#include "SysTickDriver.h"

void LCD_Init(I2C_Handler_t *ptrHandlerI2C);
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data);
void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str);
void LCD_Clear (I2C_Handler_t *ptrHandlerI2C);
void LCD_setCursor (I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void LCD_ClearScreen(I2C_Handler_t *ptrHandlerI2C);
void LCD_writeData(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);

#endif /* LCDDRIVER_H_ */

