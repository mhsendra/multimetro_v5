#include "mode_mosfet.h"
#include "lcd_ui.h"
#include "globals.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "autoOff.h"
#include "measurement.h"
#include <Arduino.h>

float measureMosfet()
{
    pinMode(pin.TP3, OUTPUT);
    digitalWrite(pin.TP3, LOW);
    pinMode(pin.TP2, INPUT);

    float vth = NAN;

    for (int step = 0; step <= 50; step++)
    {
        float vg = step / 50.0f * 5.0f;
        pinMode(pin.TP1, OUTPUT);
        analogWrite(pin.TP1, (int)(vg / 5.0f * 255));
        delay(2);

        measurement_result_t meas = measure_channels();
        float vd = meas.voltage;
        if (vd > 4.95f)
            vd = 4.95f;
        if (vd < 4.0f)
        {
            vth = vg;
            break;
        }
    }

    pinMode(pin.TP1, INPUT);
    pinMode(pin.TP2, INPUT);
    pinMode(pin.TP3, INPUT);
    return vth;
}

void showMosfet(void)
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("Detectando..."));
    delay(200);

    float vth = measureMosfet();
    if (!isnan(vth))
        backlight_activity();

    if (autoHold_update(vth))
    {
        float held = autoHold_getHeldValue();
        lcd_ui_clear(&lcd);
        lcd_driver_print_P(&lcd, PSTR("MOSFET (HOLD)"));
        lcd_ui_setCursor(&lcd, 0, 1);

        if (isnan(held))
        {
            lcd_driver_print_P(&lcd, PSTR("OL"));
            return;
        }

        lcd_driver_print_P(&lcd, PSTR("Vth:"));
        lcd_driver_printFloat(&lcd, held, 2);
        lcd_driver_print_P(&lcd, PSTR("V"));
        return;
    }

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("MOSFET"));
    lcd_ui_setCursor(&lcd, 0, 1);
    if (isnan(vth))
        lcd_driver_print_P(&lcd, PSTR("OL"));
    else
    {
        lcd_driver_print_P(&lcd, PSTR("Vth:"));
        lcd_driver_printFloat(&lcd, vth, 2);
        lcd_driver_print_P(&lcd, PSTR("V"));
    }
}

void measureMosfetMode() { showMosfet(); }