#include "CurrentSensor.h"
#include "hardware/adc.h"

CurrentSensor::CurrentSensor(float r_ref)
{
    m_valueShuntResistor = r_ref;
}

void CurrentSensor::init()
{
    adc_init();        //ADC initialization
    adc_gpio_init(26); //enable ADC0 on pin 26
    adc_gpio_init(27); //enable ADC1 on pin 27
}

void CurrentSensor::selectChannel(uint8_t chaNo)
{
    adc_select_input(chaNo&1);
}

uint16_t CurrentSensor::getValueMilliAmpere()
{
    uint16_t result = adc_read(); //ADC value reading
    float voltage_value = result * (3.3/4095); //ADC to voltage conversion
    float current_value = voltage_value / m_valueShuntResistor;
    return(static_cast<uint16_t>(current_value * 1000));
}
