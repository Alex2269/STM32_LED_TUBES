/***********************************************************************
 port from arduino project to avr-gcc
 https://github.com/Play-Zone/TM1638Module
*************************************************************************/

#ifndef tm1638lib_h
#define tm1638lib_h

#include "stdint.h"
#include "main.h"

void pin_mode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t GPIO_Mode);
uint32_t pin_read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t TM1638_Read(void);
void TM1638_Write(uint8_t DATA);
void Write_COM(uint8_t cmd);
void Write_DATA(uint8_t add, uint8_t DATA);
void TM1638Lib(int DIO, int CLK, int STB);
void LedDisplay(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7);
void TM1638_indicate(uint32_t number);
void LedDisplay_two(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7);
void TM1638_indicate_two(uint32_t number);
uint8_t ReadKey(void);

extern int DIO;// = 8;
extern int CLK;// = 9;
extern int STB;// = 10;

#endif


