#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "config.h"
#include "adcmanager.h"

typedef struct
{
    float current; // Corriente medida con shunt 0.1Ω
    float voltage; // Voltaje medido con shunt 0.033Ω
    ADC_RANGE_ID range_current;
    ADC_RANGE_ID range_voltage;
} measurement_result_t;

// Inicialización del módulo de medición
void measurement_init(void);

// Lee ambos canales y devuelve resultados con autorango aplicado
measurement_result_t measure_channels(void);

#endif // MEASUREMENT_H