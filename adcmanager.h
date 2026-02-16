#ifndef ADCMANAGER_H
#define ADCMANAGER_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

// =========================
// Tipos públicos
// =========================

typedef enum
{
    ADC_CH_0 = 0,
    ADC_CH_1,
    ADC_CH_2,
    ADC_CH_3
} adc_channel_t;

typedef enum
{
    ADC_SPS_8,
    ADC_SPS_16,
    ADC_SPS_32,
    ADC_SPS_64,
    ADC_SPS_128,
    ADC_SPS_250,
    ADC_SPS_475,
    ADC_SPS_860
} adc_sps_t;

typedef enum
{
    RANGE_NONE,

    RANGE_DC_200mV,
    RANGE_DC_2V,
    RANGE_DC_20V,
    RANGE_DC_200V,

    RANGE_AC_2V,
    RANGE_AC_20V,
    RANGE_AC_200V,

    RANGE_CURR_20mA,
    RANGE_CURR_200mA,
    RANGE_CURR_16A,

    RANGE_OHM_100,
    RANGE_OHM_10K,
    RANGE_OHM_1M,

    RANGE_TRANSISTOR
} adc_range_id_t;

// =========================
// API pública
// =========================

void adc_manager_init(void);

void adc_manager_select(adc_range_id_t range);

bool adc_manager_read_raw(int16_t *raw);

float adc_manager_read_voltage(void);

adc_range_id_t adc_manager_current_range(void);

void adc_manager_set_sps(adc_sps_t sps);

#endif // ADCMANAGER_H