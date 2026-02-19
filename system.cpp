#include "system.h"
#include "matrix.h"
#include "adcmanager.h"

static SystemMode current_mode = SYS_NONE;

static void system_safe_transition(void)
{
    matrix_disconnect_all();
    delay(20); // descarga relés
}

SystemMode system_get_mode(void)
{
    return current_mode;
}

void system_set_mode(SystemMode new_mode)
{
    if (new_mode == current_mode)
        return;

    system_safe_transition();

    switch (new_mode)
    {
    case SYS_CURRENT_LOW:
        matrix_shunt_low();
        delay(5);
        break;

    case SYS_CURRENT_MID:
        matrix_shunt_mid();
        delay(5);
        break;

    case SYS_CURRENT_HIGH:
        matrix_shunt_high();
        delay(10);
        break;

    case SYS_OHM_LOW:
        matrix_ohm_low();
        delay(5);
        break;

    case SYS_OHM_MID:
        matrix_ohm_mid();
        delay(5);
        break;

    case SYS_OHM_HIGH:
        matrix_ohm_high();
        delay(5);
        break;

    case SYS_VOLT:
        // activar divisor si existe
        break;

    default:
        break;
    }

    current_mode = new_mode;
}