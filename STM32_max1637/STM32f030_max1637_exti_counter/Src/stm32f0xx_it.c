/* USER CODE BEGIN 0 */
extern uint16_t tim_counter;
/* USER CODE END 0 */

  /* USER CODE BEGIN EXTI0_1_IRQn 0 */
  ++tim_counter;
  if(tim_counter>9999)tim_counter=0;
  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
