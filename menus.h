#ifndef MENUS_H
#define MENUS_H

#include <Arduino.h>
#include "lcd_ui.h"
#include "globals.h"
#include "config.h" // <- ya tiene SubModeEntry, ModeTable, Mode

// -------------------- VARIABLES EXTERNAS --------------------
extern Mode modes[];
extern int numModes;

// -------------------- FUNCIONES --------------------
void showMenu(int modeIndex); // Muestra menú de un modo según submodo actual
// int readSelector();           // Lee selector físico (3 bits)

#endif