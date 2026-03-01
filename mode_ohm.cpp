// ===============================
// mode_ohm.cpp
// ===============================

#include "mode_ohm.h"
#include "globals.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "lcd_driver.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "filters.h"
#include <math.h>
#include "measurement.h"

// ===============================
// Variables internas
// ===============================
static float filterOhm = -1e9f;

void resetOhmMode()
{
    filterOhm = -1e9f;
}

void ohm_select_range(ADC_RANGE_ID range)
{
    digitalWrite(pin.SSR_LOW, LOW);
    digitalWrite(pin.SSR_MID, LOW);
    digitalWrite(pin.SSR_HIGH, LOW);

    switch (range)
    {
    case RANGE_LOW:
        digitalWrite(pin.SSR_LOW, HIGH);
        break;

    case RANGE_MEDIUM:
        digitalWrite(pin.SSR_MID, HIGH);
        break;

    case RANGE_HIGH:
        digitalWrite(pin.SSR_HIGH, HIGH);
        break;
    }
}

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
    filterOhm = applyEMA(R, filterOhm, FILTER_ALPHA);

    return filterOhm;
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
// Símbolo de rango
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

// ===============================
// Funciones de pantalla
// ===============================
static void showOHM_R(void)
{
    float R = measureOHM_calibrated(ohmActiveRange);
    ADC_RANGE_ID newRange = ohm_autorange(R, ohmActiveRange);

    if (newRange != ohmActiveRange)
    {
        ohmActiveRange = newRange;
        ohm_select_range(ohmActiveRange);
        delay(10); // estabilización opcional
        return;    // en el siguiente ciclo se mide ya con el rango correcto
    }

    if (autoHold_update(R))
        R = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_printFloat(&lcd, R, 1);
    lcd_driver_print_P(&lcd, PSTR(" Ohm "));
    lcd_driver_print(&lcd, getOhmRangeSymbol(ohmActiveRange));
}

static void showOHM_Cont(void)
{
    float R = measureOHM_calibrated(ohmActiveRange);
    ADC_RANGE_ID newRange = ohm_autorange(R, ohmActiveRange);

    if (newRange != ohmActiveRange)
    {
        ohmActiveRange = newRange;
        ohm_select_range(ohmActiveRange);
        delay(10); // estabilización opcional
        return;    // en el siguiente ciclo se mide ya con el rango correcto
    }
    static bool beepState = false;
    if (R < OHM_CONT_THRESHOLD - 2.0f)
        beepState = true;
    if (R > OHM_CONT_THRESHOLD + 2.0f)
        beepState = false;

    lcd_ui_clear(&lcd);
    if (beepState)
    {
        lcd_driver_print_P(&lcd, PSTR("BEEP "));
        tone(pin.PIN_BUZZER, 4000);
    }
    else
    {
        lcd_driver_print_P(&lcd, PSTR("---- "));
        noTone(pin.PIN_BUZZER);
    }

    lcd_driver_print(&lcd, getOhmRangeSymbol(ohmActiveRange));
}

static void showOHM_Rel(void)
{
    float R = measureOHM_calibrated(ohmActiveRange);
    ADC_RANGE_ID newRange = ohm_autorange(R, ohmActiveRange);

    if (newRange != ohmActiveRange)
    {
        ohmActiveRange = newRange;
        ohm_select_range(ohmActiveRange);
        delay(10); // estabilización opcional
        return;    // en el siguiente ciclo se mide ya con el rango correcto
    }
    static float relRef = NAN;
    if (isnan(relRef))
        relRef = R;
    float diff = R - relRef;

    if (autoHold_update(diff))
        diff = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("REL "));
    lcd_driver_printFloat(&lcd, diff, 1);
    lcd_driver_print_P(&lcd, PSTR(" "));
    lcd_driver_print(&lcd, getOhmRangeSymbol(ohmActiveRange));
}

static void showOHM_Cable(void)
{
    float R = measureOHM_calibrated(ohmActiveRange);
    ADC_RANGE_ID newRange = ohm_autorange(R, ohmActiveRange);

    if (newRange != ohmActiveRange)
    {
        ohmActiveRange = newRange;
        ohm_select_range(ohmActiveRange);
        delay(10); // estabilización opcional
        return;    // en el siguiente ciclo se mide ya con el rango correcto
    }
    lcd_ui_clear(&lcd);
    if (R < 2.0f)
        lcd_driver_print_P(&lcd, PSTR("CABLE OK "));
    else
        lcd_driver_print_P(&lcd, PSTR("NO CABLE "));
    lcd_driver_print(&lcd, getOhmRangeSymbol(ohmActiveRange));
}

// ===============================
// Wrappers para punteros a función
// ===============================
void measureOHM_Main(void) { measureOHM_MODE(OHM_MAIN); }
void measureOHM_Cont_Wrap(void) { measureOHM_MODE(OHM_CONT); }
void measureOHM_Rel_Wrap(void) { measureOHM_MODE(OHM_REL); }
void measureOHM_Cable_Wrap(void) { measureOHM_MODE(OHM_CABLE); }

// ===============================
// Modo completo por submodo
// ===============================
void measureOHM_MODE(OhmSubMode submode)
{
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
    default:
        break;
    }
}
