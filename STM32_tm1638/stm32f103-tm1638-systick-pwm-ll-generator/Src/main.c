/* USER CODE BEGIN Header */
/**
 * simple pwm generator from 300 uno_second to 100 millisecond and more
 * using SysTick timer only
 * the only experiment using SysTick timer
**/
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_tick.h"
#include "TM1638Lib.h"
// #include "TM1638.h"
#include "gpio_pin.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int16_t key_old;
int16_t key_new;
uint16_t update_tim_period=50;
volatile uint32_t clk_counter;
volatile uint16_t duty_cycle = 50;

// #pragma GCC push_options
// #pragma GCC optimize("O3")
void SYSTICK_callback(void)
{
  static volatile uint64_t pwm_counter;
  static volatile uint32_t this_counter;
  ++this_counter;

  if(!pwm_counter) ticks_set(0); // start chronometry

  if(this_counter > update_tim_period)
  {
    this_counter = 0;
    ++pwm_counter;
    if(pwm_counter > 99)
    {
        pwm_counter = 0;
        clk_counter = ticks_get(); // read chronometry
    }
  }

  if(pwm_counter < duty_cycle)
  {
    GPIO_Pin_Clear(GPIOC, 13);
  }
  else
  {
    GPIO_Pin_Set(GPIOC, 13);
  }
}
// #pragma GCC pop_options

/**
 * it is clocked from system quanta of rtos
 * width base quants_base for example is 100 quanta
 */
void soft_pwm(GPIO_TypeDef *GPIOx, uint32_t Pin, uint16_t duty_cycle, uint16_t quants_base)
{
  LL_mDelay(duty_cycle);
  LL_GPIO_ResetOutputPin(GPIOx, Pin); 

  LL_mDelay(quants_base-duty_cycle);
  LL_GPIO_SetOutputPin(GPIOx, Pin); 
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
  */
  LL_GPIO_AF_Remap_SWJ_NOJTAG();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  SysTick_config(105); // 116 (140); // value(352) = 1usec 
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  // HAL_TIM_Base_Init(&htim4);
  // HAL_TIM_PeriodElapsedCallback(&htim4);
  // HAL_TIM_Base_Start_IT(&htim4); // timer run
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  ticks_init();

  TM1638_indicate(12345678);
  LL_mDelay(123456);
  
  // displayFloat(88226.135,3);
  // LL_mDelay(523456);
  // setDigital(2,9);
  // setDigital(1,5);
  // LL_mDelay(323456);
  // key_new = Read_Key();
  // displayInt(duty_cycle*1e6L+(clk_counter/71.25f));

  while (1)
  {
    key_new = ReadKey();

    if(key_new)
    {
      // HAL_TIM_Base_Stop(&htim4);
      // HAL_TIM_Base_Stop_IT(&htim4);
    }

    if(key_new&&(key_new!=13)&&(key_new!=14)&&(key_new!=15)&&(key_new!=16))
    {
      key_old=key_new;
      update_tim_period = key_old*50;
    }

    if(key_new==13)
    {
      key_old--;
      duty_cycle--;
    }

    if(key_new==14)
    {
      key_old++;
      duty_cycle++;
    }

    if(key_new==15)
    {
      key_old--;
      update_tim_period--;
      if(update_tim_period<1)
         update_tim_period=1;
    }

    if(key_new==16)
    {
      key_old++;
      update_tim_period++;
    }

    if(key_old <= 1) key_old = 1;
    if(key_old > 299) key_old = 300;

    if(duty_cycle <= 1) duty_cycle = 1;
    if(duty_cycle >= 99) duty_cycle = 99;

    LL_mDelay(23550);

    // multiplicate duty_cycle and time period for show on one display
    TM1638_indicate_two(duty_cycle*1e6L+(clk_counter/100.0f)*1.40f);

    key_new = 0;

    // TIM4_reinit();
    // HAL_TIM_Base_Init(&htim4);
    // HAL_TIM_Base_Start_IT(&htim4); // timer run

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
