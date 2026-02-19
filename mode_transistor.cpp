#include "mode_transistor.h"
#include "diode_utils.h"
#include "lcd_ui.h"
#include "globals.h"
#include "backlight.h"
#include "auto_Hold.h"
#include "autoOff.h"
#include <Arduino.h>
#include <math.h>
#include <stdio.h>

// Medición hFE
static float measureHFE_raw(uint8_t base, uint8_t collector, uint8_t emitter, bool isNPN)
{
    pinMode(emitter, OUTPUT);
    digitalWrite(emitter, isNPN ? LOW : HIGH);

    pinMode(base, OUTPUT);
    digitalWrite(base, isNPN ? HIGH : LOW);

    delayMicroseconds(200);
    pinMode(collector, INPUT);

    float vc = analogRead(collector) * 5.0 / 1023.0;

    pinMode(base, INPUT);
    pinMode(collector, INPUT);
    pinMode(emitter, INPUT);

    if (vc > 4.95f || vc < 0.05f)
        return NAN;
    return (isNPN ? (5.0f - vc) : vc) * 100.0f;
}

// Detección de transistor
static bool detectTransistor(uint8_t &base, uint8_t &collector, uint8_t &emitter, bool &isNPN)
{
    uint8_t TP[3] = {pin.TP1, pin.TP2, pin.TP3};

    for (int b = 0; b < 3; b++)
    {
        int p1 = (b + 1) % 3;
        int p2 = (b + 2) % 3;

        bool d1 = diodeConducts(TP[b], TP[p1]);
        bool d2 = diodeConducts(TP[b], TP[p2]);
        bool d3 = diodeConducts(TP[p1], TP[b]);
        bool d4 = diodeConducts(TP[p2], TP[b]);

        if (d1 && d2 && !d3 && !d4)
        {
            base = TP[b];
            collector = TP[p1];
            emitter = TP[p2];
            isNPN = true;
            return true;
        }
        if (!d1 && !d2 && d3 && d4)
        {
            base = TP[b];
            collector = TP[p1];
            emitter = TP[p2];
            isNPN = false;
            return true;
        }
    }

    return false;
}

// Mostrar transistor
void showTransistor(void)
{
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_printAt(&lcd, 0, 0, "Detectando...");
    delay(300);

    uint8_t base, collector, emitter;
    bool isNPN;

    if (!detectTransistor(base, collector, emitter, isNPN))
    {
        lcd_printAt(&lcd, 0, 0, "No detectado   ");
        lcd_printAt(&lcd, 0, 1, "                ");
        return;
    }

    float hfe = measureHFE_raw(base, collector, emitter, isNPN);
    if (autoHold_update(hfe))
        hfe = autoHold_getHeldValue();

    char line1[17], line2[17];
    uint8_t b = getTPNumber(base);
    uint8_t c = getTPNumber(collector);
    uint8_t e = getTPNumber(emitter);

    snprintf(line1, sizeof(line1), "%s B:%d C:%d", isNPN ? "NPN" : "PNP", b, c);
    if (isnan(hfe))
        snprintf(line2, sizeof(line2), "E:%d hFE:OL", e);
    else
        snprintf(line2, sizeof(line2), "E:%d hFE:%d", e, (int)hfe);

    lcd_printAt(&lcd, 0, 0, line1);
    lcd_printAt(&lcd, 0, 1, line2);
}

void measureTRANSISTOR(void) { showTransistor(); }