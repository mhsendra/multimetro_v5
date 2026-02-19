#include "mode_ohm.h"
#include "config.h"
#include "globals.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "lcd_driver.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "range_control.h"
#include "filters.h"
#include <math.h>
#include "measurement.h"
#include "config.h"

// ===============================
// Variables internas
// ===============================
static float filter_ohm = NAN;
static float ohm_reference = NAN;

// ===============================
// Lectura RAW + calibración
// ===============================
static float measureOHM_raw(ADC_RANGE_ID range)
{
    measurement_result_t meas = measure_channels();
    float v_adc = meas.voltage;

    if (fabs(v_adc) > 4.95f)
        return INFINITY;

    switch (range)
    {
    case RANGE_LOW:
        return v_adc / I_TEST_100;
    case RANGE_MEDIUM:
        return v_adc / I_TEST_10K;
    case RANGE_HIGH:
        return v_adc / I_TEST_1M;
    default:
        return NAN;
    }
}

static float measureOHM_calibrated(ADC_RANGE_ID range)
{
    float R = measureOHM_raw(range);
    if (isnan(R) || isinf(R))
        return R;

    R *= cal.ohm; // factor de calibración
    filter_ohm = applyEMA(R, filter_ohm, filter_alpha);

    return filter_ohm;
}

// ===============================
// Auto-rango
// ===============================
static ADC_RANGE_ID ohm_autorange(float R, ADC_RANGE_ID current)
{
    switch (current)
    {
    case RANGE_LOW:
        if (R > OHM_100_MAX)
            return RANGE_MEDIUM;
        break;
    case RANGE_MEDIUM:
        if (R > OHM_10K_MAX)
            return RANGE_HIGH;
        if (R < OHM_10K_MIN)
            return RANGE_LOW;
        break;
    case RANGE_HIGH:
        if (R < OHM_1M_MIN)
            return RANGE_MEDIUM;
        break;
    default:
        break;
    }
    return current;
}

// ===============================
// Funciones de pantalla
// ===============================
static const char *getOhmRangeSymbol(ADC_RANGE_ID range)
{
    switch (range)
    {
    case RANGE_LOW:
        return "L";
    case RANGE_MEDIUM:
        return "M";
    case RANGE_HIGH:
        return "H";
    default:
        return "?";
    }
}

void showOHM_R(void)
{
    float R = measureOHM_calibrated(currentOhmRange);
    currentOhmRange = ohm_autorange(R, currentOhmRange);

    if (autoHold_update(R))
        R = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_printFloat(&lcd, R, 1);
    lcd_driver_print(&lcd, " Ohm ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(currentOhmRange));
}

void showOHM_Cont(void)
{
    float R = measureOHM_calibrated(currentOhmRange);
    currentOhmRange = ohm_autorange(R, currentOhmRange);

    static bool beepState = false;
    if (R < OHM_CONT_THRESHOLD - 2.0f)
        beepState = true;
    if (R > OHM_CONT_THRESHOLD + 2.0f)
        beepState = false;

    lcd_ui_clear(&lcd);
    if (beepState)
    {
        lcd_driver_print(&lcd, "BEEP ");
        tone(pin.PIN_BUZZER, 4000);
    }
    else
    {
        lcd_driver_print(&lcd, "---- ");
        noTone(pin.PIN_BUZZER);
    }

    lcd_driver_print(&lcd, getOhmRangeSymbol(currentOhmRange));
}

void showOHM_Rel(void)
{
    float R = measureOHM_calibrated(currentOhmRange);
    currentOhmRange = ohm_autorange(R, currentOhmRange);

    static float relRef = NAN;
    if (isnan(relRef))
        relRef = R;
    float diff = R - relRef;

    if (autoHold_update(diff))
        diff = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "REL ");
    lcd_driver_printFloat(&lcd, diff, 1);
    lcd_driver_print(&lcd, " ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(currentOhmRange));
}

void showOHM_Cable(void)
{
    float R = measureOHM_calibrated(currentOhmRange);
    currentOhmRange = ohm_autorange(R, currentOhmRange);

    lcd_ui_clear(&lcd);
    if (R < 2.0f)
        lcd_driver_print(&lcd, "CABLE OK ");
    else
        lcd_driver_print(&lcd, "NO CABLE ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(currentOhmRange));
}

// ===============================
// Modo completo por submodo
// ===============================
void measureOHM_MODE(OhmSubMode submode)
{
    rng_restore_for_ohm();
    adc_manager_set_sps(ADC_SPS_475);

    switch (submode)
    {
    case OHM_MAIN:
        showOHM_R();
        break;
    case OHM_CONT:
        showOHM_Cont();
        break;
    case OHM_REL:
        showOHM_Rel();
        break;
    case OHM_CABLE:
        showOHM_Cable();
        break;
    }
}

// ===============================
// Wrappers para menú
// ===============================
void measureOHM_Main(void) { showOHM_R(); }
void measureOHM_Cont_Wrap(void) { showOHM_Cont(); }
void measureOHM_Rel_Wrap(void) { showOHM_Rel(); }
void measureOHM_Cable_Wrap(void) { showOHM_Cable(); }