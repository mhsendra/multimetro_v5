#include "onoff.h"

void OnOff::begin() {
    pinMode(pin.PIN_ONOFF, INPUT_PULLUP);
}

bool OnOff::isOn() {
    return digitalRead(pin.PIN_ONOFF) == LOW;
}

