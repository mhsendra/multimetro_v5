#include <Arduino.h>
#include "mode_cap.h"
#include "measurement.h"
#include "lcd_ui.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "range_control.h"
#include "filters.h"
#include "globals.h"
#include "cap_utils.h"
#include "config.h"
#include "mode_state.h"
#include "mode_current.h"

void resetCapMode()
{
    // reset acumuladores si existen
}

// Descarga el condensador
void dischargeCap()
{
    pinMode(pin.CAP_CHARGE_PIN, OUTPUT);
    digitalWrite(pin.CAP_CHARGE_PIN, LOW);
    delay(5);
    pinMode(pin.CAP_CHARGE_PIN, INPUT);
}

// Protección por voltaje residual
bool capResidualVoltageTooHigh()
{
    float v_adc = measureADC_Single(ADC_CH_VOLTAGE);
    float v = v_adc * 0.110f; // ejemplo de divisor
    return v > CAP_RESIDUAL_VOLT_MAX;
}

// Medición de capacitancia
float measureCapacitance()
{
    backlight_activity();

    if (capResidualVoltageTooHigh())
        return -1;

    dischargeCap();

    pinMode(pin.CAP_CHARGE_PIN, OUTPUT);
    digitalWrite(pin.CAP_CHARGE_PIN, HIGH);

    unsigned long tStart = micros();
    while (true)
    {
        float v = measureADC_Single(ADC_CH_VOLTAGE);
        if (v >= CAP_THRESHOLD)
            break;
        if (micros() - tStart > CAP_TIMEOUT_US)
            return -2;
    }

    unsigned long dt = micros() - tStart;

    float R = CAP_R_UF; // default
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

    float C = (float)dt / (R * 1e6);

    if (C > 0)
        backlight_activity();
    return C;
}

// Medición ESR
float measureESR_raw()
{
    float i = measureCURRENT_RAW();
    if (i < 1e-5f)
        return INFINITY;
    return 5.0f / i;
}

float measureESR()
{
    float esr = measureESR_raw();
    if (isnan(esr) || isinf(esr))
        return esr;
    return esr * cal.esrFactor;
}

// Pantallas
void showCapacitance()
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("Detectando..."));
    delay(200);

    float C = measureCapacitance();
    if (autoHold_update(C))
        C = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    switch ((int)C)
    {
    case -1:
        lcd_driver_print_P(&lcd, PSTR("RESIDUAL"));
        return;
    case -2:
        lcd_driver_print_P(&lcd, PSTR("TIMEOUT"));
        return;
    }

    if (capSubMode == CAP_RANGE_NF)
        lcd_driver_printFloat(&lcd, C * 1e9, 1), lcd_driver_print_P(&lcd, PSTR(" nF"));
    else if (capSubMode == CAP_RANGE_UF)
        lcd_driver_printFloat(&lcd, C * 1e6, 1), lcd_driver_print_P(&lcd, PSTR(" uF"));
    else
        lcd_driver_printFloat(&lcd, C * 1e3, 2), lcd_driver_print_P(&lcd, PSTR(" mF"));
}

void showESR()
{
    backlight_activity();
    autoHold_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("Detectando..."));
    delay(200);

    float esr = measureESR();
    if (autoHold_update(esr))
        esr = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    if (esr == INFINITY)
    {
        lcd_driver_print_P(&lcd, PSTR("OPEN"));
        return;
    }
    if (isnan(esr) || esr < 0)
    {
        lcd_driver_print_P(&lcd, PSTR("ERROR"));
        return;
    }

    lcd_driver_printFloat(&lcd, esr, 2);
    lcd_driver_print_P(&lcd, PSTR(" Ohm"));
}

// ===============================
// Wrappers para menú
// ===============================
void measureCAP_Cap(void) { showCapacitance(); }
void measureCAP_ESR(void) { showESR(); }

// ===============================
// Función pública para dispatcher
// ===============================
void measureCAPMode(void)
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
    default:
        break;
    }
}