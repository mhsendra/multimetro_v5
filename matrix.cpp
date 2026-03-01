#include "matrix.h"
#include "globals.h"
#include <Arduino.h>
#include "io_expander_mcp23017.h"

// =====================================================
// DESCONEXIÓN GENERAL
// =====================================================
void matrix_disconnect_all()
{
    // SSR controlados por el MCU
    digitalWrite(pin.SSR_LOW, LOW);
    digitalWrite(pin.SSR_MID, LOW);
    digitalWrite(pin.SSR_HIGH, LOW);
    digitalWrite(pin.SSR_SHUNT, LOW);

    // SSR controlado por el MCP23017
    mcp23017.digitalWrite(mcpPin.SSR_ZENER_CTRL, LOW);
}

// =====================================================
// TEST POINTS (ADC selecciona canal internamente)
// =====================================================
void matrix_select_tp1() { matrix_disconnect_all(); }
void matrix_select_tp2() { matrix_disconnect_all(); }
void matrix_select_tp3() { matrix_disconnect_all(); }

// =====================================================
// SHUNTS (CORRIENTE)
// =====================================================
// Todos los shunts están en el MCU → digitalWrite(pin.xxx)

void matrix_shunt_low()
{
    matrix_disconnect_all();
    digitalWrite(pin.SSR_SHUNT, HIGH); // Shunt 0.1Ω
}

void matrix_shunt_mid()
{
    matrix_disconnect_all();
    digitalWrite(pin.SSR_SHUNT, HIGH); // Shunt 0.033Ω
}

void matrix_shunt_high()
{
    matrix_disconnect_all();
    digitalWrite(pin.SSR_SHUNT, HIGH); // Shunt 0.01Ω o equivalente
}

// =====================================================
// RANGOS OHM
// =====================================================
// Según tu esquema, los rangos OHM están en el MCU (PD5, PD6, PD7)

void matrix_ohm_low()
{
    matrix_disconnect_all();
    digitalWrite(pin.SSR_LOW, HIGH);
}

void matrix_ohm_mid()
{
    matrix_disconnect_all();
    digitalWrite(pin.SSR_MID, HIGH);
}

void matrix_ohm_high()
{
    matrix_disconnect_all();
    digitalWrite(pin.SSR_HIGH, HIGH);
}

// =====================================================
// ZENER (MCP23017)
// =====================================================
void matrix_zener()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(mcpPin.SSR_ZENER_CTRL, HIGH);
}

// =====================================================
// INICIALIZACIÓN
// =====================================================
void matrix_init()
{
    // Configurar todos los pines del MCP23017 como OUTPUT
    for (uint8_t i = 0; i < 16; i++)
    {
        mcp23017.pinMode(i, OUTPUT);
        mcp23017.digitalWrite(i, LOW);
    }

    // Configurar los SSR del MCU como OUTPUT
    pinMode(pin.SSR_LOW, OUTPUT);
    pinMode(pin.SSR_MID, OUTPUT);
    pinMode(pin.SSR_HIGH, OUTPUT);
    pinMode(pin.SSR_SHUNT, OUTPUT);

    // Asegurar que arrancan apagados
    digitalWrite(pin.SSR_LOW, LOW);
    digitalWrite(pin.SSR_MID, LOW);
    digitalWrite(pin.SSR_HIGH, LOW);
    digitalWrite(pin.SSR_SHUNT, LOW);
}
