#include "globals.h"
#include "autoOff.h"
#include <Arduino.h>
#include <math.h>
#include "mode_diode.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "filters.h"
#include "auto_hold.h"
#include "backlight.h"
#include "config.h"
#include "range_control.h"

// Pines usados para el test
static const uint8_t TP[2] = {pin.TP1, pin.TP2};

// =====================================================
// MEDICIÓN DE DIODO (Vf) — ADS1115
// =====================================================
float measureDiode()
{
    rng_release_for_gpio(); // liberar RNG para este modo

    // Inyectar corriente: TP1 = HIGH, TP2 = entrada
    pinMode(TP[0], OUTPUT);
    pinMode(TP[1], INPUT);
    digitalWrite(TP[0], HIGH);
    delay(5);

    // Seleccionar rango adecuado
    adc_manager_select(RANGE_DC_2V);
    adc_manager_set_sps(ADC_SPS_475);

    int16_t raw = 0;
    if (!adc_manager_read_raw(&raw))
    {
        // Restaurar pines
        pinMode(TP[0], INPUT);
        pinMode(TP[1], INPUT);
        return NAN; // error de lectura
    }

    float Vf = adc_manager_read_voltage();

    // Restaurar pines
    pinMode(TP[0], INPUT);
    pinMode(TP[1], INPUT);

    if (isnan(Vf) || Vf < 0.0f)
        return NAN;

    return Vf;
}

// =====================================================
// PANTALLA
// =====================================================
void showDiode()
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando...");
    delay(200);

    float Vf = measureDiode();

    if (!isnan(Vf))
    {
        backlight_activity();
    }

    // --- AUTO HOLD ---
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

    // --- LECTURA NORMAL ---
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "DIODE: ");

    if (Vf < 0.05f)
        lcd_driver_print(&lcd, "SHORT");
    else
        lcd_driver_printFloat(&lcd, Vf, 3), lcd_driver_print(&lcd, " V");
}