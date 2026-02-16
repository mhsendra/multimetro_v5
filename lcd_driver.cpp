#include "lcd_driver.h"

void lcd_driver_init(LCD_Handle *handle, uint8_t addr, uint8_t cols, uint8_t rows)
{
    // Crear el objeto LiquidCrystal_I2C dinámicamente y asignarlo al puntero
    handle->lcd = new LiquidCrystal_I2C(addr, cols, rows);

    handle->lcd->begin(cols, rows);
    handle->lcd->backlight();
}

void lcd_driver_clear(LCD_Handle *handle)
{
    handle->lcd->clear();
}

void lcd_driver_print(LCD_Handle *handle, const char *text)
{
    handle->lcd->print(text);
}

void lcd_driver_printFloat(LCD_Handle *handle, float value, uint8_t decimals)
{
    handle->lcd->print(value, decimals);
}

void lcd_driver_setCursor(LCD_Handle *handle, uint8_t col, uint8_t row)
{
    handle->lcd->setCursor(col, row);
}

void lcd_driver_backlightOn(LCD_Handle *handle)
{
    handle->lcd->backlight();
}

void lcd_driver_backlightOff(LCD_Handle *handle)
{
    handle->lcd->noBacklight();
}
