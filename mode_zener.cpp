#include <Arduino.h>
#include "autoOff.h"
#include "mode_zener.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "auto_hold.h"
#include "backlight.h"
#include "globals.h"
#include "range_control.h"
#include "config.h"
#include "io_expander_mcp23017.h"

/* =====================================================
 * CONFIGURACIÓN DEL DIVISOR PARA ZENER
 * ===================================================== */
// RUP = 100k, RDOWN = 22k  → factor ≈ 0.18
static constexpr float ZENER_DIV_FACTOR = (22.0f / (100.0f + 22.0f));

// Índice del pin BOOST_HV_CTRL en el MCP23017
#define BOOST_HV_CTRL 0

void setupExpanders()
{
    mcpExpander.begin();
    pcf8574.begin();

    // Configura pines
    mcp23017.pinMode(0, OUTPUT);
    pcfExpander.pinMode(0, OUTPUT);
}

/* =====================================================
 * CONTROL DEL BOOSTER
 * ===================================================== */
static inline void booster_set_5V()
{
    mcpExpander.digitalWrite(BOOST_HV_CTRL, HIGH); // MOSFET ON → 5V
}

static inline void booster_set_24V()
{
    mcpExpander.digitalWrite(BOOST_HV_CTRL, LOW); // MOSFET OFF → 24V
}

static inline void booster_init()
{
    mcpExpander.digitalWrite(BOOST_HV_CTRL, HIGH); // modo seguro al arrancar
}

/* =====================================================
 * MEDIR DIODO ZENER
 * ===================================================== */
float measureZener()
{
    rng_release_for_gpio(); // liberar RNG para este modo

    booster_set_24V(); // activar modo alta tensión
    delay(10);         // estabilizar el booster

    // Preparar pines de test
    pinMode(pin.TP1, OUTPUT);
    digitalWrite(pin.TP1, HIGH); // corriente de prueba
    pinMode(pin.TP2, INPUT);

    // Configurar ADC
    adc_manager_select(RANGE_DC_20V);
    adc_manager_set_sps(ADC_SPS_475);

    delay(10); // estabilizar señal

    // Leer tensión en el divisor
    float v_adc = adc_manager_read_voltage();

    // Protección por saturación del ADC
    if (v_adc > 4.95f)
        v_adc = 4.95f;

    // Convertir a tensión real del zener
    float vz = v_adc / ZENER_DIV_FACTOR;

    // Reset de pines
    pinMode(pin.TP1, INPUT);
    pinMode(pin.TP2, INPUT);

    booster_set_5V(); // volver a modo normal

    if (vz < 0.5f) // muy bajo, probablemente no hay zener
        return NAN;

    return vz;
}

/* =====================================================
 * PANTALLA Y AUTO HOLD
 * ===================================================== */
void mode_zener_run()
{
    mcpExpander.begin(); // inicializa MCP23017
    booster_init();      // asegurar estado inicial

    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando Zener...");
    delay(200);

    float vz = measureZener();

    if (!isnan(vz))
        backlight_activity();

    // --- AUTO HOLD ---
    if (autoHold_update(vz))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, "ZENER (HOLD)");
        lcd_ui_setCursor(&lcd, 0, 1);

        if (isnan(held))
        {
            lcd_driver_print(&lcd, "OL");
            return;
        }

        lcd_driver_print(&lcd, "Vz:");
        lcd_driver_printFloat(&lcd, held, 2);
        lcd_driver_print(&lcd, " V");
        return;
    }

    // --- Lectura normal ---
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "ZENER");
    lcd_ui_setCursor(&lcd, 0, 1);

    if (isnan(vz))
    {
        lcd_driver_print(&lcd, "OL");
        return;
    }

    lcd_driver_print(&lcd, "Vz:");
    lcd_driver_printFloat(&lcd, vz, 2);
    lcd_driver_print(&lcd, " V");
}