/* USER CODE BEGIN Header */
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
//#include <stdlib.h>
#include "pins_ext.h"
#include "TM1638Lib.h"

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
uint16_t update_tim_period=500;
int16_t key_old;
int16_t key_new;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint64_t ms_counter;
volatile uint64_t tmp_counter;
volatile int8_t duty_cycle = 50;
  
// void my_SYSTICK_Callback(void)
void HAL_SYSTICK_Callback(void)
{
  ms_counter++;

  if(ms_counter>(update_tim_period-1))
  {
    ms_counter = 0;
    tmp_counter++;
    if(tmp_counter > 100) tmp_counter = 0;
  }

  if(tmp_counter < duty_cycle)
  {
    // CLEAR_BIT(GPIOC->ODR,GPIO_ODR_ODR13);
    // GPIOC->BRR = GPIO_BRR_BR13;
    GPIOC->BRR=1<<13;
  }
  else
  {
    // SET_BIT(GPIOC->ODR,GPIO_ODR_ODR13);
    // GPIOC->BSRR = GPIO_BRR_BR13;
    GPIOC->BSRR=1<<13;
  }
}

void toggle_PC13(void)
{
    GPIOC->ODR ^= 1<<13;
}

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
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  SysTick_Config(SystemCoreClock/100000);
  /* USER CODE BEGIN SysInit */

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
  TM1638_indicate(12345678);
  
  HAL_Delay(29750);

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
    }

    if(key_new==16)
    {
      key_old++;
      update_tim_period++;
    }

    if(key_old <= 1) key_old = 1;
    if(key_old > 299) key_old = 300;

    if(duty_cycle < 1) duty_cycle = 0;
    if(duty_cycle > 99) duty_cycle = 100;

    HAL_Delay(7525);

    TM1638_indicate_two(duty_cycle*100000+(update_tim_period>>1));

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
