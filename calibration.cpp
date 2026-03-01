#include "calibration.h"
#include "adcmanager.h"
#include "mode_vac.h"
#include "mode_vdc.h"
#include "mode_ohm.h"
#include "mode_current.h"
#include "mode_freq.h"
#include "induct.h"
#include <EEPROM.h>
#include <Arduino.h>
#include "lcd_ui.h"
#include "globals.h"
#include "config.h"
#include "measurement.h"
#include "pins.h"

// =====================================================
// CARGAR CALIBRACIÓN
// =====================================================
void loadCalibration()
{
    EEPROM.get(0, cal);

    if (isnan(cal.vdc))
    {
        cal.vdc = 1.0;
        cal.vac = 1.0;
        cal.ohm = 1.0;

        cal.currShuntGain = 1.0;
        cal.currShuntOffset = 0.0;
    }
}

// =====================================================
// MEDICIÓN OHM RAW
// =====================================================
static float measureOHM_RAW(OhmSubMode mode)
{
    // Activar SSR según submodo OHM
    ohm_select_range(ohmActiveRange);

    // Medición
    measurement_result_t meas = measure_channels();

    // Apagar SSR tras medir
    digitalWrite(pin.SSR_LOW, LOW);
    digitalWrite(pin.SSR_MID, LOW);
    digitalWrite(pin.SSR_HIGH, LOW);

    return meas.voltage;
}

// =====================================================
// CALIBRACIONES INDIVIDUALES
// =====================================================
void calibrateVDC()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("VDC: aplicar 5.00V"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_meas = measureVDC();
    cal.vdc = 5.00 / v_meas;
}

void calibrateVAC()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("VAC: aplicar 230V"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    measurement_result_t meas = measure_channels();
    float v_meas = meas.voltage;
    cal.vac = 230.0 / v_meas;
}

void calibrateOHM()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("OHM: conectar 1k"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    OhmSubMode mode = OHM_MAIN;          // rango principal para calibración
    ohm_select_range(ohmActiveRange);    // activa SSR correcto
    float r_meas = measureOHM_RAW(mode); // medición RAW

    cal.ohm = 1000.0 / r_meas; // guardar factor de calibración
}

void calibrateCurrent_mA()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("I mA: 0mA"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.currShuntOffset = measureCURRENT_RAW();

    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("I mA: 100mA"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCURRENT_RAW();
    cal.currShuntGain = 0.100 / (v_load - cal.currShuntOffset);
}

void calibrateCurrent_5A()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("I 5A: aplicar 5A"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCURRENT_RAW();
    cal.currShuntGain = 5.0 / (v_load - cal.currShuntOffset);
}

// =====================================================
// ACS 16A pendiente
// =====================================================
// void calibrateCurrent_16A() { /* pendiente */ }

void calibrateESR()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("ESR: corto"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float i0 = measureCURRENT_RAW();
    if (i0 < 0.00001f)
        i0 = 0.00001f;

    cal.esrFactor = 5.0 / i0;
}

void calibrateFrequency()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("FREQ: 1kHz"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float f_meas = measureFrequency_calibrated();
    if (f_meas < 1)
        f_meas = 1;

    cal.freqFactor = 1000.0 / f_meas;
}

void calibrateInductance()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("IND: 1mH"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float L_meas = measureInductance_calibrated();
    if (L_meas < 0.000001f)
        L_meas = 0.000001f;

    cal.inductFactor = 0.001 / L_meas;
}

// =====================================================
// MODO CALIBRACIÓN COMPLETO
// =====================================================
void enterCalibration()
{
    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("CALIBRACION"));
    delay(1500);

    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print_P(&lcd, PSTR("1) Desconectar entradas"));
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print_P(&lcd, PSTR("OK=CAL"));

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.vdc = cal.vac = cal.ohm = 1.0;

    calibrateVDC();
    calibrateVAC();
    calibrateOHM();
    calibrateCurrent_mA();
    calibrateCurrent_5A();
    // calibrateCurrent_16A(); // pendiente
    calibrateESR();
    calibrateFrequency();
    calibrateInductance();

    EEPROM.put(0, cal);

    lcd_ui_clear(&lcd);
    lcd_driver_print_P(&lcd, PSTR("CAL COMPLETA"));
    delay(2000);
}