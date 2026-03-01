#include "mode_NCV.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "pins.h"
#include "globals.h"
#include "lcd_ui.h"

void measureNCV()
{
    backlight_activity();
    autoHold_reset();

    // Filtrado simple: 8 muestras rápidas
    uint8_t countHigh = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (ioExpander->digitalRead(mcpPin.NCV) == HIGH)
            countHigh++;
        delay(2);
    }

    // Nivel NCV (0 = nada, 8 = máximo)
    uint8_t level = countHigh;

    // AutoHold
    float levelFloat = (float)level;
    if (autoHold_update(levelFloat))
        levelFloat = autoHold_getHeldValue();

    // Pantalla
    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("NCV"));

    lcd_ui_setCursor(&lcd, 0, 1);

    if (levelFloat < 1)
    {
        lcd_driver_print_P(&lcd, PSTR("No detectado"));
        return;
    }

    // Barra de intensidad NCV
    lcd_driver_print_P(&lcd, PSTR("Nivel: "));
    lcd_driver_printInt(&lcd, (int)levelFloat);
    lcd_driver_print_P(&lcd, PSTR("/8"));
}
