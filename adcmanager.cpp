#include "adcmanager.h"
#include "globals.h"
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "config.h"
#include <math.h>

#define ADS1115_ADDR 0x48 // dirección I2C por defecto

adsGain_t gain = GAIN_ONE; // valor inicial
ADC_RANGE_ID selected = RANGE_HIGH;

// =========================
// Instancia ADS1115
// =========================
static Adafruit_ADS1115 ads;

// =========================
// Estado interno
// =========================
static enum ADC_SPS current_sps = ADC_SPS_250;

float measureADC_Single(ADC_CHANNEL_SINGLE ch)
{
    int16_t raw = 0;

    switch (ch)
    {
    case ADC_CH_ZENER:
        raw = ads.readADC_SingleEnded(0);
        break;

    case ADC_CH_VOLTAGE:
        raw = ads.readADC_SingleEnded(1);
        break;

    case ADC_CH_OHM:
        raw = ads.readADC_SingleEnded(2);
        break;

    case ADC_CH_NCV:
        raw = ads.readADC_SingleEnded(3);
        break;

    default:
        return NAN;
    }

    return ads.computeVolts(raw);
}

// Configura la velocidad de muestreo
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

// Devuelve el gain de Adafruit según el rango
static adsGain_t gain_for_range(ADC_RANGE_ID range)
{
    switch (range)
    {
    case RANGE_LOW:
        return GAIN_SIXTEEN; // ±0.256V
    case RANGE_MEDIUM:
        return GAIN_EIGHT; // ±0.512V
    case RANGE_HIGH:
        return GAIN_ONE; // ±4.096V
    default:
        return GAIN_ONE;
    }
}

// =========================
// API pública
// =========================

void adc_manager_init(void)
{
    Wire.begin();
    ads.begin();
    ads.setGain(GAIN_ONE);
    current_sps = ADC_SPS_250;
    apply_sps();
}

ADC_RANGE_ID adc_manager_autorange(enum ADC_CHANNEL_DIFF channel, float *mv_out)
{
    int16_t raw = 0;
    ADC_RANGE_ID selected = RANGE_HIGH;

    // Lectura preliminar en rango alto
    ads.setGain(GAIN_ONE);

    if (channel == ADC_CH_SHUNT1)
        raw = ads.readADC_Differential_0_1();
    else
        raw = ads.readADC_Differential_2_3();

    float mv = ads.computeVolts(raw) * 1000.0f;

    // Selección de rango basado en mV y shunt
    if (channel == ADC_CH_SHUNT1) // mA
    {
        if (mv < 20.0f)
        {
            gain = GAIN_SIXTEEN;
            selected = RANGE_LOW;
        }
        else
        {
            gain = GAIN_EIGHT;
            selected = RANGE_MEDIUM;
        }
    }
    else // A
    {
        if (mv < 165.0f)
        {
            gain = GAIN_EIGHT;
            selected = RANGE_MEDIUM;
        }
        else
        {
            gain = GAIN_ONE;
            selected = RANGE_HIGH;
        }
    }

    // Aplicar gain final y descartar primera lectura
    ads.setGain(gain_for_range(selected));
    if (channel == ADC_CH_SHUNT1)
        ads.readADC_Differential_0_1();
    else
        ads.readADC_Differential_2_3();
    delay(1); // opcional

    // Lectura definitiva
    if (channel == ADC_CH_SHUNT1)
        raw = ads.readADC_Differential_0_1();
    else
        raw = ads.readADC_Differential_2_3();

    if (mv_out)
        *mv_out = ads.computeVolts(raw) * 1000.0f;

    return selected;
}

// Leer raw ADC de un canal diferencial
bool adc_manager_read_raw(int16_t *raw, enum ADC_CHANNEL_DIFF channel)
{
    if (!raw)
        return false;

    switch (channel)
    {
    case ADC_CH_SHUNT1:
        *raw = ads.readADC_Differential_0_1();
        break;
    case ADC_CH_SHUNT2:
        *raw = ads.readADC_Differential_2_3();
        break;
    default:
        return false;
    }
    return true;
}

// Leer voltaje en mV
float adc_manager_read_voltage(ADC_CHANNEL_DIFF channel)
{
    int16_t raw;
    if (!adc_manager_read_raw(&raw, channel))
        return 0.0f;

    return ads.computeVolts(raw) * 1000.0f;
}

// Selecciona el rango para el ADC según la corriente
void adc_manager_select(CurrentRange range)
{
    switch (range)
    {
    case CURR_RANGE_mA:        // hasta 20 mA
        ads.setGain(GAIN_ONE); // ±4.096V
        ads_mux = ADC_CH_SHUNT1;
        break;

    case CURR_RANGE_5A:        // hasta 5 A
        ads.setGain(GAIN_TWO); // ±2.048V
        ads_mux = ADC_CH_SHUNT1;
        break;

    case CURR_RANGE_16A:       // hasta 16 A
        ads.setGain(GAIN_TWO); // ±2.048V
        ads_mux = ADC_CH_SHUNT2;
        break;
    }
}

// Ajustar velocidad de muestreo
void adc_manager_set_sps(enum ADC_SPS sps)
{
    current_sps = sps;
    apply_sps();
}