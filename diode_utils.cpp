#include <Arduino.h>
#include "diode_utils.h"
#include "adcmanager.h"
#include "globals.h"

uint8_t getTPNumber(uint8_t pinValue)
{
    if (pinValue == pin.TP1)
        return 1;
    if (pinValue == pin.TP2)
        return 2;
    if (pinValue == pin.TP3)
        return 3;
    return 0;
}

bool diodeConducts(uint8_t pinAnode, uint8_t pinCathode)
{
    // Aquí asumimos que ya se aplicó tensión de prueba entre anodo y cátodo

    float mv = 0;

    // Medimos el voltaje entre anodo y cátodo
    ADC_RANGE_ID selectedRange = adc_manager_autorange(ADC_CH_SHUNT1, &mv);

    float v = mv / 1000.0f;

    // Consideramos que conduce si supera 0.2 V (diode forward drop)
    return (v > 0.2f);
}