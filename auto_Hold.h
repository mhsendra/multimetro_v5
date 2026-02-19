#ifndef AUTO_HOLD_H
#define AUTO_HOLD_H

void autoHold_reset();
bool autoHold_update(float value);
bool autoHold_isHeld();
float autoHold_getHeldValue();

#endif