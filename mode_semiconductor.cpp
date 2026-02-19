#include "mode_semiconductor.h"
#include "mode_diode.h"
#include "mode_transistor.h"
#include "mode_mosfet.h"
#include "mode_zener.h"
#include "backlight.h"

// Dispatcher: llama al modo específico
void measureSemiconductorMode(SemiconductorSubMode submode)
{
    switch (submode)
    {
    case SEMI_DIODE:
        measureDiode_Main();
        break;
    case SEMI_TRANSISTOR:
        measureTRANSISTOR();
        break;
    case SEMI_MOSFET:
        measureMosfetMode();
        break;
    case SEMI_ZENER:
        measureZENER_MODE();
        break;
    }
}