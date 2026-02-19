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
static float filter_power = -1e9;
static float vdc_reference = NAN;
static unsigned long lastEnergyUpdate = 0;
static float energy_Wh = 0.0f;

// ===============================
// Medición
// ===============================
float measureVDC(void)
{
    measurement_result_t meas = measure_channels();
    float v = meas.voltage;

    if (isinf(v))
        return v;

    filter_vdc = applyEMA(v, filter_vdc, filter_alpha);
    return filter_vdc;
}

float measureVDC_Relative(void)
{
    float v = measureVDC();
    if (isnan(vdc_reference))
        vdc_reference = v;
    return v - vdc_reference;
}

float measurePower(void)
{
    measurement_result_t meas = measure_channels();
    float v = meas.voltage;
    float i = filter_current;

    if (isinf(v) || isinf(i))
        return INFINITY;

    float p = v * i;
    filter_power = applyEMA(p, filter_power, filter_alpha);
    return filter_power;
}

float measureEnergy(void)
{
    unsigned long now = millis();
    if (lastEnergyUpdate == 0)
    {
        lastEnergyUpdate = now;
        return energy_Wh;
    }

    float dt_h = (now - lastEnergyUpdate) / 3600000.0f;
    float p = measurePower();
    if (!isinf(p))
        energy_Wh += p * dt_h;
    lastEnergyUpdate = now;
    return energy_Wh;
}

float measureCurrentEstimated(void)
{
    return measureCURRENT_RAW() * 1000.0f; // mA
}

// ===============================
// Pantallas
// ===============================
void showVDC(void)
{
    float v = measureVDC();
    if (autoHold_update(v))
        v = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    if (isinf(v))
    {
        lcd_driver_print(&lcd, "VDC: OVL");
        return;
    }

    lcd_driver_print(&lcd, "VDC: ");
    if (use_millivolts(v))
    {
        lcd_driver_printFloat(&lcd, v * 1000.0f, 1);
        lcd_driver_print(&lcd, " mV");
    }
    else
    {
        lcd_driver_printFloat(&lcd, v, 3);
        lcd_driver_print(&lcd, " V");
    }
}

void showVDC_Relative(void)
{
    float v = measureVDC_Relative();
    lcd_ui_clear(&lcd);
    if (isinf(v))
    {
        lcd_driver_print(&lcd, "REL: OVL");
        return;
    }

    lcd_driver_print(&lcd, "REL: ");
    if (use_millivolts(fabs(v)))
    {
        lcd_driver_printFloat(&lcd, v * 1000.0f, 1);
        lcd_driver_print(&lcd, " mV");
    }
    else
    {
        lcd_driver_printFloat(&lcd, v, 3);
        lcd_driver_print(&lcd, " V");
    }
}

void showPower(void)
{
    float p = measurePower();
    lcd_ui_clear(&lcd);
    if (isinf(p))
    {
        lcd_driver_print(&lcd, "P: OVL");
        return;
    }

    lcd_driver_print(&lcd, "P: ");
    lcd_driver_printFloat(&lcd, p, 2);
    lcd_driver_print(&lcd, " W");
}

void showEnergy(void)
{
    float e = measureEnergy();
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "E: ");
    lcd_driver_printFloat(&lcd, e, 3);
    lcd_driver_print(&lcd, " Wh");
}

void showEstCurrent(void)
{
    float i = measureCurrentEstimated();
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "I est: ");
    lcd_driver_printFloat(&lcd, i, 1);
    lcd_driver_print(&lcd, " mA");
}

// ===============================
// Modo completo por submodo
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