#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include "globals.h"

// Desconecta absolutamente todo (estado seguro)
void matrix_disconnect_all();

// Restaura el estado previo del PCF8574
void matrix_restore();

// Selección de test points (ADC)
void matrix_select_tp1();
void matrix_select_tp2();
void matrix_select_tp3();

// Selección de shunts (corriente)
void matrix_shunt_low();
void matrix_shunt_mid();
void matrix_shunt_high();

// Selección de rangos OHM
void matrix_ohm_low();
void matrix_ohm_mid();
void matrix_ohm_high();

#endif