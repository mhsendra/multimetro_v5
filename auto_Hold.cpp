#include "auto_hold.h"
#include <Arduino.h>

static float lastValue = NAN;
static float holdValue = NAN;
static unsigned long stableStart = 0;
static bool isHeld = false;

const float HOLD_THRESHOLD = 0.005f; // 0.5% variación
const unsigned long HOLD_TIME = 400; // ms

void autoHold_reset()
{
    lastValue = NAN;
    holdValue = NAN;
    stableStart = 0;
    isHeld = false;
}

bool autoHold_update(float value)
{

    if (isnan(lastValue))
    {
        lastValue = value;
        stableStart = millis();
        return false;
    }

    float diff = fabs(value - lastValue);

    if (diff < HOLD_THRESHOLD * fabs(value))
    {
        if (!isHeld && millis() - stableStart > HOLD_TIME)
        {
            holdValue = value;
            isHeld = true;
        }
    }
    else
    {
        stableStart = millis();
        isHeld = false;
    }

    lastValue = value;
    return isHeld;
}

bool autoHold_isHeld()
{
    return isHeld;
}

float autoHold_getHeldValue()
{
    return holdValue;
}