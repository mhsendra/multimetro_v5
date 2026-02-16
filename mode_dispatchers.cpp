// globals
#include "globals.h"
#include "autoOff.h"

#include "mode_dispatchers.h"

// includes de todos los modos
#include "mode_vdc.h"
#include "mode_vac.h"
#include "mode_current.h"
#include "mode_ohm.h"
#include "mode_cap.h"
#include "mode_freq.h"
#include "mode_diode.h"
#include "mode_transistor.h"
#include "mode_mosfet.h"
#include "mode_zener.h"
#include "induct.h"

void dispatchMode(MainMode selectedMode)
{
    switch (selectedMode)
    {
    case MODE_VDC:
        measureVDC_MODE();
        break;

    case MODE_VAC:
        measureVAC_MODE();
        break;

    case MODE_CURRENT:
        measureCURRENT();
        break;

    case MODE_OHM:
        measureOHM_MODE();
        break;

    case MODE_CAP:
        measureCAPMode();
        break;

    case MODE_FREQ:
        measureFREQ();
        break;

    case MODE_PN: // Aquí reemplazamos mode_pn.cpp
        switch (diodeSubMode)
        {
        case DIODE_MAIN:
            showDiode();
            break;

        case DIODE_TRANSISTOR:
            showTransistor();
            break;

        case DIODE_MOSFET:
            showMosfet();
            break;

        case DIODE_ZENER:
            mode_zener_run();
            break;
        }
        break;

    case MODE_INDUCT:
        measureInductanceMode();
        break;

    default:
        break;
    }
}