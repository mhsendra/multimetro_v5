#include "autoOff.h"
#include "globals.h"
#include "config.h"
#include <Arduino.h>

static unsigned long lastAutoOffActivity = 0;
static bool autoOffTriggered = false;

void autoOff_reset()
{
    lastAutoOffActivity = millis();
    autoOffTriggered = false;
}

void autoOff_activity()
{
    lastAutoOffActivity = millis();
    autoOffTriggered = false;
}

void autoOff_update()
{
    if (!autoOffTriggered && (millis() - lastAutoOffActivity > AUTO_OFF_TIMEOUT_MS))
    {
        autoOffTriggered = true;

        // Apagar multímetro
        digitalWrite(pin.PIN_ONOFF, LOW);
    }
}