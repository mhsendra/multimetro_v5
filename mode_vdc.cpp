#include "mode_vdc.h"
#include "mode_current.h"
#include "measurement.h"
#include "lcd_ui.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "globals.h"
#include <math.h>
#include "config.h"

// ===============================
// Variables internas
// ===============================
static float filterVdc = 0.0f;
static Butterworth2 bwVdc;
static float filterPower = -1e9f;
static float vdc_reference = NAN;
static unsigned long lastEnergyUpdate = 0;
static float energy_Wh = 0.0f;
static float filterCurrentEstimated = -1e9f;

void resetVdcMode()
{
    filterVdc = -1e9f;
    filterCurrentEstimated = -1e9f;
    filterPower = -1e9f;

    bwVdc.reset();
}

// ===============================
// Medición interna
// ===============================
static float measureVDC_internal()
{
    measurement_result_t meas = measure_channels();
    float v = meas.voltage;
    if (isinf(v))
        return v;

    filterVdc = applyEMA(v, filterCurrentEstimated, FILTER_ALPHA);

    return filterVdc;
}

static float measureVDC_Relative_internal()
{
    float v = measureVDC_internal();
    if (isnan(vdc_reference))
        vdc_reference = v;
    return v - vdc_reference;
}

static float measurePower_internal()
{
    measurement_result_t meas = measure_channels();
    float v = meas.voltage;
    float i = filterCurrentEstimated;

    if (isinf(v) || isinf(i))
        return INFINITY;

    float p = v * i;
    filterPower = applyEMA(p, filterPower, FILTER_ALPHA);
    return filterPower;
}

static float measureEnergy_internal()
{
    unsigned long now = millis();
    if (lastEnergyUpdate == 0)
    {
        lastEnergyUpdate = now;
        return energy_Wh;
    }

    float dt_h = (now - lastEnergyUpdate) / 3600000.0f;
    float p = measurePower_internal();
    if (!isinf(p))
        energy_Wh += p * dt_h;
    lastEnergyUpdate = now;
    return energy_Wh;
}

static float measureCurrentEstimated_internal()
{
    return measureCURRENT_RAW() * 1000.0f; // mA
}

// ===============================
// Funciones de pantalla internas
// ===============================
static void showVDC(void)
{
    float v = measureVDC_internal();
    if (autoHold_update(v))
        v = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    if (isinf(v))
    {
        lcd_driver_print_P(&lcd, PSTR("VDC: OVL"));
        return;
    }

    lcd_driver_print_P(&lcd, PSTR("VDC: "));
    if (use_millivolts(v))
    {
        lcd_driver_printFloat(&lcd, v * 1000.0f, 1);
        lcd_driver_print_P(&lcd, PSTR(" mV"));
    }
    else
    {
        lcd_driver_printFloat(&lcd, v, 3);
        lcd_driver_print_P(&lcd, PSTR(" V"));
    }
}

static void showVDC_Relative(void)
{
    float v = measureVDC_Relative_internal();
    lcd_ui_clear(&lcd);
    if (isinf(v))
    {
        lcd_driver_print_P(&lcd, PSTR("REL: OVL"));
        return;
    }

    lcd_driver_print_P(&lcd, PSTR("REL: "));
    if (use_millivolts(fabs(v)))
    {
        lcd_driver_printFloat(&lcd, v * 1000.0f, 1);
        lcd_driver_print_P(&lcd, PSTR(" mV"));
    }
    else
    {
        lcd_driver_printFloat(&lcd, v, 3);
        lcd_driver_print_P(&lcd, PSTR(" V"));
    }
}

static void showPower(void)
{
    float p = measurePower_internal();
    lcd_ui_clear(&lcd);
    if (isinf(p))
    {
        lcd_driver_print_P(&lcd, PSTR("P: OVL"));
        return;
    }

    lcd_driver_print_P(&lcd, PSTR("P: "));
    lcd_driver_printFloat(&lcd, p, 2);
    lcd_driver_print_P(&lcd, PSTR(" W"));
}

static void showEnergy(void)
{
    float e = measureEnergy_internal();
    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("E: "));
    lcd_driver_printFloat(&lcd, e, 3);
    lcd_driver_print_P(&lcd, PSTR(" Wh"));
}

static void showEstCurrent(void)
{
    float i = measureCurrentEstimated_internal();
    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("I est: "));
    lcd_driver_printFloat(&lcd, i, 1);
    lcd_driver_print_P(&lcd, PSTR(" mA"));
}

// ===============================
// Dispatcher por submodo
// ===============================
void measureVDC_MODE(VdcSubMode submode)
{
    switch (submode)
    {
    case VDC_MAIN:
        showVDC();
        break;
    case VDC_REL:
        showVDC_Relative();
        break;
    case VDC_POWER:
        showPower();
        break;
    case VDC_ENERGY:
        showEnergy();
        break;
    case VDC_CURRENT_EST:
        showEstCurrent();
        break;
    }
}

// ===============================
// Wrappers para menú
// ===============================
void measureVDC_Main(void) { showVDC(); }
void measureVDC_Rel(void) { showVDC_Relative(); }
void measureVDC_Power(void) { showPower(); }
void measureVDC_Energy(void) { showEnergy(); }
void measureVDC_EstCurr(void) { showEstCurrent(); }