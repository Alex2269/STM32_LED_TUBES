
/* USER CODE BEGIN 0 */
extern uint32_t tim_counter;
/* USER CODE END 0 */

/**
* @brief This function handles EXTI line[9:5] interrupts.
*/
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
  ++tim_counter; // if(tim_counter>9999)tim_counter=0;
  /* USER CODE END EXTI9_5_IRQn 1 */
}

