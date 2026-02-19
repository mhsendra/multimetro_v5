#ifndef MENUS_H
#define MENUS_H

#include "globals.h"

// -------------------- TIPOS --------------------
typedef void (*MeasureFunc)(); // Puntero a función de medición

// -------------------- MENÚ --------------------
void showMenu(int modeIndex);
void updateSelectedMode();

#endif // MENUS_H