#pragma once

#include <stdint.h>
#include <stdlib.h>

//===========================================================
// Description
// Implementation of an IIR Lowpass filter
// the lower the value of alpha, the strong the filter effect
// alpha must be lower 1 and greter 0
//============================================================
class LowPassFilter
{
    public:
    LowPassFilter(float filter_koeff);

    void reset(float def_value = 0.);
    float pushValue(float new_value);
    float getFilteredValue();

    private:
    float m_alpha;
    float m_measValue;
    float m_filterValue;
};