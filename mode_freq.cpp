#include "mode_freq.h"
#include "lcd_ui.h"
#include "globals.h"
#include "backlight.h"
#include "auto_hold.h"
#include "autoOff.h"
#include "measurement.h"
#include <Arduino.h>
#include "mode_state.h"
#include "mode_NCV.h"

void resetFreqMode()
{
    // reset contadores internos
}

// =====================================================
// LECTURA ANALÓGICA PARA FRECUENCIA
// =====================================================
static float freq_read_voltage()
{
    measurement_result_t meas = measure_channels();
    return meas.voltage;
}

// =====================================================
// FRECUENCIA — RAW (sin calibrar)
// =====================================================
float measureFrequency_raw()
{
    const unsigned long TIMEOUT = 200000; // 200 ms
    const float TH = 0.5f;

    unsigned long tStart = micros();

    while (freq_read_voltage() < TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    while (freq_read_voltage() > TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    while (freq_read_voltage() < TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;

    unsigned long t1 = micros();

    while (freq_read_voltage() > TH)
        if (micros() - t1 > TIMEOUT)
            return 0.0f;
    while (freq_read_voltage() < TH)
        if (micros() - t1 > TIMEOUT)
            return 0.0f;

    unsigned long t2 = micros();
    float period_us = (float)(t2 - t1);
    if (period_us <= 0.0f)
        return 0.0f;

    return 1e6f / period_us;
}

// =====================================================
// FRECUENCIA — CALIBRADA
// =====================================================
float measureFrequency_calibrated()
{
    float f = measureFrequency_raw();
    if (f <= 0.0f)
        return 0.0f;
    return f * cal.freqFactor;
}

// =====================================================
// DUTY CYCLE (%)
// =====================================================
float measureDutyCycle()
{
    const unsigned long TIMEOUT = 200000;
    const float TH = 0.5f;

    unsigned long tStart = micros();
    while (freq_read_voltage() < TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    while (freq_read_voltage() > TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    while (freq_read_voltage() < TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;

    unsigned long tRise = micros();
    while (freq_read_voltage() > TH)
        if (micros() - tRise > TIMEOUT)
            return 0.0f;
    unsigned long tFall = micros();
    while (freq_read_voltage() < TH)
        if (micros() - tRise > TIMEOUT)
            return 0.0f;
    unsigned long tNextRise = micros();

    float high_us = (float)(tFall - tRise);
    float period_us = (float)(tNextRise - tRise);
    if (period_us <= 0.0f)
        return 0.0f;

    return (high_us / period_us) * 100.0f;
}

// =====================================================
// PULSE WIDTH (HIGH time, us)
// =====================================================
float measurePulseWidth()
{
    const unsigned long TIMEOUT = 200000;
    const float TH = 0.5f;

    unsigned long tStart = micros();
    while (freq_read_voltage() < TH)
        if (micros() - tStart > TIMEOUT)
            return 0.0f;

    unsigned long tRise = micros();
    while (freq_read_voltage() > TH)
        if (micros() - tRise > TIMEOUT)
            return 0.0f;
    unsigned long tFall = micros();

    return (float)(tFall - tRise);
}

// =====================================================
// PERIODO (us) — CALIBRADO
// =====================================================
float measurePeriod()
{
    float f = measureFrequency_calibrated();
    if (f <= 0.0f)
        return 0.0f;
    return 1e6f / f;
}

// =====================================================
// PANTALLAS
// =====================================================
void showFrequency()
{
    float f = measureFrequency_calibrated();
    if (f > 0.0f)
        backlight_activity();

    if (autoHold_update(f))
        f = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("FREQ:"));
    lcd_driver_printFloat(&lcd, f, 1);
    lcd_driver_print_P(&lcd, PSTR(" Hz"));
}

void showDutyCycle()
{
    float d = measureDutyCycle();
    if (d > 0.0f)
        backlight_activity();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("DUTY:"));
    lcd_driver_printFloat(&lcd, d, 1);
    lcd_driver_print_P(&lcd, PSTR(" %"));
}

void showPulseWidth()
{
    float pw = measurePulseWidth();
    if (pw > 0.0f)
        backlight_activity();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("PW:"));
    lcd_driver_printFloat(&lcd, pw, 1);
    lcd_driver_print_P(&lcd, PSTR(" us"));
}

void showPeriod()
{
    float p = measurePeriod();
    if (p > 0.0f)
        backlight_activity();

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("PER:"));
    lcd_driver_printFloat(&lcd, p, 1);
    lcd_driver_print_P(&lcd, PSTR(" us"));
}

// =====================================================
// MODO COMPLETO FRECUENCIA
// =====================================================
void measureFREQ()
{
    backlight_activity();

    switch (freqSubMode)
    {
    case FREQ_MAIN:
        showFrequency();
        break;
    case FREQ_DUTY:
        showDutyCycle();
        break;
    case FREQ_PULSE_WIDTH:
        showPulseWidth();
        break;
    case FREQ_PERIOD:
        showPeriod();
        break;
    case FREQ_NCV:
        measureNCV(); // aquí usas mcpPin.NCV, etc.
        break;
    }
}

// =====================================================
// WRAPPERS PARA SISTEMA DE MENÚ
// =====================================================

void measureFreq_Main(void)
{
    showFrequency();
}

void measureFreq_Duty(void)
{
    showDutyCycle();
}

void measureFreq_PulseWidth(void)
{
    showPulseWidth();
}

void measureFreq_Period(void)
{
    showPeriod();
}