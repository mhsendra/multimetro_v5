#ifndef RANGE_CONTROL_H
#define RANGE_CONTROL_H

// Libera RNG0 / RNG1 / RNG2 para usarlos como GPIO
void rng_release_for_gpio();

// Restaura RNG0 / RNG1 / RNG2 para el modo OHM
void rng_restore_for_ohm();

#endif