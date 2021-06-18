
#define PORT GPIOB
#define HIGH 1
#define LOW 0

void delay_us(uint16_t us);
void indication(int n);
void write(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);
void start(void);
void stop(void);
void writeValue(uint8_t value);

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}bool;

uint8_t digits[] =
{
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

void delay_us(uint16_t us)
{
  while (us)
  {
//    _delay_us(1);
    us--;
  }
}

void indication(int n)
{
  int n1, n2, n3, n4;
  n1 = (int) n % 10;
  n2 = (int) ((n % 100)-n1)/10;
  n3 = (int) ((n % 1000) - n2 - n1) / 100;
  n4 = (int) ((n % 10000) - n3 - n2 - n1) / 1000;
  write(digits[n4], digits[n3], digits[n2], digits[n1]);
}

void write(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
{
  start();
  writeValue(0x8c);
  start();
  writeValue(0x40);
  stop();
  start();
  writeValue(0xc0);
  writeValue(first);
  writeValue(second);
  writeValue(third);
  writeValue(fourth);
  stop();
}

void start(void)
{
  HAL_GPIO_WritePin(PORT, CLK_Pin, HIGH);
  HAL_GPIO_WritePin(PORT, DIO_Pin, HIGH);
  HAL_Delay(1/200);
  HAL_GPIO_WritePin(PORT, DIO_Pin, LOW);
  HAL_GPIO_WritePin(PORT, CLK_Pin, LOW);
  HAL_Delay(1/200);
}

void stop(void)
{
  HAL_GPIO_WritePin(PORT, CLK_Pin, LOW);
  HAL_GPIO_WritePin(PORT, DIO_Pin, LOW);
  HAL_Delay(1/200);
  HAL_GPIO_WritePin(PORT, CLK_Pin, HIGH);
  HAL_GPIO_WritePin(PORT, DIO_Pin, HIGH);
  HAL_Delay(1/200);
}

void writeValue(uint8_t value)
{
  uint8_t i;
  for(i = 0; i < 8; i++)
  {
    HAL_GPIO_WritePin(PORT, CLK_Pin, LOW);
    HAL_Delay(1/200);
    bool not_null = ((value & (1 << i)) >> i);
    if(not_null)
      HAL_GPIO_WritePin(PORT, DIO_Pin, HIGH);
    else
      HAL_GPIO_WritePin(PORT, DIO_Pin, LOW);
    HAL_Delay(1/200);
    HAL_GPIO_WritePin(PORT, CLK_Pin, HIGH);
    HAL_Delay(1/200);
  }
  HAL_GPIO_WritePin(PORT, CLK_Pin, LOW);
  HAL_Delay(1/200);
  HAL_GPIO_WritePin(PORT, CLK_Pin, HIGH);
  HAL_Delay(1/200);
}
