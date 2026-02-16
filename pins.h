#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

struct Pins
{

    // Selector rotativo
    const uint8_t SEL0 = 2; // PD2
    const uint8_t SEL1 = 3; // PD3
    const uint8_t SEL2 = 4; // PD4

    // Rango OHM → 74HC138
    const uint8_t RNG0 = 8;  // PB0
    const uint8_t RNG1 = 9;  // PB1
    const uint8_t RNG2 = 10; // PB2

    // Rango OHM
    const uint8_t SSR_LOW = 5;  // PD5
    const uint8_t SSR_MID = 6;  // PD6
    const uint8_t SSR_HIGH = 7; // PD7

    // Corriente / Shunts
    const uint8_t SSR_SHUNT = 0; // PD0

    // Buzzer
    const uint8_t PIN_BUZZER = 1; // PD1

    // Test points
    const uint8_t TP1 = A2; // PC2
    const uint8_t TP2 = A1; // PC1
    const uint8_t TP3 = A0; // PC0

    // Capacitor
    const uint8_t CAP_CHARGE_PIN = 12; // PB2

    // Frecuencia
    const uint8_t FREQ_IN = 13; // PB5

    // Calibración y encendido (PB6 y PB7)
    const uint8_t PIN_CAL = 20;   // PB6 / XTAL1
    const uint8_t PIN_ONOFF = 21; // PB7 / XTAL2
};
struct MCP_Pins
{
    uint8_t BOOST_HV_CTRL = 0;
    uint8_t NCV = 1;
};

#endif