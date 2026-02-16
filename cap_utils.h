#ifndef CAP_UTILS_H
#define CAP_UTILS_H

#include <Arduino.h>

// Descarga del condensador
void dischargeCap();

// Protección por voltaje residual + preparación
bool prepareCapMeasurement();

#endif