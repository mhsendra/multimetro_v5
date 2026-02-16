#ifndef MODE_ZENER_H
#define MODE_ZENER_H

#include <Arduino.h>

// Inicialización del modo Zener
void mode_zener_init();

// Encender y apagar el AQY
void zener_switch_on();
void zener_switch_off();

// Medición del voltaje Zener
float mode_zener_measure();

// Bucle principal del modo Zener
void mode_zener_run();

#endif