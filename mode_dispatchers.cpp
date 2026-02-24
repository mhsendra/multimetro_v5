// globals
#include "globals.h"
#include "autoOff.h"
#include "mode_dispatchers.h"
#include "mode_state.h"

// includes de todos los modos
#include "mode_vdc.h"
#include "mode_vac.h"
#include "mode_current.h"
#include "mode_ohm.h"
#include "mode_cap.h"
#include "mode_freq.h"
#include "mode_semiconductor.h"
#include "induct.h"

void dispatchMode(MainMode selectedMode)
{
    switch (selectedMode)
    {
    case MODE_VDC:
        measureVDC_MODE(vdcSubMode);
        break;

    case MODE_VAC:
        measureVAC_MODE(vacSubMode);
        break;

    case MODE_CURRENT:
        measureCURRENT();
        break;

    case MODE_OHM:
        measureOHM_MODE(ohmSubMode);
        break;

    case MODE_CAP:
        measureCAPMode();
        break;

    case MODE_FREQ:
        measureFREQ();
        break;

    case MODE_SEMICONDUCTOR:
        measureSemiconductorMode(semiconductorSubMode);
        break;

    case MODE_INDUCT:
        measureInductanceMode();
        break;

    default:
        break;
    }
}

void resetModeState(MainMode mode)
{
    switch (mode)
    {
    case MODE_VDC:
        resetVdcMode();
        break;

    case MODE_VAC:
        resetVacMode();
        break;

    case MODE_CURRENT:
        resetCurrentMode();
        break;

    case MODE_OHM:
        resetOhmMode();
        break;

    case MODE_CAP:
        resetCapMode();
        break;

    case MODE_FREQ:
        resetFreqMode();
        break;

    case MODE_SEMICONDUCTOR:
        resetSemiconductorMode();
        break;

    case MODE_INDUCT:
        resetInductMode();
        break;

    default:
        break;
    }
}