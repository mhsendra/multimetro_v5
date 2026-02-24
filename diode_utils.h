#ifndef DIODE_UTILS_H
#define DIODE_UTILS_H

#include <Arduino.h>
#include "config.h"
#include "adcmanager.h"

bool diodeConducts(uint8_t pinAnode, uint8_t pinCathode);

#endif