#include "globals.h"
#include "mode_state.h"

// === Pines ===
Pins pin;
MCP_Pins mcpPin;

// Medidas
float vdcRef = 0.0f;
float ohmRef = 0.0f;

// LCD
LCD_Handle lcd;

// Filtros EMA
float filter_vdc = 0.0f;
float filter_current = 0.0f;
float filter_continuity = 0.0f;
float filter_alpha = 0.1f;

// Filtros Butterworth
Butterworth2 bw_vdc;
Butterworth2 bw_vac;
Butterworth2 bw_current;

// Estado expanders
uint8_t pcf_state = 0;

// Objetos hardware globales
Adafruit_MCP23X17 mcp23017;

IOExpander *ioExpander = nullptr;

// VAC RMS
float vac_rms_accum = 0.0f;
float vac_rms_alpha = 0.05f;

// VDC ranges
float vdc_ranges[3] = {2.0f, 20.0f, 200.0f};
int vdc_range = 1;
float acsOffset = 0.0f;

// Calibración
Calibration cal = {
    .vdc = 1.0f,
    .vac = 1.0f,
    .ohm = 1.0f,
    .curr_shunt_gain = 1.0f,
    .curr_shunt_offset = 0.0f,
    .esr_factor = 1.0f,
    .zener_factor = 1.0f // NUEVO
};

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
uint16_t ads_mux = 0;
uint16_t ads_gain = 1;
uint16_t ads_sps = 0;

const float SHUNT_5A_LIMIT = 5.0f;
const float SHUNT_16A_LIMIT = 16.0f;
const float SHUNT1_MAX_CURRENT = 4.8f;
const float SHUNT2_MAX_CURRENT = 15.8f;

// Definición única de submodos
// CapSubMode capSubMode = CAP_RANGE_UF;
// OhmSubMode ohmSubMode = OHM_MAIN;
// VdcSubMode vdcSubMode = VDC_MAIN;
//  VacSubMode vacSubMode = VAC_MAIN;
//   FreqSubMode freqSubMode = FREQ_MAIN;
// SemiconductorSubMode semiconductorSubMode = SEMI_DIODE;

// Definición única de rangos
CurrentRange currentRange = CURR_RANGE_mA;
ADC_RANGE_ID currentOhmRange = RANGE_MEDIUM;