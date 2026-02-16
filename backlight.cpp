#include "backlight.h"
#include "lcd_ui.h"
#include "globals.h"
#include "config.h"
#include <Arduino.h>

void backlight_activity()
{
    lastBacklightActivity = millis();
    backlightOff = false;
    lcd_ui_backlightOn(&lcd);
}

void backlight_reset()
{
    lastBacklightActivity = millis();
    backlightOff = false;
    lcd_ui_backlightOn(&lcd);
}

void backlight_update()
{
    if (!backlightOff && (millis() - lastBacklightActivity > BACKLIGHT_TIMEOUT_MS))
    {
        lcd_ui_backlightOff(&lcd);
        backlightOff = true;
    }
}