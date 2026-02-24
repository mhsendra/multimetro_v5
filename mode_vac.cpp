#include "mode_vac.h"
#include "measurement.h"
#include "lcd_ui.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "globals.h"
#include <math.h>
#include "config.h"

static Butterworth2 bwVac;

void resetVacMode()
{
    bwVac.reset();
}

// Canal ADC a usar para VAC
#define ADC_CHANNEL_FOR_VAC ADC_CH_VOLTAGE

// ===============================
// Lectura RMS
// ===============================
static float measureVAC_RMS_raw()
{
    // Leer el ADC correspondiente
    float v = measureADC_Single(ADC_CHANNEL_FOR_VAC);
    return v;
}

// ===============================
// Lectura VAC relativa
// ===============================
static float measureVAC_relative()
{
    static float ref = NAN;
    float v = measureVAC_RMS_raw();

    if (isnan(ref))
        ref = v;

    return v - ref;
}

// ===============================
// Pantallas
// ===============================
static void showVAC(void)
{
    backlight_activity();
    autoHold_reset();

    float v = measureVAC_RMS_raw();
    if (autoHold_update(v))
        v = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (isinf(v))
    {
        lcd_driver_print_P(&lcd, PSTR("VAC: OVL"));
        return;
    }

    lcd_driver_print_P(&lcd, PSTR("VAC: "));
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

static void showVAC_Relative(void)
{
    backlight_activity();
    autoHold_reset();

    float v = measureVAC_relative();
    if (autoHold_update(v))
        v = autoHold_getHeldValue();

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

// ===============================
// Modo completo por submodo
// ===============================
void measureVAC_MODE(VacSubMode submode)
{
    switch (submode)
    {
    case VAC_MAIN:
        showVAC();
        break;
    case VAC_REL:
        showVAC_Relative();
        break;
    }
}

// ===============================
// Wrappers para menú
// ===============================
void measureVAC_Main(void) { showVAC(); }
void measureVAC_Rel(void) { showVAC_Relative(); }