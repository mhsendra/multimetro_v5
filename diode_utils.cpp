#include <Arduino.h>
#include "diode_utils.h"
#include "adcmanager.h"
#include "globals.h"
#include "range_control.h"

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
    // Liberamos cualquier rango de medición previo
    rng_release_for_gpio();

    // Aplicamos tensión de prueba: ánodo HIGH, cátodo INPUT
    pinMode(pinAnode, OUTPUT);
    pinMode(pinCathode, INPUT);
    digitalWrite(pinAnode, HIGH);
    delay(5); // pequeña espera para estabilizar

    // Medimos voltaje entre ánodo y cátodo usando ADC single-ended (canal ZENER)
    float Vf = measureADC_Single(ADC_CH_ZENER);

    // Restauramos pines a INPUT para no afectar otros modos
    pinMode(pinAnode, INPUT);
    pinMode(pinCathode, INPUT);

    // Comprobamos si conduce (>0.2 V típico para diodo de silicio)
    if (isnan(Vf))
        return false;

    return (Vf > 0.2f);
}