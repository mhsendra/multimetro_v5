#ifndef MODE_VDC_H
#define MODE_VDC_H

#include <stdbool.h>
#include "config.h"

// =====================================================
// Medición
// =====================================================
float measureVDC(void);
float measureVDC_Relative(void);
float measurePower(void);
float measureEnergy(void);
float measureCurrentEstimated(void);

// =====================================================
// Pantallas
// =====================================================
void showVDC(void);
void showVDC_Relative(void);
void showPower(void);
void showEnergy(void);
void showEstCurrent(void);

// =====================================================
// Modo completo
// =====================================================
void measureVDC_MODE(VdcSubMode submode);

// =====================================================
// Wrappers para menú
// =====================================================
void measureVDC_Main(void);
void measureVDC_Rel(void);
void measureVDC_Power(void);
void measureVDC_Energy(void);
void measureVDC_EstCurr(void);

#endif // MODE_VDC_H