#include <Arduino.h>
#include "autoOff.h"
#include "mode_mosfet.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "auto_hold.h"
#include "backlight.h"
#include "globals.h"
#include "range_control.h"
#include "config.h"

/* =====================================================
 * MEDIR MOSFET (Vth)
 * ===================================================== */
float measureMosfet()
{
    rng_release_for_gpio(); // liberar RNG para este modo

    // Drenador a GND
    pinMode(pin.TP3, OUTPUT);
    digitalWrite(pin.TP3, LOW);

    // Drenador/medición
    pinMode(pin.TP2, INPUT);

    float vth = NAN;

    // Barrido de puerta 0 → 5 V
    for (int step = 0; step <= 50; step++)
    {
        float vg = (step / 50.0f) * 5.0f;

        // Aplicar tensión de puerta
        pinMode(pin.TP1, OUTPUT);
        analogWrite(pin.TP1, (int)(vg / 5.0f * 255));
        delay(2);

        // Medir tensión en drenador usando ADC V4
        adc_manager_select(RANGE_DC_20V);
        adc_manager_set_sps(ADC_SPS_475);

        float vd = adc_manager_read_voltage();

        // Protección por saturación
        if (vd > 4.95f)
            vd = 4.95f;

        // Cuando el MOSFET empieza a conducir, VD cae
        if (vd < 4.0f)
        {
            vth = vg;
            break;
        }
    }

    // Reset pines
    pinMode(pin.TP1, INPUT);
    pinMode(pin.TP2, INPUT);
    pinMode(pin.TP3, INPUT);

    return vth;
}

/* =====================================================
 * PANTALLA
 * ===================================================== */
void showMosfet()
{
    // Reset de sistemas auxiliares
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando...");
    delay(200);

    float vth = measureMosfet();

    // Registrar actividad
    if (!isnan(vth))
    {
        backlight_activity();
    }

    // --- AUTO HOLD ---
    if (autoHold_update(vth))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, "MOSFET (HOLD)");
        lcd_ui_setCursor(&lcd, 0, 1);

        if (isnan(held))
        {
            lcd_driver_print(&lcd, "OL");
            return;
        }

        lcd_driver_print(&lcd, "Vth:");
        lcd_driver_printFloat(&lcd, held, 2);
        lcd_driver_print(&lcd, "V");
        return;
    }

    // --- Lectura normal ---
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "MOSFET");
    lcd_ui_setCursor(&lcd, 0, 1);

    if (isnan(vth))
    {
        lcd_driver_print(&lcd, "OL");
        return;
    }

    lcd_driver_print(&lcd, "Vth:");
    lcd_driver_printFloat(&lcd, vth, 2);
    lcd_driver_print(&lcd, "V");
}

/* =====================================================
 * API PÚBLICA DEL MODO MOSFET
 * ===================================================== */
void measureMosfetMode()
{
    showMosfet();
}