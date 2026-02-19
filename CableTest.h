#ifndef CABLE_TEST_H
#define CABLE_TEST_H

#include <Arduino.h>
#include "globals.h"

// -------------------------------------------------
// Función principal para medir y mostrar el cable
// -------------------------------------------------
void measureCableMode(void);

// -------------------------------------------------
// Funciones de medición directa (opcional)
// -------------------------------------------------
float measureCable(void); // Retorna la resistencia medida en Ohm

#endif // CABLE_TEST_H
