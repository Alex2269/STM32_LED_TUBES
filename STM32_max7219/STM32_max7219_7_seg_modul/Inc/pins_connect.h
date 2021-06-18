/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __pins_connect_H
#define __pins_connect_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Defines ------------------------------------------------------------------*/

#define CLK_Pin GPIO_PIN_0
#define CLK_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_1
#define CS_GPIO_Port GPIOA
#define DIN_Pin GPIO_PIN_2
#define DIN_GPIO_Port GPIOA

// void HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);
// mode: GPIO_MODE_INPUT;GPIO_MODE_OUTPUT_PP;GPIO_MODE_OUTPUT_OD; "pull push"/"open drain"

#ifdef __cplusplus
}
#endif

#endif /* __pins_connect_H */
