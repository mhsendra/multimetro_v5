#ifndef MODE_OHM_H
#define MODE_OHM_H

#include <Arduino.h>
#include "config.h"

// Prototipos
void measureOHM_MODE();
bool detectVoltageOnOhm();
void setOhmRange(OhmRange range);
float calculateResistance(float v, OhmRange range);
float measureOhmValue();

void autoRangeOhms(float R);
void medirOhmiosContinuidad();
void showContinuity();
void showOhmRelative();
void showOhmMinMax();
void showCableTest();

#endif