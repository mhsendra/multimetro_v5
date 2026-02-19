#ifndef SYSTEM_H
#define SYSTEM_H

typedef enum
{
    SYS_NONE,
    SYS_CURRENT_LOW,
    SYS_CURRENT_MID,
    SYS_CURRENT_HIGH,
    SYS_OHM_LOW,
    SYS_OHM_MID,
    SYS_OHM_HIGH,
    SYS_VOLT
} SystemMode;

void system_set_mode(SystemMode mode);
SystemMode system_get_mode(void);

#endif