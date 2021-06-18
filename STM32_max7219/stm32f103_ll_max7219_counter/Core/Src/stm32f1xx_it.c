/* USER CODE BEGIN 0 */
extern uint32_t tim_counter;
/* USER CODE END 0 */

  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
  ++tim_counter; // if(tim_counter>9999)tim_counter=0;
  /* USER CODE END EXTI9_5_IRQn 1 */
