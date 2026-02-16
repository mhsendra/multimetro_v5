#include "mode_vac.h"
#include "autoOff.h"
#include "adcmanager.h"
#include "globals.h"
#include "lcd_ui.h"
#include "config.h"
#include <math.h>
#include "auto_Hold.h"
#include "backlight.h"
#include "range_control.h"

// =====================================================
// CONFIGURACIÓN VAC
// =====================================================
#define VAC_SAMPLES 256 // si quieres puedes dejarlo para compatibilidad

// =====================================================
// AUTO-RANGO VISUAL (mV / V) con histéresis
// =====================================================
static bool use_millivolts_vac(float v)
{
    static bool in_mV = false;
    if (v < 0.95f)
        in_mV = true;
    if (v > 1.05f)
        in_mV = false;
    return in_mV;
}

// =====================================================
// PANTALLAS
// =====================================================
void showVAC(void)
{
    float v = vac_rms_accum; // usar RMS deslizante global

    if (autoHold_update(v))
        v = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (isinf(v))
    {
        lcd_driver_print(&lcd, "VAC: OVL");
        return;
    }

    lcd_driver_print(&lcd, "VAC: ");
    if (use_millivolts_vac(v))
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

void showVAC_Relative(void)
{
    float v = measureVAC_Relative(); // puedes mantener la versión relativa con offset
    lcd_ui_clear(&lcd);

    if (isinf(v))
    {
        lcd_driver_print(&lcd, "REL AC: OVL");
        return;
    }

    lcd_driver_print(&lcd, "REL AC: ");
    if (use_millivolts_vac(fabs(v)))
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

// =====================================================
// VAC — CALIBRADO
// =====================================================
float measureVAC_calibrated(void)
{
    float v = adc_manager_read_voltage() * cal.vac;

    // Si medición fuera de rango
    if (fabs(v) > 4.95f)
        return INFINITY;

    return v;
}

// =====================================================
// VAC RELATIVO
// =====================================================
static float vac_reference = NAN;

float measureVAC_Relative(void)
{
    float v = measureVAC_calibrated();
    if (isnan(vac_reference))
        vac_reference = v;
    return v - vac_reference;
}

// =====================================================
// MODO VAC ITERATIVO (llamar desde loop)
// =====================================================
void measureVAC_MODE(void)
{
    // 1️⃣ Leer ADC y aplicar calibración
    float v = adc_manager_read_voltage() * cal.vac;

    // 2️⃣ RMS deslizante IIR
    vac_rms_accum = sqrtf((1.0f - vac_rms_alpha) * vac_rms_accum * vac_rms_accum + vac_rms_alpha * v * v);

    // 3️⃣ Actualizar pantalla según submodo
    switch (vacSubMode)
    {
    case VAC_MAIN:
        showVAC();
        break;

    case VAC_REL:
        showVAC_Relative();
        break;
    }

    // 4️⃣ Actividad backlight
    backlight_activity();
}