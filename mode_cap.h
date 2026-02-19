#ifndef MODE_CAP_H
#define MODE_CAP_H

#include <Arduino.h>
#include <stdbool.h>

// Inicialización
void mode_cap_init(void);

// Modo completo CAP
void measureCAPMode(void);

// descarga del capacitor
void dischargeCap();

#endif // MODE_CAP_H