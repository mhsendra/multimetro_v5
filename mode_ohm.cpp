#include "mode_ohm.h"
#include "autoOff.h"
#include "adcmanager.h"
#include "globals.h"
#include "lcd_ui.h"
#include "config.h"
#include "OhmMinMax.h"
#include "auto_Hold.h"
#include "backlight.h"
#include <math.h>
#include "range_control.h"

// =====================================================
// CONSTANTES DE CORRIENTE DE TEST (AJUSTABLES)
// =====================================================
#define I_TEST_100 0.001f    // ~1 mA   (rango 100 Ω)
#define I_TEST_10K 0.0001f   // ~0.1 mA (rango 10 kΩ)
#define I_TEST_1M 0.0000001f // ~0.1 µA (rango 1 MΩ)

// =====================================================
// MAPEO DE RANGO ADC → SÍMBOLO L / M / H
// =====================================================
static const char *getOhmRangeSymbol(adc_range_id_t range)
{
    switch (range)
    {
    case RANGE_OHM_100:
        return "L";
    case RANGE_OHM_10K:
        return "M";
    case RANGE_OHM_1M:
        return "H";
    default:
        return "?";
    }
}

// =====================================================
// AUTO‑RANGO SOBRE RANGOS ADC (RANGE_OHM_*)
// =====================================================
static adc_range_id_t ohm_autorange(float R, adc_range_id_t current)
{
    switch (current)
    {
    case RANGE_OHM_100:
        if (R > OHM_100_MAX)
            return RANGE_OHM_10K;
        break;
    case RANGE_OHM_10K:
        if (R > OHM_10K_MAX)
            return RANGE_OHM_1M;
        if (R < OHM_10K_MIN)
            return RANGE_OHM_100;
        break;
    case RANGE_OHM_1M:
        if (R < OHM_1M_MIN)
            return RANGE_OHM_10K;
        break;
    default:
        break;
    }
    return current;
}

// =====================================================
// CONTROL DE RELÉS OHM (74HC138 → CD4053B)
// =====================================================
static void ohm_set_relays(adc_range_id_t range)
{
    switch (range)
    {
    case RANGE_OHM_100:
        digitalWrite(pin.RNG2, LOW);
        digitalWrite(pin.RNG1, LOW);
        digitalWrite(pin.RNG0, LOW);
        break;
    case RANGE_OHM_10K:
        digitalWrite(pin.RNG2, LOW);
        digitalWrite(pin.RNG1, LOW);
        digitalWrite(pin.RNG0, HIGH);
        break;
    case RANGE_OHM_1M:
        digitalWrite(pin.RNG2, LOW);
        digitalWrite(pin.RNG1, HIGH);
        digitalWrite(pin.RNG0, LOW);
        break;
    default:
        break;
    }
}

// =====================================================
// PROTECCIÓN OHM — DETECCIÓN DE TENSIÓN EXTERNA
// =====================================================
static float ohm_check_voltage(void)
{
    adc_manager_select(RANGE_DC_20V);
    float v_adc = adc_manager_read_voltage();
    return v_adc * 0.110f;
}

static void showOHM_Protect(void)
{
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "OHM PROTECT");
}

// =====================================================
// OHM — MEDICIÓN RAW
// =====================================================
static float measureOHM_raw(void)
{
    float v_adc = adc_manager_read_voltage();
    if (fabs(v_adc) > 4.95f)
        return INFINITY;

    switch (adc_manager_current_range())
    {
    case RANGE_OHM_100:
        return v_adc / I_TEST_100;
    case RANGE_OHM_10K:
        return v_adc / I_TEST_10K;
    case RANGE_OHM_1M:
        return v_adc / I_TEST_1M;
    default:
        return NAN;
    }
}

// =====================================================
// OHM — CALIBRADO Y FILTRADO EMA
// =====================================================
static float measureOHM_calibrated(void)
{
    float R = measureOHM_raw();
    if (isinf(R))
        return R;

    // Aplicar calibración
    R *= cal.ohm;

    // ------------------------------
    // Aplicar EMA global para suavizar
    // ------------------------------
    filter_ohm = applyEMA(R, filter_ohm, filter_alpha);

    return filter_ohm;
}

// =====================================================
// SUBMODOS
// =====================================================
static void showContinuity(float R, adc_range_id_t range)
{
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
    lcd_driver_print(&lcd, getOhmRangeSymbol(range));
}

static void showOhmRelative(float R, adc_range_id_t range)
{
    if (isnan(ohmRef))
        ohmRef = R;
    float diff = R - ohmRef;
    if (autoHold_update(diff))
        diff = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "REL ");
    lcd_driver_printFloat(&lcd, diff, 1);
    lcd_driver_print(&lcd, " ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(range));
}

static void showOhmMinMax(adc_range_id_t range)
{
    lcd_ui_clear(&lcd);
    ohmMinMax_show();
    lcd_driver_print(&lcd, " ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(range));
}

static void showCableTest(float R, adc_range_id_t range)
{
    lcd_ui_clear(&lcd);
    if (R < 2.0f)
        lcd_driver_print(&lcd, "CABLE OK ");
    else
        lcd_driver_print(&lcd, "NO CABLE ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(range));
}

static void showOhmMain(float R, adc_range_id_t range)
{
    if (autoHold_update(R))
        R = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_printFloat(&lcd, R, 1);
    lcd_driver_print(&lcd, " Ohm ");
    lcd_driver_print(&lcd, getOhmRangeSymbol(range));
}

// =====================================================
// MODO COMPLETO OHM CON FILTROS (EMA)
// =====================================================
void measureOHM_MODE(void)
{
    rng_restore_for_ohm();
    adc_manager_set_sps(ADC_SPS_475);

    float v_ext = ohm_check_voltage();
    if (v_ext > OHM_PROTECT_THRESHOLD)
    {
        showOHM_Protect();
        return;
    }

    static adc_range_id_t range = RANGE_OHM_10K;
    ohm_set_relays(range);
    adc_manager_select(range);

    float R = measureOHM_calibrated(); // Valor OHM calibrado

    // =====================================================
    // Aplicar EMA global a OHM y continuidad según submodo
    // =====================================================
    if (!isinf(R) && !isnan(R))
    {
        if (ohmSubMode == OHM_CONT)
        {
            // Continuidad: EMA específica
            filter_continuity = applyEMA(R, filter_continuity, filter_alpha);
            R = filter_continuity;
        }
        else
        {
            // OHM normal: EMA global
            filter_ohm = applyEMA(R, filter_ohm, filter_alpha);
            R = filter_ohm;
        }

        // Auto-rango
        adc_range_id_t newRange = ohm_autorange(R, range);
        if (newRange != range)
        {
            range = newRange;
            backlight_activity();
            return; // medir con nuevo rango en siguiente ciclo
        }

        backlight_activity();
    }

    // =====================================================
    // Mostrar según submodo
    // =====================================================
    switch (ohmSubMode)
    {
    case OHM_MAIN:
        showOhmMain(R, range);
        break;
    case OHM_CONT:
        showContinuity(R, range);
        break;
    case OHM_REL:
        showOhmRelative(R, range);
        break;
    case OHM_MINMAX:
        if (!isinf(R) && !isnan(R))
            ohmMinMax_update(R);
        showOhmMinMax(range);
        break;
    case OHM_CABLE:
        showCableTest(R, range);
        break;
    default:
        break;
    }
}