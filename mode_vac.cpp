#include "mode_vac.h"
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
static float filter_vac = NAN;
static float vac_reference = NAN;

// ===============================
// Medición
// ===============================
float measureVAC_RMS(void)
{
    // En tu código actual, measureVAC_Relative() devuelve la medición de RMS
    return measureVAC_Relative();
}

float measureVAC_Relative(void)
{
    // Misma función que antes, devuelve RMS relativo
    static float ref = NAN;
    float v = measureVAC_Relative(); // tu función existente
    if (isnan(ref))
        ref = v;
    return v - ref;
}

// ===============================
// Pantallas
// ===============================
void showVAC(void)
{
    float v = measureVAC_RMS();
    if (autoHold_update(v))
        v = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (isinf(v))
    {
        lcd_driver_print(&lcd, "VAC: OVL");
        return;
    }

    lcd_driver_print(&lcd, "VAC: ");
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

void showVAC_Relative(void)
{
    float v = measureVAC_Relative();
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