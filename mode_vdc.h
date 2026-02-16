#ifndef MODE_VDC_H
#define MODE_VDC_H

#include <Arduino.h>
#include "globals.h"

// =====================================================
// FUNCIONES DE MEDICIÓN
// =====================================================
float measureVDC_raw(void);
float measureVDC_calibrated(void);
float measureVDC_Relative(void);
float measurePower(void);
float measureEnergy(void);

// =====================================================
// FUNCIONES DE PANTALLA
// =====================================================
void showVDC(void);
void showVDC_Relative(void);
void showPower(void);
void showEnergy(void);

// =====================================================
// MODO COMPLETO VDC
// =====================================================
void measureVDC_MODE(void);

// =====================================================
// UTILIDADES
// =====================================================
bool use_millivolts(float v);

#endif // MODE_VDC_H