#include "measurement.h"

void measurement_init(void)
{
    adc_manager_init();
}

// Lectura de ambos canales con autorango y escalado
measurement_result_t measure_channels(void)
{
    measurement_result_t res;

    float raw1_mv, raw2_mv;

    // Autorango para shunt 0.1Ω (corriente)
    res.range_current = adc_manager_autorange(ADC_CH_SHUNT1, &raw1_mv);
    res.current = raw1_mv / SHUNT1_R;

    // Autorango para shunt 0.033Ω (voltaje)
    res.range_voltage = adc_manager_autorange(ADC_CH_SHUNT2, &raw2_mv);
    res.voltage = raw2_mv / SHUNT2_R;

    return res;
}

float measureVAC_raw(void)
{
    measurement_result_t meas = measure_channels();
    return meas.voltage;
}