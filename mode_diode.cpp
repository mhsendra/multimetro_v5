#include "mode_diode.h"
#include "globals.h"
#include "lcd_ui.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "measurement.h"
#include "range_control.h"
#include <Arduino.h>
#include <math.h>

// Pines de test
static const uint8_t TP[2] = {pin.TP1, pin.TP2};

// Medición principal
float measureDiode(void)
{
    rng_release_for_gpio();

    pinMode(TP[0], OUTPUT);
    pinMode(TP[1], INPUT);
    digitalWrite(TP[0], HIGH);
    delay(5);

    measurement_result_t meas = measure_channels();
    float Vf = meas.voltage;

    pinMode(TP[0], INPUT);
    pinMode(TP[1], INPUT);

    if (isnan(Vf) || Vf < 0.0f)
        return NAN;
    return Vf;
}

// Pantalla diodo
void showDiode(void)
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando...");
    delay(200);

    float Vf = measureDiode();
    if (!isnan(Vf))
        backlight_activity();

    if (autoHold_update(Vf))
    {
        float held = autoHold_getHeldValue();
        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, "DIODE (HOLD)");
        lcd_ui_setCursor(&lcd, 0, 1);

        if (isnan(held))
        {
            lcd_driver_print(&lcd, "OL");
            return;
        }
        if (held < 0.05f)
        {
            lcd_driver_print(&lcd, "SHORT");
            return;
        }

        lcd_driver_printFloat(&lcd, held, 3);
        lcd_driver_print(&lcd, " V");
        return;
    }

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "DIODE: ");
    if (Vf < 0.05f)
        lcd_driver_print(&lcd, "SHORT");
    else
    {
        lcd_driver_printFloat(&lcd, Vf, 3);
        lcd_driver_print(&lcd, " V");
    }
}

// Wrappers para dispatcher
void measureDiode_Main(void) { showDiode(); }