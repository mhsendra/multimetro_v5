#ifndef MODE_OHM_H
#define MODE_OHM_H

#include <stdbool.h>
#include "config.h"

// =====================================================
// Medición
// =====================================================
float measureOHM_Raw(void);
float measureOHM_Calibrated(void);
float measureOHM_ForSubMode(void); // general para submodos si necesitas

// =====================================================
// Pantallas
// =====================================================
void showOHM_R(void);
void showOHM_Cont(void);
void showOHM_Rel(void);
void showOHM_Cable(void);

// =====================================================
// Modo completo
// =====================================================
void measureOHM_MODE(OhmSubMode submode);

// =====================================================
// Wrappers para menú
// =====================================================
void measureOHM_Main(void);
void measureOHM_Cont_Wrap(void);
void measureOHM_Rel_Wrap(void);
void measureOHM_Cable_Wrap(void);

#endif // MODE_OHM_H