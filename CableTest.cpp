#include <Arduino.h>
#include "CableTest.h"
#include "mode_ohm.h"
#include "lcd_ui.h"
#include "auto_hold.h"
#include "backlight.h"
#include "autoOff.h"
#include "range_control.h"
#include "globals.h"
#include "config.h"

// Umbral para considerar cable OK
static constexpr float CABLE_MAX_R = 5.0f;

/* =====================================================
 * MEDICIÓN DE CABLE (usa modo OHM)
 * ===================================================== */
float measureCable()
{
    float R = measureOhmValue();

    if (R < 0 || isnan(R))
        return NAN;

    return R;
}

/* =====================================================
 * PANTALLA
 * ===================================================== */
void showCable()
{
    // Reset de sistemas auxiliares
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "Detectando...");
    delay(200);

    float R = measureCable();

    // Registrar actividad
    if (!isnan(R))
    {
        backlight_activity();
        autoOff_activity();
    }

    // =================================================
    // AUTO HOLD
    // =================================================
    if (autoHold_update(R))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear(&lcd);
        lcd_printAt(&lcd, 0, 0, "CABLE (HOLD)");

        if (isnan(held))
        {
            lcd_printAt(&lcd, 0, 1, "ERROR");
            return;
        }

        if (held <= CABLE_MAX_R)
        {
            lcd_printAt(&lcd, 0, 1, "OK  (");
            lcd_printFloatAt(&lcd, 5, 1, held, 2);
            lcd_printAt(&lcd, 10, 1, " Ohm)");
        }
        else
        {
            lcd_printAt(&lcd, 0, 1, "BREAK (");
            lcd_printFloatAt(&lcd, 7, 1, held, 2);
            lcd_printAt(&lcd, 12, 1, " Ohm)");
        }
        return;
    }

    // =================================================
    // LECTURA NORMAL
    // =================================================
    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "CABLE");

    if (isnan(R))
    {
        lcd_printAt(&lcd, 0, 1, "ERROR");
        return;
    }

    if (R <= CABLE_MAX_R)
    {
        lcd_printAt(&lcd, 0, 1, "OK  (");
        lcd_printFloatAt(&lcd, 5, 1, R, 2);
        lcd_printAt(&lcd, 10, 1, " Ohm)");
    }
    else
    {
        lcd_printAt(&lcd, 0, 1, "BREAK (");
        lcd_printFloatAt(&lcd, 7, 1, R, 2);
        lcd_printAt(&lcd, 12, 1, " Ohm)");
    }
}

/* =====================================================
 * API PÚBLICA DEL MODO CABLE
 * ===================================================== */
void measureCableMode()
{
    showCable();
}