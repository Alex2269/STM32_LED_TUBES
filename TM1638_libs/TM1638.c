/*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* See Github project https://github.com/supermfc/TM1638 for latest
*/

#include "TM1638.h"

uint8_t key[16];

// display model,how get this array,pls view the excel file.
uint8_t LED_MODEL[17] =
{
  // 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    -
  0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E,0x02
};

// Display Buffer,Default don't display any char or digital
uint8_t LedData[8] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static inline void
pin_mode(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, uint32_t GPIO_Mode)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_Mode;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  if(GPIO_Mode == LL_GPIO_MODE_OUTPUT) GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  if(GPIO_Mode == LL_GPIO_MODE_INPUT)  GPIO_InitStruct.OutputType = LL_GPIO_MODE_INPUT;
  LL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

static inline uint32_t
pin_read(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  pin_mode(GPIOx, GPIO_Pin, LL_GPIO_MODE_INPUT);
  return LL_GPIO_IsInputPinSet(GPIOx, GPIO_Pin);
}

// construct function,set the pin and mode
void TM1638_gpio_init(void)
{
  // DIO = DIO_Pin;
  // SCLK = CLK_Pin;
  // STB = STB_Pin;
  // LL_GPIO_MODE_INPUT
  pin_mode(DIO_GPIO_Port, DIO_Pin, LL_GPIO_MODE_OUTPUT);
  pin_mode(CLK_GPIO_Port, CLK_Pin, LL_GPIO_MODE_OUTPUT);
  pin_mode(STB_GPIO_Port, STB_Pin, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

/**
  * @brief removes one byte of data at a time, starting with the leftmost or smallest (rightmost)
  * @param Pin: Output pin for each bit
  * @param CLK_Pin: pin (int) to be switched after setting the pin to the correct value
  * @param  bitOrder: MSBFIRST / LSBFIRST
  * @param value: data to be removed (bytes)
  * @retval None
  */
static inline void
shiftOut(uint8_t bitOrder, uint8_t value)
{
  uint8_t i;

  LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin);
  for (i = 0; i < 8; i++)
  {
    int bit = bitOrder == LSBFIRST ? i : (7 - i);
    if( (value >> bit) & 0x1 )
    {
      LL_GPIO_SetOutputPin(DIO_GPIO_Port, DIO_Pin);
    }
    else
    {
      LL_GPIO_ResetOutputPin(DIO_GPIO_Port, DIO_Pin);
    }
    LL_GPIO_TogglePin(CLK_GPIO_Port, CLK_Pin);
    LL_GPIO_TogglePin(CLK_GPIO_Port, CLK_Pin);
  }
}

/**
  * @brief moves one byte of data at a time, starting at the far left or the lowest (the far right)
  * @param Pin: Enter a pin for each bit
  * @param CLK_Pin: The pin signal to be switched to is read from the pin
  * @param  bitOrder: MSBFIRST/LSBFIRST
  * @retval read value (bytes)
  */
static inline uint32_t
shiftIn(uint32_t bitOrder)
{
  uint8_t value = 0 ;
  uint8_t i;

  for ( i = 0 ; i < 8 ; ++i )
  {
    LL_GPIO_SetOutputPin(CLK_GPIO_Port, CLK_Pin);
    if (bitOrder == LSBFIRST )
    {
      value |= pin_read(DIO_GPIO_Port, DIO_Pin) << i ;
    }
    else
    {
      value |= pin_read(DIO_GPIO_Port, DIO_Pin) << (7 - i) ;
    }
    LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin);
  }
  return value ;
}

// only display 0-9 A-F and the '-' char.
void setLetter(int place,char letter)
{
  place = 7-place;
  if( (letter >= 'A') & ( letter <= 'F'))
  LedData[place] = LED_MODEL[letter-55];
  if( letter == '-')
  LedData[place] = LED_MODEL[16];
  update();
}

// set display the digital in the special place
void setDigital(int place,int digital)
{
  place = 7 - place;
  LedData[place] = LED_MODEL[digital];
  update();
}

// display a int number,only 0-9 can be displayed
void displayInt(int num)
{
  char string[9]= {0};

  sprintf(string,"%8d",num);
  for(int i = 7; i >=0; i--)
  {
    switch(string[i])
    {
      case ' ':
      break;
      case '-':
       LedData[i] = LED_MODEL[16];
      break;
      default:
       LedData[i] = LED_MODEL[string[i]-'0'];
       // LedData[7-i] = LED_MODEL[1];
      break;
    }
  }
  update();
}

