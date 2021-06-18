/* USER CODE BEGIN Includes */
#include "1637.h"
#include "main.h"

// GPIOC HEATING_Pin GPIO_PIN_13
// GPIOC MEASURE_Pin GPIO_PIN_14
/* USER CODE END Includes */
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint32_t adcResult = 0;
long num;
uint8_t adc_counter;
long display;
uint16_t tim_counter; // ms interval
/* USER CODE END PV */
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim4);
  HAL_TIM_Base_Start_IT(&htim4);
  /* USER CODE END 2 */

  /* USER CODE BEGIN 3 */
    if(tim_counter>50&&tim_counter<65) // подать накал
    {
      HAL_GPIO_WritePin(GPIOC, HEATING_Pin, GPIO_PIN_RESET);
    }
    else // снять накал
      HAL_GPIO_WritePin(GPIOC, HEATING_Pin, GPIO_PIN_SET);

    if(tim_counter>910&&tim_counter<925)
    {
      HAL_GPIO_WritePin(GPIOC, MEASURE_Pin, GPIO_PIN_RESET); // подключить через делитель к земле
      //HAL_GPIO_WritePin(GPIOC, MEASURE_Pin, GPIO_PIN_SET); // подать напряжение измерения      
      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);
      adcResult += HAL_ADC_GetValue(&hadc1);
      adc_counter++; // Увеличиваем счетчик измерений
      display = adcResult/adc_counter/1.246; // Вычисляем среднее значение
      adc_counter = 0; // Обнуляем счетчик измерений
      adcResult = 0; // Обнуляем значение
      HAL_ADC_Stop(&hadc1);
      indication(display);
    }
    else
      HAL_GPIO_WritePin(GPIOC, MEASURE_Pin, GPIO_PIN_SET); // отключить делитель от земли
      // HAL_GPIO_WritePin(GPIOC, MEASURE_Pin, GPIO_PIN_RESET); // снять напряжение измерения
  }
  /* USER CODE END 3 */
