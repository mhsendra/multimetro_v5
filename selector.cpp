#include "selector.h"

int readSelector(IOExpander *ioExpander)
{
    uint8_t b0, b1, b2;

    // Si se pasa el expansor, leer a través de él
    if (ioExpander)
    {
        b0 = ioExpander->digitalRead(pin.SEL0);
        b1 = ioExpander->digitalRead(pin.SEL1);
        b2 = ioExpander->digitalRead(pin.SEL2);
    }
    else
    {
        // Sino, leer directamente de los pines físicos
        b0 = digitalRead(pin.SEL0);
        b1 = digitalRead(pin.SEL1);
        b2 = digitalRead(pin.SEL2);
    }

    return (b2 << 2) | (b1 << 1) | b0;
}