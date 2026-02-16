#ifndef MODE_DISPATCHERS_H
#define MODE_DISPATCHERS_H

#include "config.h"

// Dispatcher principal
void dispatchMode(MainMode selectedMode);

// Sub-dispatchers
void dispatchPNMode(DiodeSubMode subMode);
void dispatchVACMode(VacSubMode subMode);
void dispatchVDCMode(VdcSubMode subMode);

#endif // MODE_DISPATCHERS_H