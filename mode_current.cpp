#include "mode_current.h"
#include "autoOff.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "globals.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "range_control.h"
#include <math.h>

// =====================================================
// CORRIENTE mA y 5A (shunt + LM358 → ADS1115 canal shunt)
// =====================================================
static float measureCurrent_Shunt_RAW(void)
{
    if (currentRange == CURR_RANGE_mA)
        adc_manager_select(RANGE_CURR_20mA);
    else // CURR_RANGE_5A
        adc_manager_select(RANGE_CURR_200mA);

    float v = adc_manager_read_voltage();
    if (fabs(v) > 4.95f)
        return INFINITY;

    v -= cal.curr_shunt_offset;
    return v * cal.curr_shunt_gain;
}

// =====================================================
// CORRIENTE 16A (ACS712 → ADS1115 canal ACS)
// =====================================================
static float measureCurrent_ACS_RAW(void)
{
    adc_manager_select(RANGE_CURR_16A);

    float v = adc_manager_read_voltage();
    if (fabs(v) > 4.95f)
        return INFINITY;

    v -= cal.acs_offset;
    return v / cal.acs_sens;
}

// =====================================================
// DISPATCHER RAW
// =====================================================
float measureCURRENT_RAW(void)
{
    rng_release_for_gpio();

    switch (currentRange)
    {
    case CURR_RANGE_mA:
    case CURR_RANGE_5A:
        return measureCurrent_Shunt_RAW();
    case CURR_RANGE_16A:
        return measureCurrent_ACS_RAW();
    default:
        return NAN;
    }
}

// =====================================================
// CALIBRADO
// =====================================================
float measureCURRENT_calibrated(void)
{
    return measureCURRENT_RAW();
}

// =====================================================
// PANTALLA (con filtros unificados, autoHold y backlight)
// =====================================================
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

    // ------------------------------
    // Filtros: EMA + Butterworth opcional
    // ------------------------------
    filter_current = applyEMA(i, filter_current, filter_alpha);
    // float i_filtered = applyButterworth(bw_current, filter_current); // opcional

    if (currentRange == CURR_RANGE_mA)
    {
        lcd_driver_print(&lcd, "I: ");
        lcd_driver_printFloat(&lcd, filter_current * 1000.0f, 1);
        lcd_driver_print(&lcd, " mA");
    }
    else
    {
        lcd_driver_print(&lcd, "I: ");
        lcd_driver_printFloat(&lcd, filter_current, 3);
        lcd_driver_print(&lcd, " A");
    }
}

// =====================================================
// MODO COMPLETO
// =====================================================
void measureCURRENT(void)
{
    adc_manager_set_sps(ADC_SPS_475);
    showCURRENT();
}