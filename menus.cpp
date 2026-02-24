#include "menus.h"
#include "CableTest.h"
#include "mode_ohm.h"
#include "mode_vdc.h"
#include "mode_vac.h"
#include "mode_diode.h"
#include "mode_zener.h"
#include "mode_transistor.h"
#include "mode_mosfet.h"
#include "mode_cap.h"
#include "induct.h"
#include "diode_utils.h"
#include "mode_freq.h"
#include "lcd_ui.h"
#include "globals.h"
#include "backlight.h"
#include "auto_Hold.h"

// ===================== TABLAS DE SUBMODOS =====================

// FREQ
SubModeEntry freqSubModes[] = {
    {FREQ_MAIN, "1) Frequency", measureFreq_Main},
    {FREQ_DUTY, "2) Duty", measureFreq_Duty},
    {FREQ_PULSE_WIDTH, "3) PulseWidth", measureFreq_PulseWidth},
    {FREQ_PERIOD, "4) Period", measureFreq_Period}};
ModeTable freqTable = {"FREQ MENU", freqSubModes, sizeof(freqSubModes) / sizeof(freqSubModes[0])};

// VDC
SubModeEntry vdcSubModes[] = {
    {VDC_MAIN, "1) Voltage", measureVDC_Main},
    {VDC_POWER, "2) Power", measureVDC_Power},
    {VDC_ENERGY, "3) Energy", measureVDC_Energy},
    {VDC_CURRENT_EST, "4) Est Curr", measureVDC_EstCurr},
    {VDC_REL, "5) Rel", measureVDC_Rel}};
ModeTable vdcTable = {"VDC MENU", vdcSubModes, sizeof(vdcSubModes) / sizeof(vdcSubModes[0])};

// VAC
SubModeEntry vacSubModes[] = {
    {VAC_MAIN, "1) RMS", measureVAC_Main},
    {VAC_REL, "2) Rel", measureVAC_Rel}};
ModeTable vacTable = {"VAC MENU", vacSubModes, sizeof(vacSubModes) / sizeof(vacSubModes[0])};

// OHM
SubModeEntry ohmSubModes[] = {
    {OHM_MAIN, "1) R", measureOHM_Main},
    {OHM_CONT, "2) CONT", measureOHM_Cont_Wrap},
    {OHM_REL, "3) REL", measureOHM_Rel_Wrap},
    {OHM_CABLE, "4) CABLE", measureOHM_Cable_Wrap}};
ModeTable ohmTable = {"OHM MENU", ohmSubModes, sizeof(ohmSubModes) / sizeof(ohmSubModes[0])};

// DIODE / SEMICONDUCTORES
SubModeEntry semiconductorSubModes[] = {
    {SEMI_DIODE, "1) Diodo", measureDiode_Main},
    {SEMI_TRANSISTOR, "2) Trans", measureTRANSISTOR},
    {SEMI_MOSFET, "3) MOSFET", measureMosfetMode},
    {SEMI_ZENER, "4) Zener", measureZENER_MODE}};
ModeTable diodeTable = {"DIODE MENU", semiconductorSubModes, sizeof(semiconductorSubModes) / sizeof(semiconductorSubModes[0])};

// CAPACITANCIA
SubModeEntry capSubModes[] = {
    {CAP_RANGE_NF, "1) nF", measureCAPMode},
    {CAP_RANGE_UF, "2) uF", measureCAPMode},
    {CAP_RANGE_MF, "3) mF", measureCAPMode},
    {CAP_ESR, "4) ESR", measureCAPMode}};
ModeTable capTable = {"CAP MENU", capSubModes, sizeof(capSubModes) / sizeof(capSubModes[0])};

// INDUCTANCIA
SubModeEntry inductSubModes[] = {
    {INDUCT_MAIN, "1) Inductance", measureInductanceMode}};
ModeTable inductTable = {"INDUCTANCE MENU", inductSubModes, sizeof(inductSubModes) / sizeof(inductSubModes[0])};

// ===================== TABLA GLOBAL DE MODOS =====================
Mode modes[] = {
    {"FREQ", &freqTable, FREQ_MAIN},
    {"VDC", &vdcTable, VDC_MAIN},
    {"VAC", &vacTable, VAC_MAIN},
    {"OHM", &ohmTable, OHM_MAIN},
    {"DIODE", &diodeTable, SEMI_DIODE},
    {"CAP", &capTable, CAP_RANGE_UF},
    {"INDUCT", &inductTable, INDUCT_MAIN}};
int numModes = sizeof(modes) / sizeof(modes[0]);

// ===================== FUNCIONES AUXILIARES =====================
void showMenu(int modeIndex)
{
    if (modeIndex < 0 || modeIndex >= numModes)
        return;

    Mode *mode = &modes[modeIndex];
    const ModeTable *table = mode->table;

    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, table->title);

    // Mostrar submodo activo y ejecutar función
    for (int i = 0; i < table->numSubModes; i++)
    {
        if (table->subModes[i].id == mode->currentSubModeId)
        {
            lcd_printAt(&lcd, 0, 1, table->subModes[i].displayName);

            if (table->subModes[i].measureFunc)
                table->subModes[i].measureFunc();

            break;
        }
    }
}