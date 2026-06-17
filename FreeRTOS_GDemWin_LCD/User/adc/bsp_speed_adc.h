#ifndef __BSP_SPEED_ADC_H
#define __BSP_SPEED_ADC_H

#include "gd32h7xx.h"
#include <stdint.h>

#define SPEED_ADC_PERIPH          ADC0
#define SPEED_ADC_CLK             RCU_ADC0
#define SPEED_ADC_CHANNEL         ADC_CHANNEL_0
#define SPEED_ADC_MAX_RAW         4095U
#define SPEED_ADC_MAX_KMH         240U

void SpeedADC_Init(void);
uint16_t SpeedADC_ReadRaw(void);
uint16_t SpeedADC_RawToKmh(uint16_t Raw);

#endif /* __BSP_SPEED_ADC_H */
