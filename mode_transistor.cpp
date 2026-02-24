#include "mode_transistor.h"
#include "globals.h"
#include "lcd_ui.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "measurement.h"
#include "range_control.h"
#include <Arduino.h>
#include <math.h>
#include <stdio.h>
#include "diode_utils.h"

// Pines de test para transistor
static const uint8_t TP[3] = {pin.TP1, pin.TP2, pin.TP3};

// =====================================
// Detecta el tipo de transistor y sus terminales
// =====================================
TransistorType detectTransistor(void)
{
    TransistorType result = TRANSISTOR_NONE;

    for (int b = 0; b < 3; b++)
    {
        int p1 = (b + 1) % 3;
        int p2 = (b + 2) % 3;

        bool d1 = diodeConducts(TP[b], TP[p1]);
        bool d2 = diodeConducts(TP[b], TP[p2]);
        bool d3 = diodeConducts(TP[p1], TP[b]);
        bool d4 = diodeConducts(TP[p2], TP[b]);

        // NPN
        if (d1 && d2 && !d3 && !d4)
        {
            result = TRANSISTOR_NPN;
            break;
        }

        // PNP
        if (!d1 && !d2 && d3 && d4)
        {
            result = TRANSISTOR_PNP;
            break;
        }
    }

    return result;
}

// =====================================
// Medición de ganancia aproximada (hFE)
// =====================================
uint16_t measureTransistorGain(void)
{
    return 100; // valor de ejemplo
}

// =====================================
// Mostrar transistor
// =====================================
void showTransistor(void)
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("Detectando..."));
    delay(200);

    TransistorType type = detectTransistor();

    lcd_ui_clear(&lcd);
    switch (type)
    {
    case TRANSISTOR_NPN:
        lcd_driver_print_P(&lcd, PSTR("NPN"));
        break;
    case TRANSISTOR_PNP:
        lcd_driver_print_P(&lcd, PSTR("PNP"));
        break;
    default:
        lcd_driver_print_P(&lcd, PSTR("NONE"));
        return;
    }

    uint16_t gain = measureTransistorGain();
    lcd_driver_print_P(&lcd, PSTR(" G:"));
    lcd_driver_printInt(&lcd, gain);
}

// =====================================
// Wrappers para menú / dispatcher
// =====================================
void measureTRANSISTOR_Main(void)
{
    showTransistor();
}

// Función genérica por compatibilidad con dispatcher
void measureTRANSISTOR(void)
{
    measureTRANSISTOR_Main();
}