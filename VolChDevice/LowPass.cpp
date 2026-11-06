#include "LowPass.h"

LowPassFilter::LowPassFilter(float filter_koeff)
{
    m_alpha = filter_koeff;
    m_measValue = 0.;
    m_filterValue = 0.;
}

void LowPassFilter::reset(float def_value)
{
    m_filterValue = def_value;
    m_measValue = def_value;
}
float LowPassFilter::pushValue(float new_value)
{
    float lastFilterValue;
    float result = m_alpha * new_value + (1-m_alpha) * m_filterValue;
    m_filterValue = result;
    return(result);
}
float LowPassFilter::getFilteredValue()
{
    return m_filterValue;
}
