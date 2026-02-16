#include "adcmanager.h"
#include <Adafruit_ADS1X15.h>

// =========================
// Instancia ADS1115
// =========================

static Adafruit_ADS1115 ads;

// =========================
// Estado interno
// =========================

static adc_range_id_t current_range = RANGE_NONE;
static adc_channel_t current_channel = ADC_CH_0;
static adsGain_t current_gain = GAIN_ONE;
static adc_sps_t current_sps = ADC_SPS_250;

// =========================
// Helpers internos
// =========================

static void apply_gain_for_range(adc_range_id_t range)
{
    switch (range)
    {
    case RANGE_DC_200mV:
        current_gain = GAIN_SIXTEEN; // ±0.256V
        break;

    case RANGE_CURR_20mA:
        current_gain = GAIN_EIGHT; // ±0.512V
        break;

    case RANGE_DC_2V:
        current_gain = GAIN_TWO; // ±2.048V
        break;

    default:
        current_gain = GAIN_ONE; // ±4.096V
        break;
    }

    ads.setGain(current_gain);
}

static void apply_sps(void)
{
    switch (current_sps)
    {
    case ADC_SPS_8:
        ads.setDataRate(RATE_ADS1115_8SPS);
        break;
    case ADC_SPS_16:
        ads.setDataRate(RATE_ADS1115_16SPS);
        break;
    case ADC_SPS_32:
        ads.setDataRate(RATE_ADS1115_32SPS);
        break;
    case ADC_SPS_64:
        ads.setDataRate(RATE_ADS1115_64SPS);
        break;
    case ADC_SPS_128:
        ads.setDataRate(RATE_ADS1115_128SPS);
        break;
    case ADC_SPS_250:
        ads.setDataRate(RATE_ADS1115_250SPS);
        break;
    case ADC_SPS_475:
        ads.setDataRate(RATE_ADS1115_475SPS);
        break;
    case ADC_SPS_860:
        ads.setDataRate(RATE_ADS1115_860SPS);
        break;
    }
}

// =========================
// API pública
// =========================

void adc_manager_init(void)
{
    ads.begin();
    current_range = RANGE_NONE;
    current_sps = ADC_SPS_250;
    apply_sps();
}

void adc_manager_select(adc_range_id_t range)
{
    current_range = range;

    switch (range)
    {
    case RANGE_CURR_20mA:
    case RANGE_CURR_200mA:
        current_channel = ADC_CH_2;
        break;

    case RANGE_CURR_16A:
        current_channel = ADC_CH_3;
        break;

    default:
        current_channel = ADC_CH_0;
        break;
    }

    apply_gain_for_range(range);
}

bool adc_manager_read_raw(int16_t *raw)
{
    if (!raw)
        return false;

    int16_t value = 0;

    switch (current_channel)
    {
    case ADC_CH_0:
        value = ads.readADC_SingleEnded(0);
        break;
    case ADC_CH_1:
        value = ads.readADC_SingleEnded(1);
        break;
    case ADC_CH_2:
        value = ads.readADC_SingleEnded(2);
        break;
    case ADC_CH_3:
        value = ads.readADC_SingleEnded(3);
        break;
    }

    *raw = value;
    return true;
}

float adc_manager_read_voltage(void)
{
    int16_t raw;
    if (!adc_manager_read_raw(&raw))
        return 0.0f;

    return ads.computeVolts(raw);
}

adc_range_id_t adc_manager_current_range(void)
{
    return current_range;
}

void adc_manager_set_sps(adc_sps_t sps)
{
    current_sps = sps;
    apply_sps();
}
float adc_manager_raw_to_voltage(int16_t raw)
{
    return ads.computeVolts(raw);
}