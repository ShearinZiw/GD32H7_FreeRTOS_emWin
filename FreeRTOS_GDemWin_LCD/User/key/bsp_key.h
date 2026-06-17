#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "gd32h7xx.h"
#include <stdint.h>

#define BOARD_KEY1_GPIO_PORT      GPIOA
#define BOARD_KEY1_GPIO_CLK       RCU_GPIOA
#define BOARD_KEY1_GPIO_PIN       GPIO_PIN_0

#define BOARD_KEY2_GPIO_PORT      GPIOD
#define BOARD_KEY2_GPIO_CLK       RCU_GPIOD
#define BOARD_KEY2_GPIO_PIN       GPIO_PIN_7

#define BOARD_KEY1_MASK           0x01U
#define BOARD_KEY2_MASK           0x02U

void BoardKey_Init(void);
uint8_t BoardKey_Scan(void);

#endif /* __BSP_KEY_H */
