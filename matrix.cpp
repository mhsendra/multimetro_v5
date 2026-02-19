#include "matrix.h"
#include "globals.h"
#include <Arduino.h>
#include "io_expander_mcp23017.h"

// =====================================================
// DESCONEXIÓN GENERAL
// =====================================================
void matrix_disconnect_all()
{
    // Apagar todos los SSR
    mcp23017.digitalWrite(pin.SSR_LOW, LOW);
    mcp23017.digitalWrite(pin.SSR_MID, LOW);
    mcp23017.digitalWrite(pin.SSR_HIGH, LOW);
    mcp23017.digitalWrite(pin.SSR_SHUNT, LOW);

    // Liberar rangos OHM
    mcp23017.digitalWrite(pin.RNG0, LOW);
    mcp23017.digitalWrite(pin.RNG1, LOW);
    mcp23017.digitalWrite(pin.RNG2, LOW);
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
void matrix_shunt_low()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(pin.SSR_SHUNT, HIGH); // Shunt 0.1Ω
}

void matrix_shunt_mid()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(pin.SSR_SHUNT, HIGH); // Shunt 0.033Ω
}

void matrix_shunt_high()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(pin.SSR_SHUNT, HIGH); // Puede ser el mismo pin en V5
}

// =====================================================
// RANGOS OHM
// =====================================================
void matrix_ohm_low()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(pin.RNG0, LOW);
    mcp23017.digitalWrite(pin.RNG1, LOW);
    mcp23017.digitalWrite(pin.RNG2, LOW);
}

void matrix_ohm_mid()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(pin.RNG0, HIGH);
    mcp23017.digitalWrite(pin.RNG1, LOW);
    mcp23017.digitalWrite(pin.RNG2, LOW);
}

void matrix_ohm_high()
{
    matrix_disconnect_all();
    mcp23017.digitalWrite(pin.RNG0, LOW);
    mcp23017.digitalWrite(pin.RNG1, HIGH);
    mcp23017.digitalWrite(pin.RNG2, LOW);
}

// =====================================================
// ZENER
// =====================================================
void matrix_zener()
{
    matrix_disconnect_all();
    // Activar SSR Zener a través del MCP23017
    mcp23017.digitalWrite(mcpPin.SSR_ZENER, HIGH);
}

// =====================================================
// INICIALIZACIÓN
// =====================================================
void matrix_init()
{
    // Configura todos los pines MCP como OUTPUT por defecto
    for (uint8_t i = 0; i < 16; i++)
    {
        mcp23017.pinMode(i, OUTPUT);
        mcp23017.digitalWrite(i, LOW);
    }
}