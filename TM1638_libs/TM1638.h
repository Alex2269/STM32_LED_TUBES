
#ifndef _tm1638_H
#define _tm1638_H

#include "stdint.h"
#include "stdio.h"
#include "ftoa_impl.h"
#include "main.h"

#include "stm32_ticks.h"
#define delayMicroseconds delay_next_us

#define LSBFIRST 0x0
#define MSBFIRST 0x1

// void pin_mode(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, uint32_t GPIO_Mode);
// uint32_t pin_read(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin);
void TM1638_gpio_init(void);
// void shiftOut(uint8_t bitOrder, uint8_t value);
// uint32_t shiftIn(uint32_t bitOrder);
void setLetter(int place,char letter);
void setDigital(int place,int digital);
void displayInt(int num);
void displayFloat(float number,int prec);
void setNoPoint(int place);
void setPoint(int place);
void turnOffDisplay(void);
void turnOnDisplay(void);
void setDisplayLight(int light);
void writeData(uint8_t data);
void clear(void);
void update(void);
uint8_t Read_Key(void);

#endif // _tm1638_H

