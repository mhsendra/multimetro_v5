#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include <Arduino.h>

class IOExpander
{
public:
    virtual ~IOExpander() {} // 🔥 Destructor virtual (corrección crítica)

    virtual void begin() {}
    virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
    virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
    virtual int digitalRead(uint8_t pin) = 0;
};

#endif