#include "led/bsp_buzzer.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

static const uint16_t _aClick[] = { 25, 0 };
static const uint16_t _aPage[]  = { 22, 28, 28, 0 };
static const uint16_t _aMedia[] = { 35, 35, 35, 0 };
static const uint16_t _aPower[] = { 90, 55, 30, 0 };
static const uint16_t _aTune[]  = { 35, 35, 35, 35, 70, 60, 35, 35, 110, 0 };

static const uint16_t * _pPattern;
static uint8_t _PatternIndex;
static uint32_t _NextStepTime;
static uint8_t _IsPlaying;

static uint32_t _GetTimeMS(void) {
    return (uint32_t)xTaskGetTickCount() * (uint32_t)portTICK_PERIOD_MS;
}

static void _SetOutput(int On) {
#if BUZZER_ACTIVE_HIGH
    if (On) {
        gpio_bit_set(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
    } else {
        gpio_bit_reset(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
    }
#else
    if (On) {
        gpio_bit_reset(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
    } else {
        gpio_bit_set(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
    }
#endif
}

void Buzzer_GPIO_Config(void) {
    rcu_periph_clock_enable(BUZZER_GPIO_CLK_PORT);
    gpio_mode_set(BUZZER_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BUZZER_GPIO_PIN);
    gpio_output_options_set(BUZZER_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, BUZZER_GPIO_PIN);
    Buzzer_Stop();
}

void Buzzer_Stop(void) {
    _IsPlaying = 0;
    _pPattern = 0;
    _PatternIndex = 0;
    _SetOutput(0);
}

void Buzzer_Play(BUZZER_SOUND Sound) {
    switch (Sound) {
    case BUZZER_SOUND_PAGE:
        _pPattern = _aPage;
        break;
    case BUZZER_SOUND_MEDIA:
        _pPattern = _aMedia;
        break;
    case BUZZER_SOUND_POWER:
        _pPattern = _aPower;
        break;
    case BUZZER_SOUND_TUNE:
        _pPattern = _aTune;
        break;
    case BUZZER_SOUND_CLICK:
    default:
        _pPattern = _aClick;
        break;
    }
    _PatternIndex = 0;
    _IsPlaying = 1;
    _SetOutput(1);
    _NextStepTime = _GetTimeMS() + _pPattern[0];
}

void Buzzer_Service(void) {
    uint32_t Now;
    uint16_t Duration;

    if ((_IsPlaying == 0) || (_pPattern == 0)) {
        return;
    }
    Now = _GetTimeMS();
    if ((int32_t)(Now - _NextStepTime) < 0) {
        return;
    }
    _PatternIndex++;
    Duration = _pPattern[_PatternIndex];
    if (Duration == 0) {
        Buzzer_Stop();
        return;
    }
    _SetOutput((_PatternIndex & 1U) == 0U);
    _NextStepTime = Now + Duration;
}
