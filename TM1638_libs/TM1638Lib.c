/***********************************************************************
 port from arduino project to stm32
 https://github.com/Play-Zone/TM1638Module
*************************************************************************/

#include "TM1638Lib.h"

uint8_t tab[32] =
{
  0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
  0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
  0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,
  0xFF,0xEF,0xF7,0xFC,0xB9,0xDE,0xF9,0xF1
};

static inline
void tic_delay(volatile uint32_t cycle_count)
{
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __asm__ volatile ("nop");
  }
}

void pin_mode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t GPIO_Mode)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_Mode;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  if(GPIO_Mode == LL_GPIO_MODE_OUTPUT)
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  if(GPIO_Mode == LL_GPIO_MODE_INPUT)
   GPIO_InitStruct.OutputType = LL_GPIO_MODE_INPUT;
  LL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

uint32_t pin_read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  pin_mode(GPIOx, GPIO_Pin, LL_GPIO_MODE_INPUT);
  // GPIO_Pin_Init_Input(GPIOx,  3);
  // return GPIO_Pin_Read(GPIOA, 3); // #include "gpio_pin.h"
  return LL_GPIO_IsInputPinSet(GPIOx, GPIO_Pin);
}

uint8_t TM1638_Read(void)
{
  uint8_t i;
  uint8_t temp = 0;
  
  for (i = 0; i<8; i++)
  {
    temp >>= 1;
    LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin);
    tic_delay(127);

    if (pin_read(DIO_GPIO_Port, DIO_Pin) == 1)
    {
      temp |= 0x80;
      tic_delay(127);
    }

    LL_GPIO_SetOutputPin(CLK_GPIO_Port, CLK_Pin);
    tic_delay(127);
  }
  
  pin_mode(DIO_GPIO_Port, DIO_Pin, LL_GPIO_MODE_OUTPUT);

  return temp;
}

void TM1638_Write(uint8_t DATA)
{
  uint8_t i;
  LL_GPIO_ResetOutputPin(DIO_GPIO_Port, DIO_Pin);
  tic_delay(127);
  for (i = 0; i<8; i++)
  {
    LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin);
    tic_delay(127);
    if (DATA & 0X01)
    {
      LL_GPIO_SetOutputPin(DIO_GPIO_Port, DIO_Pin);
      tic_delay(127);
    }
    else
    {
      LL_GPIO_ResetOutputPin(DIO_GPIO_Port, DIO_Pin);
      tic_delay(127);
    }
    DATA >>= 1;
    LL_GPIO_SetOutputPin(CLK_GPIO_Port, CLK_Pin);
    tic_delay(127);
  }
}

