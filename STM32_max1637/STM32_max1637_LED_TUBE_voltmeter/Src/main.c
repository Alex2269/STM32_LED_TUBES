/* USER CODE BEGIN Includes */
#include "1637.h"
/* USER CODE END Includes */
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint32_t adcResult = 0;
long num;
uint8_t adc_counter;
long display;
/* USER CODE END PV */
  /* USER CODE BEGIN 3 */
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    adcResult += HAL_ADC_GetValue(&hadc1);
    adc_counter++; // Увеличиваем счетчик измерений
    if(adc_counter > 5) // Если количество измерений больше N
    {
      display = adcResult/adc_counter/1.246; // Вычисляем среднее значение
      adc_counter = 0; // Обнуляем счетчик измерений
      adcResult = 0; // Обнуляем значение
      HAL_ADC_Stop(&hadc1);
    }
    indication(display);
    HAL_Delay(20);
  }
  /* USER CODE END 3 */
