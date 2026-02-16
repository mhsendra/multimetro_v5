#include "mode_transistor.h"
#include "diode_utils.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "globals.h"
#include "config.h"
#include "range_control.h"
#include "backlight.h"
#include "auto_hold.h"
#include "autoOff.h"
#include <Arduino.h>

// Pines usados para el test
static const uint8_t TP[3] = {pin.TP1, pin.TP2, pin.TP3};

/* =====================================================
 * DETECTAR BASE
 * ===================================================== */
int findBase(uint8_t *basePin, bool &isNPN)
{
    rng_release_for_gpio(); // liberar RNG para este modo

    for (int b = 0; b < 3; b++)
    {
        int c = (b + 1) % 3;
        int e = (b + 2) % 3;

        bool d1 = diodeConducts(TP[b], TP[c]); // B→C
        bool d2 = diodeConducts(TP[b], TP[e]); // B→E

        bool d3 = diodeConducts(TP[c], TP[b]); // C→B
        bool d4 = diodeConducts(TP[e], TP[b]); // E→B

        if (d1 && d2 && !d3 && !d4)
        {
            *basePin = TP[b];
            isNPN = true;
            return 1;
        }

        if (!d1 && !d2 && d3 && d4)
        {
            *basePin = TP[b];
            isNPN = false;
            return 1;
        }
    }

    return 0;
}

/* =====================================================
 * MEDIR hFE
 * ===================================================== */
float measureHFE(uint8_t basePin, bool isNPN)
{
    uint8_t c = 0, e = 0;

    for (int i = 0; i < 3; i++)
    {
        if (TP[i] != basePin)
        {
            if (c == 0)
                c = TP[i];
            else
                e = TP[i];
        }
    }

    pinMode(basePin, OUTPUT);
    digitalWrite(basePin, isNPN ? HIGH : LOW);
    delayMicroseconds(100);

    // --- Lectura ADC usando RANGE_TRANSISTOR ---
    adc_manager_select(RANGE_TRANSISTOR);
    adc_manager_set_sps(ADC_SPS_860);

    float vc = adc_manager_read_voltage();

    // Protección por saturación del ADC
    if (vc > 4.95f)
        return NAN;

    pinMode(basePin, INPUT);

    float hfe = (isNPN ? (OHM_VREF - vc) : vc) * 100.0f;
    return hfe;
}

/* =====================================================
 * PANTALLA
 * ===================================================== */
void showTransistor()
{
    // Reset de sistemas auxiliares
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    uint8_t basePin = 0;
    bool isNPN = true;

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando...");
    delay(300);

    if (!findBase(&basePin, isNPN))
    {
        backlight_activity();
        autoOff_activity();

        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, "No detectado");
        return;
    }

    float hfe = measureHFE(basePin, isNPN);

    // Registrar actividad
    backlight_activity();
    autoOff_activity();

    // --- AUTO HOLD ---
    if (autoHold_update(hfe))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, isNPN ? "NPN (HOLD)" : "PNP (HOLD)");
        lcd_ui_setCursor(&lcd, 0, 1);
        lcd_driver_print(&lcd, "hFE:");
        lcd_driver_printFloat(&lcd, held, 0);
        return;
    }

    // --- Lectura normal ---
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, isNPN ? "NPN" : "PNP");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "hFE:");
    lcd_driver_printFloat(&lcd, hfe, 0);
}