#include "menus.h"
#include "lcd_ui.h"
#include "globals.h"

// -------------------- TABLAS DE SUBMODOS --------------------
// FRECUENCIA
const SubModeEntry freqSubModes[] = {
    {FREQ_MAIN, "1) Frequency"},
    {FREQ_DUTY, "2) Duty"},
    {FREQ_PULSE_WIDTH, "3) PulseWidth"},
    {FREQ_PERIOD, "4) Period"}};
const ModeTable freqTable = {"FREQ MENU", freqSubModes, sizeof(freqSubModes) / sizeof(freqSubModes[0])};

// VDC
const SubModeEntry vdcSubModes[] = {
    {VDC_MAIN, "1) Voltage"},
    {VDC_POWER, "2) Power"},
    {VDC_ENERGY, "3) Energy"},
    {VDC_CURRENT_EST, "4) Est Curr"},
    {VDC_REL, "5) Rel"}};
const ModeTable vdcTable = {"VDC MENU", vdcSubModes, sizeof(vdcSubModes) / sizeof(vdcSubModes[0])};

// VAC
const SubModeEntry vacSubModes[] = {
    {VAC_MAIN, "1) RMS"},
    {VAC_REL, "2) Rel"}};
const ModeTable vacTable = {"VAC MENU", vacSubModes, sizeof(vacSubModes) / sizeof(vacSubModes[0])};

// OHM
const SubModeEntry ohmSubModes[] = {
    {OHM_MAIN, "1) R"},
    {OHM_CONT, "2) Cont"},
    {OHM_REL, "3) Rel"},
    {OHM_MINMAX, "4) Min/Max"},
    {OHM_CABLE, "5) Cable"}};
const ModeTable ohmTable = {"OHM MENU", ohmSubModes, sizeof(ohmSubModes) / sizeof(ohmSubModes[0])};

// DIODE
const SubModeEntry diodeSubModes[] = {
    {DIODE_MAIN, "1) Diodo"},
    {DIODE_TRANSISTOR, "2) Trans"},
    {DIODE_MOSFET, "3) MOSFET"},
    {DIODE_ZENER, "4) Zener"},
    {DIODE_AUTO, "5) Auto"}};
const ModeTable diodeTable = {"DIODE MENU", diodeSubModes, sizeof(diodeSubModes) / sizeof(diodeSubModes[0])};

// -------------------- TABLA GLOBAL DE MODOS --------------------
Mode modes[] = {
    {"FREQ", &freqTable, FREQ_MAIN},
    {"VDC", &vdcTable, VDC_MAIN},
    {"VAC", &vacTable, VAC_MAIN},
    {"OHM", &ohmTable, OHM_MAIN},
    {"DIODE", &diodeTable, DIODE_MAIN}};
int numModes = sizeof(modes) / sizeof(modes[0]);

// -------------------- MENÚ GENÉRICO --------------------
void showMenu(int modeIndex)
{
    if (modeIndex < 0 || modeIndex >= numModes)
        return;

    Mode *mode = &modes[modeIndex];
    const ModeTable *table = mode->table;

    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, table->title);

    for (int i = 0; i < table->numSubModes; i++)
    {
        if (table->subModes[i].id == mode->currentSubModeId)
        {
            lcd_printAt(&lcd, 0, 1, table->subModes[i].displayName);
            break;
        }
    }
}

// -------------------- SELECTOR --------------------
int readSelector()
{
    int b0 = digitalRead(pin.SEL0);
    int b1 = digitalRead(pin.SEL1);
    int b2 = digitalRead(pin.SEL2);
    return (b2 << 2) | (b1 << 1) | b0;
}