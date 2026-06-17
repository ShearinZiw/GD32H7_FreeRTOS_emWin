#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H

#include "gd32h7xx.h"

#define BUZZER_GPIO_PORT       GPIOB
#define BUZZER_GPIO_CLK_PORT   RCU_GPIOB
#define BUZZER_GPIO_PIN        GPIO_PIN_0
#define BUZZER_ACTIVE_HIGH     1

typedef enum {
    BUZZER_SOUND_CLICK = 0,
    BUZZER_SOUND_PAGE,
    BUZZER_SOUND_MEDIA,
    BUZZER_SOUND_POWER
} BUZZER_SOUND;

void Buzzer_GPIO_Config(void);
void Buzzer_Play(BUZZER_SOUND Sound);
void Buzzer_Service(void);
void Buzzer_Stop(void);

#endif /* __BSP_BUZZER_H */