void Write_COM(uint8_t cmd)
{
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(cmd);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

void Write_DATA(uint8_t add, uint8_t DATA)
{
  Write_COM(0x44);
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(0xc0 | add);
  TM1638_Write(DATA);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

void TM1638Lib(int DIO, int CLK, int STB)
{
  uint8_t i;
  Write_COM(0x8b);
  Write_COM(0x40);
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(0xc0);
  for (i = 0; i<16; i++)
  {
    TM1638_Write(0x00);
  }
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

void LedDisplay(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7)
{
  uint8_t writedata0, writedata1, writedata2, writedata3, writedata4, writedata5, writedata6, writedata7;
  writedata0 = 
  ((tab[data7] & 0x01)) +
  ((tab[data6] & 0x01) << 1) +
  ((tab[data5] & 0x01) << 2) +
  ((tab[data4] & 0x01) << 3) +
  ((tab[data3] & 0x01) << 4) +
  ((tab[data2] & 0x01) << 5) +
  ((tab[data1] & 0x01) << 6) +
  ((tab[data0] & 0x01) << 7);

  writedata1 =
  ((tab[data7] & 0x02) >> 1) +
  ((tab[data6] & 0x02)) +
  ((tab[data5] & 0x02) << 1) +
  ((tab[data4] & 0x02) << 2) +
  ((tab[data3] & 0x02) << 3) +
  ((tab[data2] & 0x02) << 4) +
  ((tab[data1] & 0x02) << 5) +
  ((tab[data0] & 0x02) << 6);

  writedata2 =
  ((tab[data7] & 0x04) >> 2) +
  ((tab[data6] & 0x04) >> 1) +
  ((tab[data5] & 0x04)) +
  ((tab[data4] & 0x04) << 1) +
  ((tab[data3] & 0x04) << 2) +
  ((tab[data2] & 0x04) << 3) +
  ((tab[data1] & 0x04) << 4) +
  ((tab[data0] & 0x04) << 5);

  writedata3 =
  ((tab[data7] & 0x08) >> 3) +
  ((tab[data6] & 0x08) >> 2) +
  ((tab[data5] & 0x08) >> 1) +
  ((tab[data4] & 0x08)) +
  ((tab[data3] & 0x08) << 1) +
  ((tab[data2] & 0x08) << 2) +
  ((tab[data1] & 0x08) << 3) +
  ((tab[data0] & 0x08) << 4);

  writedata4 =
  ((tab[data7] & 0x10) >> 4) +
  ((tab[data6] & 0x10) >> 3) +
  ((tab[data5] & 0x10) >> 2) +
  ((tab[data4] & 0x10) >> 1) +
  ((tab[data3] & 0x10)) +
  ((tab[data2] & 0x10) << 1) +
  ((tab[data1] & 0x10) << 2) +
  ((tab[data0] & 0x10) << 3);

  writedata5 =
  ((tab[data7] & 0x20) >> 5) +
  ((tab[data6] & 0x20) >> 4) +
  ((tab[data5] & 0x20) >> 3) +
  ((tab[data4] & 0x20) >> 2) +
  ((tab[data3] & 0x20) >> 1) +
  ((tab[data2] & 0x20)) +
  ((tab[data1] & 0x20) << 1) +
  ((tab[data0] & 0x20) << 2);

  writedata6 =
  ((tab[data7] & 0x40) >> 6) +
  ((tab[data6] & 0x40) >> 5) +
  ((tab[data5] & 0x40) >> 4) +
  ((tab[data4] & 0x40) >> 3) +
  ((tab[data3] & 0x40) >> 2) +
  ((tab[data2] & 0x40) >> 1) +
  ((tab[data1] & 0x40)) +
  ((tab[data0] & 0x40) << 1);

  writedata7 =
  ((tab[data7] & 0x80) >> 7) +
  ((tab[data6] & 0x80) >> 6) +
  ((tab[data5] & 0x80) >> 5) +
  ((tab[data4] & 0x80) >> 4) +
  ((tab[data3] & 0x80) >> 3) +
  ((tab[data2] & 0x80) >> 2) +
  ((tab[data1] & 0x80) >> 1) +
  ((tab[data0] & 0x80));

  Write_COM(0x8a);
  Write_COM(0x40);
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(0xc0);
  TM1638_Write(writedata0);
  TM1638_Write(0x80);
  TM1638_Write(writedata1);
  TM1638_Write(0x00);
  TM1638_Write(writedata2);
  TM1638_Write(0x00);
  TM1638_Write(writedata3);
  TM1638_Write(0x00);
  TM1638_Write(writedata4);
  TM1638_Write(0x00);
  TM1638_Write(writedata5);
  TM1638_Write(0x00);
  TM1638_Write(writedata6);
  TM1638_Write(0x00);
  TM1638_Write(writedata7);
  TM1638_Write(0x00);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

void TM1638_indicate(uint32_t number)
{
  uint8_t num[8] = {0};
  uint8_t i = 0;
  for(uint8_t clr = 0; clr<8; clr++)
  {
    num[clr]=0;
  }
  while(number)
  {
    num[i] = number%10;
    i++;
    number /= 10;
  }
  i=0;
  LedDisplay(num[7],num[6],num[5],num[4],num[3],num[2],num[1],num[0]);
}

void LedDisplay_two(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7)
{
  uint8_t writedata0, writedata1, writedata2, writedata3, writedata4, writedata5, writedata6, writedata7;
  writedata0 = 
  ((tab[data7] & 0x01)) +
  ((tab[data6] & 0x01) << 1) +
  ((tab[data5] & 0x01) << 2) +
  ((tab[data4] & 0x01) << 3) +
  ((tab[data3] & 0x01) << 4) +
  ((tab[data2] & 0x00) << 5) +
  ((tab[data1] & 0x01) << 6) +
  ((tab[data0] & 0x01) << 7);

  writedata1 =
  ((tab[data7] & 0x02) >> 1) +
  ((tab[data6] & 0x02)) +
  ((tab[data5] & 0x02) << 1) +
  ((tab[data4] & 0x02) << 2) +
  ((tab[data3] & 0x02) << 3) +
  ((tab[data2] & 0x0) << 4) +
  ((tab[data1] & 0x02) << 5) +
  ((tab[data0] & 0x02) << 6);

  writedata2 =
  ((tab[data7] & 0x04) >> 2) +
  ((tab[data6] & 0x04) >> 1) +
  ((tab[data5] & 0x04)) +
  ((tab[data4] & 0x04) << 1) +
  ((tab[data3] & 0x04) << 2) +
  ((tab[data2] & 0x0) << 3) +
  ((tab[data1] & 0x04) << 4) +
  ((tab[data0] & 0x04) << 5);

  writedata3 =
  ((tab[data7] & 0x08) >> 3) +
  ((tab[data6] & 0x08) >> 2) +
  ((tab[data5] & 0x08) >> 1) +
  ((tab[data4] & 0x08)) +
  ((tab[data3] & 0x08) << 1) +
  ((tab[data2] & 0x0) << 2) +
  ((tab[data1] & 0x08) << 3) +
  ((tab[data0] & 0x08) << 4);

  writedata4 =
  ((tab[data7] & 0x10) >> 4) +
  ((tab[data6] & 0x10) >> 3) +
  ((tab[data5] & 0x10) >> 2) +
  ((tab[data4] & 0x10) >> 1) +
  ((tab[data3] & 0x10)) +
  ((tab[data2] & 0x0) << 1) +
  ((tab[data1] & 0x10) << 2) +
  ((tab[data0] & 0x10) << 3);

  writedata5 =
  ((tab[data7] & 0x20) >> 5) +
  ((tab[data6] & 0x20) >> 4) +
  ((tab[data5] & 0x20) >> 3) +
  ((tab[data4] & 0x20) >> 2) +
  ((tab[data3] & 0x20) >> 1) +
  ((tab[data2] & 0x0)) +
  ((tab[data1] & 0x20) << 1) +
  ((tab[data0] & 0x20) << 2);

  writedata6 =
  ((tab[data7] & 0x40) >> 6) +
  ((tab[data6] & 0x40) >> 5) +
  ((tab[data5] & 0x40) >> 4) +
  ((tab[data4] & 0x40) >> 3) +
  ((tab[data3] & 0x40) >> 2) +
  ((tab[data2] & 0x0) >> 1) +
  ((tab[data1] & 0x40)) +
  ((tab[data0] & 0x40) << 1);

  writedata7 =
  ((tab[data7] & 0x80) >> 7) +
  ((tab[data6] & 0x80) >> 6) +
  ((tab[data5] & 0x80) >> 5) +
  ((tab[data4] & 0x80) >> 4) +
  ((tab[data3] & 0x80) >> 3) +
  ((tab[data2] & 0x0) >> 2) +
  ((tab[data1] & 0x80) >> 1) +
  ((tab[data0] & 0x80));

  Write_COM(0x8a);
  Write_COM(0x40);
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(0xc0);
  TM1638_Write(writedata0);
  TM1638_Write(0x80);
  TM1638_Write(writedata1);
  TM1638_Write(0x00);
  TM1638_Write(writedata2);
  TM1638_Write(0x00);
  TM1638_Write(writedata3);
  TM1638_Write(0x00);
  TM1638_Write(writedata4);
  TM1638_Write(0x00);
  TM1638_Write(writedata5);
  TM1638_Write(0x00);
  TM1638_Write(writedata6);
  TM1638_Write(0x00);
  TM1638_Write(writedata7);
  TM1638_Write(0x00);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

void TM1638_indicate_two(uint32_t number)
{
  uint8_t num[8] = {0};
  uint8_t i = 0;
  for(uint8_t clr = 0; clr<8; clr++)
  {
    num[clr]=0;
  }
  while(number)
  {
    num[i] = number%10;
    i++;
    number /= 10;
  }
  i=0;
  LedDisplay_two(num[7],num[6],num[5],num[4],num[3],num[2],num[1],num[0]);
}

uint8_t ReadKey(void)
{
  uint8_t c[4], i, key_value = 0;
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(0x42);
  tic_delay(1000);
  for (i = 0; i<4; i++)
  {
    c[i] = TM1638_Read();
  }
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
  if (c[0] == 0x04) key_value = 1;
  if (c[0] == 0x40) key_value = 2;
  if (c[1] == 0x04) key_value = 3;
  if (c[1] == 0x40) key_value = 4;
  if (c[2] == 0x04) key_value = 5;
  if (c[2] == 0x40) key_value = 6;
  if (c[3] == 0x04) key_value = 7;
  if (c[3] == 0x40) key_value = 8;
  if (c[0] == 0x02) key_value = 9;
  if (c[0] == 0x20) key_value = 10;
  if (c[1] == 0x02) key_value = 11;
  if (c[1] == 0x20) key_value = 12;
  if (c[2] == 0x02) key_value = 13;
  if (c[2] == 0x20) key_value = 14;
  if (c[3] == 0x02) key_value = 15;
  if (c[3] == 0x20) key_value = 16;
  return (key_value);
}

