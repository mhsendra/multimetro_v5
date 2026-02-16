#ifndef MODE_INDUCT_H
#define MODE_INDUCT_H

// --- medición ---
float measureInductance_raw();
float measureInductance_calibrated();

// --- UI ---
void showInductance();

// --- API del modo ---
void measureInductanceMode();

#endif