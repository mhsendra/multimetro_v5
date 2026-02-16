#include "globals.h"

// Objetos hardware
// PCF8574 pcf8574(PCF8574_ADDR);
PCF8574 pcf8574;
Adafruit_MCP23X17 mcp23017;

// Wrappers
IOExpanderPCF8574 pcfExpander(&pcf8574, I2C_ADDR_PCF8574);
IOExpanderMCP23017 mcpExpander(&mcp23017, I2C_ADDR_MCP23017);

// Punteros a expanders
IOExpander *expanders[2] = {&pcfExpander, &mcpExpander};

// Otros globals
Pins pin;
MCP_Pins mcpPin;

float vdcRef = 0;
float ohmRef = 0;
float ohmMin = 0;
float ohmMax = 0;

LCD_Handle lcd;

float filter_vdc = 0;
float filter_current = 0;
float filter_ohm = 0;
float filter_continuity = 0;
float filter_alpha = 0;

Butterworth2 bw_vdc;
Butterworth2 bw_vac;
Butterworth2 bw_current;

MainMode selectedMode = MODE_OFF;
DiodeSubMode diodeSubMode = DIODE_SUBMODE_NONE;
CapSubMode capSubMode = CAP_SUBMODE_NONE;
FreqSubMode freqSubMode = FREQ_SUBMODE_NONE;
OhmSubMode ohmSubMode = OHM_SUBMODE_NONE;
VdcSubMode vdcSubMode = VDC_SUBMODE_NONE;
VacSubMode vacSubMode = VAC_SUBMODE_NONE;
OhmRange currentOhmRange = OHM_RANGE_AUTO;
CurrentRange currentRange = CURRENT_RANGE_AUTO;

uint8_t pcf_state = 0;
uint8_t matrix_pcf_state = 0;

ExpanderSelector selector;

float vac_rms_accum = 0;
float vac_rms_alpha = 0;

float vdc_ranges[3] = {0};
int vdc_range = 0;
float acsOffset = 0;

Calibration cal;

bool cableOK = false;
unsigned long lastBreak = 0;

unsigned long lastActivityTime = 0;
bool autoOffActive = false;
unsigned long lastBacklightActivity = 0;
bool backlightOff = false;

Adafruit_ADS1115 ads;
uint16_t ads_mux = 0;
uint16_t ads_gain = 0;
uint16_t ads_sps = 0;