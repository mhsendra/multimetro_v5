#ifndef GLOBALS_H
#define GLOBALS_H

#include "pins.h"
#include "config.h"
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include "filters.h"
#include "lcd_driver.h"
#include <Adafruit_MCP23X17.h>
#include "io_expander.h"
#include "io_expander_mcp23017.h"

// === Pines ===
extern Pins pin;
extern MCP_Pins mcpPin;

// Medidas
extern float ohmRef;

// LCD
extern LCD_Handle lcd;

// Filtros EMA
constexpr float FILTER_ALPHA = 0.1f;

// Estado expanders
extern uint8_t pcfState;

// Objetos hardware globales
extern Adafruit_MCP23X17 mcp23017;

extern IOExpander *ioExpander;

// VDC ranges
extern float vdcRanges[3];
extern int vdcRange;

// Cable test
extern bool cableOK;
extern unsigned long lastBreak;

// Auto-off / backlight
extern unsigned long lastActivityTime;
extern bool autoOffActive;
extern unsigned long lastBacklightActivity;
extern bool backlightOff;

// ADC

extern ADC_CHANNEL_DIFF ads_mux;

extern uint16_t adsGain;
extern uint16_t adsSps;

extern Calibration cal;

extern const float SHUNT_5A_LIMIT;
extern const float SHUNT_16A_LIMIT;
extern const float SHUNT1_MAX_CURRENT;
extern const float SHUNT2_MAX_CURRENT;

extern ADC_RANGE_ID ohmActiveRange;
extern CurrentRange currentRange;

// Estados iniciales
#define MAIN_MODE_OFF 0
#define DIODE_SUBMODE_NONE 0
/*#define CAP_SUBMODE_NONE 0
#define FREQ_SUBMODE_NONE 0
#define OHM_SUBMODE_NONE 0
#define VDC_SUBMODE_NONE 0
#define VAC_SUBMODE_NONE 0*/

// Rangos
#define OHM_RANGE_AUTO 0
#define CURRENT_RANGE_AUTO 0

bool use_millivolts(float value);

#endif