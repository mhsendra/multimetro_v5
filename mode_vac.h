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
// Modo completo
// =====================================================
void measureVAC_MODE(VacSubMode submode);

// =====================================================
// Wrappers para menú
// =====================================================
void measureVAC_Main(void);
void measureVAC_Rel(void);

// Canal ADC a usar para VAC
#define ADC_CHANNEL_FOR_VAC ADC_CH_VOLTAGE

// Wrapper genérico para leer un canal single
float measureADC_Single(enum ADC_CHANNEL_SINGLE ch);

#endif // MODE_VAC_H