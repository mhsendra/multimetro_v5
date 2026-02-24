#ifndef ADCMANAGER_H
#define ADCMANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h" // Incluye los enums y constantes de hardware
#include <Adafruit_ADS1X15.h>
#include <math.h>

uint16_t ads1115_readSingleEnded(uint8_t channel);
float ads1115_readSingleEndedVolts(uint8_t channel);
float measureADC_Single(enum ADC_CHANNEL_SINGLE ch);
// =========================
// Inicialización
// =========================
void adc_manager_init(void);

// =========================
// Autorango automático
// Devuelve el rango seleccionado y opcionalmente el voltaje leído en mV
// =========================
ADC_RANGE_ID adc_manager_autorange(enum ADC_CHANNEL_DIFF channel, float *mv_out);

// =========================
// Lectura raw ADC de un canal diferencial
// =========================
bool adc_manager_read_raw(int16_t *raw, enum ADC_CHANNEL_DIFF channel);

// =========================
// Lectura de voltaje en mV
// =========================
float adc_manager_read_voltage(ADC_CHANNEL_DIFF channel);

void adc_manager_select(CurrentRange range);

// =========================
// Ajuste de velocidad de muestreo (SPS)
// =========================
void adc_manager_set_sps(enum ADC_SPS sps);

#endif // ADCMANAGER_H