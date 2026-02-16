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

        cal.curr_shunt_gain = 1.0;
        cal.curr_shunt_offset = 0.0;

        cal.acs_offset = 2.5;
        cal.acs_sens = 0.185;
    }
}

// =====================================================
// CALIBRACIONES INDIVIDUALES
// =====================================================
void calibrateVDC()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "VDC: aplicar 5.00V");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_meas = measureVDC_calibrated();
    cal.vdc = 5.00 / v_meas;
}

void calibrateVAC()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "VAC: aplicar 230V");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_meas = measureVAC_calibrated();
    cal.vac = 230.0 / v_meas;
}

void calibrateOHM()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "OHM: conectar 1k");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float r_meas = measureOhmValue();
    cal.ohm = 1000.0 / r_meas;
}

void calibrateCurrent_mA()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "I mA: 0mA");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.curr_shunt_offset = measureCURRENT_calibrated();

    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "I mA: 100mA");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCURRENT_calibrated();
    cal.curr_shunt_gain = 0.100 / (v_load - cal.curr_shunt_offset);
}

void calibrateCurrent_5A()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "I 5A: aplicar 5A");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCURRENT_calibrated();
    cal.curr_shunt_gain = 5.0 / (v_load - cal.curr_shunt_offset);
}

void calibrateCurrent_16A()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "ACS: 0A");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.acs_offset = measureCurrent_ACS_RAW();

    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "ACS: 10A");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCurrent_ACS_RAW();
    cal.acs_sens = (v_load - cal.acs_offset) / 10.0;
}

void calibrateESR()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "ESR: corto");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float i0 = measureCURRENT_calibrated();
    if (i0 < 0.00001f)
        i0 = 0.00001f;

    cal.esr_factor = 5.0 / i0;
}

void calibrateFrequency()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "FREQ: 1kHz");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float f_meas = measureFrequency_calibrated();
    if (f_meas < 1)
        f_meas = 1;

    cal.freq_factor = 1000.0 / f_meas;
}

void calibrateInductance()
{
    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "IND: 1mH");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float L_meas = measureInductance_calibrated();
    if (L_meas < 0.000001f)
        L_meas = 0.000001f;

    cal.induct_factor = 0.001 / L_meas;
}

// =====================================================
// MODO CALIBRACIÓN COMPLETO
// =====================================================
void enterCalibration()
{
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "CALIBRACION");
    delay(1500);

    lcd_ui_clear(&lcd);
    lcd_ui_setCursor(&lcd, 0, 0);
    lcd_driver_print(&lcd, "1) Desconectar entradas");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "OK=CAL");

    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.vdc = cal.vac = cal.ohm = 1.0;
    cal.acs_offset = measureCurrent_ACS_RAW();

    calibrateVDC();
    calibrateVAC();
    calibrateOHM();
    calibrateCurrent_mA();
    calibrateCurrent_5A();
    calibrateCurrent_16A();
    calibrateESR();
    calibrateFrequency();
    calibrateInductance();

    EEPROM.put(0, cal);

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "CAL COMPLETA");
    delay(2000);
}