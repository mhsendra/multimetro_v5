#ifndef ONOFF_H
#define ONOFF_H

#include <Arduino.h>
#include "globals.h"

class OnOff
{
public:
    void begin();
    bool isOn();
};

#endif