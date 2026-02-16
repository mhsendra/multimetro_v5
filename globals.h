#ifndef GLOBALS_H
#define GLOBALS_H

#include "pins.h"
#include "config.h"
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include "filters.h"
#include "lcd_driver.h"
#include <PCF8574.h>
#include <Adafruit_MCP23X17.h>
#include "expander_selector.h"
#include "io_expander.h"
#include "io_expander_mcp23017.h"
#include "io_expander_pcf8574.h"

// === Pines ===
extern Pins pin;
extern MCP_Pins mcpPin;

// Medidas
extern float vdcRef;
extern float ohmRef;
extern float ohmMin;
extern float ohmMax;

// LCD
extern LCD_Handle lcd;

// Filtros EMA
extern float filter_vdc;
extern float filter_current;
extern float filter_ohm;
extern float filter_continuity;
extern float filter_alpha;

// Filtros Butterworth
extern Butterworth2 bw_vdc;
extern Butterworth2 bw_vac;
extern Butterworth2 bw_current;

// Modos y submodos
extern MainMode selectedMode;
extern DiodeSubMode diodeSubMode;
extern CapSubMode capSubMode;
extern FreqSubMode freqSubMode;
extern OhmSubMode ohmSubMode;
extern VdcSubMode vdcSubMode;
extern VacSubMode vacSubMode;
extern OhmRange currentOhmRange;
extern CurrentRange currentRange;

// Estado expanders
extern uint8_t pcf_state;

// Objetos hardware globales
extern PCF8574 pcf8574;
extern Adafruit_MCP23X17 mcp23017;

// Wrappers
extern IOExpanderPCF8574 pcfExpander;
extern IOExpanderMCP23017 mcpExpander;
extern IOExpander *expanders[2];

// Selector
extern ExpanderSelector selector;

// VAC RMS
extern float vac_rms_accum;
extern float vac_rms_alpha;

// VDC ranges
extern float vdc_ranges[3];
extern int vdc_range;
extern float acsOffset;

// Calibración
extern Calibration cal;

// Cable test
extern bool cableOK;
extern unsigned long lastBreak;

// Auto-off / backlight
extern unsigned long lastActivityTime;
extern bool autoOffActive;
extern unsigned long lastBacklightActivity;
extern bool backlightOff;

// ADC
extern Adafruit_ADS1115 ads;
extern uint16_t ads_mux;
extern uint16_t ads_gain;
extern uint16_t ads_sps;

// Estado PCF8574
extern uint8_t matrix_pcf_state;

// Estados iniciales
#define MAIN_MODE_OFF 0
#define DIODE_SUBMODE_NONE 0
#define CAP_SUBMODE_NONE 0
#define FREQ_SUBMODE_NONE 0
#define OHM_SUBMODE_NONE 0
#define VDC_SUBMODE_NONE 0
#define VAC_SUBMODE_NONE 0

// Rangos
#define OHM_RANGE_AUTO 0
#define CURRENT_RANGE_AUTO 0

#endif