#include "matrix.h"
#include "globals.h"
#include <stdint.h>

// ===============================
// PCF8574 bit masks (shadow byte)
// ===============================
#define PCF_ZENER_AQY (1 << 7)

void matrix_disconnect_all()
{
    // Apagar SSR del ATmega
    digitalWrite(pin.SSR_LOW, LOW);
    digitalWrite(pin.SSR_MID, LOW);
    digitalWrite(pin.SSR_HIGH, LOW);
    digitalWrite(pin.SSR_SHUNT, LOW);

    // PCF shadow register
    pcf_state &= ~PCF_ZENER_AQY;
    for (uint8_t i = 0; i < 8; i++)
    {
        pcfExpander.digitalWrite(i, (pcf_state >> i) & 0x01);
    }
}

// -------------------------------
// Test points (ADS1115 selecciona canal internamente)
// -------------------------------
void matrix_select_tp1()
{
    // El ADS1115 selecciona canal dentro de readADC()
}

void matrix_select_tp2()
{
    // El ADS1115 selecciona canal dentro de readADC()
}

void matrix_select_tp3()
{
    // El ADS1115 selecciona canal dentro de readADC()
}

// -------------------------------
// Shunts (corriente)
// -------------------------------
void matrix_shunt_low()
{
    digitalWrite(pin.SSR_SHUNT, HIGH);
}

void matrix_shunt_mid()
{
    digitalWrite(pin.SSR_SHUNT, HIGH);
}

void matrix_shunt_high()
{
    digitalWrite(pin.SSR_SHUNT, HIGH);
}

// -------------------------------
// Rangos OHM (74HC138)
// -------------------------------
void matrix_ohm_low()
{
    digitalWrite(pin.RNG0, LOW);
    digitalWrite(pin.RNG1, LOW);
    digitalWrite(pin.RNG2, LOW);
}

void matrix_ohm_mid()
{
    digitalWrite(pin.RNG0, HIGH);
    digitalWrite(pin.RNG1, LOW);
    digitalWrite(pin.RNG2, LOW);
}

void matrix_ohm_high()
{
    digitalWrite(pin.RNG0, LOW);
    digitalWrite(pin.RNG1, HIGH);
    digitalWrite(pin.RNG2, LOW);
}