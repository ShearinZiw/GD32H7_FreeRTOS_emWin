#include "adc/bsp_speed_adc.h"

static uint16_t _LastRaw;

void SpeedADC_Init(void) {
    rcu_periph_clock_enable(RCU_SYSCFG);
    rcu_periph_clock_enable(SPEED_ADC_CLK);
    syscfg_analog_switch_enable(SYSCFG_PA0_ANALOG_SWITCH);

    adc_deinit(SPEED_ADC_PERIPH);
    adc_clock_config(SPEED_ADC_PERIPH, ADC_CLK_SYNC_HCLK_DIV6);
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    adc_special_function_config(SPEED_ADC_PERIPH, ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(SPEED_ADC_PERIPH, ADC_CONTINUOUS_MODE, DISABLE);
    adc_data_alignment_config(SPEED_ADC_PERIPH, ADC_DATAALIGN_RIGHT);
    adc_resolution_config(SPEED_ADC_PERIPH, ADC_RESOLUTION_12B);
    adc_channel_length_config(SPEED_ADC_PERIPH, ADC_REGULAR_CHANNEL, 1U);
    adc_regular_channel_config(SPEED_ADC_PERIPH, 0U, SPEED_ADC_CHANNEL, 480U);
    adc_external_trigger_config(SPEED_ADC_PERIPH, ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
    adc_end_of_conversion_config(SPEED_ADC_PERIPH, ADC_EOC_SET_CONVERSION);
    adc_enable(SPEED_ADC_PERIPH);
    adc_calibration_mode_config(SPEED_ADC_PERIPH, ADC_CALIBRATION_OFFSET_MISMATCH);
    adc_calibration_number(SPEED_ADC_PERIPH, ADC_CALIBRATION_NUM1);
    adc_calibration_enable(SPEED_ADC_PERIPH);
}

uint16_t SpeedADC_ReadRaw(void) {
    uint32_t Timeout;

    adc_flag_clear(SPEED_ADC_PERIPH, ADC_FLAG_EOC);
    adc_software_trigger_enable(SPEED_ADC_PERIPH, ADC_REGULAR_CHANNEL);
    Timeout = 100000U;
    while ((adc_flag_get(SPEED_ADC_PERIPH, ADC_FLAG_EOC) == RESET) && (Timeout > 0U)) {
        Timeout--;
    }
    if (Timeout == 0U) {
        return _LastRaw;
    }
    _LastRaw = (uint16_t)(adc_regular_data_read(SPEED_ADC_PERIPH) & 0x0FFFU);
    return _LastRaw;
}

uint16_t SpeedADC_RawToKmh(uint16_t Raw) {
    return (uint16_t)(((uint32_t)Raw * SPEED_ADC_MAX_KMH + (SPEED_ADC_MAX_RAW / 2U)) / SPEED_ADC_MAX_RAW);
}
