#include <Arduino.h>
#include <Wire.h>
#include "globals.h"
#include "menus.h"
#include "backlight.h"
#include "AutoOff.h"
#include "auto_Hold.h"
#include "lcd_ui.h"
#include "adcmanager.h"
#include "io_expander_mcp23017.h"
#include <Adafruit_MCP23X17.h>
#include "config.h"
#include "mode_dispatchers.h"

// Variables globales
int currentModeIndex = 0;
Adafruit_MCP23X17 mcpHardware;

void setup()
{
    // -------------------- INICIALIZACIÓN BÁSICA --------------------
    Wire.begin();         // Inicializar bus I2C
    Serial.begin(115200); // Serial para debug

    // -------------------- MCP23017 --------------------
    ioExpander = new IOExpanderMCP23017(&mcpHardware, I2C_ADDR_MCP23017);
    ioExpander->begin(); // Configura todos los pines como INPUT + pull-up

    // Configurar pines del selector como INPUT_PULLUP
    for (uint8_t i = mcpPin.SEL0; i <= mcpPin.SEL7; i++)
        ioExpander->pinMode(i, INPUT_PULLUP);

    // -------------------- LCD 1602 I2C --------------------
    // Dirección 0x7C según tu módulo
    lcd_driver_init(&lcd, 0x7C, LCD_COLS, LCD_ROWS);
    lcd_ui_clear(&lcd);

    // -------------------- ADC --------------------
    adc_manager_init(); // Inicializar ADCs internos

    // -------------------- BACKLIGHT / AUTO-OFF / AUTO-HOLD --------------------
    backlight_reset();
    autoOff_reset();
    autoHold_reset();

    // -------------------- MENÚ --------------------
    currentModeIndex = 0;
    modes[currentModeIndex].currentSubModeId = modes[currentModeIndex].table->subModes[0].id;
    showMenu(currentModeIndex);

    Serial.println("Setup completado!");
}

// -------------------- LOOP --------------------
void loop()
{
    static int lastModeIndex = -1;

    // -------------------------------------------------
    // 1️⃣ Leer selector desde MCP23017
    // -------------------------------------------------
    int newModeIndex = -1;

    for (uint8_t i = 0; i < 8; i++)
    {
        // Selector activo en LOW (porque usamos INPUT_PULLUP)
        if (ioExpander->digitalRead(mcpPin.SEL0 + i) == LOW)
        {
            newModeIndex = i;
            break; // Solo un modo activo
        }
    }

    // -------------------------------------------------
    // 2️⃣ Detectar cambio de modo
    // -------------------------------------------------
    if (newModeIndex != -1 && newModeIndex != lastModeIndex)
    {
        currentModeIndex = newModeIndex;
        lastModeIndex = newModeIndex;

        // Reset estados
        autoHold_reset();
        backlight_reset();

        // Submodo por defecto
        modes[currentModeIndex].currentSubModeId =
            modes[currentModeIndex].table->subModes[0].id;

        lcd_ui_clear(&lcd);
        showMenu(currentModeIndex);
    }

    // -------------------------------------------------
    // 3️⃣ Ejecutar modo activo
    // -------------------------------------------------
    if (currentModeIndex >= 0)
    {
        dispatchMode((MainMode)currentModeIndex);
    }

    // -------------------------------------------------
    // 4️⃣ Gestión sistemas auxiliares
    // -------------------------------------------------
    autoOff_update();
    backlight_update();
}