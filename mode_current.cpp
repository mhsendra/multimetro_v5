#include "mode_current.h"
#include "globals.h"
#include "lcd_ui.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "range_control.h"
#include "config.h"
#include <math.h>
#include "adcmanager.h"
#include "mode_state.h"

// ------------------------------------------------------------
// Medición raw según shunt
// ------------------------------------------------------------
float measureCurrent_Shunt_RAW(CurrentRange range)
{
    float v;

    if (range == CURR_RANGE_mA || range == CURR_RANGE_5A)
        v = adc_manager_read_voltage(ADC_CH_SHUNT1);
    else
        v = adc_manager_read_voltage(ADC_CH_SHUNT2);

    if (fabs(v) > 4.95f)
        return INFINITY;

    if (range == CURR_RANGE_mA || range == CURR_RANGE_5A)
        return ((v - cal.curr_shunt_offset) / SHUNT1_R) * cal.curr_shunt_gain;
    else
        return ((v - cal.curr_shunt_offset) / SHUNT2_R) * cal.curr_shunt_gain;
}

// ------------------------------------------------------------
// Medición RAW con selección automática de shunt
// ------------------------------------------------------------
float measureCURRENT_RAW(void)
{
    rng_release_for_gpio();

    float i = measureCurrent_Shunt_RAW(CURR_RANGE_5A);

    if (fabs(i) > SHUNT1_MAX_CURRENT)
    {
        currentRange = CURR_RANGE_16A;
        i = measureCurrent_Shunt_RAW(CURR_RANGE_16A);
    }
    else
    {
        currentRange = CURR_RANGE_5A;
    }

    return i;
}

// ------------------------------------------------------------
// Medición calibrada (wrapper)
float measureCURRENT_calibrated(void)
{
    return measureCURRENT_RAW();
}

// ------------------------------------------------------------
// Mostrar en LCD con filtros y backlight
// ------------------------------------------------------------
void showCURRENT(void)
{
    float i = measureCURRENT_calibrated();

    if (!isinf(i) && !isnan(i))
        backlight_activity();

    if (autoHold_update(i))
        i = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (isinf(i))
    {
        lcd_driver_print(&lcd, "I: OVL");
        return;
    }

    filter_current = applyEMA(i, filter_current, filter_alpha);

    lcd_driver_print(&lcd, "I: ");

    if (currentRange == CURR_RANGE_mA)
    {
        lcd_driver_printFloat(&lcd, filter_current * 1000.0f, 1);
        lcd_driver_print(&lcd, " mA");
    }
    else
    {
        lcd_driver_printFloat(&lcd, filter_current, 3);
        lcd_driver_print(&lcd, " A");
    }
}

// ------------------------------------------------------------
// Modo completo
// ------------------------------------------------------------
void measureCURRENT(void)
{
    adc_manager_set_sps(ADC_SPS_475);
    showCURRENT();
}