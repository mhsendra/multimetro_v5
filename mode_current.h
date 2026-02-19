#ifndef MODE_CURRENT_H
#define MODE_CURRENT_H

#include <stdbool.h>
#include "config.h"

// ------------------------------------------------------------
// Funciones públicas
// ------------------------------------------------------------
float measureCurrent_Shunt_RAW(CurrentRange range);
float measureCURRENT_RAW(void);
float measureCURRENT_calibrated(void);

void showCURRENT(void);
void measureCURRENT(void);

#endif // MODE_CURRENT_H
