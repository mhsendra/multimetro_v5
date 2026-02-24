#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Struct que representa el LCD
struct LCD_Handle
{
    LiquidCrystal_I2C *lcd;
};

// Inicializa el LCD
void lcd_driver_init(LCD_Handle *handle, uint8_t addr, uint8_t cols, uint8_t rows);

// Funciones de bajo nivel
void lcd_driver_clear(LCD_Handle *handle);
void lcd_driver_print(LCD_Handle *handle, const char *text);
void lcd_driver_printFloat(LCD_Handle *handle, float value, uint8_t decimals);
void lcd_driver_setCursor(LCD_Handle *handle, uint8_t col, uint8_t row);
void lcd_driver_backlightOn(LCD_Handle *handle);
void lcd_driver_backlightOff(LCD_Handle *handle);
void lcd_driver_printInt(LCD_Handle *handle, uint16_t value);
void lcd_driver_print_P(LCD_Handle *handle, const char *progmem_s);

#endif