#ifndef MODE_FREQ_H
#define MODE_FREQ_H

// =====================================================
// Medición
// =====================================================
float measureFrequency_raw(void);
float measureFrequency_calibrated(void);

float measureDutyCycle(void);
float measurePulseWidth(void);
float measurePeriod(void);

// =====================================================
// Pantallas
// =====================================================
void showFrequency(void);
void showDutyCycle(void);
void showPulseWidth(void);
void showPeriod(void);

// =====================================================
// Modo completo
// =====================================================
void measureFREQ(void);

void measureFreq_Main(void);
void measureFreq_Duty(void);
void measureFreq_PulseWidth(void);
void measureFreq_Period(void);

#endif // MODE_FREQ_H