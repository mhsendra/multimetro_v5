#ifndef MODE_VAC_H
#define MODE_VAC_H

#include <stdbool.h>
#include "config.h"

// =====================================================
// Medición
// =====================================================
float measureVAC_RMS(void);
float measureVAC_Relative(void);

// =====================================================
// Pantallas
// =====================================================
void showVAC(void);
void showVAC_Relative(void);

// =====================================================
// Modo completo
// =====================================================
void measureVAC_MODE(VacSubMode submode);

// =====================================================
// Wrappers para menú
// =====================================================
void measureVAC_Main(void);
void measureVAC_Rel(void);

#endif // MODE_VAC_H