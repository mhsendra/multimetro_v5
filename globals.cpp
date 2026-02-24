#include "globals.h"
#include "mode_state.h"

// === Pines ===
Pins pin;
MCP_Pins mcpPin;

// Medidas
float ohmRef = 0.0f;

// LCD
LCD_Handle lcd;

// Objetos hardware globales
Adafruit_MCP23X17 mcp23017;

IOExpander *ioExpander = nullptr;

// VDC ranges
float vdcRanges[3] = {2.0f, 20.0f, 200.0f};
int vdcRange = 1;

// Calibración
Calibration cal = {
    .vdc = 1.0f,
    .vac = 1.0f,
    .ohm = 1.0f,
    .currShuntGain = 1.0f,
    .currShuntOffset = 0.0f,
    .esrFactor = 1.0f,
    .zenerFactor = 1.0f,
    .freqFactor = 1.0f,
    .inductFactor = 1.0f};

// ===============================
// Auto-rango visual (mV / V) con histéresis
// ===============================
bool use_millivolts(float v)
{
    static bool in_mV = false;
    if (v < 0.95f)
        in_mV = true;
    if (v > 1.05f)
        in_mV = false;
    return in_mV;
}

// Cable test
bool cableOK = false;
unsigned long lastBreak = 0;

// Auto-off / backlight
unsigned long lastActivityTime = 0;
bool autoOffActive = false;
unsigned long lastBacklightActivity = 0;
bool backlightOff = false;

// ADC
ADC_CHANNEL_DIFF ads_mux = ADC_CH_SHUNT1;
uint16_t adsGain = 1;
uint16_t adsSps = 0;

const float SHUNT_5A_LIMIT = 5.0f;
const float SHUNT_16A_LIMIT = 16.0f;
const float SHUNT1_MAX_CURRENT = 4.8f;
const float SHUNT2_MAX_CURRENT = 15.8f;

// Definición única de rangos
CurrentRange currentRange = CURR_RANGE_mA;
ADC_RANGE_ID ohmActiveRange = RANGE_MEDIUM;