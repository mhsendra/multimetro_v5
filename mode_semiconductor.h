#ifndef MODE_SEMICONDUCTOR_H
#define MODE_SEMICONDUCTOR_H

#include "config.h"

void resetSemiconductorMode();

// Dispatcher principal del modo Semiconductor
void measureSemiconductorMode(SemiconductorSubMode submode);

#endif