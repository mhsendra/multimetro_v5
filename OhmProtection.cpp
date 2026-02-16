#include "OhmProtection.h"
#include "adcmanager.h"
#include "config.h"

bool detectVoltageOnOhm()
{
    // Selecciona el rango usado para la medición de protección de OHM
    adc_manager_select(RANGE_OHM_100);

    // Promediar varias lecturas
    const int samples = 20;
    float sum = 0.0f;

    for (int i = 0; i < samples; i++)
    {
        float v = adc_manager_read_voltage();
        sum += v;
    }

    float avg = sum / samples;

    return avg > OHM_PROTECT_THRESHOLD;
}