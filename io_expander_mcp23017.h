#ifndef IO_EXPANDER_MCP23017_H
#define IO_EXPANDER_MCP23017_H

#include "io_expander.h"
#include <Adafruit_MCP23X17.h>

class IOExpanderMCP23017 : public IOExpander
{
public:
    IOExpanderMCP23017(Adafruit_MCP23X17 *expander, uint8_t i2cAddr); // <--- ahora recibe dirección
    void begin() override;
    void pinMode(uint8_t pin, uint8_t mode) override;
    void digitalWrite(uint8_t pin, uint8_t value) override;
    int digitalRead(uint8_t pin) override;

private:
    Adafruit_MCP23X17 *_expander;
    uint8_t _i2cAddr; // <--- almacenar dirección I2C
    bool _initialized = false;
};

#endif