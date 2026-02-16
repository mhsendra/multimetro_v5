#ifndef MODE_FREQ_H
#define MODE_FREQ_H

#include <Arduino.h>
#include "config.h"

// Medidas base
float measureFrequency_raw();
float measureFrequency_calibrated();
float measureDutyCycle();
float measurePulseWidth();
float measurePeriod();

// Pantallas
void showFrequency();
void showDutyCycle();
void showPulseWidth();
void showPeriod();

// Modo completo
void measureFREQ();

#endif