#include "globals.h"

static bool rng_released = false;

// Guardar estado original por si quieres restaurar valores
static uint8_t rng0_saved;
static uint8_t rng1_saved;
static uint8_t rng2_saved;

void rng_release_for_gpio()
{
    if (rng_released)
        return;

    // Guardar estado actual
    rng0_saved = digitalRead(pin.RNG0);
    rng1_saved = digitalRead(pin.RNG1);
    rng2_saved = digitalRead(pin.RNG2);

    // Liberar pines
    pinMode(pin.RNG0, INPUT);
    pinMode(pin.RNG1, INPUT);
    pinMode(pin.RNG2, INPUT);

    rng_released = true;
}

void rng_restore_for_ohm()
{
    if (!rng_released)
        return;

    pinMode(pin.RNG0, OUTPUT);
    pinMode(pin.RNG1, OUTPUT);
    pinMode(pin.RNG2, OUTPUT);

    digitalWrite(pin.RNG0, rng0_saved);
    digitalWrite(pin.RNG1, rng1_saved);
    digitalWrite(pin.RNG2, rng2_saved);

    rng_released = false;
}