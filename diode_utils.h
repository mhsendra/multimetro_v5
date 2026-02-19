#ifndef DIODE_UTILS_H
#define DIODE_UTILS_H

#include <Arduino.h>

bool diodeConducts(uint8_t pinA, uint8_t pinB);
// Devuelve 1, 2 o 3 según el pin TP
uint8_t getTPNumber(uint8_t pin);

#endif