
#ifndef _MAX7219DISPLAY_H
#define _MAX7219DISPLAY_H

#include <inttypes.h>

uint8_t PIN_CLK;
uint8_t PIN_DIN;
uint8_t PIN_CS;
uint8_t _nDig;
uint8_t _brightness;

static const uint8_t SEG_G = 0x01;
static const uint8_t SEG_F = 0x02;
static const uint8_t SEG_E = 0x04;
static const uint8_t SEG_D = 0x08;
static const uint8_t SEG_C = 0x10;
static const uint8_t SEG_B = 0x20;
static const uint8_t SEG_A = 0x40;
static const uint8_t DP= 0x80;

void MAX7219_init(uint8_t pinCLK, uint8_t pinDIN, uint8_t pinCS);
void Display_presend(uint8_t nDig, uint8_t brightness);
void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos);
void showNumberDec(long num, uint8_t leading_zero, uint8_t length, uint8_t pos);
void shift_Out(uint8_t val);
void write7219 (uint8_t addr, uint8_t data);

#endif /* _MAX7219DISPLAY_H */
