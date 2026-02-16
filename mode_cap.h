#ifndef MODE_CAP_H
#define MODE_CAP_H

#include <Arduino.h>
#include "config.h"

// Medición principal de capacitancia
float measureCapacitance();

// ESR integrado como submodo CAP
float measureESR();

// Pantallas
void showCapacitance();
void showESR();

// Modo CAP completo
void measureCAPMode();

#endif