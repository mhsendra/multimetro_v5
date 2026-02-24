#include "mode_diode.h"
#include "globals.h"
#include "lcd_ui.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "measurement.h"
#include "range_control.h"
#include <Arduino.h>
#include <math.h>

// Pines de prueba para diodo
static const uint8_t TP[2] = {pin.TP1, pin.TP2};

// =====================================
// Medición del diodo según submodo
// =====================================
float measureDiode(DiodeSubMode submode)
{
    rng_release_for_gpio();

    if (submode == DIODE_ZENER)
    {
        // Invertir polaridad: TP[1] -> ánodo, TP[0] -> cátodo
        pinMode(TP[0], INPUT);
        pinMode(TP[1], OUTPUT);
        digitalWrite(TP[1], HIGH);
        delay(5);
        float Vz = measureADC_Single(ADC_CH_ZENER);
        pinMode(TP[0], INPUT);
        pinMode(TP[1], INPUT);
        return Vz;
    }

    // Diodo normal: TP[0] -> ánodo, TP[1] -> cátodo
    pinMode(TP[0], OUTPUT);
    pinMode(TP[1], INPUT);
    digitalWrite(TP[0], HIGH);
    delay(5);
    float Vf = measureADC_Single(ADC_CH_ZENER);
    pinMode(TP[0], INPUT);
    pinMode(TP[1], INPUT);

    if (isnan(Vf) || Vf < 0.0f)
        return NAN;

    return Vf;
}

// =====================================
// Pantalla del diodo
// =====================================
void showDiode(DiodeSubMode submode)
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("Detectando..."));
    delay(200);

    float Vf = measureDiode(submode);
    if (!isnan(Vf))
        backlight_activity();

    if (autoHold_update(Vf))
        Vf = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("DIODE: "));

    if (isnan(Vf))
        lcd_driver_print_P(&lcd, PSTR("OL"));
    else if (Vf < 0.05f)
        lcd_driver_print_P(&lcd, PSTR("SHORT"));
    else
    {
        lcd_driver_printFloat(&lcd, Vf, 3);
        lcd_driver_print_P(&lcd, PSTR(" V"));
    }
}

// =====================================
// Wrappers para menú / dispatcher
// =====================================
void measureDiode_Main(void) { showDiode(DIODE_MAIN); }
void measureDiode_Rev(void) { showDiode(DIODE_REV); }
void measureDiode_Zener(void) { showDiode(DIODE_ZENER); }

// Función genérica por compatibilidad con dispatcher
void measureDiode_MODE(DiodeSubMode submode)
{
    switch (submode)
    {
    case DIODE_MAIN:
        measureDiode_Main();
        break;
    case DIODE_REV:
        measureDiode_Rev();
        break;
    case DIODE_ZENER:
        measureDiode_Zener();
        break;
    default:
        break;
    }
}