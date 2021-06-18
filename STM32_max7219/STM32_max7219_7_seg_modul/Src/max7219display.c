#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "max7219display.h"
#include "pins_ext.h"
#include "pins_connect.h"

#define true 1
#define false 0

#define HIGH 1
#define LOW 0

// MAX7219 Registers
#define MAX7219_NOP 0x00
#define MAX7219_DIG0 0x01
#define MAX7219_DIG1 0x02
#define MAX7219_DIG2 0x03
#define MAX7219_DIG3 0x04
#define MAX7219_DIG4 0x05
#define MAX7219_DIG5 0x06
#define MAX7219_DIG6 0x07
#define MAX7219_DIG7 0x08
#define MAX7219_MODE 0x09
#define MAX7219_INT 0x0A
#define MAX7219_LIM 0x0B
#define MAX7219_SHUT 0x0C
#define MAX7219_TEST 0x0F

static const uint8_t digitToSegment[] =
{
  // XABCDEFG
  0b01111110, // 0
  0b00110000, // 1
  0b01101101, // 2
  0b01111001, // 3
  0b00110011, // 4
  0b01011011, // 5
  0b01011111, // 6
  0b01110000, // 7
  0b01111111, // 8
  0b01111011 // 9
};

void MAX7219_init(void)
{
  // Set the pin direction and default value
  pin_write(CLK_GPIO_Port, CLK_Pin, LOW);
  pin_write(DIN_GPIO_Port, DIN_Pin, LOW);
  pin_write(CS_GPIO_Port, CS_Pin, HIGH);
}

void Display_presend(uint8_t nDig)
{
  if ((nDig > 0) && (nDig < 9))
    _nDig = nDig;
  else
    _nDig = 8;
  // Configure the controler
  write7219 (MAX7219_SHUT, 0x00);
  write7219 (MAX7219_TEST, 0x00);
  write7219 (MAX7219_MODE, 0x00);
  write7219 (MAX7219_INT, 0x0F);
  write7219 (MAX7219_LIM, _nDig-1);
  uint8_t i;
  for (i = 0; i < 8; i++)
    write7219 (MAX7219_DIG0 + i, 0);
  write7219 (MAX7219_SHUT, 0x01);
}

void setBrightness(uint8_t brightness)
{
  if (brightness > 15)
    brightness = 15;
  write7219 (MAX7219_INT, brightness);
}

void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
  // Sanity check
  if (pos >= _nDig)
    pos = _nDig-1;
  if ((pos+length) > _nDig)
    length = _nDig - pos;
  // write the values
  uint8_t i;
  for (i = 0; i < length; i++)
    write7219 (MAX7219_DIG0 + pos + i, segments[i]);
}

void showNumberDec(long num, bool leading_zero, uint8_t length, uint8_t pos)
{
  uint8_t prim;
  uint8_t aux[8];
  bool showZero = leading_zero;
  uint8_t i;
  Display_presend(8);
  // Sanity check
  if (pos >= _nDig)
    pos = _nDig-1;
  if ((pos+length) > _nDig)
    length = _nDig - pos;
  // Take care of negative numbers
  prim = MAX7219_DIG7 - pos;
  if (num < 0)
  {
    num = -num;
    write7219 (prim, SEG_G);
    prim--;
    length--;
  }
  // Generate digits
  for (i = 0; i < length; i++)
  {
    aux[i] = (uint8_t)(num % 10L);
    num = num / 10L;
  }
  // Write the digits
  for(i = 0; i < length; i++)
  {
    uint8_t dig = aux[length - i - 1];
    if (!showZero && (dig == 0) && (i != (length-1)))
      write7219 (prim - i, 0);
    else
    {
      if (dig != 0)
      showZero = true;
      write7219 (prim - i, digitToSegment[dig]);
    }
  }
}

void shift_Out(uint8_t val)
{
  uint8_t i;
  uint8_t not_null = 0;
  for (i = 0; i < 8; i++)
  {
    not_null = !!(val & (1 << (7 - i)));
    if(not_null)
    {
      pin_write(DIN_GPIO_Port, DIN_Pin, HIGH);
    }
    else
    {
      pin_write(DIN_GPIO_Port, DIN_Pin, LOW);
    }
    pin_pulse_hi_low(CLK_GPIO_Port, CLK_Pin, 3);
    // pin_write(CLK_GPIO_Port, CLK_Pin, HIGH);
    // pin_write(CLK_GPIO_Port, CLK_Pin, LOW);
  }
}

void write7219 (uint8_t addr, uint8_t data)
{
  pin_write(CS_GPIO_Port, CS_Pin, LOW);
  shift_Out (addr);
  shift_Out (data);
  pin_write(CS_GPIO_Port, CS_Pin, HIGH);
}
