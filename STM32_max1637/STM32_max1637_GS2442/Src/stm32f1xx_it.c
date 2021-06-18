/* USER CODE BEGIN 0 */
extern uint16_t tim_counter;
/* USER CODE END 0 */
  /* USER CODE BEGIN TIM4_IRQn 0 */
  tim_counter++;
  if(tim_counter>999)
    tim_counter = 0;
  /* USER CODE END TIM4_IRQn 0 */
