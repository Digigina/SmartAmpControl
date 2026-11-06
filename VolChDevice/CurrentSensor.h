#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"

class CurrentSensor
{
    public:
    CurrentSensor(float r_ref);

    void init();

    void selectChannel(uint8_t chaNo);
    uint16_t getValueMilliAmpere();

    private:
    float m_valueShuntResistor;
};