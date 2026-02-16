#ifndef FILTERS_H
#define FILTERS_H

#include <Arduino.h>

// =====================================================
// FILTRO BUTTERWORTH DE 2º ORDEN (pasa-bajos)
// =====================================================
class Butterworth2 {
public:
    Butterworth2() { reset(); }

    void reset() {
        x1 = x2 = 0.0f;
        y1 = y2 = 0.0f;
    }

    float update(float x) {
        // Coeficientes para fc ≈ 20 Hz @ 1 kHz sample
        const float a0 = 0.06745527f;
        const float a1 = 0.13491055f;
        const float a2 = 0.06745527f;
        const float b1 = -1.1429805f;
        const float b2 = 0.4128016f;

        float y = a0*x + a1*x1 + a2*x2 - b1*y1 - b2*y2;

        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;

        return y;
    }

private:
    float x1, x2;
    float y1, y2;
};

// =====================================================
// FILTRO EMA
// =====================================================
float applyEMA(float x, float &state, float alpha);

// =====================================================
// FILTRO RMS MÓVIL
// =====================================================
float movingRMS(float x);

// =====================================================
// MEDIANA DE 3
// =====================================================
float median3(float a, float b, float c);

// =====================================================
// BUTTERWORTH WRAPPER
// =====================================================
float applyButterworth(Butterworth2 &f, float x);

// =====================================================
// INICIALIZACIÓN DE FILTROS
// =====================================================
void initFilters();

#endif