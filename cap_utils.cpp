#include "cap_utils.h"
#include "adcmanager.h"
#include "globals.h"
#include "config.h"

// =====================================================
// DESCARGA DEL CONDENSADOR
// =====================================================
void dischargeCap()
{
    pinMode(pin.CAP_CHARGE_PIN, OUTPUT);
    digitalWrite(pin.CAP_CHARGE_PIN, LOW);
    delay(CAP_DISCHARGE_MS);
}

// =====================================================
// PROTECCIÓN POR VOLTAJE RESIDUAL
// =====================================================
bool prepareCapMeasurement()
{
    // Seleccionar rango DC_20V del ADS1115
    adc_manager_select(RANGE_DC_20V);
    adc_manager_set_sps(ADC_SPS_475);

    // Leer tensión directamente desde el pin CAP (TP correspondiente)
    float v = adc_manager_read_voltage();

    if (v > CAP_RESIDUAL_VOLT_MAX)
        return false;

    dischargeCap();
    return true;
}