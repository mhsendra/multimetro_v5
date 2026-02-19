#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>

// =====================================================
// Inicialización de la matriz
// =====================================================
void matrix_init();
void matrix_disconnect_all();

// =====================================================
// Test Points
// =====================================================
void matrix_select_tp1();
void matrix_select_tp2();
void matrix_select_tp3();

// =====================================================
// Shunts (corriente)
// =====================================================
void matrix_shunt_low();  // 0.1Ω
void matrix_shunt_mid();  // 0.033Ω
void matrix_shunt_high(); // para rangos mayores (si aplica)

// =====================================================
// Rangos OHM
// =====================================================
void matrix_ohm_low();
void matrix_ohm_mid();
void matrix_ohm_high();

// =====================================================
// Zener
// =====================================================
void matrix_zener();

#endif