#include "OhmMinMax.h"
#include "lcd_ui.h"
#include "mode_ohm.h"
#include "globals.h"

void ohmMinMax_reset()
{
    ohmMin = INFINITY;
    ohmMax = 0;
}

void ohmMinMax_update(float R)
{
    if (R < ohmMin)
        ohmMin = R;
    if (R > ohmMax)
        ohmMax = R;
}

void ohmMinMax_show()
{
    lcd_ui_clear(&lcd);

    lcd_printAt(&lcd, 0, 0, "MIN:");
    lcd_printFloatAt(&lcd, 4, 0, ohmMin, 1);

    lcd_printAt(&lcd, 0, 1, "MAX:");
    lcd_printFloatAt(&lcd, 4, 1, ohmMax, 1);
}