#ifndef MODE_VAC_H
#define MODE_VAC_H

#include <Arduino.h>
#include "globals.h"
#include "config.h"

// =====================================================
// FUNCIONES DE MEDICIÓN
// =====================================================
float measureVAC_raw(void);
float measureVAC_calibrated(void);
float measureVAC_Relative(void);

// =====================================================
// FUNCIONES DE PANTALLA
// =====================================================
void showVAC(void);
void showVAC_Relative(void);

// =====================================================
// MODO COMPLETO VAC
// =====================================================
void measureVAC_MODE(void);

#endif // MODE_VAC_H