// prec , because display a float,so only the 0-9 or '.' can be displayed.
void displayFloat(float number,int prec)
{
  uint8_t str[10]= {0};

  ftoa_local(number, (uint8_t *)str, prec); // convert the float type to a string.
  int j = 9;
  while(str[j]==0)
  j--;
  for(int i = 7; i >= 0; i--)
  {
    switch(str[j])
    {
      case ' ':
      break;
      case '.':
       LedData[i] |= 0x01;
       i++;
      break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
       LedData[i] |= LED_MODEL[str[j]-'0'] ;
      break;
    }
    j--;
  }
  update();
}

// turn off the dg at special place
void setNoPoint(int place)
{
  LedData[7-place] &= 0xFE;
  update();
}

// set the dg turn on at special place
void setPoint(int place)
{
  LedData[7-place] |= 0x01;
  update();
}

// turn off the display
void turnOffDisplay(void)
{
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  writeData(0x88);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

void turnOnDisplay(void)
{
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  delayMicroseconds(5);
  writeData(0x8A);
  delayMicroseconds(5);
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

// the light scope is 0-7
void setDisplayLight(int light)
{
  if(light>=0 && light <=7)
  {
    LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
    delayMicroseconds(5);
    writeData(0x80|light);
    delayMicroseconds(5);
    LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
  }
}

void writeData(uint8_t data)
{
  shiftOut(LSBFIRST,data);
}

// clear the display data
void clear(void)
{
  for(int i = 0; i<8; i++)
  {
    LedData[i] = 0x00;    
  }
  update();
}

// write data to the TM1638
void update(void)
{
  uint8_t buff[8] =
  {
    0x00
  };
  for(int i = 0; i < 8 ; i++)
  {
    uint8_t temp = 0x00;
    for (int j = 0; j < 8; j++)
    {
      temp = LedData[j] & ( 1 << (7-i) ); // first get the high bit
      buff[i] = buff[i] | (temp << i) >> j;
    }
  }
  // writeData(0x40); // write data command
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  writeData(0xc0); // set the reg display address to the 0x00
  // write the 16 byte display data
  for( int i = 0; i < 8 ; i++)
  {
    writeData(buff[i]);
    // Serial.println(buff[i]);
    writeData(0x00);
  }
  LL_GPIO_SetOutputPin(STB_GPIO_Port, STB_Pin);
}

static inline uint8_t
TM1638_Read(void)
{
  uint8_t i;
  uint8_t temp = 0;
  for (i = 0; i<8; i++)
  {
    temp >>= 1;
    LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin);
    delayMicroseconds(127);
    if (pin_read(DIO_GPIO_Port, DIO_Pin) == 1)
    {
      temp |= 0x80;
      delayMicroseconds(127);
    }
    LL_GPIO_SetOutputPin(CLK_GPIO_Port, CLK_Pin);
    delayMicroseconds(127);
  }
  pin_mode(DIO_GPIO_Port, DIO_Pin, LL_GPIO_MODE_OUTPUT);
  return temp;
}

static inline void
TM1638_Write(uint8_t DATA)
{
  uint8_t i;
  LL_GPIO_ResetOutputPin(DIO_GPIO_Port, DIO_Pin);
  delayMicroseconds(127);
  for (i = 0; i<8; i++)
  {
    LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin);
    delayMicroseconds(127);
    if (DATA & 0X01)
    {
      LL_GPIO_SetOutputPin(DIO_GPIO_Port, DIO_Pin);
      delayMicroseconds(127);
    }
    else
    {
      LL_GPIO_ResetOutputPin(DIO_GPIO_Port, DIO_Pin);
      delayMicroseconds(127);
    }
    DATA >>= 1;
    LL_GPIO_SetOutputPin(CLK_GPIO_Port, CLK_Pin);
    delayMicroseconds(127);
  }
}

uint8_t Read_Key(void)
{
  uint8_t c[4], i, key_value = 0;
  LL_GPIO_ResetOutputPin(STB_GPIO_Port, STB_Pin);
  TM1638_Write(0x42);
  delayMicroseconds(1000);
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

