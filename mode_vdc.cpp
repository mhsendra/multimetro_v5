#include "mode_vdc.h"
#include "autoOff.h"
#include "adcmanager.h"
#include "globals.h"
#include "lcd_ui.h"
#include "config.h"
#include "auto_Hold.h"
#include "mode_current.h"
#include "backlight.h"
#include "range_control.h"
#include <math.h>

// =====================================================
// AUTO-RANGO VISUAL (mV / V) con histéresis
// =====================================================
bool use_millivolts(float v)
{
    static bool in_mV = false;

    if (v < 0.95f)
        in_mV = true;
    if (v > 1.05f)
        in_mV = false;

    return in_mV;
}

// =====================================================
// VDC — usando ADS1115
// =====================================================
float measureVDC_raw(void)
{
    float v_adc = adc_manager_read_voltage();

    if (fabs(v_adc) > 4.09f)
        return INFINITY;

    adc_range_id_t r = adc_manager_current_range();
    float scale = 1.0f;

    switch (r)
    {
    case RANGE_DC_200mV:
        scale = 0.0011f;
        break;
    case RANGE_DC_2V:
        scale = 0.011f;
        break;
    case RANGE_DC_20V:
        scale = 0.110f;
        break;
    case RANGE_DC_200V:
        scale = 1.10f;
        break;
    default:
        return NAN;
    }

    return v_adc * scale;
}

float measureVDC_calibrated(void)
{
    float v = measureVDC_raw();
    if (isinf(v))
        return v;

    // ------------------------------
    // Aplicar filtros
    // ------------------------------
    float v_scaled = v * cal.vdc;

    // EMA principal
    filter_vdc = applyEMA(v_scaled, filter_vdc, filter_alpha);

    // Opcional: Butterworth
    float v_butter = applyButterworth(bw_vdc, v_scaled);

    // Retornamos EMA filtrado como valor principal
    return filter_vdc;
}

// =====================================================
// VDC RELATIVO
// =====================================================
static float vdc_reference = NAN;

float measureVDC_Relative(void)
{
    float v = measureVDC_calibrated();
    if (isnan(vdc_reference))
        vdc_reference = v;

    return v - vdc_reference;
}

// =====================================================
// POWER FILTRADA (W)
// =====================================================
static float filter_power = -1e9; // EMA de potencia

float measurePower(void)
{
    // Tomar V y I filtrados
    float v = filter_vdc;     // ya filtrado por EMA de VDC
    float i = filter_current; // ya filtrado por EMA de corriente

    if (isinf(v) || isinf(i))
        return INFINITY;

    float p = v * i;

    // Suavizado adicional opcional de la potencia
    filter_power = applyEMA(p, filter_power, filter_alpha);

    return filter_power;
}

// =====================================================
// ENERGY FILTRADA (Wh)
// =====================================================
static unsigned long lastEnergyUpdate = 0;
static float energy_Wh = 0;

float measureEnergy(void)
{
    unsigned long now = millis();

    if (lastEnergyUpdate == 0)
    {
        lastEnergyUpdate = now;
        return energy_Wh;
    }

    // Tiempo en horas desde la última actualización
    float dt_h = (now - lastEnergyUpdate) / 3600000.0f;

    // Tomar potencia filtrada
    float p = measurePower();

    // Acumular energía si la potencia es válida
    if (!isinf(p))
        energy_Wh += p * dt_h;

    lastEnergyUpdate = now;
    return energy_Wh;
}

// =====================================================
// PANTALLAS
// =====================================================
void showVDC(void)
{
    float v = measureVDC_calibrated();
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

// =====================================================
// MODO COMPLETO VDC
// =====================================================
void measureVDC_MODE(void)
{
    rng_release_for_gpio();

    backlight_activity();

    adc_manager_select(RANGE_DC_20V);

    switch (vdcSubMode)
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
        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, "I est: ");
        lcd_driver_printFloat(&lcd, measureCURRENT_calibrated() * 1000.0f, 1);
        lcd_driver_print(&lcd, " mA");
        break;
    }
}