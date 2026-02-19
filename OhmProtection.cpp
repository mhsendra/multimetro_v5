#include <Arduino.h>
#include "OhmProtection.h"
#include "adcmanager.h"
#include "globals.h"

/* =====================================================
 * DETECCIÓN DE VOLTAJE SOBRE RESISTENCIA (Protección OHM)
 * ===================================================== */
bool detectVoltageOnOhm()
{
    // Variable para almacenar el valor medido en mV
    float mv = 0;

    // Selección automática de rango para el canal diferencial de OHM
    // IMPORTANTE: indicar el canal concreto a leer
    ADC_RANGE_ID selectedRange = adc_manager_autorange(ADC_CH_SHUNT1, &mv);
    // si quieres medir con otro shunt, usa ADC_CH_SHUNT2

    // Convertir a voltios
    float v = mv / 1000.0f;

    // Si hay algún voltaje, hay tensión aplicada y no es seguro medir OHM
    if (v > 0.01f) // umbral 10 mV
    {
        return true;
    }

    return false;
}
