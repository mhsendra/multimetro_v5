#include "mode_zener.h"
#include "matrix.h"
#include "measurement.h"
#include "lcd_ui.h"
#include "globals.h"
#include "auto_Hold.h"
#include "backlight.h"

static constexpr float ZENER_DIV_FACTOR = 22.0f / (100.0f + 22.0f);
static constexpr float ZENER_ADC_MAX = 4.95f;

static float zener_rms_accum = 0.0f;
static constexpr float zener_rms_alpha = 0.05f;

static float measureZener_raw()
{
    matrix_disconnect_all();
    matrix_zener();
    delay(10);

    measurement_result_t meas = measure_channels();
    float vz_adc = meas.voltage;
    if (vz_adc > ZENER_ADC_MAX)
        vz_adc = ZENER_ADC_MAX;

    float vz = vz_adc / ZENER_DIV_FACTOR;
    return (vz < 0.5f) ? NAN : vz;
}

static float measureZener_calibrated()
{
    float vz = measureZener_raw();
    if (!isnan(vz))
    {
        zener_rms_accum = sqrtf((1.0f - zener_rms_alpha) * zener_rms_accum * zener_rms_accum +
                                zener_rms_alpha * vz * vz);
        return zener_rms_accum;
    }
    return NAN;
}

void showZenerMain()
{
    float vz = measureZener_calibrated();
    if (autoHold_update(vz))
        vz = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "ZENER: ");
    if (isnan(vz))
        lcd_driver_print(&lcd, "OL");
    else
    {
        lcd_driver_printFloat(&lcd, vz, 2);
        lcd_driver_print(&lcd, " V");
    }

    backlight_activity();
}

void measureZENER_MODE()
{
    backlight_activity();
    autoHold_reset();
    showZenerMain();
}