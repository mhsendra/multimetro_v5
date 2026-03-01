#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

struct Pins
{

    // Rango OHM
    // PB0
    // PB1
    // PB2// Selector rotativo

    // Capacitor
    const uint8_t CAP_CHARGE_PIN = 12; // PB4

    // Frecuencia
    const uint8_t FREQ_IN = 13; // PB5

    // Calibración y encendido (PB6 y PB7)
    const uint8_t PIN_CAL = 20;   // PB6 / XTAL1
    const uint8_t PIN_ONOFF = 21; // PB7 / XTAL2

    // Test points
    const uint8_t TP3 = A0; // PC0
    const uint8_t TP2 = A1; // PC1
    const uint8_t TP1 = A2; // PC2
    // PC3
    // PC4 SCL
    // PC5 SDA
    // PC6 RESET

    // Corriente / Shunts
    const uint8_t SSR_SHUNT = 0; // PD0

    // Buzzer
    const uint8_t PIN_BUZZER = 1; // PD1

    // PD2
    // PD3
    const uint8_t POWER_HOLD_CTRL = 4; // PD4

    // Rango OHM
    const uint8_t SSR_LOW = 5;  // PD5
    const uint8_t SSR_MID = 6;  // PD6
    const uint8_t SSR_HIGH = 7; // PD7
};
struct MCP_Pins
{
    uint8_t BOOST_HV_CTRL = 0;
    uint8_t NCV = 1;
    uint8_t SSR_ZENER_CTRL = 2;
    uint8_t SEL0 = 3;  // Posición 0
    uint8_t SEL1 = 4;  // Posición 1
    uint8_t SEL2 = 5;  // Posición 2
    uint8_t SEL3 = 6;  // Posición 3
    uint8_t SEL4 = 7;  // Posición 4
    uint8_t SEL5 = 8;  // Posición 5
    uint8_t SEL6 = 9;  // Posición 6
    uint8_t SEL7 = 10; // Posición 7
};

#endif