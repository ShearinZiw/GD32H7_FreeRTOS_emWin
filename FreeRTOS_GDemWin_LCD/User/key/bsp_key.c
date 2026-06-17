#include "key/bsp_key.h"

static uint8_t _StablePressed;
static uint8_t _DebouncePressed;

static uint8_t _ReadRawPressed(void) {
    uint8_t State;

    State = 0U;
    if (gpio_input_bit_get(BOARD_KEY1_GPIO_PORT, BOARD_KEY1_GPIO_PIN) == RESET) {
        State |= BOARD_KEY1_MASK;
    }
    if (gpio_input_bit_get(BOARD_KEY2_GPIO_PORT, BOARD_KEY2_GPIO_PIN) == RESET) {
        State |= BOARD_KEY2_MASK;
    }
    return State;
}

void BoardKey_Init(void) {
    rcu_periph_clock_enable(BOARD_KEY1_GPIO_CLK);
    rcu_periph_clock_enable(BOARD_KEY2_GPIO_CLK);

    gpio_mode_set(BOARD_KEY1_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BOARD_KEY1_GPIO_PIN);
    gpio_mode_set(BOARD_KEY2_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BOARD_KEY2_GPIO_PIN);

    _StablePressed = _ReadRawPressed();
    _DebouncePressed = _StablePressed;
}

uint8_t BoardKey_Scan(void) {
    uint8_t Raw;
    uint8_t NewPress;

    Raw = _ReadRawPressed();
    if (Raw != _DebouncePressed) {
        _DebouncePressed = Raw;
        return 0U;
    }

    NewPress = (uint8_t)(Raw & (uint8_t)~_StablePressed);
    _StablePressed = Raw;
    return NewPress;
}
