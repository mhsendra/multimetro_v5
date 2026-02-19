#include "cap_utils.h"
#include "adcmanager.h"
#include "globals.h"

bool prepareCapMeasurement()
{
    float mv = 0;

    // Autorango para medición de capacitancia
    ADC_RANGE_ID selectedRange = adc_manager_autorange(ADC_CH_SHUNT1, &mv);

    float v = mv / 1000.0f;

    // Si hay tensión residual, no iniciar carga
    return (v < 0.01f);
}