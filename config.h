#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>

// =========================
// Canales diferenciales ADC
// =========================
enum ADC_CHANNEL_DIFF
{
    ADC_CH_SHUNT1 = 0, // AIN0–AIN1
    ADC_CH_SHUNT2 = 1  // AIN2–AIN3
};

// =========================
// Canales single-ended ADC
// =========================
enum ADC_CHANNEL_SINGLE
{
    ADC_CH_ZENER = 0,   // AIN0
    ADC_CH_VOLTAGE = 1, // AIN1 – Voltaje general
    ADC_CH_OHM = 2,     // AIN2 – Para medir resistencias con fuente controlada
    ADC_CH_NCV = 3      // AIN3 – Non-contact voltage virtual pin
};

// =========================
// Rangos de medición
// =========================
enum ADC_RANGE_ID
{
    RANGE_NONE = 0,
    RANGE_LOW,
    RANGE_MEDIUM,
    RANGE_HIGH
};

// =========================
// Velocidad de muestreo
// =========================
enum ADC_SPS
{
    ADC_SPS_8,
    ADC_SPS_16,
    ADC_SPS_32,
    ADC_SPS_64,
    ADC_SPS_128,
    ADC_SPS_250,
    ADC_SPS_475,
    ADC_SPS_860
};

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
    MODE_SEMICONDUCTOR,
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
    MEASURE_L = 7,
    MODE_NCV = 8

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
    FREQ_PERIOD,
    FREQ_NCV
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
    OHM_CABLE
};

enum CapSubMode
{
    CAP_RANGE_NF,
    CAP_RANGE_UF,
    CAP_RANGE_MF,
    CAP_ESR
};

enum SemiconductorSubMode
{
    SEMI_DIODE,
    SEMI_TRANSISTOR,
    SEMI_MOSFET,
    SEMI_ZENER,
};

enum DiodeSubMode
{
    DIODE_MAIN, // medición típica de forward voltage
    DIODE_REV,  // polarización inversa (si aplica)
    DIODE_ZENER // para zeners > 4.3 V usando ADC_CH_ZENER
};

enum TransistorType
{
    TRANSISTOR_NONE = 0, // No detectado o N/A
    TRANSISTOR_NPN,      // NPN
    TRANSISTOR_PNP       // PNP
};
enum InductSubMode
{
    INDUCT_MAIN // único submodo de inductancia
};

// =====================================================
// CONSTANTES
// =====================================================
#define CAP_ADC_CH ADC_OHM

#define OHM_VREF 5.0
#define OHM_PROTECT_THRESHOLD 0.8f
#define VAC_DIVIDER 101.0

#define SHUNT_GAIN 6.6f

#define OHM_100_MAX 80.0f   // si pasa de 80 Ω → subir a 10K
#define OHM_10K_MAX 8000.0f // si pasa de 8 kΩ → subir a 1M

#define OHM_10K_MIN 80.0f  // si baja de 80 Ω → bajar a 100
#define OHM_1M_MIN 8000.0f // si baja de 8 kΩ → bajar a 10K

#define CONT_ON_OHMS 30.0
#define CONT_OFF_OHMS 60.0

// =====================================================
// Constantes de corriente de test
// =====================================================
#define I_TEST_100 0.001f
#define I_TEST_10K 0.0001f
#define I_TEST_1M 0.0000001f

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
#define I2C_ADDR_MCP23017 0x20
#define I2C_ADDR_LCD 0x3E

#define LCD_COLS 16
#define LCD_ROWS 2

// =========================
// Constantes de hardware
// =========================
#define SHUNT1_R 0.1f
#define SHUNT2_R 0.033f

// -------------------- SUBMODOS GENÉRICOS --------------------
typedef void (*MeasureFunction)(void); // 👈 puntero a función

struct SubModeEntry
{
    int id;                      // Enum del submodo
    const char *displayName;     // Texto LCD
    MeasureFunction measureFunc; // 👈 función asociada
};

struct Calibration
{
    float vdc;
    float vac;
    float ohm;
    float currShuntGain;
    float currShuntOffset;
    float esrFactor;
    float zenerFactor;
    float freqFactor;
    float inductFactor;
};

// -------------------- TABLA DE SUBMODOS --------------------
struct ModeTable
{
    const char *title;            // Nombre del menú
    const SubModeEntry *subModes; // Puntero a tabla de submodos
    int numSubModes;              // Número de submodos
};

// -------------------- MODO PRINCIPAL --------------------
struct Mode
{
    const char *name;       // Nombre lógico del modo (ej: "VDC")
    const ModeTable *table; // Tabla de submodos
    int currentSubModeId;   // Submodo activo
};

// -------------------- VARIABLES EXTERNAS --------------------
extern Mode modes[];
extern int numModes;

#endif