#include "diode_utils.h"
#include "adcmanager.h"
#include "globals.h"
#include "config.h"
#include "range_control.h"

// =====================================================
// Devuelve true si hay conducción entre pinA → pinB
// =====================================================
bool diodeConducts(uint8_t pinA, uint8_t pinB)
{
    // 1) Configurar pinA como salida y ponerlo a HIGH
    pinMode(pinA, OUTPUT);
    digitalWrite(pinA, HIGH);

    // 2) Configurar pinB como entrada
    pinMode(pinB, INPUT);

    delayMicroseconds(200);

    // 3) Liberar ADC / seleccionar canal automáticamente
    rng_release_for_gpio();

    // 4) Leer tensión usando adc_manager
    float v = adc_manager_read_voltage();

    // 5) Resetear pines
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);

    // 6) Consideramos conducción si la caída es > 0.15 V
    return (v > 0.15f);
}