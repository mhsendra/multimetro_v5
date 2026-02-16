#ifndef LCD_UI_H
#define LCD_UI_H

#include "lcd_driver.h"

// Funciones UI de alto nivel
void lcd_ui_setup(LCD_Handle *handle);
void lcd_ui_clear(LCD_Handle *handle);
void lcd_ui_setCursor(LCD_Handle *handle, uint8_t col, uint8_t row);
void lcd_ui_backlightOn(LCD_Handle *handle);
void lcd_ui_backlightOff(LCD_Handle *handle);
// Helpers para imprimir en LCD
void lcd_printAt(LCD_Handle *lcd, uint8_t col, uint8_t row, const char *text);
void lcd_printFloatAt(LCD_Handle *lcd, uint8_t col, uint8_t row, float value, uint8_t decimals);

#endif