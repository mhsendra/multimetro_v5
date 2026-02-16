#ifndef MODE_CURRENT_H
#define MODE_CURRENT_H

#include <Arduino.h>
#include "globals.h"

// =====================================================
// FUNCIONES DE MEDICIÓN
// =====================================================
float measureCURRENT_RAW(void);
float measureCURRENT_calibrated(void);
static float measureCurrent_Shunt_RAW(void); // interno, opcional exponer
static float measureCurrent_ACS_RAW(void);   // interno, opcional exponer

// =====================================================
// FUNCIONES DE PANTALLA
// =====================================================
void showCURRENT(void);

// =====================================================
// MODO COMPLETO
// =====================================================
void measureCURRENT(void);

#endif // MODE_CURRENT_H