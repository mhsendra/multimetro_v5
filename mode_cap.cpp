#include <Arduino.h>
#include "Mode_CAP.h"
#include "measurement.h"
#include "lcd_ui.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "mode_current.h"
#include "range_control.h"
#include "filters.h"
#include "cap_utils.h"
#include "config.h"
#include <math.h>
#include "pins.h"
#include "globals.h"
#include "mode_state.h"

void dischargeCap()
{
    pinMode(pin.CAP_CHARGE_PIN, OUTPUT);
    digitalWrite(pin.CAP_CHARGE_PIN, LOW);
    delay(5); // espera a descargar
    pinMode(pin.CAP_CHARGE_PIN, INPUT);
}

// =====================================================
// PROTECCIÓN POR VOLTAJE RESIDUAL
// =====================================================
static bool capResidualVoltageTooHigh()
{
    measurement_result_t meas = measure_channels();
    float v_adc = meas.voltage;

    // Escala del divisor para DC_20V
    float v = v_adc * 0.110f;
    return v > CAP_RESIDUAL_VOLT_MAX;
}

// =====================================================
// MEDICIÓN DE CAPACITANCIA
// =====================================================
float measureCapacitance()
{
    backlight_activity();

    // 1. Protección por voltaje residual
    if (capResidualVoltageTooHigh())
        return -1;

    // 2. Descargar condensador
    dischargeCap();

    // 3. Preparar carga
    pinMode(pin.CAP_CHARGE_PIN, OUTPUT);
    digitalWrite(pin.CAP_CHARGE_PIN, HIGH);

    // 4. Medir tiempo hasta umbral
    unsigned long tStart = micros();

    while (true)
    {
        measurement_result_t meas = measure_channels();
        float v = meas.voltage;

        if (v >= CAP_THRESHOLD)
            break;

        if (micros() - tStart > CAP_TIMEOUT_US)
            return -2; // timeout
    }

    unsigned long dt = micros() - tStart;

    // 5. Seleccionar resistencia según submodo
    float R = CAP_R_UF;
    switch (capSubMode)
    {
    case CAP_RANGE_NF:
        R = CAP_R_NF;
        break;
    case CAP_RANGE_UF:
        R = CAP_R_UF;
        break;
    case CAP_RANGE_MF:
        R = CAP_R_MF;
        break;
    default:
        break;
    }

    // 6. Calcular capacitancia
    float C = (float)dt / (R * 1e6);

    if (C > 0 && !isnan(C) && !isinf(C))
        backlight_activity();

    return C;
}

// =====================================================
// ESR RAW
// =====================================================
float measureESR_raw()
{
    float i = measureCURRENT_RAW();
    ;
    if (i < 1e-5f)
        return INFINITY;

    return 5.0f / i;
}

// =====================================================
// ESR CALIBRADO
// =====================================================
float measureESR()
{
    float esr = measureESR_raw();
    if (isnan(esr) || isinf(esr))
        return esr;

    return esr * cal.esr_factor;
}

// =====================================================
// PANTALLA CAPACITANCIA
// =====================================================
void showCapacitance()
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando...");
    delay(200);

    float C = measureCapacitance();

    if (autoHold_update(C))
        C = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    switch ((int)C)
    {
    case -1:
        lcd_driver_print(&lcd, "RESIDUAL");
        return;
    case -2:
        lcd_driver_print(&lcd, "TIMEOUT");
        return;
    }

    if (capSubMode == CAP_RANGE_NF)
        lcd_driver_printFloat(&lcd, C * 1e9, 1), lcd_driver_print(&lcd, " nF");
    else if (capSubMode == CAP_RANGE_UF)
        lcd_driver_printFloat(&lcd, C * 1e6, 1), lcd_driver_print(&lcd, " uF");
    else
        lcd_driver_printFloat(&lcd, C * 1e3, 2), lcd_driver_print(&lcd, " mF");
}

// =====================================================
// PANTALLA ESR
// =====================================================
void showESR()
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Detectando...");
    delay(200);

    float esr = measureESR();

    if (autoHold_update(esr))
        esr = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (esr == INFINITY)
    {
        lcd_driver_print(&lcd, "OPEN");
        return;
    }
    if (isnan(esr) || esr < 0)
    {
        lcd_driver_print(&lcd, "ERROR");
        return;
    }

    lcd_driver_printFloat(&lcd, esr, 2);
    lcd_driver_print(&lcd, " Ohm");
}

// =====================================================
// API PÚBLICA DEL MODO CAP
// =====================================================
void measureCAPMode()
{
    rng_release_for_gpio();

    switch (capSubMode)
    {
    case CAP_RANGE_NF:
    case CAP_RANGE_UF:
    case CAP_RANGE_MF:
        showCapacitance();
        break;
    case CAP_ESR:
        showESR();
        break;
    }
}