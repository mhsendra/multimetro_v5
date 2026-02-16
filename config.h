#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =====================================================
// ENUMS
// =====================================================
enum MainMode
{
    MODE_OFF = 0,
    MODE_VDC,
    MODE_VAC,
    MODE_CURRENT,
    MODE_OHM,
    MODE_CAP,
    MODE_FREQ,
    MODE_PN,
    MODE_INDUCT
};

enum MeasureMode
{
    MEASURE_VDC = 0,
    MEASURE_VAC = 1,
    MEASURE_OHM_CONT = 2,
    MEASURE_DIODE_TRANS = 3,
    MEASURE_CURR_mA = 4,
    MEASURE_CURR_A = 5,
    MEASURE_CAP = 6,
    MEASURE_L = 7
};

enum PNType
{
    PN_NONE,
    PN_DIODE,
    PN_TRANSISTOR
};

enum OhmRange
{
    OHM_RANGE_LOW,
    OHM_RANGE_MID,
    OHM_RANGE_HIGH
};

enum CurrentRange
{
    CURR_RANGE_mA,
    CURR_RANGE_5A,
    CURR_RANGE_16A
};

enum FreqSubMode
{
    FREQ_MAIN,
    FREQ_DUTY,
    FREQ_PULSE_WIDTH,
    FREQ_PERIOD
};

enum VdcSubMode
{
    VDC_MAIN,
    VDC_POWER,
    VDC_ENERGY,
    VDC_CURRENT_EST,
    VDC_REL
};

enum VacSubMode
{
    VAC_MAIN,
    VAC_REL
};

enum OhmSubMode
{
    OHM_MAIN,
    OHM_CONT,
    OHM_REL,
    OHM_MINMAX,
    OHM_CABLE
};

enum DiodeSubMode
{
    DIODE_MAIN,
    DIODE_ZENER,
    DIODE_AUTO,
    DIODE_TRANSISTOR,
    DIODE_MOSFET
};

enum CapSubMode
{
    CAP_RANGE_NF,
    CAP_RANGE_UF,
    CAP_RANGE_MF,
    CAP_ESR
};

// =====================================================
// CONSTANTES
// =====================================================
#define ADC_VOLT 0
#define ADC_OHM 1
#define ADC_IMA 2
#define ADC_ACS 3
#define CAP_ADC_CH ADC_OHM

#define OHM_VREF 5.0
#define OHM_PROTECT_THRESHOLD 0.8f
#define VAC_DIVIDER 101.0

#define SHUNT_MAIN_OHMS 0.10f
#define SHUNT_16A_OHMS 0.033f
#define SHUNT_GAIN 6.6f

#define OHM_100_MAX 80.0f   // si pasa de 80 Ω → subir a 10K
#define OHM_10K_MAX 8000.0f // si pasa de 8 kΩ → subir a 1M

#define OHM_10K_MIN 80.0f  // si baja de 80 Ω → bajar a 100
#define OHM_1M_MIN 8000.0f // si baja de 8 kΩ → bajar a 10K

#define CONT_ON_OHMS 30.0
#define CONT_OFF_OHMS 60.0

#define CAP_THRESHOLD (OHM_VREF * 0.632)
#define CAP_TIMEOUT_US 3000000UL
#define CAP_R_OHMS 100000.0f
#define CAP_RESIDUAL_VOLT_MAX 0.3f
#define CAP_DISCHARGE_MS 200
#define CAP_R_NF 1000000.0f
#define CAP_R_UF 100000.0f
#define CAP_R_MF 10000.0f

#define IND_ADC_CH ADC_OHM
#define IND_R_OHMS 100.0f
#define IND_THRESHOLD (OHM_VREF * 0.632f)
#define IND_TIMEOUT_US 200000UL

#define LCD_UPDATE_MS 250
#define AUTO_OFF_TIMEOUT_MS (3UL * 60UL * 1000UL)
#define BACKLIGHT_TIMEOUT_MS 15000

// Ganancia del divisor de ohmios
#define OHM_GAIN 0.001f

// Umbral de continuidad (ohmios)
#define OHM_CONT_THRESHOLD 50.0f

// config.h

// Direcciones I2C
#define I2C_ADDR_PCF8574 0x20
#define I2C_ADDR_MCP23017 0x21
#define I2C_ADDR_LCD 0x27

#define LCD_COLS 16
#define LCD_ROWS 2

// =====================================================
// CALIBRACIÓN
// =====================================================
struct Calibration
{

    float vdc;
    float ohm;
    float vac;

    // --- Corriente shunt (mA y 5A) ---
    float curr_shunt_gain;   // A/V
    float curr_shunt_offset; // V

    // --- Corriente ACS712 (16A) ---
    float acs_offset; // V
    float acs_sens;   // V/A

    // --- ESR ---
    float esr_factor;

    // --- Frecuencia ---
    float freq_factor;

    // --- Inductancia ---
    float induct_factor;
};

// -------------------- SUBMODOS GENÉRICOS --------------------
typedef struct
{
    int id;                  // Enum del submodo
    const char *displayName; // Texto que aparece en el LCD
} SubModeEntry;

// -------------------- TABLA DE SUBMODOS --------------------
typedef struct
{
    const char *title;            // Nombre del menú
    const SubModeEntry *subModes; // Puntero a tabla de submodos
    int numSubModes;              // Número de submodos
} ModeTable;

// -------------------- MODO PRINCIPAL --------------------
typedef struct
{
    const char *name;       // Nombre lógico del modo (ej: "VDC")
    const ModeTable *table; // Tabla de submodos
    int currentSubModeId;   // Submodo activo
} Mode;

// -------------------- VARIABLES EXTERNAS --------------------
extern Mode modes[];
extern int numModes;

#endif