#include "io_expander_mcp23017.h"
#include <Arduino.h>

// Constructor
IOExpanderMCP23017::IOExpanderMCP23017(Adafruit_MCP23X17 *expander, uint8_t i2cAddr)
{
    _expander = expander;
    _i2cAddr = i2cAddr; // guardar dirección
}

// Inicialización
void IOExpanderMCP23017::begin()
{
    _initialized = _expander->begin_I2C(_i2cAddr); // usar la dirección pasada
    if (!_initialized)
    {
        Serial.println("Error: MCP23017 no inicializado!");
        return;
    }

    for (uint8_t i = 0; i < 16; i++)
    {
        _expander->pinMode(i, INPUT);
        _expander->digitalWrite(i, HIGH); // pull-up
    }
}

// pinMode
void IOExpanderMCP23017::pinMode(uint8_t pin, uint8_t mode)
{
    if (!_initialized)
        return;
    _expander->pinMode(pin, mode == OUTPUT ? OUTPUT : INPUT);
    if (mode == INPUT_PULLUP)
        _expander->digitalWrite(pin, HIGH);
}

// digitalWrite
void IOExpanderMCP23017::digitalWrite(uint8_t pin, uint8_t value)
{
    if (!_initialized)
        return;
    _expander->digitalWrite(pin, value);
}

// digitalRead
int IOExpanderMCP23017::digitalRead(uint8_t pin)
{
    if (!_initialized)
        return 0;
    return _expander->digitalRead(pin);
}