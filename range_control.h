#ifndef RANGE_CONTROL_H
#define RANGE_CONTROL_H

#include "config.h" // Para OhmSubMode
#include <Arduino.h>

// =====================================================
// LIBERAR / RESTAURAR RNG (usados como GPIO en OHM)
// =====================================================

void rng_release_for_gpio();
void rng_restore_for_ohm();

// =====================================================
// SELECCIÓN DE RANGO OHM (control SSR)
// =====================================================

void ohm_select_range(OhmSubMode mode);

#endif // RANGE_CONTROL_H