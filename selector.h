#ifndef SELECTOR_H
#define SELECTOR_H

#include <Arduino.h>
#include "globals.h"
#include "pins.h"
#include "io_expander.h"

// Lee el selector físico (3 bits) usando el expansor I2C o directamente los pines
int readSelector(IOExpander *ioExpander = nullptr);

#endif