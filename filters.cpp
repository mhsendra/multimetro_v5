#include "filters.h"
#include "globals.h"

// =====================================================
// EMA
// =====================================================
float applyEMA(float x, float &state, float alpha)
{
    if (state < -1e8)
    {
        state = x;
        return x;
    }
    state = state + alpha * (x - state);
    return state;
}

// =====================================================
// RMS MÓVIL
// =====================================================
float movingRMS(float x)
{
    static float acc = 0;
    static int count = 0;

    acc += x * x;
    count++;

    if (count >= 50)
    {
        float rms = sqrt(acc / count);
        acc = 0;
        count = 0;
        return rms;
    }

    return sqrt(acc / max(1, count));
}

// =====================================================
// MEDIANA DE 3
// =====================================================
float median3(float a, float b, float c)
{
    float m;

    // Ordenar a, b, c sin usar swap()
    if (a > b)
    {
        float t = a;
        a = b;
        b = t;
    }
    if (b > c)
    {
        float t = b;
        b = c;
        c = t;
    }
    if (a > b)
    {
        float t = a;
        a = b;
        b = t;
    }

    return b;
}

// =====================================================
// BUTTERWORTH WRAPPER
// =====================================================
float applyButterworth(Butterworth2 &f, float x)
{
    return f.update(x);
}

// =====================================================
// INICIALIZACIÓN DE FILTROS
// =====================================================
void initFilters()
{
    bw_vdc.reset();
    bw_vac.reset();
    bw_current.reset();
}