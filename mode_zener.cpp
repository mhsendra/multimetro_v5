#include "mode_zener.h"
#include "matrix.h"
#include "measurement.h"
#include "lcd_ui.h"
#include "globals.h"
#include "auto_Hold.h"
#include "backlight.h"
#include <math.h>

static constexpr float ZENER_DIV_FACTOR = 22.0f / (100.0f + 22.0f);
static constexpr float ZENER_ADC_MAX = 4.95f;

static float zener_rms_accum = 0.0f;
static constexpr float zener_rms_alpha = 0.05f;

// ===============================
// Medición RAW usando single-ended
// ===============================
static float measureZener_raw()
{
    matrix_disconnect_all();
    matrix_zener();
    delay(10);

    float vz_adc = measureADC_Single(ADC_CH_ZENER);

    if (vz_adc > ZENER_ADC_MAX)
        vz_adc = ZENER_ADC_MAX;

    float vz = vz_adc / ZENER_DIV_FACTOR;

    // Si es menor que un diodo normal, no es Zener real
    return (vz < 0.5f) ? NAN : vz;
}

// ===============================
// RMS suavizado
// ===============================
static float measureZener_calibrated()
{
    float vz = measureZener_raw();

    if (!isnan(vz))
    {
        zener_rms_accum =
            sqrtf((1.0f - zener_rms_alpha) * zener_rms_accum * zener_rms_accum +
                  zener_rms_alpha * vz * vz);

        return zener_rms_accum;
    }

    return NAN;
}

// ===============================
// Pantalla
// ===============================
void showZenerMain()
{
    float vz = measureZener_calibrated();

    if (autoHold_update(vz))
        vz = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("ZENER: "));

    if (isnan(vz))
    {
        lcd_driver_print_P(&lcd, PSTR("OL"));
    }
    else
    {
        lcd_driver_printFloat(&lcd, vz, 2);
        lcd_driver_print_P(&lcd, PSTR(" V"));
    }

    backlight_activity();
}

// ===============================
// Modo principal
// ===============================
void measureZENER_MODE()
{
    backlight_activity();
    autoHold_reset();
    showZenerMain();
}